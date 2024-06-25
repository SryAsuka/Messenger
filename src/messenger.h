#pragma once

#include <QList>
#include <QObject>
#include <QQmlEngine>
#include "ipmsg/ipmsgfilerecv.h"
#include "ipmsg/ipmsgfileserver.h"
#include "ipmsg/ipmsgudpsession.h"
#include "ipmsg/ipmsguser.h"

class Messenger : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<QList<QString>> userInfo MEMBER mUserInfo READ userInfo NOTIFY userInfoChanged)
public:
    explicit Messenger(QObject *parent = nullptr);
    ~Messenger();
    IpMsgUser mSelf;
    QString toUnicode(QByteArray data);
    QByteArray fromUnicode(QString data);

public slots:
    void UdpSessionHandle(QHostAddress src, QByteArray data);
    Q_INVOKABLE void fresh();

    Q_INVOKABLE void setUserInfo();
    Q_INVOKABLE QList<QList<QString>> userInfo();

Q_SIGNALS:
    void userInfoChanged();

private:
    QList<IpMsgUser *> mUsers;
    IpMsgUdpSession session;
    IpMsgFileServer *fileServer;
    QString mCodec = "GBK";
    unsigned int mStartIp = 0;
    unsigned int mEndIp = 0;

    QList<QList<QString>> mUserInfo;
};
