#ifndef FORMCHAT_H
#define FORMCHAT_H

#include <QHostAddress>


#include "ipmsg/ipmsgcommon.h"

class FormChat :public QObject
{
    Q_OBJECT
public:
    explicit FormChat(QObject *parent = nullptr);
    ~FormChat();

    Q_INVOKABLE void updateChatHistory(QString data);
    void setUserName(QString value);
    void setHost(QString value);
    void setGroupName(QString value);
    void setClient(QString value);
    QString FileSizeConvert(qint64 size);
    QList<fileEntryT *> fileList;
    void updateFileProgress(quint32 fileId, int progress);
    void updateFileError(quint32 fileId, int progress);
    void delFixedShareFile(int index);
    void addRemoteShareFile(fileEntryT *newfile);
    void delFixedRemoteShareFile(int index);
signals:
    void acceptFile(fileEntryT *file);
    void chatHistoryUpdated();
protected:
    bool event(QEvent *event);
signals:
    void sent(QString data, QHostAddress dest);
    void addSendFile(QString file);
    void delSendFile(int index);
    void delAllSendFile();
    // void acceptFile(fileEntryT *file);
    void rejectFile(fileEntryT *file);
    void cancelFile(fileEntryT *file);
    void recvfinish(quint32 fileId);
    void recverror(quint32 fileId);

private:

    QString mUser;
    QString mHost;
    QString mGroup;
    QString mClient;

    QObject *mChatUser;
public slots:
    void addShareFile(const QStringList &files);
private slots:
    void chatSend();

    void delShareFile();
    void delAllShareFile();
    void acceptShareFile();
    void rejectShareFile();
    void acceptAllShareFile();
    void rejectAllShareFile();
    void on_tableWidgetSendFileList_customContextMenuRequested(const QPoint &pos);
    void on_tableWidgetRecvFileList_customContextMenuRequested(const QPoint &pos);
    void fileRecvFinished(quint32 fileId);
    void fileRecvProgress(quint32 fileId, int progress);
    void fileRecvError(quint32 fileId, int progress);
};



#endif // FORMCHAT_H
