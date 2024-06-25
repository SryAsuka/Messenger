#include "messenger.h"
#include <QMessageBox>
#include <QString>
#include <QTextCodec>
#include <QTimer>
#include <unistd.h>

extern quint32 g_pkg_seq;

Messenger::Messenger(QObject *parent)
    : QObject{parent}
{
    mSelf.userId = fromUnicode(QString(QHostInfo::localHostName()));
    mSelf.userVer = QString(IPMSG_VERSION).toUtf8();
    mSelf.userPktSeq = 0;
    mSelf.userHostName = QHostInfo::localHostName().toUtf8();
    mSelf.userNickName = fromUnicode(QString(QHostInfo::localHostName()));
    mSelf.userGroupName = fromUnicode(QString(tr(IPMSG_DEFAULT_GROUPNAME)));

    mStartIp = QHostAddress("10.253.212.1").toIPv4Address();
    mEndIp = QHostAddress("10.253.212.245").toIPv4Address();

    connect(&session,
            SIGNAL(ipMsgUdpSessionDataReady(QHostAddress, QByteArray)),
            this,
            SLOT(UdpSessionHandle(QHostAddress, QByteArray)));

    if (session.mUdpBind != true) {
        qDebug() << "error udp";
        exit(1);
    }

    if (true == session.IpMsgUdpSessionValid()) {
        session.ipMsgUserRefresh(&mSelf, mStartIp, mEndIp);
    }
}

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

void Messenger::UdpSessionHandle(QHostAddress src, QByteArray data)
{
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

    for (i = 5; i < values.count(); i++) {
        chatData.append(values[i]);
        chatData.append(':');
    }

    pktCommand = values.at(4).toUInt();

    if (QString::number(pktCommand, 10) != values.at(4)) {
        qDebug() << "Invalid command, ignore it";
        return;
    }

    command = pktCommand & 0x000000FF;
    commandOpt = pktCommand & 0xffffff00;

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

        if (IPMSG_BR_ENTRY == command) {
            session.ipMsgRespBr(&mSelf, mUsers.at(i));
        }
        setUserInfo();
    }
}

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
