#include "formchat.h"
#include <QFileDialog>
#include <QProgressBar>
#include "messenger.h"
#include<QDebug>



static uint32_t g_send_file_id = 100;

QString FormChat::FileSizeConvert(qint64 size)
{
    if (size > 1024 * 1024 * 1024) {
        return QString::number(size / (1024 * 1024 * 1024.0), 10, 1) + "GB";
    } else if (size > 1024 * 1024) {
        return QString::number(size / (1024 * 1024.0), 10, 1) + "MB";
    } else if (size > 1024) {
        return QString::number(size / (1024.0), 10, 1) + "KB";
    } else
        return QString::number(size) + "B";
}

FormChat::FormChat(QObject *parent)
    : QObject{parent}
{

    QStringList header;

}

FormChat::~FormChat()
{
    qDebug() << "Messenger will close";
}

void FormChat::addShareFile(const QStringList &files,QString des)
{
    fileEntryT *fileNode=nullptr;
    // qDebug()<<files.length();
    for (const QString &file : files) {
        QFileInfo info(file);
        fileNode = new fileEntryT();
        fileNode->fileOut = true;
        fileNode->info.fileName = info.fileName();
        fileNode->info.size = info.size();
        fileNode->info.permission = info.permissions();
        fileNode->info.absoluteFilePath = info.absoluteFilePath();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        fileNode->info.metadataChangeTime = info.metadataChangeTime().toSecsSinceEpoch();
#else
        fileNode->info.metadataChangeTime = finfo.created().toTime_t();
#endif

        fileNode->fileId = g_send_file_id++;
        fileNode->fileHost = QHostAddress(des).toIPv4Address();
        fileNode->fileOffset = 0;
        fileNode->fileTranStatus = FILE_TRANS_STATUS_IDLE;
        fileList.append(fileNode);
        qDebug()<<fileList.length()<<"文件长度";

    }
}

void FormChat::delFixedRemoteShareFile(int index)
{
    // if (index >= 0 && index < ui->tableWidgetRecvFileList->columnCount()) {
    //     fileList.removeAt(index);
    //     ui->tableWidgetRecvFileList->removeRow(index);
    // }
}
void FormChat::delFixedShareFile(int index)
{
    // if (index >= 0 && index < ui->tableWidgetSendFileList->columnCount()) {
    //     //emit delSendFile(ui->tableWidgetSendFileList->currentRow());
    //     fileList.removeAt(index);
    //     ui->tableWidgetSendFileList->removeRow(index);
    // }
}
void FormChat::delShareFile()
{
    // int index = ui->tableWidgetSendFileList->currentRow();
    // delFixedShareFile(index);
}

void FormChat::delAllShareFile()
{
    // //emit delAllSendFile();
    // fileList.clear();
    // ui->tableWidgetSendFileList->clearContents();
    // ui->tableWidgetSendFileList->setRowCount(0);
}

void FormChat::addRemoteShareFile(fileEntryT *newfile)
{

    fileList.append(newfile);
    // emit addSendFile(file);


}

void FormChat::acceptShareFile()
{
    // qDebug() << __FUNCTION__;
    // if (ui->tableWidgetRecvFileList->rowCount() <= 0) {
    //     return;
    // }
    // qDebug() << "Count" << ui->tableWidgetRecvFileList->columnCount();
    // QString dir = QFileDialog::getExistingDirectory(nullptr,
    //                                                 tr("Select a directory to save files"),
    //                                                 QDir::homePath());
    // if (dir.length() == 0) {
    //     dir = QDir::homePath();
    // }

    // int index = ui->tableWidgetRecvFileList->currentRow();
    // if (fileList.at(index)->fileTranStatus == FILE_TRANS_STATUS_IDLE) {
    //     fileList.at(index)->info.absoluteFilePath = dir;
    //     fileList.at(index)->info.absoluteFilePath.append(QDir::separator());
    //     fileList.at(index)->info.absoluteFilePath.append(fileList.at(index)->info.fileName);
        // emit acceptFile(fileList.at(index));
    // }
}

void FormChat::rejectShareFile()
{
}

void FormChat::acceptAllShareFile()
{

}

void FormChat::rejectAllShareFile()
{
}
bool FormChat::event(QEvent *event)
{
    return true;
}



void FormChat::updateChatHistory(QString data)
{
    emit chatHistoryUpdated(data);
}
void FormChat::chatSend(QString text,QString des)
{
    QHostAddress dest(des);
    emit sent(text, dest);
}



void FormChat::updateFileProgress(quint32 fileId, int progress)
{
    QProgressBar *sizeBar = new QProgressBar();
    sizeBar->setFormat(FileSizeConvert(fileList.at(fileId)->info.size));
    sizeBar->setAlignment(Qt::AlignHCenter);
    sizeBar->setTextVisible(true);
    sizeBar->setValue(progress);
}

void FormChat::updateFileError(quint32 fileId, int progress)
{
}

void FormChat::fileRecvError(quint32 fileId, int progress)
{
    for (int i = 0; i < fileList.count(); i++) {
        if (fileList.at(i)->fileId == fileId && fileList.at(i)->fileOut == false)
        //&& fileList.at(i)->fileTranStatus == FILE_TRANS_STATUS_RUN)
        {
            QProgressBar *sizeBar = new QProgressBar();
            sizeBar->setFormat(tr("Error"));
            sizeBar->setAlignment(Qt::AlignHCenter);
            sizeBar->setTextVisible(true);
            sizeBar->setValue(progress);
            fileList.at(i)->fileTranStatus = FILE_TRANS_STATUS_FINISHED;
            emit recverror(fileList.at(i)->fileId);
            break;
        }
    }
}

void FormChat::fileRecvProgress(quint32 fileId, int progress)
{
    //qDebug()<<"RecvFile SLOT:"<<QThread::currentThreadId();
    for (int i = 0; i < fileList.count(); i++) {
        if (fileList.at(i)->fileId == fileId && fileList.at(i)->fileOut == false
            && fileList.at(i)->fileTranStatus == FILE_TRANS_STATUS_RUN) {
            QProgressBar *sizeBar = new QProgressBar();
            sizeBar->setFormat(FileSizeConvert(fileList.at(i)->info.size));
            sizeBar->setTextVisible(true);
            sizeBar->setValue(progress);
            break;
        }
    }
}

void FormChat::fileRecvFinished(quint32 fileId)
{
    for (int i = 0; i < fileList.count(); i++) {
        if (fileList.at(i)->fileId == fileId && fileList.at(i)->fileOut == false
            && fileList.at(i)->fileTranStatus == FILE_TRANS_STATUS_RUN) {
            fileList.at(i)->fileTranStatus = FILE_TRANS_STATUS_FINISHED;
            QProgressBar *sizeBar = new QProgressBar();
            sizeBar->setFormat(FileSizeConvert(fileList.at(i)->info.size));
            sizeBar->setTextVisible(true);
            sizeBar->setValue(100);
            fileList.at(i)->fileTranStatus = FILE_TRANS_STATUS_FINISHED;
            emit recvfinish(fileList.at(i)->fileId);
            break;
        }
    }
}
