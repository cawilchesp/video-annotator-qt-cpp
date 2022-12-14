#include "annotationwindow.h"
#include "ui_annotationwindow.h"

#include <QMessageBox>
#include <QSettings>

AnnotationWindow::AnnotationWindow(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::AnnotationWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    ui->btnOk->setText(QChar(0x2713));
    ui->btnCancel->setText(QChar(0x2717));
}

AnnotationWindow::~AnnotationWindow()
{
    delete ui;
}

void AnnotationWindow::on_btnOk_clicked()
{
    if (ui->editAnotacion->text() != "") {
        anotacion = ui->editAnotacion->text();
        this->close();
    } else {
        QMessageBox::information(this, tr("Advertencia"), tr("Debe ingresar una anotación"));
    }
}

void AnnotationWindow::on_editAnotacion_returnPressed()
{
    if (ui->editAnotacion->text() != "") {
        anotacion = ui->editAnotacion->text();
        this->close();
    } else {
        QMessageBox::information(this, tr("Advertencia"), tr("Debe ingresar una anotación"));
    }
}

void AnnotationWindow::on_btnCancel_clicked()
{
    this->close();
}
