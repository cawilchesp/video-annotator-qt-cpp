#include "labelwindow.h"
#include "ui_labelwindow.h"

#include <QMessageBox>
#include <QSettings>

LabelWindow::LabelWindow(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::LabelWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    //---------------------------- Valores de Configuración -----------------------------
    QSettings TVASettings("config.ini" , QSettings::IniFormat);                            // Archivo de configuración
    classList = TVASettings.value("Class_List").toStringList();

    if (classList.length() != 0) {
        ui->classComboBox->addItems(classList);
        ui->classComboBox->addItem(tr("Borrar lista"));
    }
    ui->classComboBox->setCurrentIndex(-1);

    ui->btnOk->setText(QChar(0x2713));
}

LabelWindow::~LabelWindow()
{
    delete ui;
}

void LabelWindow::on_btnOk_clicked()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);

    if (ui->editAnotacion->text() != "") {
        anotacion = ui->editAnotacion->text().toLower();
        int index = classList.indexOf(QRegularExpression(anotacion, QRegularExpression::CaseInsensitiveOption));
        if (index == -1) {
            classList.push_back(anotacion);
            TVASettings.setValue("Class_List",classList);
        }
        this->close();
    } else {
        QMessageBox::information(this, tr("Advertencia"), tr("Debe ingresar una anotación"));
    }
}

void LabelWindow::on_editAnotacion_returnPressed()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);

    if (ui->editAnotacion->text() != "") {
        anotacion = ui->editAnotacion->text().toLower();
        int index = classList.indexOf(QRegularExpression(anotacion, QRegularExpression::CaseInsensitiveOption));
        if (index == -1) {
            classList.push_back(anotacion);
            TVASettings.setValue("Class_List",classList);
        }
        this->close();
    } else {
        QMessageBox::information(this, tr("Advertencia"), tr("Debe ingresar una anotación"));
    }
}

void LabelWindow::on_classComboBox_currentIndexChanged(const QString &arg1)
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    if (arg1 != "Borrar lista") {
        ui->editAnotacion->setText(arg1);
    } else {
        ui->classComboBox->clear();
        classList.clear();
        TVASettings.setValue("Class_List",classList);
    }
}
