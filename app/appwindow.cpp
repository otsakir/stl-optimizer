#include "appwindow.h"
#include "ui_appwindow.h"
#include "dockwidget.h"
#include "glwidget.h"

#include <QDockWidget>
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

    DockWidget *dock = new DockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);

    connect(dock, &DockWidget::buttonRebaseClicked, glWidget, &GLWidget::rebaseOnFace);
}

AppWindow::~AppWindow()
{

    delete ui;
}

