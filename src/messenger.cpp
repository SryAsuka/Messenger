#include "messenger.h"
#include <QMessageBox>
#include <QString>
#include<QByteArray>
#include <QTextCodec>
#include <QTimer>
#include <unistd.h>
#include<QDebug>
#include"ipmsg/filetransworker.h"
#include"ipmsg/ipmsgcommon.h"
static uint32_t g_send_file_id = 100;

extern quint32 g_pkg_seq;

Messenger::Messenger(QObject *parent)
    : QObject{parent}
{
    //初始化用户信息
    mSelf.userId = fromUnicode(QString(QHostInfo::localHostName()));
    mSelf.userVer = QString(IPMSG_VERSION).toUtf8();
    mSelf.userPktSeq = 0;
    mSelf.userHostName = QHostInfo::localHostName().toUtf8();
    mSelf.userNickName = fromUnicode(QString(QHostInfo::localHostName()));
    mSelf.userGroupName = fromUnicode(QString(tr(IPMSG_DEFAULT_GROUPNAME)));
//设置ip范围
    mStartIp = QHostAddress("10.0.3.1").toIPv4Address();
    mEndIp = QHostAddress("10.0.3.254").toIPv4Address();
  //实例化fileServer
    fileServer = new IpMsgFileServer();
//连接信号和槽
    connect(&session,
            SIGNAL(ipMsgUdpSessionDataReady(QHostAddress, QByteArray)),
            this,
            SLOT(UdpSessionHandle(QHostAddress, QByteArray)));
    connect(fileServer,
            SIGNAL(ipMsgFileServerDataReady(quint32, quint16, QByteArray)),
            this,
            SLOT(ipMsgFileServerHandle(quint32, quint16, QByteArray)));
    if (session.mUdpBind != true) {
        qDebug() << "error udp";
        exit(1);
    }
    if (fileServer->mFileServerListen != true) {
       qDebug() << "error";
        exit(1);
    }
    if (true == session.IpMsgUdpSessionValid()) {
        session.ipMsgUserRefresh(&mSelf, mStartIp, mEndIp);
    }
}
//字符编码转换函数
QString Messenger::toUnicode(QByteArray data)
{
    return QTextCodec::codecForName(mCodec.toStdString().c_str())->toUnicode(data);
}

QByteArray Messenger::fromUnicode(QString data)
{
    return QTextCodec::codecForName(mCodec.toStdString().c_str())->fromUnicode(data);
}

Messenger::~Messenger()
{
    qDebug() << "Messenger will close";
}


