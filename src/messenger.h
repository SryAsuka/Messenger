#pragma once
#include"QByteArray"
#include <QList>
#include <QObject>
#include <QQmlEngine>
#include "ipmsg/ipmsgfilerecv.h"
#include "ipmsg/ipmsgfileserver.h"
#include "ipmsg/ipmsgudpsession.h"
#include "ipmsg/ipmsguser.h"
#include"ipmsg/ipmsgtcpserver.h"
#include"ipmsg/ipmsgfileclient.h"
#include"ipmsg/ipmsgtcpsession.h"
#include"formchat.h"

class Messenger : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<QList<QString>> userInfo MEMBER mUserInfo READ userInfo NOTIFY userInfoChanged)
    Q_PROPERTY(QString chatmess READ chatmess WRITE setChatmess NOTIFY chatmessChanged)
public:
    explicit Messenger(QObject *parent = nullptr);
    ~Messenger();
    IpMsgUser mSelf;
    FormChat formChat;
    QString toUnicode(QByteArray data);
    QByteArray fromUnicode(QString data);

    QString chatmess() const;
    void setChatmess(const QString &newChatmess);

    QList<fileEntryT *> fileList;

public slots:

    void UdpSessionHandle(QHostAddress src, QByteArray data);
    void ipMsgFileServerHandle(quint32 ip, quint16 port, QByteArray data);
    void startChat(int clickedIndex);

    Q_INVOKABLE void fresh();//用户信息刷新
    Q_INVOKABLE void setUserInfo();//用户信息设置
    Q_INVOKABLE QList<QList<QString>> userInfo();//用户信息获取

    void ipMsgChatSent(QString data,QHostAddress des);
    void receiveMessage(const QString &message);

private slots:
    void ipMsgFileTransProgress(quint32 fileId, int progress);
    void ipMsgFileTransFinished(quint32 fileId);
    void ipMsgFileTransError(quint32 fileId, int progress);
    void ipMsgAcceptFile(fileEntryT *file);
    void ipMsgRejectFile(fileEntryT *file);
    void ipMsgRecvFileFinish(quint32 fileId);
    void ipMsgRecvFileError(quint32 fileId);

Q_SIGNALS:
    void userInfoChanged();
    void chatmessChanged();
    void ipMsgFileClientErrorSig(quint32 fileid, int progress);
    void ipMsgFileClientProgressSig(quint32 fileid, int progress);
    void ipMsgFileClientFinishSig(quint32 fileid);



private:
    QList<IpMsgUser *> mUsers;
    IpMsgUdpSession session;
    IpMsgFileServer *fileServer;
    QString mCodec = "GBK";
    unsigned int mStartIp = 0;
    unsigned int mEndIp = 0;
    QList<QList<QString>> mUserInfo;

    QString m_chatmess;



};
