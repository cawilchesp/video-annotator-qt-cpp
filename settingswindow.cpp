#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QSettings>
#include <QFileDialog>
#include <QMenu>
#include <QColorDialog>

settingsWindow::settingsWindow(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::settingsWindow)
{
    ui->setupUi(this);

    //---------------------------- Valores de Configuración -----------------------------
    QSettings TVASettings("config.ini" , QSettings::IniFormat);                            // Archivo de configuración
    videoFolder = TVASettings.value("Video_Folder","C:\\").toString();                     // Carpeta de videos
    resultsFolder = TVASettings.value("Results_Folder","C:\\").toString();                 // Carpeta de resultados
    languageSet = TVASettings.value("Language",0).toInt();                                 // Idioma seleccionado
    fontSizeDefault = TVASettings.value("Font_Size",10).toInt();                            // Tamaño de la fuente
    annotationFontDefault = TVASettings.value("Font","Arial").toString();                  // Fuente
    annotationCustomColor = TVASettings.value("Text_Color").value<QColor>();
    customColorObject = TVASettings.value("Object_Color").value<QColor>();
    thickness = TVASettings.value("Thickness",1).toInt();
    temaDefault = TVASettings.value("Theme",0).toInt();

    //---------------------------- Configuración de widgets -----------------------------
    ui->videoFolderPathlbl->setText(videoFolder);
    ui->resultsFolderPathlbl->setText(resultsFolder);
    ui->sizeSpinBox->setValue(fontSizeDefault);
    ui->fontComboBox->setCurrentFont(QFont(annotationFontDefault));
    ui->thicknessSpinBox->setValue(thickness);

    // Texto ----------------------------------------------------------------------------
    QMenu *textoColorMenu = new QMenu;
    textoColorMenu->addAction(ui->actionTextoNegro);
    textoColorMenu->addAction(ui->actionTextoBlanco);
    textoColorMenu->addAction(ui->actionTextoAmarillo);
    textoColorMenu->addAction(ui->actionTextoAzul);
    textoColorMenu->addAction(ui->actionTextoRojo);
    textoColorMenu->addAction(ui->actionTextoVerde);
    textoColorMenu->addAction(ui->actionTextCustomColor);
    ui->textoColorToolButton->setMenu(textoColorMenu);
    ui->textoColorToolButton->setText(QChar(0x2B24));

    // Herramientas ---------------------------------------------------------------------
    QMenu *objetoColorMenu = new QMenu;
    objetoColorMenu->addAction(ui->actionObjetoNegro);
    objetoColorMenu->addAction(ui->actionObjetoBlanco);
    objetoColorMenu->addAction(ui->actionObjetoAmarillo);
    objetoColorMenu->addAction(ui->actionObjetoAzul);
    objetoColorMenu->addAction(ui->actionObjetoRojo);
    objetoColorMenu->addAction(ui->actionObjetoVerde);
    objetoColorMenu->addAction(ui->actionObjectCustomColor);
    ui->ObjectColorToolButton->setMenu(objetoColorMenu);
    ui->ObjectColorToolButton->setText(QChar(0x2B24));

    // Tema -----------------------------------------------------------------------------
    if (temaDefault == 0) {
        // Tema Claro
        this->setStyleSheet("QDialog#settingsWindow {background-color: rgb(240,240,240);}");
        ui->videoFolderPathlbl->setStyleSheet("QLabel {border: 0px solid; border-radius:12; background-color: white; padding: 0 12 0 12}");
        ui->resultsFolderPathlbl->setStyleSheet("QLabel {border: 0px solid; border-radius:12; background-color: white; padding: 0 12 0 12}");
        ui->videoFolderPathbtn->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->resultsFolderPathbtn->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
        ui->fontComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: white; padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(255, 255, 255);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);} QComboBox QAbstractItemView { background-color: rgb(255,255,255);}");
        ui->thicknessSpinBox->setStyleSheet("QSpinBox {border: 0px solid; border-radius: 12; background-color: white; padding: 0 0 0 12} QSpinBox::up-button {border-color: white} QSpinBox::down-button {border-color: white} QSpinBox::up-arrow {image: url(:/imagenes/Imagenes/UpTriangle.png)} QSpinBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png)}");
        ui->sizeSpinBox->setStyleSheet("QSpinBox {border: 0px solid; border-radius: 12; background-color: white; padding: 0 0 0 12} QSpinBox::up-button {border-color: white} QSpinBox::down-button {border-color: white} QSpinBox::up-arrow {image: url(:/imagenes/Imagenes/UpTriangle.png)} QSpinBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png)}");
        ui->label_1->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->label_2->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->label_3->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->label_4->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->label_5->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->label_6->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->label_7->setStyleSheet("QLabel {color: rgb(0,0,0)}");
    } else if (temaDefault == 1) {
        // Tema Oscuro
        this->setStyleSheet("QDialog#settingsWindow {background-color: rgb(68,68,68);}");
        ui->videoFolderPathlbl->setStyleSheet("QLabel {border: 0px solid; border-radius:12; background-color: rgb(240,240,240); padding: 0 12 0 12}");
        ui->resultsFolderPathlbl->setStyleSheet("QLabel {border: 0px solid; border-radius:12; background-color: rgb(240,240,240); padding: 0 12 0 12}");
        ui->videoFolderPathbtn->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->resultsFolderPathbtn->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
        ui->fontComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);} QComboBox QAbstractItemView { background-color: rgb(240,240,240);}");
        ui->thicknessSpinBox->setStyleSheet("QSpinBox {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); padding: 0 0 0 12} QSpinBox::up-button {border-color: rgb(240,240,240)} QSpinBox::down-button {border-color: white} QSpinBox::up-arrow {image: url(:/imagenes/Imagenes/UpTriangle.png)} QSpinBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png)}");
        ui->sizeSpinBox->setStyleSheet("QSpinBox {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); padding: 0 0 0 12} QSpinBox::up-button {border-color: rgb(240,240,240)} QSpinBox::down-button {border-color: white} QSpinBox::up-arrow {image: url(:/imagenes/Imagenes/UpTriangle.png)} QSpinBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png)}");
        ui->label_1->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->label_2->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->label_3->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->label_4->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->label_5->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->label_6->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->label_7->setStyleSheet("QLabel {color: rgb(178,178,178)}");
    }
}