void Messenger::startChat(int clickedIndex){
    mUsers.at(clickedIndex)->chatForm=&formChat;//将聊天框赋给选中用户

     mUsers.at(clickedIndex)->updateChatFormHistory();


}
//UDP会话处理槽函数
void Messenger::UdpSessionHandle(QHostAddress src, QByteArray data)
{
    //解析数据并处理用户信息更新
    IpMsgUser *user = nullptr;
    uint32_t pktCommand;
    uint8_t command;
    uint32_t commandOpt;
    QList<QByteArray> values;
    QList<QByteArray> others;
    QByteArray chatData;
    QList<QByteArray> files;

    int i = 0;

    values = data.split(':');

    if (values.count() < 5) {
        qDebug() << "Invalid pkt, ignore it";
        return;
    }
//聊天数据提取
    for (i = 5; i < values.count(); i++) {
        chatData.append(values[i]);
        chatData.append(':');
    }
//命令字段解析
    pktCommand = values.at(4).toUInt();

    if (QString::number(pktCommand, 10) != values.at(4)) {
        qDebug() << "Invalid command, ignore it";
        return;
    }

    command = pktCommand & 0x000000FF;
    commandOpt = pktCommand & 0xffffff00;

//处理用户信息更新：
    if (IPMSG_BR_ENTRY == command || IPMSG_ANSENTRY == command) {
        for (i = 0; i < mUsers.count(); i++) {
            if (mUsers.at(i)->userHostAddr == src) {
                break;
            }
        }

        if (i != mUsers.count()) {
            user = mUsers.at(i);
        } else {
            user = new IpMsgUser();
            user->chatForm=&formChat;
            connect(user->chatForm,
                    SIGNAL(sent(QString, QHostAddress)),
                    this,
                    SLOT(ipMsgChatSent(QString, QHostAddress)));
            connect(user->chatForm,
                    SIGNAL(acceptFile(fileEntryT *)),
                    this,
                    SLOT(ipMsgAcceptFile(fileEntryT *)));
            // connect(user->chatForm,
            //         SIGNAL(rejectFile(fileEntryT *)),
            //         this,
            //         SLOT(ipMsgRejectFile(fileEntryT *)));
            // connect(user->chatForm,
            //         SIGNAL(recvfinish(quint32)),
            //         this,
            //         SLOT(ipMsgRecvFileFinish(quint32)));
            // connect(user->chatForm,
            //         SIGNAL(recverror(quint32)),
            //         this,
            //         SLOT(ipMsgRecvFileError(quint32)));
        } //User information update

        user->userVer = values.at(0);
        mSelf.userPktSeq = values.at(1).toUInt();

        user->userId = values.at(2);

        user->userHostName = values.at(3);

        user->userHostAddr = src;

        if (chatData.length() > 0) {
            others = chatData.split(0);
            if (others.count() >= 1) {
                for (int i = 0; i < others.count(); i++) {
                    if (i == 0) {
                        user->userNickName = others.at(0);
                    } else if (i == 1) {
                        user->userGroupName = others.at(1);
                    }
                }
            }
        }
        if (i == mUsers.count()) {
            mUsers.append(user);

        } else {

        }
//响应用户加入：
        if (IPMSG_BR_ENTRY == command) {
            session.ipMsgRespBr(&mSelf, mUsers.at(i));
        }
        setUserInfo();
    }

    //处理聊天信息
    else if (IPMSG_SENDMSG == command) {

        for (i = 0; i < mUsers.count(); i++) {
            if (mUsers.at(i)->userHostAddr == src) {
                mSelf.userPktSeq = values.at(1).toUInt();

                break;
            }
        }

        if(IPMSG_FILEATTACHOPT & commandOpt) {
            files = chatData.split(0);
            // QString receiveFile = toUnicode(mSelf.userNickName + " "+ QDateTime::currentDateTime().toString("hh:mm:ss").toUtf8())+"上传了一个文件:"+QString::fromUtf8(files[1]);
            // receiveMessage(receiveFile);

            // Handle File attched requirment.
            if (i != mUsers.count()) {

                for (int k = 0; k < files.count(); k++) {
                    QList<QByteArray> fileList = files.at(k).split('\a');
                    for (int b = 0; b < fileList.count(); b++) {
                        QList<QByteArray> tmp = fileList.at(b).split(':');
                        if (tmp.count() > 5) {
                            fileEntryT *newFile = new fileEntryT();
                            newFile->fileOut = false;
                            newFile->fileId = tmp.at(0).toUInt();
                            newFile->info.fileName = tmp.at(1);
                            newFile->info.size = tmp.at(2).toUInt(nullptr, 16);
                            newFile->info.metadataChangeTime = tmp.at(3).toUInt(nullptr, 16);
                            newFile->info.permission = tmp.at(4).toUInt(nullptr, 16);
                            newFile->fileHost = mUsers.at(i)->userHostAddr.toIPv4Address();
                            newFile->fileTranStatus = FILE_TRANS_STATUS_IDLE;
                            if (newFile->info.permission != IPMSG_FILE_REGULAR) {
                                qDebug() << "Not support file type" << newFile->info.permission;
                                mUsers.at(i)->appendChatHistory(toUnicode(
                                    mUsers.at(i)->userNickName + " "
                                    + QDateTime::currentDateTime().toString("hh:mm:ss").toUtf8()
                                    + "\n"));
                                mUsers.at(i)->appendChatHistory(tr("Do not support directory")
                                                                + "\"" + toUnicode(tmp.at(1)) + "\""
                                                                + "\n");
                                mUsers.at(i)->updateChatFormHistory();
                                delete newFile;
                                continue;
                            }
                            // mUsers.at(i)->chatForm->addRemoteShareFile(newFile);
                            qDebug() << "Add11111" << tmp.at(1);
                        }
                    }
                }
            }
        }
        //接受信息，
        if (commandOpt & IPMSG_SENDCHECKOPT) {
            if (i != mUsers.count()) {
                session.ipMsgRespOK(&mSelf, mUsers.at(i));
                // mUsers.at(i)->chatForm->setWindowTitle(toUnicode(mUsers.at(i)->userNickName));
                if (!(IPMSG_FILEATTACHOPT & commandOpt)) {
                    mUsers.at(i)->appendChatHistory(toUnicode(
                        mUsers.at(i)->userNickName + " "
                        + QDateTime::currentDateTime().toString("hh:mm:ss").toUtf8() + ":"));

                    mUsers.at(i)->appendChatHistory(toUnicode(chatData.split(0).at(0) + "\n"));
                    mUsers.at(i)->updateChatFormHistory();
                }

            }
        }

    }

    else if (IPMSG_RECVMSG == command) {
    mSelf.userPktSeq = values.at(1).toUInt();
    for (i = 0; i < mUsers.count(); i++) {
        if (mUsers.at(i)->userHostAddr == src) {
            if (mUsers.at(i)->offlineTimer != 0) {
                killTimer(mUsers.at(i)->offlineTimer);
                mUsers.at(i)->offlineTimer = 0;
            }
            mUsers.at(i)->userCfmSeq = chatData.split(0).at(0).toUInt();
            //qDebug()<<"Got cfm "<<mUsers.at(i)->userCfmSeq<<"from"<<src.toString();
            break;
        }
    }
}
}


