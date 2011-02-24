#include <QtGui>
#include "mainwindow.h"
#include "mdichild.h"
#include "csettings.h"
#include "graph.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include	<string.h>

#include        "memory.h"
#include        "types.h"
#include        "gvplugin.h"
#include        "gvcjob.h"
#include        "gvcint.h"
#include        "gvcproc.h"
QTextEdit* globTextEdit;
int errorPipe(char* errMsg)
{
    globTextEdit->setText(globTextEdit->toPlainText()+QString(errMsg));
    return 0;
}
void LoadLayouts(QComboBox* cb)
{
    char val[]="casdadasd";
    Agraph_t* g = agopen("g", AGDIGRAPH);
    aginit();
    GVC_t* gvc=gvContext();

    QStringList sl= QString(gvplugin_list(gvc, API_layout, val)).trimmed().split(" ");
    cb->clear();
    for (int id=0;id < sl.count(); id ++)
    {
	cb->addItem(sl[id]);
    };
    gvFreeLayout(gvc, g);
}void LoadRenderers(QComboBox* cb)
{
    char val[]="casdadasd";
    Agraph_t* g = agopen("g", AGDIGRAPH);
    GVC_t* gvc=gvContext();
    QStringList sl= QString(gvplugin_list(gvc, API_device, val)).trimmed().split(" ");
    cb->clear();
    for (int id=0;id < sl.count(); id ++)
    {
	cb->addItem(sl[id]);
    };
    gvFreeLayout(gvc, g);
    
}
CMainWindow::CMainWindow()
{

        QWidget* centralwidget = new QWidget(this);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QVBoxLayout* verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mdiArea = new QMdiArea(centralwidget);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));

        verticalLayout->addWidget(mdiArea);

        QTextEdit* textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setMinimumSize(QSize(0, 80));
        textEdit->setMaximumSize(QSize(16777215, 120));
	globTextEdit=textEdit;
	agseterrf(errorPipe);
        verticalLayout->addWidget(textEdit);


        verticalLayout_2->addLayout(verticalLayout);

       setCentralWidget(centralwidget);





    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(slotRefreshMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    frmSettings= new CFrmSettings();

    actions();
    menus();
    toolBars();
    statusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("GVEdit For Graphviz ver:1.01"));
    this->resize(1024,900);
    this->move(0,0);
    setUnifiedTitleAndToolBarOnMac(true);
//    (QComboBox*)frmSettings->findChild<QComboBox*>("cbLayout")
    LoadLayouts((QComboBox*)frmSettings->findChild<QComboBox*>("cbLayout"));
    LoadRenderers((QComboBox*)frmSettings->findChild<QComboBox*>("cbExtension"));
    statusBar()->showMessage(tr("Ready"));
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void CMainWindow::slotNew()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void CMainWindow::slotOpen()
{
    QStringList filters;
     filters << "*.cpp" << "*.cxx" << "*.cc";

    QFileDialog fd;
//    fd.setProxyModel(new FileFilterProxyModel());
    fd.setNameFilter("XML (*.xml)");
   QString fileName =fd.getOpenFileName(this);
//    QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
}

void CMainWindow::slotSave()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void CMainWindow::slotSaveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void CMainWindow::slotCut()
{
    if (activeMdiChild())
        activeMdiChild()->cut();
}

void CMainWindow::slotCopy()
{
    if (activeMdiChild())
        activeMdiChild()->copy();
}

void CMainWindow::slotPaste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}

void CMainWindow::slotAbout()
{
   QMessageBox::about(this, tr("About MDI"),
            tr("The <b>MDI</b> example demonstrates how to write multiple "
               "document interface applications using Qt."));
}


void CMainWindow::slotSettings()
{
    frmSettings->showSettings(activeMdiChild());

}

void CMainWindow::updateFileMenu()
{
    if (!activeMdiChild())
    {
        saveAct->setEnabled(false);
	saveAsAct->setEnabled(false);
	pasteAct->setEnabled(false);
	closeAct->setEnabled(false);
	closeAllAct->setEnabled(false);
	tileAct->setEnabled(false);
	cascadeAct->setEnabled(false);
	nextAct->setEnabled(false);
	previousAct->setEnabled(false);
	separatorAct->setVisible(false);
    }
    else
    {
        saveAct->setEnabled(true);
	saveAsAct->setEnabled(true);
	pasteAct->setEnabled(true);
	closeAct->setEnabled(true);
	closeAllAct->setEnabled(true);
	tileAct->setEnabled(true);
	cascadeAct->setEnabled(true);
	nextAct->setEnabled(true);
	previousAct->setEnabled(true);
	separatorAct->setVisible(true);
	if(activeMdiChild()->textCursor().hasSelection())
	{
	    cutAct->setEnabled(true);
	    copyAct->setEnabled(true);

	}
	else
	{
	    cutAct->setEnabled(false);
	    copyAct->setEnabled(false);

	}



    }

}

