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

    connect(ui->sliderX, &QSlider::valueChanged, glWidget, &GLWidget::setXRotation);
    connect(ui->sliderY, &QSlider::valueChanged, glWidget, &GLWidget::setYRotation);
    connect(ui->sliderZ, &QSlider::valueChanged, glWidget, &GLWidget::setZRotation);
    connect(glWidget, &GLWidget::zoomChangedBy, glWidget, &GLWidget::updateZoomLevel);
    connect(glWidget, &GLWidget::xRotationChanged, this, &AppWindow::setXSlider);
    //connect(glWidget, &GLWidget::cl)

    DockWidget *dock = new DockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);
}

AppWindow::~AppWindow()
{

    delete ui;
}

void AppWindow::setXSlider(int value)
{
    ui->sliderX->setSliderPosition(value);
}