//用户信息刷新函数
void Messenger::fresh()
{

    mUserInfo.clear();
    mUsers.clear();
    if (true == session.IpMsgUdpSessionValid()) {
        session.ipMsgUserRefresh(&mSelf, mStartIp, mEndIp);
    }
    setUserInfo();
}

void Messenger::setUserInfo()
{
    QList<QList<QString>> userProperties;

    for (const auto &user : mUsers) {
        QList<QString> properties;
        properties.append(user->userId);
        properties.append(user->userHostAddr.toString());
        userProperties.append(properties);
    }

    mUserInfo = userProperties;
    emit userInfoChanged();
}

QList<QList<QString>> Messenger::userInfo()
{
    return mUserInfo;
}


// 发送信息/文件
void Messenger::ipMsgChatSent(QString data,QHostAddress des)
{
    QHostAddress dest(des);
    QList<fileEntryT *> files;
    for (int i = 0; i < mUsers.count(); i++) {
        if (dest == mUsers.at(i)->userHostAddr) {
            if (data.length() > 0) {

                mUsers.at(i)->userPktSeq = g_pkg_seq++;
                session.ipMsgSendData(&mSelf, dest, fromUnicode(data));
                //Update Chat history in Chat form.
                mUsers.at(i)->appendChatHistory(
                    toUnicode(mSelf.userNickName + " "
                             + QDateTime::currentDateTime().toString("hh:mm:ss").toUtf8() + ":"));
                mUsers.at(i)->appendChatHistory(data);
                mUsers.at(i)->appendChatHistory("\n");
                mUsers.at(i)->updateChatFormHistory();
                // mUsers.at(i)->chatForm->getChat()->clear();
                mUsers.at(i)->offlineTimer = startTimer(200);
            }

            if (mUsers.at(i)->chatForm->fileList.count() > 0) {

                for (int j = 0; j < mUsers.at(i)->chatForm->fileList.length(); j++) {

                    if (mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus
                            == FILE_TRANS_STATUS_IDLE
                        && mUsers.at(i)->chatForm->fileList.at(j)->fileOut == true) {
                        files.append(mUsers.at(i)->chatForm->fileList.at(j));

                        mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus
                            = FILE_TRANS_STATUS_QUEUE;
                        mSelf.fileQueue.append(mUsers.at(i)->chatForm->fileList.at(j));
                        qDebug() << "add new file to queue:"
                                 << mUsers.at(i)->chatForm->fileList.at(j)->info.fileName;

                    }
                }
                if (files.count() > 0)
                    session.ipMsgSendFiles(&mSelf, mUsers.at(i)->userHostAddr, files);

                for (int j = 0; j < mUsers.at(i)->chatForm->fileList.length(); j++) {

                    qDebug()<<mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus;
                    if (mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus
                            == FILE_TRANS_STATUS_QUEUE
                        && mUsers.at(i)->chatForm->fileList.at(j)->fileOut == true) {

                        this->ipMsgAcceptFile(mUsers.at(i)->chatForm->fileList.at(j));}}

            }
            break;
        }

    }

}

