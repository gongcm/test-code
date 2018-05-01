#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QToolBar;
class QTextEdit;
class QStatusBar;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

private:
    void createMenus();
    void createActions();
    void createToolBars();
    
protected slots:
    void slotNewFile();
    void slotOpenFile();
    void slotSaveFile();
    void slotExit();
    void slotAbout();
    void slottextChanged();

private:
    
    QTextCodec *codec;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *aboutMenu;

    QToolBar *fileTool;
    QToolBar *editTool;

    QStatusBar *statusbar;

    QAction *fileOpenAction;
    QAction *fileNewAction;
    QAction *fileSaveAction;
    QAction *exitAction;
    QAction *copyAction;
    QAction *cutAction;
    QAction *pasteAction;
    QAction *aboutAction;
 
    QTextEdit *text;
    bool bIstextChanged;
};

#endif // MAINWINDOW_H
