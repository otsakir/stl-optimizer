#include "appwindow.h"
#include "ui_appwindow.h"
#include "glwidget.h"

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
}

AppWindow::~AppWindow()
{
    delete ui;
}

void AppWindow::setXSlider(int value)
{
    ui->sliderX->setSliderPosition(value);
}
