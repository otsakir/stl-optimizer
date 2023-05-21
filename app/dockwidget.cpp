#include "dockwidget.h"
#include "qdebug.h"
#include "ui_dockwidget.h"

DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    ui->setupUi(this);
}

DockWidget::~DockWidget()
{
    delete ui;
}

void DockWidget::on_toolButton_4_clicked()
{
    qDebug() << "first button clicked";
    emit buttonRebaseClicked();
}

