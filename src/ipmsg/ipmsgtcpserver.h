#ifndef IPMSGTCPSERVER_H
#define IPMSGTCPSERVER_H

#include <QObject>
#include <QTcpServer>

#include<QTcpSocket>

class IpMsgTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    IpMsgTcpServer();

public slots:
    void sendMsgToAllUsers(const QByteArray &msg);//向所有已连接的客户端发送消息。

protected:
    //void incomingConnection(qintptr handle);
    void incomingConnection(qintptr socketDescriptor) override;
private:
    QList<QTcpSocket*> mClients;
};

#endif // IPMSGTCPSERVER_H