settingsWindow::~settingsWindow()
{
    delete ui;
}

void settingsWindow::on_videoFolderPathbtn_clicked()
{
    //----------------------------- Selección de la carpeta -----------------------------
    QString videoFolderTemp = videoFolder;
    videoFolderTemp = QFileDialog::getExistingDirectory(this, tr("Seleccione la carpeta con los archivos de video"), videoFolder, QFileDialog::ShowDirsOnly);
    if (videoFolderTemp != "") {
        videoFolder = videoFolderTemp;
    }
    ui->videoFolderPathlbl->setText(videoFolder);
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    TVASettings.setValue("Video_Folder", videoFolder);
}

void settingsWindow::on_resultsFolderPathbtn_clicked()
{
    //----------------------------- Selección de la carpeta -----------------------------
    QString resultsFolderTemp = resultsFolder;
    resultsFolderTemp = QFileDialog::getExistingDirectory(this, tr("Seleccione la carpeta donde se guardarán los resultados"), resultsFolder, QFileDialog::ShowDirsOnly);
    if (resultsFolderTemp != "") {
        resultsFolder = resultsFolderTemp;
    }
    ui->resultsFolderPathlbl->setText(resultsFolder);
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    TVASettings.setValue("Results_Folder", resultsFolder);
}

void settingsWindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    TVASettings.setValue("Font",f.toString());
}

void settingsWindow::on_thicknessSpinBox_valueChanged(int arg1)
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    TVASettings.setValue("Thickness",arg1);
}

