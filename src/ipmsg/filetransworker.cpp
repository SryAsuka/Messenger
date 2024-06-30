#include "filetransworker.h"
#include <QFile>
#include <QFileInfo>
#include <QHostAddress>
#include <QThread>
FileTransWorker::FileTransWorker(QObject *parent)//构造函数
    : QObject(parent)
{}
void FileTransWorker::fileTransStart()
{
    QByteArray data;//用于存储从文件读取的数据
    qint64 ret = 0;//用于存储 ​QTcpSocket::write()​ 的返回值。
    int wr_len = 0;//用于跟踪已写入的字节数。
    QFile file;//用于打开和读取文件。
    qint64 fileSend = 0;//已发送的文件大小。
    qint64 fileSize = 0;//文件的总大小。

    QTcpSocket *mSock = new QTcpSocket();

    mSock->setSocketDescriptor(mHandle);//设置套接字的描述符
    qDebug() << "mHandle value:" << mHandle;


    qDebug() << "Socket state:" << mSock->state();
    qDebug() << "Socket error:" << mSock->errorString();

    qDebug() << "will trans File" << mFilename << "Sock" << mHandle
             << mSock->peerAddress().toString() << mSock->peerPort();
    qDebug()<<mSock<<mSock->isValid()<<mFileTranStopFlag<<QFileInfo(mFilename).isFile();
    if (nullptr != mSock && mSock->isValid() && false == mFileTranStopFlag//检查msock是否有效，文件是否存在，文件停止标志是否为false
        && true == QFileInfo(mFilename).isFile()) {
        //qDebug()<<"File"<<mFilename<<"Sock"<<mSock->peerAddress().toString()<<mSock->peerPort();

        file.setFileName(mFilename);
        fileSize = file.size();
        //进入循环，直到文件传输停止标志为真、文件读取完毕或套接字无效。
        if (file.open(QFile::ReadOnly)) {
            while (false == mFileTranStopFlag && false == file.atEnd() && mSock->isValid()) {
                data.clear();
                data.resize(0);
                data = file.read(8192);
                //如果读取的数据长度大于 0，进入内部循环，直到所有数据都被写入套接字。
                if (data.length() > 0) {
                    wr_len = 0;
                    while (wr_len != data.length()) {
                        ret = mSock->write(data.right(data.length() - wr_len),
                                           data.length() - wr_len);
                        if (-1 != ret) {
                            wr_len += ret;
                            fileSend += ret;
                            if ((fileSend * 100 / fileSize) != mProgress) {
                                mProgress = (fileSend * 100 / fileSize);
                                emit fileTransProgress(mFileId, mProgress);
                            }
                            mSock->waitForBytesWritten();
                            continue;
                        } else {
                            break;
                        }
                    }
                    if (-1 == ret) {
                        emit fileTransError(mFileId, mProgress);
                        break;
                    }
                } else {
                    emit fileTransError(mFileId, mProgress);
                    break;
                }
            }
            file.close();
            emit fileTransFinished(mFileId);
        }
    } else {
        emit fileTransError(mFileId, mProgress);
    }
    if (mSock->state() == QTcpSocket::ConnectedState) {
        mSock->disconnectFromHost();
    }

    if (mSock->state() != QTcpSocket::UnconnectedState) {
        mSock->waitForDisconnected();
    }
    mSock->close();
    delete mSock;//
    return;
}
