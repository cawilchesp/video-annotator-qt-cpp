#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDialog>
#include <QDockWidget>

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = nullptr);
    ~HelpWindow();

private slots:


private:
    Ui::HelpWindow *ui;

    QDockWidget *helpWidget;

};

#endif // HELPWINDOW_H
