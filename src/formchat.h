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


    QString FileSizeConvert(qint64 size);
    QList<fileEntryT *> fileList;
    void updateFileProgress(quint32 fileId, int progress);
    void updateFileError(quint32 fileId, int progress);
    void delFixedShareFile(int index);
    void addRemoteShareFile(fileEntryT *newfile);
    void delFixedRemoteShareFile(int index);
signals:
    void acceptFile(fileEntryT *file);
    void chatHistoryUpdated(QString data);

protected:
    bool event(QEvent *event);
signals:
    void sent(QString data, QHostAddress dest);
    void addSendFile(QString file);
    void delSendFile(int index);
    void delAllSendFile();

    void rejectFile(fileEntryT *file);
    void cancelFile(fileEntryT *file);
    void recvfinish(quint32 fileId);
    void recverror(quint32 fileId);

private:

    QObject *mChatUser;
public slots:
    Q_INVOKABLE void addShareFile(const QStringList &files,QString des);
    void chatSend(QString text,QString des);
private slots:


    void delShareFile();
    void delAllShareFile();
    void acceptShareFile();
    void rejectShareFile();
    void acceptAllShareFile();
    void rejectAllShareFile();

    void fileRecvFinished(quint32 fileId);
    void fileRecvProgress(quint32 fileId, int progress);
    void fileRecvError(quint32 fileId, int progress);
};



#endif // FORMCHAT_H
