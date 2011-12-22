#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

class QWebView;
class Highlighter;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    MainWindow(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent (QResizeEvent *event);

private slots:
    void updatePreview();
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void exportToHTML();
    void exportToPDF();
    void onLoadFinished(bool);

private:
    void init();
    void createActions();
    void createMenus();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    MainWindow *findMainWindow(const QString &fileName);

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QMenu *exportMenu;

    QString curFile;
    bool isUntitled;

    QSplitter *splitter;
    QTextEdit *editArea;
    QWebView *previewArea;
    Highlighter *highlighter; 
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *closeAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;    
    QAction *exportHTMLAct;
    QAction *exportPDFAct;
};

#endif
