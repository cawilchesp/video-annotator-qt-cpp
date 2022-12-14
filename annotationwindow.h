#ifndef ANNOTATIONWINDOW_H
#define ANNOTATIONWINDOW_H

#include <QDialog>

namespace Ui {
class AnnotationWindow;
}

class AnnotationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AnnotationWindow(QWidget *parent = nullptr);
    ~AnnotationWindow();

    QString anotacion;

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();
    void on_editAnotacion_returnPressed();

private:
    Ui::AnnotationWindow *ui;

    int languageSet;
};

#endif // ANNOTATIONWINDOW_H