QString Messenger::chatmess() const
{
    return m_chatmess;
}

void Messenger::setChatmess(const QString &newChatmess)
{
    if (m_chatmess == newChatmess)
        return;

    m_chatmess = newChatmess;
    emit chatmessChanged();
}

void Messenger::receiveMessage(const QString &message)
{
    setChatmess(message);
}

void Messenger::ipMsgFileServerHandle(quint32 ip, quint16 port, QByteArray data)
{
    // qDebug()<<"IpMsgFileServerHandle";
    uint32_t reqFileId = 0;
    uint32_t command;

    QHostAddress addr;

    QByteArray dataSample
        = "1_lbt80_0#160#FC4DD4D98024#0#0#0#4000#9:1546091332:caoweigang:CAOWEIGANG:96:5:2710:0:";
    QList<QByteArray> value = data.split(':');

    addr.setAddress(ip);
    // qDebug() << "Handle data" << data << "From" << addr.toString() << "Port:" << port;
    if (value.count() > 8) {
        command = value.at(4).toUInt(nullptr, 10) & 0xff;
        reqFileId = value.at(6).toUInt(nullptr, 16);

        // qDebug() << "TCP Command:" << command << "File Id:" << reqFileId;

        if (command != IPMSG_GETFILEDATA) {
            return;
        }
    } else {
        qDebug() << "Error" << __FUNCTION__ << __LINE__;
        return;
    }

    for (int i = 0; i < mSelf.fileQueue.size(); i++) {
        // qDebug()<<reqFileId;
        // qDebug()<<mSelf.fileQueue.at(i)->fileId;
        // qDebug()<<ip;

        // qDebug()<<mSelf.fileQueue.at(i)->fileHost;
        // qDebug()<<mSelf.fileQueue.at(i)->fileTranStatus;
        // qDebug()<<mSelf.fileQueue.at(i)->fileOut;


        if (reqFileId == mSelf.fileQueue.at(i)->fileId && ip == mSelf.fileQueue.at(i)->fileHost
            && mSelf.fileQueue.at(i)->fileTranStatus == FILE_TRANS_STATUS_RUN
            && mSelf.fileQueue.at(i)->fileOut == true) {
            qDebug()<<"helloword";
            mSelf.fileQueue.at(i)->filePort = port;
            mSelf.fileQueue.at(i)->fileTranStatus = FILE_TRANS_STATUS_RUN;
            {
                qDebug()<<"helloword";
                FileTransWorker *trans = new FileTransWorker();
                trans->mFilename = mSelf.fileQueue.at(i)->info.absoluteFilePath;
                trans->mFileId = mSelf.fileQueue.at(i)->fileId;
                trans->mHandle = (fileServer->fileServerSockMatch(mSelf.fileQueue.at(i)->fileHost,
                                                                  mSelf.fileQueue.at(i)->filePort))
                                     ->socketDescriptor();

                // qDebug()<<"Got File"<<trans->mFilename<<"Sock"<<trans->mSock->peerAddress().toString()<<trans->mSock->peerPort();

                connect(trans,
                        SIGNAL(fileTransProgress(quint32, int)),
                        this,
                        SLOT(ipMsgFileTransProgress(quint32, int)));
                connect(trans,
                        SIGNAL(fileTransFinished(quint32)),
                        this,
                        SLOT(ipMsgFileTransFinished(quint32)));
                connect(trans,
                        SIGNAL(fileTransError(quint32, int)),
                        this,
                        SLOT(ipMsgFileTransError(quint32, int)));
                QThread *thread = new QThread();
                trans->moveToThread(thread);
                connect(thread, SIGNAL(started()), trans, SLOT(fileTransStart()));
                //connect(trans, SIGNAL(fileTransFinished()), thread, SLOT(quit()));
                connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
                connect(thread, SIGNAL(finished()), trans, SLOT(deleteLater()));
                thread->start();
            }
            mSelf.fileQueue.removeAt(i);
            break;
        }
    }
}


