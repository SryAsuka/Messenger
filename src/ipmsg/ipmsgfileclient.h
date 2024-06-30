#ifndef IPMSGFILECLIENT_H
#define IPMSGFILECLIENT_H

#include <QFileInfo>
#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include "ipmsgcommon.h"
#include "ipmsguser.h"
//文件传输的不同状态
#define FILE_TRANS_STATUS_IDLE 0
#define FILE_TRANS_STATUS_QUEUE 1
#define FILE_TRANS_STATUS_PRERUN 2
#define FILE_TRANS_STATUS_RUN 3
#define FILE_TRANS_STATUS_FINISHED 4
//用户信息
typedef struct userInfoT
{
    QByteArray userVer;
    QByteArray userId;
    QByteArray userNickName;
    QByteArray userGroupName;
    QByteArray userHostName;
    QByteArray userEmail;
    QByteArray userIcon;
    QHostAddress userHostAddr;//主机地址
    uint32_t userPktSeq;//数据包序列号
    uint32_t userCfmSeq;//确认序列号
} userInfoT;
//定义文件传输客户端类
class IpMsgFileClient : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgFileClient(IpMsgUser *user, QObject *parent = nullptr);
    fileEntryT recvFileInfo;//接收文件信息
    QTcpSocket *sock;//tcp套接子
    int mProgress;//文件传输进度
    userInfoT me;//当前用户信息
signals:
    void ipMsgFileClientErrorSig(quint32 fileid, int progress);
    void ipMsgFileClientProgressSig(quint32 fileid, int progress);
    void ipMsgFileClientFinishSig(quint32 fileid);

public slots:

private slots:
    void ipMsgFileClientStart();
    void ipMsgFileClientConnected();
    void ipMsgFileClientError(QAbstractSocket::SocketError error);
    void ipMsgFileClientDisconnected();
    void ipMsgFileClientRecv();
    void ipMsgFileClientClose();

private:
    QFile mFile;//写入接收文件数据
    qint64 mSize;//接受文件大小
};

#endif // IPMSGFILECLIENT_H
