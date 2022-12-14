#include "helpwindow.h"
#include "ui_helpwindow.h"
#include "helpbrowser.h"

#include <QSettings>
#include <QDockWidget>
#include <QSplitter>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QTabWidget>
#include <QHelpIndexWidget>
#include <QHBoxLayout>

HelpWindow::HelpWindow(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);

    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    int languageSet = TVASettings.value("Language",0).toInt();
    QString helpEnginePath, textViewerSource;
    if (languageSet == 0) {
        helpEnginePath = "Ayuda/helpFileES.qhc";
        textViewerSource = "qthelp://trafficvideoannotator.qt.help/helpdocs/1infoGenES.html";
    } else if (languageSet == 1) {
        helpEnginePath = "Ayuda/helpFileEN.qhc";
        textViewerSource = "qthelp://trafficvideoannotator.qt.help/helpdocs/1infoGenEN.html";
    }

    QHelpEngine *helpEngine = new QHelpEngine(helpEnginePath, this);
    helpEngine->setupData();

    QTabWidget *tWidget = new QTabWidget();
    tWidget->setMaximumWidth(200);
    tWidget->addTab(helpEngine->contentWidget(), tr("Contenido"));
    tWidget->addTab(helpEngine->indexWidget(), tr("Índice"));

    HelpBrowser *textViewer = new HelpBrowser(helpEngine);
    textViewer->setSource(QUrl(textViewerSource));
    connect(helpEngine->contentWidget(), SIGNAL(linkActivated(QUrl)), textViewer, SLOT(setSource(QUrl)));
    connect(helpEngine->indexWidget(), SIGNAL(linkActivated(QUrl, QString)), textViewer, SLOT(setSource(QUrl)));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tWidget);
    layout->addWidget(textViewer);

    this->setLayout(layout);
}

HelpWindow::~HelpWindow()
{
    delete ui;
}