void Messenger::ipMsgFileTransProgress(quint32 fileId, int progress)
{
    qDebug()<<"File ID"<<fileId<<"Progress"<<progress;
    //qDebug()<<"SendFile SLOT:"<<QThread::currentThreadId();
    for (int i = 0; i < mUsers.length(); i++) {
        for (int j = 0; j < fileList.count(); j++) {
            if (fileList.at(j)->fileId == fileId
                && fileList.at(j)->fileOut == true) {
                // mUsers.at(i)->chatForm->updateFileProgress(j, progress);
                return;
            }
        }


    }
}

void Messenger::ipMsgFileTransError(quint32 fileId, int progress)
{
    qDebug() <<"ipMsgFileTransError";
    for (int i = 0; i < mUsers.length(); i++) {
        if (mUsers.at(i)->chatForm != nullptr) {
            for (int j = 0; j < fileList.count(); j++) {
                if (fileList.at(j)->fileId == fileId
                    && fileList.at(j)->fileOut == true) {
                    mUsers.at(i)->appendChatHistory(toUnicode(
                        mSelf.userNickName + " "
                        + QDateTime::currentDateTime().toString("hh:mm:ss").toUtf8() + ":"));
                    mUsers.at(i)->appendChatHistory(
                        tr("Send") + " \""
                        + fileList.at(j)->info.absoluteFilePath + "\" "
                        + tr("error"));
                    mUsers.at(i)->appendChatHistory("\n");
                    mUsers.at(i)->updateChatFormHistory();
            // mUsers.at(i)->chatForm->updateFileError(j, progress);
            // mUsers.at(i)->chatForm->delFixedShareFile(j);
                    return;
        }
            }}

    }


}

void Messenger::ipMsgFileTransFinished(quint32 fileId)
{
    // int i = 0;
    qDebug() << "File ID" << fileId << "trans finished";

    for (int i = 0; i < mUsers.length(); i++) {
        for (int j = 0; j < fileList.length(); j++) {
            if (fileList.at(j)->fileId == fileId
                && fileList.at(j)->fileOut == true) {
                mUsers.at(i)->appendChatHistory(toUnicode(
                    mSelf.userNickName + " "
                    + QDateTime::currentDateTime().toString("hh:mm:ss").toUtf8() + "\n"));
                mUsers.at(i)->appendChatHistory(
                    tr("Send") + " \""
                    + fileList.at(j)->info.absoluteFilePath + "\" "
                    + tr("finished"));
                mUsers.at(i)->appendChatHistory("\n");
                mUsers.at(i)->updateChatFormHistory();
                fileList.at(j)->fileTranStatus
                    = FILE_TRANS_STATUS_FINISHED;
                // chatForm->delFixedShareFile(j);
                return;
            }
        }


    }
}

void Messenger::ipMsgRecvFileFinish(quint32 fileId)
{
    FormChat *form = qobject_cast<FormChat *>(sender());
    IpMsgUser *user = nullptr;
    int j = 0;
    int i = 0;
    if (form != nullptr) {
        for (j = 0; j < mUsers.length(); j++) {
            if (form == mUsers.at(j)->chatForm) {
                user = mUsers.at(j);
                break;
            }
        }
        if (j == mUsers.length())
            return;

        for (i = 0; i < fileList.length(); i++) {
            if (fileList.at(i)->fileId == fileId && fileList.at(i)->fileOut == false
                && fileList.at(i)->fileTranStatus == FILE_TRANS_STATUS_FINISHED) {
                user->appendChatHistory(
                    toUnicode(user->userNickName + " "
                              + QDateTime::currentDateTime().toString("hh:mm:ss").toUtf8() + "\n"));
                user->appendChatHistory(tr("Receive") + " \""
                                        + fileList.at(i)->info.absoluteFilePath + "\" "
                                        + tr("finished"));
                user->appendChatHistory("\n");
                user->updateChatFormHistory();
                fileList.removeAt(i);
                break;
            }
        }
    }
}

