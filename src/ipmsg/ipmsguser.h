#ifndef IPMSGUSER_H
#define IPMSGUSER_H

#include <QHostAddress>
#include <QHostInfo>
// #include <QMainWindow>
#include <QObject>
#include <QWidget>
#include "formchat.h"
#include "ipmsgcommon.h"

class IpMsgUser : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgUser(QObject *parent = nullptr);
    ~IpMsgUser();
    void dumpUserInfo();//打印用户信息
    void appendChatHistory(QString data);//向聊天历史中添加信息
    void updateChatFormHistory();//更新聊天表单的历史记录

    QList<fileEntryT *> fileQueue;
    int fileSendFlag;
    QByteArray userVer;
    QByteArray userId;
    QByteArray userNickName;
    QByteArray userGroupName;
    QByteArray userHostName;
    QByteArray userEmail;
    QByteArray userIcon;
    QHostAddress userHostAddr;
    uint32_t userPktSeq;
    uint32_t userCfmSeq;
    FormChat *chatForm = nullptr;
    QString chatHistory;
    QByteArray chatBuffer;
    int offlineTimer = 0;
    int blinkTimer = 0;
    int blinkFlag = 0;

signals:
    void sendFileUpdate();//当文件发送状态更新时发出的信号。

private slots:
};

#endif // IPMSGUSER_H
