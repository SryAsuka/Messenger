#ifndef IPMSGFILERECV_H
#define IPMSGFILERECV_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include "ipmsgcommon.h"
#include "ipmsgfileclient.h"

class IpMsgFileRecv : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgFileRecv(IpMsgUser *user, fileEntryT *file, QObject *parent = nullptr);
    IpMsgFileClient *client = nullptr;
signals:
    void fileRecvCancel();

public slots:
    void ipMsgFileClientCancel(fileEntryT *file);//用于处理文件接收取消的逻辑。
};

#endif // IPMSGFILERECV_H
