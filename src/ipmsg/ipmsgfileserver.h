#ifndef IPMSGFILESERVER_H
#define IPMSGFILESERVER_H

#include <QFileInfo>
#include <QHostAddress>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "ipmsgcommon.h"

class IpMsgFileServer : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgFileServer(QObject *parent = nullptr);
    QTcpSocket *fileServerSockMatch(quint32 host, quint16 port);
    bool mFileServerListen = false;

signals:
    void ipMsgFileServerDataReady(quint32 ip, quint16 port, QByteArray data);
    void ipMsgFileServerSendFileFinished(quint32 fileId);

    void fileSeverFilerTransProgress(quint32 fileId, int val); // 进度信号，[0, 100],指示进度
    void fileServerFileTransFinished(quint32 fileId);          // 结束信号
    void fileServerTransFinished();

public slots:
    void fileServerNewConnection();//新连接
    void fileServerReadyRead();//数据可读
    void fileServerSockDisconnect();//套接子断开
    void fileServerSockError(QAbstractSocket::SocketError error);//错误

private:
    QTcpServer mServer;//监听传入的tcp连接
    QList<fileEntryT *> fileSendQueue;//是一个列表，用于存储待发送的文件条目
    QList<QTcpSocket *> mSocks;// 是一个列表，用于存储活动的 TCP 套接字
    QThread *mSendThread = nullptr;//用于处理文件发送的线程
};

#endif // IPMSGFILESERVER_H
