#ifndef IPMSGTCPSESSION_H
#define IPMSGTCPSESSION_H

#include <QDataStream>
#include <QHostAddress>
// #include <QMainWindow>
#include <QNetworkInterface>
#include <QObject>
#include <QTcpServer>
#include <QWidget>

class IpMsgTcpSession : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgTcpSession(QObject *parent = nullptr);

signals:

public slots:

private:
    //Incoming. Outgoing
    qint32 tcpDir;
    QHostAddress destHost;
    uint16_t destPort;
    QByteArray fileName;
    QByteArray destFileName;
    //Connecting, Connected,CloseByRemote,Error,Disconnected
    qint32 sessionStatus;
    //Wait,Receiving,Sending
    qint32 dataStatus;
    //0 - 100
    qint32 filePersent;
    QTcpServer tcpServer;
};

#endif // IPMSGTCPSESSION_H
