#include "fileviewer.h"
#include <QVBoxLayout>
#include <QString>
#include <QFileInfoList>
#include <QIcon>
#include <QFileIconProvider>
#include <QDir>
#include <QFileInfoList>
#include <QDateTime>

#include <qDebug>

FileViewer::FileViewer( QWidget *parent, Qt::WindowFlags  f )
    : QWidget( parent, f )
{  
    setWindowTitle(tr("File Viewer"));

    //路径显示行编辑框
    LineEditDir = new QLineEdit();

    //文件目录列表框
    ListWidgetFile = new QListWidget();
    //connect(ListWidgetFile,SIGNAL(itemPressed(QListWidgetItem * )),this,SLOT(slotShowDir(QListWidgetItem *)));
    connect(ListWidgetFile,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(slotShowDir(QListWidgetItem *)));

    QVBoxLayout *vbLayout = new QVBoxLayout( this );
    vbLayout->addWidget( LineEditDir );
    vbLayout->addWidget( ListWidgetFile );
    this->setLayout(vbLayout);

    this->setMinimumSize(640,480);

    //得到当前路径
    rootpath = QDir::currentPath();
    QDir rootDir(rootpath);

    LineEditDir->setText(rootpath);
    connect(LineEditDir, SIGNAL(returnPressed()), this, SLOT(slotGetfileInfoList()));

    watcher = new QFileSystemWatcher(this);
    connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(slotDirChanged()));
    watcher->addPath(rootpath);

    QStringList string;
    string << "*" ;
    QFileInfoList fileinfolist = rootDir.entryInfoList (string);
    showFileInfoList(fileinfolist);
}

FileViewer::~FileViewer()
{

}

//显示文件信息列表
void FileViewer::showFileInfoList(QFileInfoList list)
{
    ListWidgetFile->clear();
    for(int i=0;i<list.count();i++)
    {
        QFileInfo info=list.at(i);
        QFileIconProvider provider;
        QIcon icon = provider.icon(info);
        QString fileName = info.fileName();

#if 0
        if((info.isDir())){
            icon.addFile(":/images/dir.png");
        }else if(info.isFile()){
            icon.addFile(":/images/file.png");
        }else if(info.isSymLink()){
            icon.addFile(":/images/file.png");
        }
#endif
        QListWidgetItem *item=new QListWidgetItem (icon,fileName);

        //大小
        QString fsize = "Size: ";
        fsize += QString::number(info.size());
        fsize += "Bytes";

        //创建时间
        QString createtm = "\nCreate time: ";
        createtm += info.created().toString();
        QString lastModifiedtm = "\nLast Modified time: ";
        lastModifiedtm += info.lastModified().toString();
        QString lastReadtm = "\nLast Read time: ";
        lastReadtm += info.lastRead().toString();

        //权限
        QString permission = "\nOwner:";
        QFile::Permissions perm = info.permissions();
        if(perm & QFile::ReadOwner)
            permission += "R";
        else
            permission += "-";
        if(perm & QFile::ExeOwner)
            permission += "W";
        else
            permission += "-";
        if(perm & QFile::ExeOwner)
            permission += "X";
        else
            permission += "-";

        permission += ";User:";
        if(perm & QFile::ReadUser)
            permission += "R";
        else
            permission += "-";
        if(perm & QFile::WriteUser)
            permission += "W";
        else
            permission += "-";
        if(perm & QFile::ExeUser)
            permission += "X";
        else
            permission += "-";

        permission += ";Group:";
        if(perm & QFile::ReadGroup)
            permission += "R";
        else
            permission += "-";
        if(perm & QFile::WriteGroup)
            permission += "W";
        else
            permission += "-";
        if(perm & QFile::ExeGroup)
            permission += "X";
        else
            permission += "-";

        permission += ";Other:";
        if(perm & QFile::ReadOther)
            permission += "R";
        else
            permission += "-";
        if(perm & QFile::WriteOther)
            permission += "W";
        else
            permission += "-";
        if(perm & QFile::ExeOther)
            permission += "X";
        else
            permission += "-";

        item->setToolTip(fsize+createtm+lastModifiedtm+lastReadtm+permission);

        ListWidgetFile->addItem(item);
    }
}

void FileViewer::slotShowDir(QListWidgetItem * item)
{
    QString path = item->text();

    QDir dir(LineEditDir->text());
    dir.cd(path);

    path = dir.absolutePath();
    LineEditDir->setText(path);

    watcher->removePath(rootpath);
    watcher->addPath(path);
    rootpath = path;

    qDebug() << watcher->directories().length() << endl;

    showFileInfo(dir);
}

void FileViewer::slotGetfileInfoList()
{
	QDir dir;
    dir.setPath(LineEditDir->text());
    showFileInfo(dir);
}

void FileViewer::showFileInfo(QDir dir)
{
    QStringList string;
    string << "*" ;
    QFileInfoList fileinfolist = dir.entryInfoList(string,QDir::AllEntries,QDir::DirsFirst);
    showFileInfoList(fileinfolist);
}

void FileViewer::slotDirChanged()
{
    QDir dir(LineEditDir->text());
    showFileInfo(dir);
}
