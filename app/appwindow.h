#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QMainWindow>

namespace Ui {
class AppWindow;
}

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppWindow(QWidget *parent = nullptr);
    ~AppWindow();

public slots:

private slots:
    void on_action_Open_triggered();

    void on_toolButtonRebase_clicked();

signals:
    void newStlFilename(QString filename);
    void buttonRebaseClicked();

private:
    Ui::AppWindow *ui;
};

#endif // APPWINDOW_H