void Messenger::ipMsgRecvFileError(quint32 fileId)
{
    FormChat *form = qobject_cast<FormChat *>(sender());
    IpMsgUser *user = nullptr;
    int j = 0;
    int i = 0;
    if (form != nullptr) {
        for (j = 0; j < mUsers.length(); j++) {
            user = mUsers.at(j);
        }
        if (j == mUsers.length())
            return;

        for (i = 0; i <fileList.length(); i++) {
            if (fileList[i]->fileId == fileId && fileList[i]->fileOut == false
                && fileList[i]->fileTranStatus == FILE_TRANS_STATUS_FINISHED) {
                user->appendChatHistory(
                    toUnicode(user->userNickName + " "
                              + QDateTime::currentDateTime().toString("hh:mm:ss").toUtf8() + "\n"));
                user->appendChatHistory(tr("Receive") + " \""
                                        + fileList[i]->info.absoluteFilePath + "\" "
                                        + tr("error"));
                user->appendChatHistory("\n");
                user->updateChatFormHistory();
                fileList.remove(i);
                break;
            }
        }
    }
}

void Messenger::ipMsgAcceptFile(fileEntryT *file)
{
    qDebug() << "Accept file" << file->info.fileName;
    for (int i = 0; i < mUsers.count(); i++) {


        if (mUsers.at(i)->userHostAddr.toIPv4Address() == file->fileHost) {
            for (int j = 0; j < mUsers.at(i)->chatForm->fileList.count(); j++) {

                if (mUsers.at(i)->chatForm->fileList.at(j)->fileId == file->fileId
                        && mUsers.at(i)->chatForm->fileList.at(j)->fileOut == true
                    && mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus
                           == FILE_TRANS_STATUS_QUEUE) {

                    mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus = FILE_TRANS_STATUS_QUEUE;
                    mUsers.at(i)->fileQueue.append(mUsers.at(i)->chatForm->fileList.at(j));
#if 1
                    mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus = FILE_TRANS_STATUS_RUN;
                    IpMsgFileRecv *recv = new IpMsgFileRecv(&mSelf, file);
                    connect(recv->client,
                            SIGNAL(ipMsgFileClientErrorSig(quint32, int)),
                            mUsers.at(i)->chatForm,
                            SLOT(fileRecvError(quint32, int)));
                    connect(recv->client,
                            SIGNAL(ipMsgFileClientProgressSig(quint32, int)),
                            mUsers.at(i)->chatForm,
                            SLOT(fileRecvProgress(quint32, int)));
                    connect(recv->client,
                            SIGNAL(ipMsgFileClientFinishSig(quint32)),
                            mUsers.at(i)->chatForm,
                            SLOT(fileRecvFinished(quint32)));
                    connect(mUsers.at(i)->chatForm,
                            SIGNAL(cancelFile(fileEntryT *)),
                            recv,
                            SLOT(ipMsgFileClientCancel(fileEntryT *)));
#endif
                    break;

                }
            }
            break;
        }
    }
}

void Messenger::ipMsgRejectFile(fileEntryT *file)
{
    qDebug() << "Reject file" << file->info.fileName;

    for (int i = 0; i < mUsers.length(); i++) {
        if (100315 == file->fileHost) {
            for (int j = 0; j < fileList.length(); j++) {
                if (fileList[j]->fileId == file->fileId
                    && fileList[j]->fileOut == false
                    && fileList[j]->fileTranStatus
                           == FILE_TRANS_STATUS_IDLE) {
                   /* mUsers.at(i)->chatForm->delFixedRemoteShareFile(j)*/;
                    break;
                }
            }
        }
    }
}
