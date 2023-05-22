#include "appwindow.h"
#include "ui_appwindow.h"
#include "glwidget.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QListWidget>

AppWindow::AppWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AppWindow)
{
    ui->setupUi(this);
    GLWidget* glWidget = new GLWidget();
    glWidget->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    ui->horizontalLayoutMain->insertWidget(0, glWidget);

    connect(glWidget, &GLWidget::zoomChangedBy, glWidget, &GLWidget::updateZoomLevel);

    QObject::connect(ui->actionE_xit, &QAction::triggered, QCoreApplication::instance(), QCoreApplication::quit, Qt::QueuedConnection);
    connect(this, &AppWindow::buttonRebaseClicked, glWidget, &GLWidget::rebaseOnFace);
    connect(this, &AppWindow::newStlFilename, glWidget, &GLWidget::onNewStlFilename);
    connect(ui->toolButtonResetCamera, &QToolButton::clicked, glWidget, &GLWidget::resetCamera);
}

AppWindow::~AppWindow()
{

    delete ui;
}


void AppWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open STL file"), "/home/nando", "STL Files (*.stl)");

    if (!fileName.isNull()) {
        setWindowTitle(QString("STL optimizer - %1").arg(fileName));
        emit newStlFilename(fileName);
    }

}


void AppWindow::on_toolButtonRebase_clicked()
{
    emit buttonRebaseClicked();
}

