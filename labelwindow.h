#ifndef LABELWINDOW_H
#define LABELWINDOW_H

#include <QDialog>

namespace Ui {
class LabelWindow;
}

class LabelWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LabelWindow(QWidget *parent = nullptr);
    ~LabelWindow();

    QString anotacion;

private slots:
    void on_btnOk_clicked();
    void on_editAnotacion_returnPressed();

    void on_classComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::LabelWindow *ui;

    int languageSet;
    QStringList classList;
};

#endif // LABELWINDOW_H
