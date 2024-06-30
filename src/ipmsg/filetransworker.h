#ifndef FILETRANSWORKER_H
#define FILETRANSWORKER_H

#include <QObject>
#include <QTcpSocket>
//文件处理工作
class FileTransWorker : public QObject
{
    Q_OBJECT
public:
    explicit FileTransWorker(QObject *parent = nullptr);
    qintptr mHandle;//套接子描述符，标识tcp连接
    QString mFilename;
    uint32_t mFileId;
    bool mFileTranStopFlag = false;//文件停止标志
    int mProgress = 0;//文件传输进度

signals://信号
    void fileTransProgress(quint32 id, int val);
    void fileTransFinished(quint32 id);//文件传输完成后，发送文件id，val是进度
    void fileTransError(quint32 id, int val);
public slots://槽函数
    void fileTransStart();
};

#endif // FILETRANSWORKER_H
