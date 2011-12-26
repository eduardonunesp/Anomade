#include <QtWebKit>
#include "MainWindow.hpp"
#include "Highlighter.hpp"
#include "../libs/markdowncxx.h"

MainWindow::MainWindow()
{
    init();
}

MainWindow::MainWindow(const QString &fileName)
{
    init();
    loadFile(fileName);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())  {
        writeSettings();
        event->accept();
    } else  {
        event->ignore();
    }
}

void MainWindow::resizeEvent (QResizeEvent *event) 
{
    previewArea->setFixedWidth(editArea->width());
}

void MainWindow::newFile() 
{
    MainWindow *other = new MainWindow;
    other->move(x() + 40, y() + 40);
    other->show();
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())  {
        MainWindow *existing = findMainWindow(fileName);
        if (existing)  {
            existing->show();
            existing->raise();
            existing->activateWindow();
            return;
        }

        if (isUntitled && editArea->document()->isEmpty()
                && !isWindowModified())  {
            loadFile(fileName);
        } else  {
            MainWindow *other = new MainWindow(fileName);
            if (other->isUntitled)  {
                delete other;
                return;
            }
            other->move(x() + 40, y() + 40);
            other->show();
        }
    }
}

bool MainWindow::save()
{
    if (isUntitled)  {
        return saveAs();
    } else  {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
            curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Anomade"),
            tr("Anomade (Another Markdown Editor 1.0)\nEduardo Nunes Pereira (eduardonunesp@gmail.com)"));
}

void MainWindow::documentWasModified()
{
    setWindowModified(true);
}

void MainWindow::init()
{
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;

    resize(800, 600);

    editArea = new QTextEdit(this);
    editArea->setAcceptRichText(true);
    editArea->setLineWrapMode(QTextEdit::NoWrap);
    editArea->setFont(QFont("Monoespace",12));
    editArea->setMinimumWidth(400);
    
    previewArea = new QWebView(this);
    previewArea->setFixedWidth(editArea->width());
    previewArea->settings()->setUserStyleSheetUrl(QUrl::fromLocalFile(":resources/my.css"));
    previewArea->setMinimumWidth(400);

    splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(editArea);
    splitter->addWidget(previewArea);
    splitter->setHandleWidth(2);

    QList<int> sizes;
    sizes << 50 << 50;
    splitter->setSizes(sizes);
    splitter->setStyleSheet("border: 1px;");
    splitter->setChildrenCollapsible(false);
    
    setCentralWidget(splitter);
    setUnifiedTitleAndToolBarOnMac(true);

    connect(editArea, SIGNAL(textChanged()), SLOT(updatePreview()));
    connect(previewArea, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished(bool)));

    highlighter = new Highlighter(editArea->document());

    createActions();
    createMenus();
    readSettings();
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    
    fileMenu->addSeparator();
    
    exportMenu = fileMenu->addMenu(tr("Export"));
    exportMenu->addAction(exportHTMLAct);
    exportMenu->addAction(exportPDFAct);
    
    fileMenu->addSeparator();

    fileMenu->addAction(closeAct);
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcut(tr("Ctrl+W"));
    closeAct->setStatusTip(tr("Close this window"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                "clipboard"));
    connect(cutAct, SIGNAL(triggered()), editArea, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                "clipboard"));
    connect(copyAct, SIGNAL(triggered()), editArea, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                "selection"));
    connect(pasteAct, SIGNAL(triggered()), editArea, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    exportHTMLAct = new QAction(tr("HTML"), this);
    exportHTMLAct->setStatusTip(tr("Export to HTML file"));
    connect(exportHTMLAct, SIGNAL(triggered()), this, SLOT(exportToHTML()));

    exportPDFAct = new QAction(tr("PDF"), this);
    exportPDFAct->setStatusTip(tr("Export to PDF file"));
    connect(exportPDFAct, SIGNAL(triggered()), this, SLOT(exportToPDF()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(editArea, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(editArea, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
}

bool MainWindow::maybeSave()
{
    if (editArea->document()->isModified())  {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Anomade"),
                tr("The document has been modified.\n"
                    "Do you want to save your changes?"),
                QMessageBox::Save | QMessageBox::Discard
                | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }

    return true;    
}


void MainWindow::readSettings()
{
    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::updatePreview()
{
    std::string htmlRes;
    markdown2html(editArea->toPlainText().toStdString(), htmlRes);
    previewArea->page()->mainFrame()->setHtml(QString(htmlRes.c_str()));
}

void MainWindow::onLoadFinished(bool isFinish)
{

}

void MainWindow::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))  {
        QMessageBox::warning(this, tr("Anomade"),
                tr("Cannot read file %1:\n%2.")
                .arg(fileName)
                .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    editArea->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled)  {
        curFile = tr("document%1.md").arg(sequenceNumber++);
    } else  {
        curFile = QFileInfo(fileName).canonicalFilePath();
    }

    editArea->document()->setModified(false);
    setWindowModified(false);
    setWindowFilePath(curFile);
}


bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))  {
        QMessageBox::warning(this, tr("Anomade"),
                tr("Cannot write file %1:\n%2.")
                .arg(fileName)
                .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << editArea->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::exportToHTML()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
            "Untitled.html");

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))  {
        QMessageBox::warning(this, tr("Anomade"),
                tr("Cannot write file %1:\n%2.")
                .arg(fileName)
                .arg(file.errorString()));
        return;
    }
    
    QFile css(":resources/my.css");
    if (!css.open(QFile::ReadOnly | QFile::Text))  {
        QMessageBox::warning(this, tr("Anomade"),
                tr("Cannot read file %1:\n%2.")
                .arg(":resources/my.css")
                .arg(css.errorString()));
    } else {
        previewArea->page()->mainFrame()->findFirstElement("style").replace(""); // clean up
        previewArea->page()->mainFrame()->findFirstElement("html").appendInside("<style>" + css.readAll() + "</style>");
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << previewArea->page()->mainFrame()->toHtml();
    QApplication::restoreOverrideCursor();
}

void MainWindow::exportToPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
            "Untitled.pdf");

    QFile css(":resources/my.css");
    if (!css.open(QFile::ReadOnly | QFile::Text))  {
        QMessageBox::warning(this, tr("Anomade"),
                tr("Cannot read file %1:\n%2.")
                .arg(":resources/my.css")
                .arg(css.errorString()));
    } else {
        previewArea->page()->mainFrame()->findFirstElement("style").replace(""); // clean up
        previewArea->page()->mainFrame()->findFirstElement("html").appendInside("<style>" + css.readAll() + "</style>");
    }

    QPrinter printer;
    printer.setPrinterName("Print to File (PDF)");
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPrintRange(QPrinter::AllPages);
    printer.setOrientation(QPrinter::Portrait);
    printer.setPaperSize(QPrinter::A4);
    printer.setResolution(QPrinter::HighResolution);
    printer.setFullPage(false);
    printer.setNumCopies(1);
    printer.setOutputFileName(fileName);
    previewArea->print(&printer);
}

MainWindow *MainWindow::findMainWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, qApp->topLevelWidgets())  {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin && mainWin->curFile == canonicalFilePath)
            return mainWin;
    }

    return 0;
}
