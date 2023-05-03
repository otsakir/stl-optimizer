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



}

AppWindow::~AppWindow()
{
    delete ui;
}