void settingsWindow::on_sizeSpinBox_valueChanged(int arg1)
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    TVASettings.setValue("Font_Size",arg1);
}

// --------------------------------------------------------------------------------------
// TEXTO
// --------------------------------------------------------------------------------------

void settingsWindow::on_textoColorToolButton_clicked()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    QColor tempColor = QColorDialog::getColor(Qt::white, this);
    if (tempColor.isValid() == true) {
        annotationCustomColor = tempColor;
    }
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    }
    TVASettings.setValue("Text_Color",annotationCustomColor);
}

void settingsWindow::on_actionTextoNegro_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    annotationCustomColor = QColor(0,0,0);
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    }
    TVASettings.setValue("Text_Color",annotationCustomColor);
}

void settingsWindow::on_actionTextoBlanco_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    annotationCustomColor = QColor(255,255,255);
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    }
    TVASettings.setValue("Text_Color",annotationCustomColor);
}

void settingsWindow::on_actionTextoAmarillo_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    annotationCustomColor = QColor(255,242,0);
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    }
    TVASettings.setValue("Text_Color",annotationCustomColor);
}

void settingsWindow::on_actionTextoAzul_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    annotationCustomColor = QColor(0,0,255);
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    }
    TVASettings.setValue("Text_Color",annotationCustomColor);
}

void settingsWindow::on_actionTextoRojo_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    annotationCustomColor = QColor(255,0,0);
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    }
    TVASettings.setValue("Text_Color",annotationCustomColor);
}

void settingsWindow::on_actionTextoVerde_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    annotationCustomColor = QColor(0,170,0);
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    }
    TVASettings.setValue("Text_Color",annotationCustomColor);
}

void settingsWindow::on_actionTextCustomColor_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    QColor tempColor = QColorDialog::getColor(Qt::white, this);
    if (tempColor.isValid() == true) {
        annotationCustomColor = tempColor;
    }
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
    }
    TVASettings.setValue("Text_Color",annotationCustomColor);
}

// --------------------------------------------------------------------------------------
// HERRAMIENTAS
// --------------------------------------------------------------------------------------

void settingsWindow::on_ObjectColorToolButton_clicked()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    QColor tempColor = QColorDialog::getColor(Qt::white, this);
    if (tempColor.isValid() == true) {
        customColorObject = tempColor;
    }
    if (temaDefault == 0) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    TVASettings.setValue("Object_Color",customColorObject);
}

void settingsWindow::on_actionObjetoNegro_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    customColorObject = QColor(0,0,0);
    if (temaDefault == 0) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    TVASettings.setValue("Object_Color",customColorObject);
}

void settingsWindow::on_actionObjetoBlanco_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    customColorObject = QColor(255,255,255);
    if (temaDefault == 0) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    TVASettings.setValue("Object_Color",customColorObject);
}

void settingsWindow::on_actionObjetoAmarillo_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    customColorObject = QColor(255,242,0);
    if (temaDefault == 0) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    TVASettings.setValue("Object_Color",customColorObject);
}

void settingsWindow::on_actionObjetoAzul_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    customColorObject = QColor(0,0,255);
    if (temaDefault == 0) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    TVASettings.setValue("Object_Color",customColorObject);
}

void settingsWindow::on_actionObjetoRojo_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    customColorObject = QColor(255,0,0);
    if (temaDefault == 0) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    TVASettings.setValue("Object_Color",customColorObject);
}

void settingsWindow::on_actionObjetoVerde_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    customColorObject = QColor(0,170,0);
    if (temaDefault == 0) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    TVASettings.setValue("Object_Color",customColorObject);
}

void settingsWindow::on_actionObjectCustomColor_triggered()
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    QColor tempColor = QColorDialog::getColor(Qt::white, this);
    if (tempColor.isValid() == true) {
        customColorObject = tempColor;
    }
    if (temaDefault == 0) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    TVASettings.setValue("Object_Color",customColorObject);
}
