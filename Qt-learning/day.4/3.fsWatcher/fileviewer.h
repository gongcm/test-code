#ifndef FILEVIEWER_H
#define FILEVIEWER_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QFileInfoList>
#include <QFileSystemWatcher>

class FileViewer : public QWidget		// file information dialogs widget
{
    Q_OBJECT
public:
    FileViewer( QWidget *parent=0, Qt::WindowFlags  f=0 );
    ~FileViewer();

protected:
    void showFileInfo(QDir dir);
    void showFileInfoList(QFileInfoList list);

protected slots:
    void slotShowDir(QListWidgetItem * item);
    void slotGetfileInfoList();
    void slotDirChanged();

private:
    QLineEdit* LineEditDir;
    QListWidget* ListWidgetFile;
    QFileSystemWatcher *watcher;
    QString rootpath;
};


#endif // FILEVIEWER_H