void CMainWindow::slotRefreshMenus()
{
    updateMenus();
}
void CMainWindow::updateMenus()
{
    this->updateFileMenu();
    this->updateWindowMenu();

}

void CMainWindow::updateWindowMenu()
{
    mWindow->clear();
    mWindow->addAction(closeAct);
    mWindow->addAction(closeAllAct);
    mWindow->addSeparator();
    mWindow->addAction(tileAct);
    mWindow->addAction(cascadeAct);
    mWindow->addSeparator();
    mWindow->addAction(nextAct);
    mWindow->addAction(previousAct);
    mWindow->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
	if (windows.at(i)->widget()->inherits("MdiChild"))
	{
	    MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());
            QString text;
	    if (i < 9) {
	        text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
	    } else {
		text = tr("%1 %2").arg(i + 1)
		                  .arg(child->userFriendlyCurrentFile());
	    }
	    QAction *action  = mWindow->addAction(text);
	    action->setCheckable(true);
	    action ->setChecked(child == activeMdiChild());
	    connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
	    windowMapper->setMapping(action, windows.at(i));
	}
    }
}

MdiChild *CMainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    child->parentFrm=this;
    QMdiSubWindow* s=mdiArea->addSubWindow(child);
    s->resize(800,600);
    s->move(mdiArea->subWindowList().count()*5,mdiArea->subWindowList().count()*5);
    connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    return child;
}

void CMainWindow::actions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(slotNew()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(slotOpen()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(slotSave()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(slotSaveAs()));
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the ""clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(slotCut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(slotCopy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(slotPaste()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()),mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(previousAct, SIGNAL(triggered()),mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(slotAbout()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


    settingsAct = new QAction(tr("Settings"), this);
    settingsAct->setStatusTip(tr("Show Graphviz Settings"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(slotSettings()));



    layoutAct = new QAction(QIcon(":/images/cut.png"),tr("Layout"), this);
    layoutAct->setStatusTip(tr("Layout the active graph"));
    connect(layoutAct, SIGNAL(triggered()), this, SLOT(about()));


}

void CMainWindow::menus()
{
    mFile = menuBar()->addMenu(tr("&File"));
    mEdit = menuBar()->addMenu(tr("&Edit"));
    mWindow = menuBar()->addMenu(tr("&Window"));
    mHelp = menuBar()->addMenu(tr("&Help"));


    mFile->addAction(newAct);
    mFile->addAction(openAct);
    mFile->addAction(saveAct);
    mFile->addAction(saveAsAct);
    mFile->addSeparator();

    mFile->addAction(exitAct);

    mEdit->addAction(cutAct);
    mEdit->addAction(copyAct);
    mEdit->addAction(pasteAct);

    mGraph =  menuBar()->addMenu (tr("&Graph"));
    mGraph->addAction(settingsAct);
    mGraph->addAction(layoutAct);
    mGraph->addSeparator();
    loadPlugins();

    updateWindowMenu();
    connect(mWindow, SIGNAL(aboutToShow()), this, SLOT(slotRefreshMenus()));
    mHelp->addAction(aboutAct);
    mHelp->addAction(aboutQtAct);
}

void CMainWindow::toolBars()
{
    tbFile = addToolBar(tr("File"));
    tbFile->addAction(newAct);
    tbFile->addAction(openAct);
    tbFile->addAction(saveAct);

    tbEdit = addToolBar(tr("Edit"));
    tbEdit->addAction(cutAct);
    tbEdit->addAction(copyAct);
    tbEdit->addAction(pasteAct);

    tbGraph = addToolBar(tr("Graph"));
    tbGraph->addAction(settingsAct);
    tbGraph->addAction(layoutAct);

}


void CMainWindow::readSettings()
{
    QSettings settings("Trolltech", "MDI Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void CMainWindow::writeSettings()
{
    QSettings settings("Trolltech", "MDI Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

MdiChild *CMainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    {
	if(activeSubWindow->widget()->inherits("MdiChild"))
	    return qobject_cast<MdiChild *>(activeSubWindow->widget());
        else
	    return qobject_cast <ImageViewer *> (activeSubWindow->widget())->graphWindow;

    }
    return 0;
}

QMdiSubWindow *CMainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
	if (window->widget()->inherits("MdiChild"))
	{

	    MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
	    if (mdiChild->currentFile() == canonicalFilePath)
		return window;
	}
	else
	{

	    MdiChild *mdiChild = qobject_cast<ImageViewer *>(window->widget())->graphWindow;
	    if (mdiChild->currentFile() == canonicalFilePath)
		return window;
	}



    }
    return 0;
}

void CMainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
void CMainWindow::loadPlugins()
{

}