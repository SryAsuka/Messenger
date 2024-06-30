#ifndef IPMSGCOMMON_H
#define IPMSGCOMMON_H

#include <QCoreApplication>
#include <QDir>

#define IPMSG_VERSION "1_QIPMSGV01"

#define IPMSG_DEFAULT_USERNAME "QIPMSG.U"
#define IPMSG_DEFAULT_NICKNAME "QIPMSG.N"
#define IPMSG_DEFAULT_GROUPNAME "QIPMSG"

typedef struct fileEntryInfoT
{
    qint64 size;//大小
    quint32 permission;//权限
    QString fileName;//名称
    QString absoluteFilePath;//路径
    quint32 fileType;//类型
    quint32 metadataChangeTime;//修改时间
} fileEntryInfoT;

//QFileInfo a;

typedef struct fileEntryT
{
    bool fileOut;//输出
    quint32 fileId;//id
    fileEntryInfoT info;//文件条目详细信息
    quint64 fileOffset;//偏移量？
    //If recv file, fileHost is source
    //If send file, fileHost is dest
    quint32 fileHost;//主机地址
    quint16 filePort;//端口号
    //IDLE,SENDING,FINISHED. -- send file
    //IDLE,RECEIVING,FINISHED -- recv file
    quint32 fileTranStatus;//传输状态
    bool fileTranStopFlag;//是否停止
} fileEntryT;
#endif // IPMSGCOMMON_H
