#include "ipmsgtcpserver.h"
#include<QDebug>

IpMsgTcpServer::IpMsgTcpServer() {}

void IpMsgTcpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket(this);
    if (clientSocket->setSocketDescriptor(socketDescriptor)) {
        mClients.append(clientSocket);
    } else {
        delete clientSocket;
    }
}

void IpMsgTcpServer::sendMsgToAllUsers(const QByteArray &msg)
{
    for (QTcpSocket *client : mClients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->write(msg);
        }
    }
}
