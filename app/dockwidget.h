#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class DockWidget;
}

class DockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockWidget(QWidget *parent = nullptr);
    ~DockWidget();

signals:
    void buttonRebaseClicked();

private slots:
    void on_toolButton_4_clicked();

private:
    Ui::DockWidget *ui;
};

#endif // DOCKWIDGET_H
