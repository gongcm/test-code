#include "mainwindow.h"  

#include <QWidget>

#include <QMainWindow>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QListWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

// 主窗口实现
MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags):
    QMainWindow(parent, flags)
{
    /*QWidget*/
    setWindowTitle(tr("Demo for DOC"));
    text = new QTextEdit(this);
    setCentralWidget(text);

    bIstextChanged = false;
    QObject::connect(text, SIGNAL(textChanged()), this, SLOT(slottextChanged()));

    statusbar = this->statusBar();

    createActions();
    createMenus();
    createToolBars();

    this->setMinimumSize(640,480);
}
MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    // file open action

    fileOpenAction = new QAction(QIcon(":/images/open.png"),tr("Open"),this);	// 打开文件
    fileOpenAction->setShortcut(tr("Ctrl+O"));
    fileOpenAction->setStatusTip(tr("open a file"));
    connect(fileOpenAction,SIGNAL(triggered()),this,SLOT(slotOpenFile()));

    
    // file new action
    fileNewAction = new QAction(QIcon(":/images/new.png"),tr("New"),this);	// 新建文件
    fileNewAction->setShortcut(tr("Ctrl+N"));
    fileNewAction->setStatusTip(tr("new file"));
    connect(fileNewAction,SIGNAL(triggered()),this,SLOT(slotNewFile()));

    // save file action
    fileSaveAction = new QAction(QPixmap(":/images/save.png"),tr("Save"),this);	// 保存文件 
    fileSaveAction->setShortcut(tr("Ctrl+S"));
    fileSaveAction->setStatusTip(tr("save file"));
    connect(fileSaveAction,SIGNAL(triggered()),this,SLOT(slotSaveFile()));

    // exit action
    exitAction = new QAction(tr("Exit"), this);	// 退出
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("exit"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(slotExit()));

    cutAction = new QAction(QIcon(":/images/cut.png"), tr("Cut"), this);		// 剪切
    cutAction->setShortcut(tr("Ctrl+X"));
    cutAction->setStatusTip(tr("cut to clipboard"));
    connect(cutAction, SIGNAL(triggered()), text, SLOT(cut()));

    copyAction = new QAction(QIcon(":/images/copy.png"), tr("Copy"), this);		// 复制
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setStatusTip(tr("copy to clipboard"));
    connect(copyAction, SIGNAL(triggered()), text, SLOT(copy()));

    pasteAction = new QAction(QIcon(":/images/paste.png"), tr("Paste"), this);		// 粘贴
    pasteAction->setShortcut(tr("Ctrl+V"));
    pasteAction->setStatusTip(tr("paste clipboard to selection"));
    connect(pasteAction, SIGNAL(triggered()), text, SLOT(paste()));

    aboutAction = new QAction(tr("About"), this);		// 关于
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(slotAbout()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("File"));
    editMenu = menuBar()->addMenu(tr("Edit"));
    aboutMenu = menuBar()->addMenu(tr("Help"));
    
    fileMenu->addAction(fileNewAction);
    fileMenu->addAction(fileOpenAction);
    fileMenu->addAction(fileSaveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    
    editMenu->addAction(copyAction);
    editMenu->addAction(cutAction);
    editMenu->addAction(pasteAction);
    
    aboutMenu->addAction(aboutAction);
         
}

void MainWindow::createToolBars()
{
	
    fileTool = addToolBar("File");
    fileTool->setMovable(false);
    editTool = addToolBar("Edit");
    
    fileTool->addAction(fileNewAction);
    fileTool->addAction(fileOpenAction);
    fileTool->addAction(fileSaveAction);
    
    editTool->addAction(copyAction);
    editTool->addAction(cutAction);
    editTool->addAction(pasteAction);
}

void MainWindow::slotNewFile()
{
    //创建并显示新窗口
    MainWindow *newdoc = new MainWindow;
    newdoc->show();
}

void MainWindow::slotOpenFile()
{
    QString str = QFileDialog::getOpenFileName(this,
                                 "Open",
                                 QDir::currentPath(),
                                 "TXT(*.txt);;All(*.*)");
    if(str == "")
        return;

    QFile file(str);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << file.errorString() << endl;
        return;
    }

    QTextStream out(&file);  // --- > QString
    QString txt = out.readAll();
    qDebug() << txt << endl;
    text->setPlainText(txt);

    file.close();
}

void MainWindow::slotSaveFile()
{
    QString str = QFileDialog::getSaveFileName(this,
                                 "Save",
                                 QDir::currentPath(),
                                 "TXT(*.txt);;All(*.*)");
    if(str == "")
        return;

    QFile file(str);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << file.errorString() << endl;
        return;
    }

    QTextStream in(&file);
    QString txt = text->toPlainText();
    qDebug() << txt << endl;
    in << txt;

    file.close();
}

void MainWindow::slotExit()
{
#if 0
    if(bIstextChanged){
       if(QMessageBox::Yes == QMessageBox::question(this, QString("Quit"), QString("To save file ?")))
           slotSaveFile();
    }
#else
    if(text->document()->isModified()){
        if(QMessageBox::Yes == QMessageBox::question(this, QString("Quit"), QString("To save file ?")))
            slotSaveFile();
     }
#endif

    this->close();
}

void MainWindow::slottextChanged()
{
    this->bIstextChanged = true;
}

void MainWindow::slotAbout()
{
}


