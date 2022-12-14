#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "framedisplay.h"
#include "helpwindow.h"
#include "calibracionwindow.h"
#include "annotationwindow.h"
#include "settingswindow.h"
#include "labelwindow.h"

#include <QFileDialog>
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QToolTip>
#include <QMenu>
#include <QColorDialog>

#include <fstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //---------------------------- Valores de Configuración -----------------------------
    QSettings TVASettings("config.ini" , QSettings::IniFormat);                            // Archivo de configuración
    videoFolder = TVASettings.value("Video_Folder","C:\\").toString();                     // Carpeta de videos
    resultsFolder = TVASettings.value("Results_Folder","C:\\").toString();                 // Carpeta de resultados
    languageSet = TVASettings.value("Language",0).toInt();                                 // Idioma seleccionado
    recentVideos = TVASettings.value("Recent_Videos").toStringList();                      // Lista de archivos recientes
    classList = TVASettings.value("Class_List").toStringList();                            // Lista de clases para las etiquetas
    fontSizeDefault = TVASettings.value("Font_Size",10).toInt();                           // Tamaño de la fuente
    annotationFontDefault = TVASettings.value("Font","Arial").toString();                  // Fuente
    annotationCustomColor = TVASettings.value("Text_Color").value<QColor>();
    customColorObject = TVASettings.value("Object_Color").value<QColor>();
    thickness = TVASettings.value("Thickness",1).toInt();
    temaDefault = TVASettings.value("Theme",0).toInt();

    //------------------------------- Creación de widgets -------------------------------
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

    ui->textFontComboBox->setCurrentFont(QFont(annotationFontDefault));
    ui->textSizeSpinBox->setValue(fontSizeDefault);
    annotationBold = false;
    ui->boldFontButton->setChecked(annotationBold);
    annotationItalic = false;
    ui->ItalicFontButton->setChecked(annotationItalic);

    annotationButtonFont = ui->textFontComboBox->currentFont();
    annotationButtonFont.setPointSize(12);
    annotationButtonFont.setBold(annotationBold);
    annotationButtonFont.setItalic(annotationItalic);
    ui->annotationButton->setFont(annotationButtonFont);
    annotationButtonState = false;
    ui->annotationButton->setChecked(annotationButtonState);

    annotationFont = annotationButtonFont;
    annotationFont.setPointSize(ui->textSizeSpinBox->value());
    annotationFont.setBold(annotationBold);
    annotationFont.setItalic(annotationItalic);

    // Herramientas ---------------------------------------------------------------------
    ui->rectanguloButton->setText(QChar(0x2610));
    ui->elipseButton->setText(QChar(0x25EF));

    rectangleToolState = false;
    lineToolState = false;
    ellipseToolState = false;
    ui->rectanguloButton->setChecked(rectangleToolState);
    ui->lineaButton->setChecked(lineToolState);
    ui->elipseButton->setChecked(ellipseToolState);

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

    QMenu *lineThicknessMenu = new QMenu;
    lineThicknessMenu->addAction(ui->action1_px);
    lineThicknessMenu->addAction(ui->action2_px);
    lineThicknessMenu->addAction(ui->action3_px);
    lineThicknessMenu->addAction(ui->action4_px);
    lineThicknessMenu->addAction(ui->action5_px);
    ui->thicknessToolButton->setMenu(lineThicknessMenu);

    if (thickness == 1) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea1.png"));
    } else if (thickness == 2) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea2.png"));
    } else if (thickness == 3) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea3.png"));
    } else if (thickness == 4) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea4.png"));
    } else if (thickness == 5) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea5.png"));
    }

    //------------------------------------ Opciones -------------------------------------
    ui->deshacerButton->setShortcut(QKeySequence("Ctrl+Z"));

    roiToolState = false;
    zoominToolState = false;
    ui->roiButton->setChecked(roiToolState);
    ui->zoomInButton->setChecked(zoominToolState);

    //---------------------------- Definición de Variables ------------------------------
    videoWidth = 0;                                                                                     // Valor inicial del ancho de la ventana del video
    videoHeight = 0;                                                                                    // Valor inicial del largo de la ventana del video
    lineInformation.push_back("\r\n");
    rectangleInformation.push_back("\r\n");
    ellipseInformation.push_back("\r\n");
    frameSelectedState = false;

    // ----------------------------- Definición del idioma ------------------------------
    T.load(":/traducciones/ingles.qm");
    ui->languageComboBox->setCurrentIndex(languageSet);
    on_languageComboBox_activated(languageSet);

    // ------------------------------- Archivos recientes -------------------------------
    if (recentVideos.length() != 0) {
        ui->recientesComboBox->addItems(recentVideos);
        ui->recientesComboBox->addItem(tr("Borrar lista"));
    }
    ui->recientesComboBox->setCurrentIndex(-1);

    // ------------------------------- Definición del tema ------------------------------
    ui->temaComboBox->setCurrentIndex(temaDefault);
    on_temaComboBox_currentIndexChanged(temaDefault);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --------------------------------------------------------------------------------------
// ARCHIVO
// --------------------------------------------------------------------------------------

void MainWindow::on_abrirButton_clicked()
{
    //---------------------------------- Configuración ----------------------------------
    QSettings TVASettings("config.ini" , QSettings::IniFormat);

    //------------------------------ Selección del archivo ------------------------------
    if (videoNameRecent == "") {
        videoName = QFileDialog::getOpenFileName(this, tr("Seleccione el archivo de video"), videoFolder, tr("Archivos de Video (*.avi *.mp4 *.mpg *.m4v *.mkv)"));
    } else {
        videoName = videoNameRecent;
    }
    videoNameRecent.clear();

    //---------------------------- Procesamiento del archivo ----------------------------
    if (videoName != "") {
        QFileInfo videoFile(videoName);
        if (videoFile.exists()) {
            //------------------------------ Reiniciar GUI ------------------------------
            disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMouseMovePos(QPoint&)), this, SLOT(mousePosition(QPoint&)));
            ui->annotationButton->setEnabled(false);
            ui->lineaButton->setEnabled(false);
            ui->rectanguloButton->setEnabled(false);
            ui->elipseButton->setEnabled(false);
            ui->roiButton->setEnabled(false);
            ui->deshacerButton->setEnabled(false);
            ui->zoomInButton->setEnabled(false);
            ui->cargarImagenButton->setEnabled(true);
            ui->saveImageButton->setEnabled(false);
            ui->saveTextButton->setEnabled(false);
            ui->loadPreviousFrameToolButton->setEnabled(false);
            ui->loadNextFrameToolButton->setEnabled(false);
            ui->calibracionButton->setEnabled(false);

            ui->frameSelectedValueLabel->clear();
            ui->frameSelectedImageLabel->clear();
            ui->informationTextEdit->clear();

            //----------------------- Verificación de calibración -----------------------
            QFileInfo CalibrationFile( QString("%1/%2%3").arg(videoFile.canonicalPath()).arg(videoFile.baseName()).arg("_CalibrationData.txt") );
            if (CalibrationFile.exists()) {
                ui->calibracionStatusLabel->setText(QChar(0x2713));
                ui->calibracionStatusLabel->setStyleSheet(QString("color: rgb(0, 170, 0);"));
                calibrationFlag = true;

                //------------------ Lectura de archivo de calibración ------------------
                QFile file( CalibrationFile.absoluteFilePath() );
                file.open(QIODevice::ReadOnly | QIODevice::Text);
                QTextStream in(&file);
                QString lines = in.readAll();
                HTextLines = lines.split(QRegExp("\t|\n"));
            } else {
                ui->calibracionStatusLabel->setText(QChar(0x2717));
                ui->calibracionStatusLabel->setStyleSheet(QString("color: rgb(255, 0, 0);"));
                calibrationFlag = false;
            }

            //----------------------------- Archivo de video ----------------------------
            video.open(videoName.toStdString());
            if(!video.isOpened()) {
                ui->statusBar->showMessage(tr("No se pudo abrir el archivo de video"), 3000);
            } else {
                //------------- Creación de carpeta de resultados del video -------------
                ui->videoNameValueLabel->setText(videoFile.fileName());
                videoResultFolder = QString("%1/%2").arg(resultsFolder).arg(videoFile.baseName());
                if (!QDir(videoResultFolder).exists()) {
                    QDir().mkdir(videoResultFolder);
                }

                //------------------------ Información del video ------------------------
                totalFrames = int(video.get(cv::CAP_PROP_FRAME_COUNT));
                videoWidth = video.get(cv::CAP_PROP_FRAME_WIDTH);
                videoHeight = video.get(cv::CAP_PROP_FRAME_HEIGHT);
                framesPerSecond = video.get(cv::CAP_PROP_FPS);

                ui->videoWidthValueLabel->setText(QString::number(videoWidth));
                ui->videoHeightValueLabel->setText(QString::number(videoHeight));

                valueFrame = 1;
                frame_msec = 1000 / framesPerSecond;
                valueFrameMsec = frame_msec;
                drawVideoFrame();

                timerForwardVideo = new QTimer(this);
                connect(timerForwardVideo, SIGNAL(timeout()), this, SLOT(playVideo()));
                timerBackwardVideo = new QTimer(this);
                connect(timerBackwardVideo, SIGNAL(timeout()), this, SLOT(playBackwardsVideo()));

                //------------------ Ajuste de las ventanas de imágenes -----------------
                if (videoWidth > videoHeight) {
                    frameWidth = (this->width() - 340) / 2;
                    frameHeight = videoHeight * frameWidth / videoWidth;
                } else {
                    frameHeight = this->height() - 190;
                    frameWidth = (frameHeight / videoHeight) * videoWidth;
                }
                ui->infoFrame->setGeometry(10, (this->height() / 2) - (int(frameHeight) / 2) + 5, int(frameWidth), 30);
                ui->videoLabel->setGeometry(10, ui->infoFrame->y() + 30, int(frameWidth), int(frameHeight));
                ui->videoFrame->setGeometry(10, ui->videoLabel->y() + int(frameHeight), int(frameWidth), 30);

                ui->cargarImagenButton->setGeometry(ui->videoLabel->width() + 20, ui->videoLabel->y() + (ui->videoLabel->height() / 2) - 15, 30, 30);

                ui->imageFrame->setGeometry(ui->cargarImagenButton->x() + 40, (this->height() / 2) - (int(frameHeight) / 2) + 5, int(frameWidth), 30);
                ui->frameSelectedImageLabel->setGeometry(ui->imageFrame->x(), ui->imageFrame->y() + 30, int(frameWidth), int(frameHeight));

                ui->textFrame->setGeometry(ui->imageFrame->x() + int(frameWidth) + 10, 100, 260, 30);
                ui->informationTextEdit->setGeometry(ui->textFrame->x(), 130, 260, this->height() - 190);

                frameSelectedState = false; // Estado de selección de cuadro, para keyPressEvent

                // -------------------------- Toolbar de video --------------------------
                speedMultiplier = 1;
                ui->sliderTrack->setMaximum(totalFrames - 1);
                ui->sliderTrack->setValue(valueFrame);
                ui->lineEditFrame->setText(QString("%1").arg(valueFrame));

                ui->SlowPlayButton->setEnabled(true);
                ui->BackwardFrameButton->setEnabled(true);
                ui->BackwardPlayButton->setEnabled(true);
                ui->PauseButton->setEnabled(true);
                ui->ForwardPlayButton->setEnabled(true);
                ui->ForwardFrameButton->setEnabled(true);
                ui->FastPlayButton->setEnabled(true);
                ui->sliderTrack->setEnabled(true);
                ui->lineEditFrame->setEnabled(true);

                //------------------- Actualizar lista de videos recientes ------------------
                int indexVideo = recentVideos.indexOf(videoName);                                           // índice del video seleccionado en la lista
                if (indexVideo != -1) {
                    recentVideos.removeAt(indexVideo);                                                      // Se quita el video de la lista
                }
                recentVideos.push_front(videoName);                                                         // El video se reubica al principio de la lista
                if (recentVideos.length() > 10) {
                    recentVideos.removeLast();                                                              // Si hay más de 10 elementos, quitar el último
                }
                TVASettings.setValue("Recent_Videos",recentVideos);

                //------------- Presentar lista actualizada de videos recientes -------------
                ui->recientesComboBox->clear();
                ui->recientesComboBox->addItems(recentVideos);
                ui->recientesComboBox->addItem(tr("Borrar lista"));

                ui->statusBar->showMessage(QString("%1: %2").arg(tr("Video cargado")).arg(videoName), 2000);
            }
        } else {
            //------------------ Mensaje de error - No existe el archivo ----------------
            QMessageBox *noFileMsgBox = new QMessageBox(QMessageBox::Critical, "Error", tr("El archivo no existe."), QMessageBox::Close);
            noFileMsgBox->exec();

            //------------------- Actualizar lista de videos recientes ------------------
            int indexVideo = recentVideos.indexOf(videoName);           // índice del video seleccionado en la lista
            if (indexVideo != -1) {
                recentVideos.removeAt(indexVideo);                      // Se quita el video de la lista
            }
            ui->recientesComboBox->clear();
            if (recentVideos.length() > 0) {
                ui->recientesComboBox->addItems(recentVideos);
                ui->recientesComboBox->addItem(tr("Borrar lista"));
            }
            TVASettings.setValue("Recent_Videos",recentVideos);
        }
    } else {
        ui->statusBar->showMessage(QString("%1").arg(tr("No se seleccionó un archivo de video")),3000);
    }
}

void MainWindow::on_recientesComboBox_activated(const QString &arg1)
{
    //---------------------------------- Configuración ----------------------------------
    QSettings TVASettings("config.ini" , QSettings::IniFormat);

    //------------------------------ Selección del archivo ------------------------------
    if (arg1 != tr("Borrar lista")) {
        videoNameRecent = arg1;
        on_abrirButton_clicked();
    } else {
        ui->recientesComboBox->clear();
        recentVideos.clear();
        TVASettings.setValue("Recent_Videos", recentVideos);
        ui->recientesComboBox->setCurrentIndex(-1);
    }
}

void MainWindow::on_settingsButton_clicked()
{
    settingsWindow settingsWindowApp;
    settingsWindowApp.exec();

    // ----------------------- Reiniciar Valores de Configuración -----------------------
    QSettings TVASettings("config.ini" , QSettings::IniFormat);                            // Archivo de configuración
    videoFolder = TVASettings.value("Video_Folder","C:\\").toString();                     // Carpeta de videos
    resultsFolder = TVASettings.value("Results_Folder","C:\\").toString();                 // Carpeta de resultados
    languageSet = TVASettings.value("Language",0).toInt();                                 // Idioma seleccionado
    fontSizeDefault = TVASettings.value("Font_Size",10).toInt();                           // Tamaño de la fuente
    annotationFontDefault = TVASettings.value("Font","Arial").toString();                  // Fuente
    annotationCustomColor = TVASettings.value("Text_Color").value<QColor>();
    customColorObject = TVASettings.value("Object_Color").value<QColor>();
    thickness = TVASettings.value("Thickness",1).toInt();
    temaDefault = TVASettings.value("Theme",0).toInt();

    // ------------------------- Reajustar GUI a nuevos valores -------------------------
    if (temaDefault == 0) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    } else if (temaDefault == 1) {
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
    }
    ui->textSizeSpinBox->setValue(fontSizeDefault);
    ui->textFontComboBox->setCurrentFont(QFont(annotationFontDefault));

    if (thickness == 1) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea1.png"));
    } else if (thickness == 2) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea2.png"));
    } else if (thickness == 3) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea3.png"));
    } else if (thickness == 4) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea4.png"));
    } else if (thickness == 5) {
        ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea5.png"));
    }
}

void MainWindow::on_languageComboBox_activated(int index)
{
    //---------------------------------- Configuración ----------------------------------
    QSettings TVASettings("config.ini" , QSettings::IniFormat);

    //------------------------------ Definición del idioma ------------------------------
    if (index == 0) {
        languageSet = 0;
        TVASettings.setValue("Language",languageSet);

        qApp->removeTranslator(&T);
        ui->retranslateUi(this);

        // Main form ------------------------------------------------------------------------
        lineTitle = "Líneas:\r\nFrame,Id,P1_x,P1_y,P2_x,P2_y,d_px,d_m\r\n";
        rectangleTitle = "Rectángulos:\r\nFrame,Id,Label,TL_x,TL_y,BR_x,BR_y,c_x,c_y\r\n";
        ellipseTitle = "Elipses:\r\nFrame,Id,Label,TL_x,TL_y,BR_x,BR_y,c_x,c_y\r\n";
    } else if (index == 1) {
        languageSet = 1;
        TVASettings.setValue("Language",languageSet);

        qApp->installTranslator(&T);
        ui->retranslateUi(this);

        // Main form ------------------------------------------------------------------------
        lineTitle = "Lines:\r\nFrame,Id,P1_x,P1_y,P2_x,P2_y,d_px,d_m\r\n";
        rectangleTitle = "Rectangles:\r\nFrame,Id,Label,TL_x,TL_y,BR_x,BR_y,c_x,c_y\r\n";
        ellipseTitle = "Ellipses:\r\nFrame,Id,Label,TL_x,TL_y,BR_x,BR_y,c_x,c_y\r\n";
    }
    lineInformation[0] = lineTitle;
    rectangleInformation[0] = rectangleTitle;
    ellipseInformation[0] = ellipseTitle;

    QString fullText;
    if (lineInformation.length() > 1) {
        for (int i = 0; i < lineInformation.length(); i++) {
            fullText.append(QString("%1").arg(lineInformation.at(i)));
        }
        fullText.append("\r\n");
    }
    if (rectangleInformation.length() > 1) {
        for (int i = 0; i < rectangleInformation.length(); i++){
            fullText.append(QString("%1").arg(rectangleInformation.at(i)));
        }
        fullText.append("\r\n");
    }
    if (ellipseInformation.length() > 1) {
        for (int i = 0; i < ellipseInformation.length(); i++) {
            fullText.append(QString("%1").arg(ellipseInformation.at(i)));
        }
    }
    ui->informationTextEdit->setPlainText(fullText);
}

void MainWindow::on_temaComboBox_currentIndexChanged(int index)
{
    QSettings TVASettings("config.ini" , QSettings::IniFormat);
    annotationCustomColor = TVASettings.value("Text_Color").value<QColor>();
    customColorObject = TVASettings.value("Object_Color").value<QColor>();
    if (index == 0) {
        // Tema Claro
        this->setStyleSheet("QMainWindow#MainWindow {background-color: rgb(240,240,240);}");
        ui->frame->setStyleSheet("QFrame#frame {background-color: rgb(240,240,240);}");
        ui->statusBar->setStyleSheet("color: rgb(0,0,0)");
        // Archivo ----------------------------------------------------------------------
        ui->abrirButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->settingsButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->recientesComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: white; padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(255, 255, 255);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->languageComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: white; padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(255, 255, 255);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->temaComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: white; padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(255, 255, 255);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        // Texto ------------------------------------------------------------------------
        ui->textFontComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: white; padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(255, 255, 255);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->textSizeSpinBox->setStyleSheet("QSpinBox {border: 0px solid; border-radius: 12; background-color: white; padding: 0 0 0 12} QSpinBox::up-button {border-color: white} QSpinBox::down-button {border-color: white} QSpinBox::up-arrow {image: url(:/imagenes/Imagenes/UpTriangle.png)} QSpinBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png)}");
        ui->boldFontButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->ItalicFontButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
        ui->annotationButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        // Formas -----------------------------------------------------------------------
        ui->thicknessToolButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12;	background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; 	border-radius: 12;	background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
        ui->rectanguloButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->lineaButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->elipseButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        // Opciones ---------------------------------------------------------------------
        ui->zoomInButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->zoomOutButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->roiButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->deshacerButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        // Modo -------------------------------------------------------------------------
        ui->loadPreviousFrameToolButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->loadNextFrameToolButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        // Ayuda ------------------------------------------------------------------------
        ui->manualButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->aboutButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        ui->aboutQtButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)}");
        // Video Info -------------------------------------------------------------------
        ui->videoNameLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->videoNameValueLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->videoWidthLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->videoWidthValueLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->videoHeightLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->videoHeightValueLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->calibracionLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->calibracionButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        // Video ------------------------------------------------------------------------
        ui->SlowPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->BackwardFrameButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->BackwardPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->PauseButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->ForwardPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->ForwardFrameButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->FastPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->lineEditFrame->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: white;} QLineEdit:!Enabled {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178);}");
        // Cargar imagen ----------------------------------------------------------------
        ui->cargarImagenButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        // Imagen Info ------------------------------------------------------------------
        ui->frameSelectedValueLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->frameSelectedLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        ui->imageNameLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        // Imagen -----------------------------------------------------------------------
        ui->imageNameValueText->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: white; padding: 0 12 0 12}");
        ui->saveImageButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        // Texto Info -------------------------------------------------------------------
        ui->textNameLabel->setStyleSheet("QLabel {color: rgb(0,0,0)}");
        // Texto ------------------------------------------------------------------------
        ui->textNameValueText->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: white; padding: 0 12 0 12}");
        ui->saveTextButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)}");
        ui->informationTextEdit->setStyleSheet("background-color: white;");
    } else if (index == 1) {
        // Tema Oscuro
        this->setStyleSheet("QMainWindow#MainWindow {background-color: rgb(68,68,68);}");
        ui->frame->setStyleSheet("QFrame#frame {background-color: rgb(178,178,178);}");
        ui->statusBar->setStyleSheet("color: rgb(240,240,240)");
        // Archivo ----------------------------------------------------------------------
        ui->abrirButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->settingsButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->recientesComboBox->setStyleSheet("QComboBox {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->languageComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->temaComboBox->setStyleSheet("QComboBox {border: 0px solid;	border-radius: 12; background-color: rgb(240,240,240); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        // Texto ------------------------------------------------------------------------
        ui->textFontComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->textSizeSpinBox->setStyleSheet("QSpinBox {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); padding: 0 0 0 12} QSpinBox::up-button {border-color: rgb(240,240,240)} QSpinBox::down-button {border-color: white} QSpinBox::up-arrow {image: url(:/imagenes/Imagenes/UpTriangle.png)} QSpinBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png)}");
        ui->boldFontButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->ItalicFontButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
        ui->annotationButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        // Formas -----------------------------------------------------------------------
        ui->thicknessToolButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12;	background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)} QToolButton::menu-button { border-color: rgb(240,240,240);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
        ui->rectanguloButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->lineaButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->elipseButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        // Opciones ---------------------------------------------------------------------
        ui->zoomInButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->zoomOutButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->roiButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->deshacerButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        // Modo -------------------------------------------------------------------------
        ui->loadPreviousFrameToolButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->loadNextFrameToolButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        // Ayuda ------------------------------------------------------------------------
        ui->manualButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->aboutButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        ui->aboutQtButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240)}");
        // Video Info -------------------------------------------------------------------
        ui->videoNameLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->videoNameValueLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->videoWidthLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->videoWidthValueLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->videoHeightLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->videoHeightValueLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->calibracionLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->calibracionButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        // Video ------------------------------------------------------------------------
        ui->SlowPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->BackwardFrameButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->BackwardPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->PauseButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->ForwardPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->ForwardFrameButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->FastPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->lineEditFrame->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240);} QLineEdit:!Enabled {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178);}");
        // Cargar imagen ----------------------------------------------------------------
        ui->cargarImagenButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        // Imagen Info ------------------------------------------------------------------
        ui->frameSelectedValueLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->frameSelectedLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        ui->imageNameLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        // Imagen -----------------------------------------------------------------------
        ui->imageNameValueText->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); padding: 0 12 0 12}");
        ui->saveImageButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        // Texto Info -------------------------------------------------------------------
        ui->textNameLabel->setStyleSheet("QLabel {color: rgb(178,178,178)}");
        // Texto ------------------------------------------------------------------------
        ui->textNameValueText->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); padding: 0 12 0 12}");
        ui->saveTextButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(178,178,178)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240)}");
        ui->informationTextEdit->setStyleSheet("background-color: rgb(178,178,178)");
    }  else if (index == 2) {
        // Tema Azul Oscuro
        this->setStyleSheet("QMainWindow#MainWindow {background-color: rgb(35,52,68);}");
        ui->frame->setStyleSheet("QFrame#frame {background-color: rgb(53,113,167);}");
        ui->statusBar->setStyleSheet("color: rgb(140,191,234)");
        // Archivo ----------------------------------------------------------------------
        ui->abrirButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->settingsButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->recientesComboBox->setStyleSheet("QComboBox {border: 0px solid;	border-radius: 12; background-color: rgb(140,191,234); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->languageComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->temaComboBox->setStyleSheet("QComboBox {border: 0px solid;	border-radius: 12; background-color: rgb(140,191,234); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        // Texto ------------------------------------------------------------------------
        ui->textFontComboBox->setStyleSheet("QComboBox {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234); padding: 0 12 0 12} QComboBox::drop-down { border-color: rgb(240, 240, 240);} QComboBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->textSizeSpinBox->setStyleSheet("QSpinBox {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234); padding: 0 0 0 12} QSpinBox::up-button {border-color: rgb(140,191,234)} QSpinBox::down-button {border-color: white} QSpinBox::up-arrow {image: url(:/imagenes/Imagenes/UpTriangle.png)} QSpinBox::down-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png)}");
        ui->boldFontButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->ItalicFontButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(53,113,167); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)} QToolButton::menu-button { border-color: rgb(140,191,234);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
        ui->annotationButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        // Formas -----------------------------------------------------------------------
        ui->thicknessToolButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12;	background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)} QToolButton::menu-button { border-color: rgb(140,191,234);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}");
        ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)} QToolButton::menu-button { border-color: rgb(140,191,234);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
        ui->rectanguloButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->lineaButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->elipseButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        // Opciones ---------------------------------------------------------------------
        ui->zoomInButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->zoomOutButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->roiButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->deshacerButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        // Modo -------------------------------------------------------------------------
        ui->loadPreviousFrameToolButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->loadNextFrameToolButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        // Ayuda ------------------------------------------------------------------------
        ui->manualButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->aboutButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        ui->aboutQtButton->setStyleSheet("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234)}");
        // Video Info -------------------------------------------------------------------
        ui->videoNameLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->videoNameValueLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->videoWidthLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->videoWidthValueLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->videoHeightLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->videoHeightValueLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->calibracionLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->calibracionButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        // Video ------------------------------------------------------------------------
        ui->SlowPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->BackwardFrameButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->BackwardPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->PauseButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->ForwardPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->ForwardFrameButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->FastPlayButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->lineEditFrame->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234);} QLineEdit:!Enabled {border: 0px solid; border-radius: 12; background-color: rgb(53,113,167);}");
        // Cargar imagen ----------------------------------------------------------------
        ui->cargarImagenButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        // Imagen Info ------------------------------------------------------------------
        ui->frameSelectedValueLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->frameSelectedLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        ui->imageNameLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        // Imagen -----------------------------------------------------------------------
        ui->imageNameValueText->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234); padding: 0 12 0 12}");
        ui->saveImageButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        // Texto Info -------------------------------------------------------------------
        ui->textNameLabel->setStyleSheet("QLabel {color: rgb(140,191,234)}");
        // Texto ------------------------------------------------------------------------
        ui->textNameValueText->setStyleSheet("QLineEdit {border: 0px solid; border-radius: 12; background-color: rgb(140,191,234); padding: 0 12 0 12}");
        ui->saveTextButton->setStyleSheet("QToolButton {border: 0px solid;	border-radius: 12; background-color: rgb(53,113,167)} QToolButton:hover {border: 0px solid; border-radius: 12;	background-color: rgb(140,191,234)}");
        ui->informationTextEdit->setStyleSheet("background-color: rgb(140,191,234)");
    }
    temaDefault = index;
    TVASettings.setValue("Theme",temaDefault);
}

// --------------------------------------------------------------------------------------
// AYUDA
// --------------------------------------------------------------------------------------

void MainWindow::on_manualButton_clicked()
{
    HelpWindow helpVideoApp;
    helpVideoApp.exec();
}

void MainWindow::on_aboutButton_clicked()
{
    //------------------------------ Definición del idioma ------------------------------
    QString message = tr("TRAFFIC VIDEO ANNOTATOR V1.0\n"
                      "\n"
                      "Desarrollado por:\n"
                      "\n"
                      "CARLOS ANDRÉS WILCHES PÉREZ\n"
                      "INGENIERO ELECTRÓNICO, MSc.\n"
                      "Contacto: c.wilches@javeriana.edu.co\n"
                      "\n"
                      "JULIÁN QUIROGA SEPÚLVEDA\n"
                      "INGENIERO ELECTRÓNICO, PhD.\n"
                      "Contacto: quiroga.j@javeriana.edu.co\n"
                      "\n"
                      "PONTIFICIA UNIVERSIDAD JAVERIANA\n"
                      "FACULTAD DE INGENIERÍA\n"
                      "DEPARTAMENTO DE INGENIERÍA ELECTRÓNICA\n"
                      "\n"
                      "Agradecimientos especiales a:\n"
                      "\n"
                      "ALEJANDRO FORERO GUZMÁN\n"
                      "INGENIERO ELECTRÓNICO, MSc.\n"
                      "Gerente del Sistema Inteligente de Transporte\n"
                      "Contacto: alejandro.forero@gmail.com\n"
                      "\n"
                      "SECRETARÍA DISTRITAL DE MOVILIDAD\n"
                      "ALCALDÍA MAYOR DE BOGOTÁ"
                      "\n"
                      "2018");

    //------------------------------- Mensaje Acerca de... ------------------------------
    QMessageBox about;
    about.setWindowTitle(tr("Acerca de..."));
    about.setWindowIcon(QIcon(QPixmap(":/imagenes/Imagenes/Icono.png")));
    about.setText(message);
    about.setIconPixmap(QPixmap(":/imagenes/Imagenes/escudoPuj.png"));
    about.setStandardButtons(QMessageBox::Ok);
    about.setDefaultButton(QMessageBox::Ok);
    about.setStyleSheet("QLabel{min-width: 300px;}");
    about.exec();
}

void MainWindow::on_aboutQtButton_clicked()
{
    QMessageBox::aboutQt(this, tr("Acerca de Qt"));
}

// --------------------------------------------------------------------------------------
// VIDEO
// --------------------------------------------------------------------------------------

void MainWindow::on_SlowPlayButton_clicked()
{
    if (speedMultiplier > 1) {
        speedMultiplier--;
        if (timerBackwardVideo->isActive() == true) {
            ui->statusBar->showMessage(QString(tr("Retrocediendo... (%1x)")).arg(speedMultiplier));
        } else if (timerForwardVideo->isActive() == true) {
            ui->statusBar->showMessage(QString(tr("Reproduciendo... (%1x)")).arg(speedMultiplier));
        } else {
            ui->statusBar->showMessage(QString(tr("Velocidad: %1x")).arg(speedMultiplier));
        }
    }
}

void MainWindow::on_BackwardFrameButton_clicked()
{
    valueFrame = ui->sliderTrack->value() - 1;
    valueFrameMsec -= frame_msec;
    drawVideoFrame();

    ui->lineEditFrame->setText(QString("%1").arg(valueFrame));
    ui->sliderTrack->setValue(valueFrame);
    ui->statusBar->showMessage(tr("Cuadro previo"),2000);
}

void MainWindow::on_BackwardPlayButton_clicked()
{
    if (timerForwardVideo->isActive() == true) {
        timerForwardVideo->stop();
    }
    ui->cargarImagenButton->setEnabled(false);
    timerBackwardVideo->start(int(frame_msec));
    ui->statusBar->showMessage(QString(tr("Retrocediendo... (%1x)")).arg(speedMultiplier));
}

void MainWindow::on_PauseButton_clicked()
{
    if (timerForwardVideo->isActive() == true) {
        timerForwardVideo->stop();
    } else {
        timerBackwardVideo->stop();
    }
    ui->cargarImagenButton->setEnabled(true);
    ui->statusBar->showMessage(tr("Pausado"));
}

void MainWindow::on_ForwardPlayButton_clicked()
{
    if (timerBackwardVideo->isActive() == true) {
        timerBackwardVideo->stop();
    }
    ui->cargarImagenButton->setEnabled(false);
    timerForwardVideo->start(int(frame_msec));
    ui->statusBar->showMessage(QString(tr("Reproduciendo... (%1x)")).arg(speedMultiplier));
}

void MainWindow::on_ForwardFrameButton_clicked()
{
    valueFrame = ui->sliderTrack->value() + 1;
    valueFrameMsec += frame_msec;
    drawVideoFrame();

    ui->lineEditFrame->setText(QString("%1").arg(valueFrame));
    ui->sliderTrack->setValue(valueFrame);
    ui->statusBar->showMessage(tr("Cuadro siguiente"),2000);
}

void MainWindow::on_FastPlayButton_clicked()
{
    if (speedMultiplier < 10) {
        speedMultiplier++;
        if (timerBackwardVideo->isActive() == true) {
            ui->statusBar->showMessage(QString(tr("Retrocediendo... (%1x)")).arg(speedMultiplier));
        } else if (timerForwardVideo->isActive() == true) {
            ui->statusBar->showMessage(QString(tr("Reproduciendo... (%1x)")).arg(speedMultiplier));
        } else {
            ui->statusBar->showMessage(QString(tr("Velocidad: %1x")).arg(speedMultiplier));
        }
    }
}

void MainWindow::on_sliderTrack_sliderMoved(int position)
{
    valueFrame = position;
    valueFrameMsec = frame_msec * valueFrame;
    drawVideoFrame();
    ui->lineEditFrame->setText(QString("%1").arg(valueFrame));
}

void MainWindow::on_lineEditFrame_returnPressed()
{
    if ((ui->lineEditFrame->text().toInt() <= totalFrames) && (ui->lineEditFrame->text().toInt() > 0)) {
        valueFrame = ui->lineEditFrame->text().toInt();
        valueFrameMsec = frame_msec * valueFrame;
        drawVideoFrame();

        ui->sliderTrack->setValue(valueFrame);
    } else {
        ui->statusBar->showMessage(QString(tr("Número de cuadro no permitido. Rango permitido: 1 - %1")).arg(totalFrames - 1));
    }
}

void MainWindow::drawVideoFrame()
{
    video.set(cv::CAP_PROP_POS_MSEC, valueFrameMsec);
    video.read(trackFrame);
    QImage qtrackframe = matToQImage(trackFrame);
    ui->videoLabel->setPixmap(QPixmap::fromImage(qtrackframe));
}

void MainWindow::playVideo()
{
    if (valueFrame <= totalFrames) {
        valueFrame = valueFrame + speedMultiplier;
        valueFrameMsec += speedMultiplier * frame_msec;
        drawVideoFrame();

        ui->lineEditFrame->setText(QString("%1").arg(valueFrame));
        ui->sliderTrack->setValue(valueFrame);
    }
}

void MainWindow::playBackwardsVideo()
{
    if (valueFrame > 0) {
        valueFrame = valueFrame - speedMultiplier;
        valueFrameMsec -= speedMultiplier * frame_msec;
        drawVideoFrame();

        ui->lineEditFrame->setText(QString("%1").arg(valueFrame));
        ui->sliderTrack->setValue(valueFrame);
    }
}

// --------------------------------------------------------------------------------------
// HERRAMIENTA ANOTACIÓN
// --------------------------------------------------------------------------------------

void MainWindow::on_textFontComboBox_currentFontChanged()
{
    annotationButtonFont = ui->textFontComboBox->currentFont();
    annotationButtonFont.setPointSize(12);
    annotationButtonFont.setBold(annotationBold);
    annotationButtonFont.setItalic(annotationItalic);
    ui->annotationButton->setFont(annotationButtonFont);

    annotationFont = ui->textFontComboBox->currentFont();
    annotationFont.setPointSize(ui->textSizeSpinBox->value());
    annotationFont.setBold(annotationBold);
    annotationFont.setItalic(annotationItalic);
}

void MainWindow::on_textSizeSpinBox_valueChanged(int arg1)
{
    annotationFont = ui->textFontComboBox->currentFont();
    annotationFont.setPointSize(arg1);
    annotationFont.setBold(annotationBold);
    annotationFont.setItalic(annotationItalic);
}

void MainWindow::on_boldFontButton_clicked()
{
    if (annotationBold == false) {
        annotationBold = true;
    } else {
        annotationBold = false;
    }
    ui->boldFontButton->setChecked(annotationBold);

    annotationButtonFont = ui->textFontComboBox->currentFont();
    annotationButtonFont.setPointSize(20);
    annotationButtonFont.setBold(annotationBold);
    annotationButtonFont.setItalic(annotationItalic);
    ui->annotationButton->setFont(annotationButtonFont);

    annotationFont = ui->textFontComboBox->currentFont();
    annotationFont.setPointSize(ui->textSizeSpinBox->value());
    annotationFont.setBold(annotationBold);
    annotationFont.setItalic(annotationItalic);
}

void MainWindow::on_ItalicFontButton_clicked()
{
    if (annotationItalic == false) {
        annotationItalic = true;
    } else {
        annotationItalic = false;
    }
    ui->ItalicFontButton->setChecked(annotationItalic);

    annotationButtonFont = ui->textFontComboBox->currentFont();
    annotationButtonFont.setPointSize(20);
    annotationButtonFont.setBold(annotationBold);
    annotationButtonFont.setItalic(annotationItalic);
    ui->annotationButton->setFont(annotationButtonFont);

    annotationFont = ui->textFontComboBox->currentFont();
    annotationFont.setPointSize(ui->textSizeSpinBox->value());
    annotationFont.setBold(annotationBold);
    annotationFont.setItalic(annotationItalic);
}

void MainWindow::on_textoColorToolButton_clicked()
{
    QColor tempColor = QColorDialog::getColor(Qt::white, this);
    if (tempColor.isValid() == true) {
        annotationCustomColor = tempColor;
    }
    ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
}

void MainWindow::on_actionTextoNegro_triggered()
{
    annotationCustomColor = QColor(0,0,0);
    ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
}

void MainWindow::on_actionTextoBlanco_triggered()
{
    annotationCustomColor = QColor(255,255,255);
    ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
}

void MainWindow::on_actionTextoAmarillo_triggered()
{
    annotationCustomColor = QColor(255,242,0);
    ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
}

void MainWindow::on_actionTextoAzul_triggered()
{
    annotationCustomColor = QColor(0,0,255);
    ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
}

void MainWindow::on_actionTextoRojo_triggered()
{
    annotationCustomColor = QColor(255,0,0);
    ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
}

void MainWindow::on_actionTextoVerde_triggered()
{
    annotationCustomColor = QColor(0,170,0);
    ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
}

void MainWindow::on_actionTextCustomColor_triggered()
{
    QColor tempColor = QColorDialog::getColor(Qt::white, this);
    if (tempColor.isValid() == true) {
        annotationCustomColor = tempColor;
    }
    ui->textoColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12;	background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(annotationCustomColor.red()).arg(annotationCustomColor.green()).arg(annotationCustomColor.blue()));
}

void MainWindow::on_annotationButton_clicked()
{
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
    rectangleToolState = false;
    ui->rectanguloButton->setChecked(rectangleToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
    lineToolState = false;
    ui->lineaButton->setChecked(lineToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
    ellipseToolState = false;
    ui->elipseButton->setChecked(ellipseToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
    roiToolState = false;
    ui->roiButton->setChecked(roiToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
    zoominToolState = false;
    ui->zoomInButton->setChecked(zoominToolState);

    if (annotationButtonState == true) {
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
        annotationButtonState = false;
    } else {
        ui->frameSelectedImageLabel->setCursor(Qt::IBeamCursor);
        connect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
        annotationButtonState = true;
    }
    ui->annotationButton->setChecked(annotationButtonState);
}

void MainWindow::mouseTextDraw(QPoint &pos)
{
    //------------------------------- Corrección de puntos ------------------------------
    pointsX.push_back(int(pos.x() * (videoWidth / frameWidth)));
    pointsY.push_back(int(pos.y() * (videoHeight / frameHeight)));

    //------------------------------ Definición de etiqueta -----------------------------
    QPoint windowPosition;
    windowPosition.setX(pos.x());
    windowPosition.setY(pos.y());
    QPoint framePosition = ui->frameSelectedImageLabel->mapToGlobal(windowPosition);

    AnnotationWindow annotationText;
    annotationText.move(framePosition.x(), framePosition.y());
    annotationText.exec();

    //-------------------------------- Dibujar anotación --------------------------------
    imageFrame = imageDeshacer.last();

    QPainter paint;
    paint.begin(&imageFrame);
    QPen paintPen(annotationCustomColor);
    paint.setPen(paintPen);
    paint.setFont(annotationFont);
    paint.drawText(pointsX.at(0), pointsY.at(0), annotationText.anotacion);

    ui->frameSelectedImageLabel->setPixmap(imageFrame);

    paint.end();

    //----------------------------------- Información -----------------------------------
    imageDeshacer.push_back(imageFrame);
    contadorDeshacer.push_back(contadorDeshacer.last());
    etiquetasDeshacer.push_back(annotationText.anotacion);
    textoDeshacer.push_back(ui->informationTextEdit->toPlainText());
    referencePoints.push_back(QPointF( -1 , -1));

    //---------------------------------- Reajustar GUI ----------------------------------
    pointsX.clear();
    pointsY.clear();
    ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
    annotationButtonState = false;
    ui->annotationButton->setChecked(annotationButtonState);
    ui->deshacerButton->setEnabled(true);
}

// --------------------------------------------------------------------------------------
// HERRAMIENTAS FORMAS
// --------------------------------------------------------------------------------------

void MainWindow::on_ObjectColorToolButton_clicked()
{
    QColor tempColor = QColorDialog::getColor(Qt::white, this);
    if (tempColor.isValid() == true) {
        customColorObject = tempColor;
    }
    ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
}

void MainWindow::on_actionObjetoNegro_triggered()
{
    customColorObject = QColor(0,0,0);
    ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
}

void MainWindow::on_actionObjetoBlanco_triggered()
{
    customColorObject = QColor(255,255,255);
    ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
}

void MainWindow::on_actionObjetoAmarillo_triggered()
{
    customColorObject = QColor(255,242,0);
    ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
}

void MainWindow::on_actionObjetoAzul_triggered()
{
    customColorObject = QColor(0,0,255);
    ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
}

void MainWindow::on_actionObjetoRojo_triggered()
{
    customColorObject = QColor(255,0,0);
    ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
}

void MainWindow::on_actionObjetoVerde_triggered()
{
    customColorObject = QColor(0,170,0);
    ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
}

void MainWindow::on_actionObjectCustomColor_triggered()
{
    QColor tempColor = QColorDialog::getColor(Qt::white, this);
    if (tempColor.isValid() == true) {
        customColorObject = tempColor;
    }
    ui->ObjectColorToolButton->setStyleSheet(QString("QToolButton {border: 0px solid; border-radius: 12; background-color: rgb(240,240,240); color: rgb(%1, %2, %3)} QToolButton:hover {border: 0px solid; border-radius: 12; background-color: rgb(178,178,178)} QToolButton::menu-button { border-color: rgb(178,178,178);} QToolButton::menu-arrow {image: url(:/imagenes/Imagenes/DownTriangle.png);}").arg(customColorObject.red()).arg(customColorObject.green()).arg(customColorObject.blue()));
}

void MainWindow::on_thicknessToolButton_clicked()
{
    ui->thicknessToolButton->showMenu();
}

void MainWindow::on_action1_px_triggered()
{
    ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea1.png"));
    thickness = 1;
}

void MainWindow::on_action2_px_triggered()
{
    ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea2.png"));
    thickness = 2;
}

void MainWindow::on_action3_px_triggered()
{
    ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea3.png"));
    thickness = 3;
}

void MainWindow::on_action4_px_triggered()
{
    ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea4.png"));
    thickness = 4;
}

void MainWindow::on_action5_px_triggered()
{
    ui->thicknessToolButton->setIcon(QIcon(":/imagenes/Imagenes/Linea5.png"));
    thickness = 5;
}

void MainWindow::on_rectanguloButton_clicked()
{
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
    annotationButtonState = false;
    ui->annotationButton->setChecked(annotationButtonState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
    lineToolState = false;
    ui->lineaButton->setChecked(lineToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
    ellipseToolState = false;
    ui->elipseButton->setChecked(ellipseToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
    roiToolState = false;
    ui->roiButton->setChecked(roiToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
    zoominToolState = false;
    ui->zoomInButton->setChecked(zoominToolState);

    if (rectangleToolState == true) {
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
        rectangleToolState = false;
    } else {
        ui->frameSelectedImageLabel->setCursor(Qt::CrossCursor);
        connect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
        rectangleToolState = true;
    }
    ui->rectanguloButton->setChecked(rectangleToolState);
}

void MainWindow::on_lineaButton_clicked()
{
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
    annotationButtonState = false;
    ui->annotationButton->setChecked(annotationButtonState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
    rectangleToolState = false;
    ui->rectanguloButton->setChecked(rectangleToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
    ellipseToolState = false;
    ui->elipseButton->setChecked(ellipseToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
    roiToolState = false;
    ui->roiButton->setChecked(roiToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
    zoominToolState = false;
    ui->zoomInButton->setChecked(zoominToolState);

    if (lineToolState == true) {
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
        lineToolState = false;
    } else {
        ui->frameSelectedImageLabel->setCursor(Qt::CrossCursor);
        connect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
        lineToolState = true;
    }
    ui->lineaButton->setChecked(lineToolState);
}

void MainWindow::on_elipseButton_clicked()
{
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
    annotationButtonState = false;
    ui->annotationButton->setChecked(annotationButtonState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
    rectangleToolState = false;
    ui->rectanguloButton->setChecked(rectangleToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
    lineToolState = false;
    ui->lineaButton->setChecked(lineToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
    roiToolState = false;
    ui->roiButton->setChecked(roiToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
    zoominToolState = false;
    ui->zoomInButton->setChecked(zoominToolState);

    if (ellipseToolState == true) {
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
        ellipseToolState = false;
    } else {
        ui->frameSelectedImageLabel->setCursor(Qt::CrossCursor);
        connect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
        ellipseToolState = true;
    }
    ui->elipseButton->setChecked(ellipseToolState);
}

void MainWindow::mouseRectangleDraw(QPoint &pos)
{
    //------------------------------- Corrección de puntos ------------------------------
    pointsX.push_back(int(pos.x() * (videoWidth / frameWidth)));
    pointsY.push_back(int(pos.y() * (videoHeight / frameHeight)));

    //---------------------------- Activación de rubber band ----------------------------
    fixed_point = pos;
    if (pointsX.length() == 1) {
        rubberBand = new QRubberBand(QRubberBand::Rectangle, ui->frameSelectedImageLabel);
        rubberBand->setGeometry(QRect( QPoint( fixed_point.x(), fixed_point.y() ) , QPoint( fixed_point.x() + 1, fixed_point.y() + 1 ) ) );
        rubberBand->show();
        rubberBandState = true;

        //----------------------- Bloqueo de botones temporalmente ----------------------
        ui->annotationButton->setEnabled(false);
        ui->lineaButton->setEnabled(false);
        ui->rectanguloButton->setEnabled(false);
        ui->elipseButton->setEnabled(false);
        ui->roiButton->setEnabled(false);
        ui->deshacerButton->setEnabled(false);
        ui->zoomInButton->setEnabled(false);
        ui->zoomOutButton->setEnabled(false);
        ui->saveImageButton->setEnabled(false);
        ui->saveTextButton->setEnabled(false);
        ui->loadPreviousFrameToolButton->setEnabled(false);
        ui->loadNextFrameToolButton->setEnabled(false);
        ui->calibracionButton->setEnabled(false);
        ui->cargarImagenButton->setEnabled(false);
    } else if (pointsX.length() == 2) {
        //--------------------------- Reajustar puntos al zoom --------------------------
        if (scaleFactor > 1) {
            pointsX[0] = int((pointsX.at(0) / scaleFactor) + positionCorrection.at(0));
            pointsY[0] = int((pointsY.at(0) / scaleFactor) + positionCorrection.at(1));
            pointsX[1] = int((pointsX.at(1) / scaleFactor) + positionCorrection.at(0));
            pointsY[1] = int((pointsY.at(1) / scaleFactor) + positionCorrection.at(1));
            scaleFactor = 1;
            positionCorrection.clear();
        }

        //--------------------- Definición de esquina, ancho y alto ---------------------
        int puntoSI_X, puntoSI_Y, anchoSeleccionado, altoSeleccionado;
        if (pointsX.at(0) > pointsX.at(1)) {
            puntoSI_X = pointsX.at(1);
            anchoSeleccionado = pointsX.at(0) - pointsX.at(1);
        } else {
            puntoSI_X = pointsX.at(0);
            anchoSeleccionado = pointsX.at(1) - pointsX.at(0);
        }
        if (pointsY.at(0) < pointsY.at(1)) {
            puntoSI_Y = pointsY.at(0);
            altoSeleccionado = pointsY.at(1) - pointsY.at(0);
        } else {
            puntoSI_Y = pointsY.at(1);
            altoSeleccionado = pointsY.at(0) - pointsY.at(1);
        }

        //---------------------------- Definición de etiqueta ---------------------------
        QPoint windowPosition;
        windowPosition.setX(pos.x());
        windowPosition.setY(pos.y());imageFrame = imageDeshacer.last();
        QPoint framePosition = ui->frameSelectedImageLabel->mapToGlobal(windowPosition);

        LabelWindow labelText;
        labelText.move(framePosition.x(), framePosition.y());
        labelText.exec();

        contadorDeshacer.push_back(contadorDeshacer.last() + 1);
        etiquetasDeshacer.push_back(QString("%1 %2").arg(contadorDeshacer.last()).arg(labelText.anotacion));

        //----------------------- Dibujar rectángulo y anotación ------------------------
        imageFrame = imageDeshacer.last();

        QPainter paint;
        paint.begin(&imageFrame);
        QPen paintPen(customColorObject);
        paintPen.setWidth(thickness);
        paint.setPen(paintPen);
        paint.drawRect(puntoSI_X, puntoSI_Y, anchoSeleccionado, altoSeleccionado);
        paint.drawEllipse(puntoSI_X + (anchoSeleccionado / 2) - 2, puntoSI_Y + (altoSeleccionado / 2) - 2, 4, 4);

        paintPen.setColor(annotationCustomColor);
        paint.setPen(paintPen);
        paint.setFont(annotationFont);
        paint.drawText(puntoSI_X, puntoSI_Y, QString("%1").arg(etiquetasDeshacer.last()));

        paint.end();

        //--------------------------------- Información ---------------------------------
        imageDeshacer.push_back(imageFrame);
        ui->frameSelectedImageLabel->setPixmap(imageDeshacer.last());
        referencePoints.push_back(QPointF( puntoSI_X + (anchoSeleccionado / 2) , puntoSI_Y + (altoSeleccionado / 2) ));

        //------------------------ Presentación de la información -----------------------
        rectangleInformation.push_back(QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\r\n").arg(valueFrame).arg(contadorDeshacer.last()).arg(labelText.anotacion).arg(puntoSI_X).arg(puntoSI_Y).arg(puntoSI_X + anchoSeleccionado).arg(puntoSI_Y + altoSeleccionado).arg(puntoSI_X + (anchoSeleccionado / 2)).arg(puntoSI_Y + (altoSeleccionado / 2)));

        QString fullText;
        if (lineInformation.length() > 1) {
            for (int i = 0; i < lineInformation.length(); i++) {
                fullText.append(QString("%1").arg(lineInformation.at(i)));
            }
            fullText.append("\r\n");
        }
        for (int i = 0; i < rectangleInformation.length(); i++){
            fullText.append(QString("%1").arg(rectangleInformation.at(i)));
            QStringList elements = rectangleInformation.at(i).split(',');
        }
        fullText.append("\r\n");
        if (ellipseInformation.length() > 1) {
            for (int i = 0; i < ellipseInformation.length(); i++) {
                fullText.append(QString("%1").arg(ellipseInformation.at(i)));
            }
        }
        ui->informationTextEdit->setPlainText(fullText);
        textoDeshacer.push_back(ui->informationTextEdit->toPlainText());

        //-------------------------------- Reajustar GUI --------------------------------
        pointsX.clear();
        pointsY.clear();
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
        rectangleToolState = false;
        ui->rectanguloButton->setChecked(rectangleToolState);
        rubberBandState = false;
        rubberBand->~QRubberBand();
        ui->cargarImagenButton->setEnabled(true);
        ui->annotationButton->setEnabled(true);
        ui->elipseButton->setEnabled(true);
        ui->lineaButton->setEnabled(true);
        ui->rectanguloButton->setEnabled(true);
        ui->roiButton->setEnabled(true);
        roiFlag = false;
        ui->deshacerButton->setEnabled(true);
        ui->zoomInButton->setEnabled(true);
        ui->zoomOutButton->setEnabled(false);
        ui->saveImageButton->setEnabled(true);
        ui->saveTextButton->setEnabled(true);
        ui->loadPreviousFrameToolButton->setEnabled(true);
        ui->loadNextFrameToolButton->setEnabled(true);
        ui->calibracionButton->setEnabled(true);
    }
}

void MainWindow::mouseLineDraw(QPoint &pos)
{
    //------------------------------- Corrección de puntos ------------------------------
    pointsX.push_back(int(pos.x() * (videoWidth / frameWidth)));
    pointsY.push_back(int(pos.y() * (videoHeight / frameHeight)));

    //------------------------- Procedimiento para dibujar línea ------------------------
    if (pointsX.length() == 1) {
        //-------------------------- Activación de rubber band --------------------------
        rubberBand = new QRubberBand(QRubberBand::Line, ui->frameSelectedImageLabel);
        if (referencePoints.length() > 0) {
            for (int i = 0 ; i < referencePoints.length() ; i++) {
                QLineF testLine(referencePoints.at(i).x(), referencePoints.at(i).y() , pointsX.at(0) , pointsY.at(0));                      // Línea entre punto de referencia y punto seleccionado
                if (testLine.length() < 20) {                                                                                               // Si la distancia de la línea < 20px
                    pointsX[0] = int(referencePoints.at(i).x());                                                                                 // Convierte el punto seleccionado en el punto de referencia
                    pointsY[0] = int(referencePoints.at(i).y());
                    fixed_point = QPoint( int(referencePoints.at(i).x() * (frameWidth / videoWidth)) , int(referencePoints.at(i).y() * (frameHeight / videoHeight)) );     // Establece como punto fijo, el punto de referencia en las coordenadas de la imagen
                    break;
                } else if (i == referencePoints.length() - 1) {
                    fixed_point = pos;                                                                                                      // Establece como punto fijo, el punto seleccionado
                }
            }
        } else {
            fixed_point = pos;                                                                                                              // Establece como punto fijo, el punto seleccionado
        }
        rubberBand->setGeometry(QRect( QPoint( fixed_point.x(), fixed_point.y() ) , QPoint( fixed_point.x() + 1, fixed_point.y() + 1 ) ) ); // Inicializa la rubber band
        rubberBand->show();                                                                                                                 // Muestra la rubber band
        rubberBandState = true;

        //----------------------- Bloqueo de botones temporalmente ----------------------
        ui->annotationButton->setEnabled(false);
        ui->lineaButton->setEnabled(false);
        ui->rectanguloButton->setEnabled(false);
        ui->elipseButton->setEnabled(false);
        ui->roiButton->setEnabled(false);
        ui->deshacerButton->setEnabled(false);
        ui->zoomInButton->setEnabled(false);
        ui->zoomOutButton->setEnabled(false);
        ui->saveImageButton->setEnabled(false);
        ui->saveTextButton->setEnabled(false);
        ui->loadPreviousFrameToolButton->setEnabled(false);
        ui->loadNextFrameToolButton->setEnabled(false);
        ui->calibracionButton->setEnabled(false);
        ui->cargarImagenButton->setEnabled(false);
    } else if (pointsX.length() == 2) {
        if (referencePoints.length() > 0) {
            for (int i = 0 ; i < referencePoints.length() ; i++) {
                QLineF testLine(referencePoints.at(i).x(), referencePoints.at(i).y() , pointsX.at(1) , pointsY.at(1));                      // Línea entre punto de referencia y punto seleccionado
                if (testLine.length() < 20) {                                                                                               // Si la distancia de la línea < 20px
                    pointsX[1] = int(referencePoints.at(i).x());                                                                                 // Convierte el punto seleccionado en el punto de referencia
                    pointsY[1] = int(referencePoints.at(i).y());
                    break;
                }
            }
        }
        //--------------------------- Reajustar puntos al zoom --------------------------
        if (scaleFactor > 1) {
            pointsX[0] = int((pointsX.at(0) / scaleFactor) + positionCorrection.at(0));
            pointsY[0] = int((pointsY.at(0) / scaleFactor) + positionCorrection.at(1));
            pointsX[1] = int((pointsX.at(1) / scaleFactor) + positionCorrection.at(0));
            pointsY[1] = int((pointsY.at(1) / scaleFactor) + positionCorrection.at(1));
            scaleFactor = 1;
            positionCorrection.clear();
        }

        //---------------------- Centro de linea para la anotación ----------------------
        int lineaCentro_X, lineaCentro_Y, m;
        if (pointsX.at(1) - pointsX.at(0) != 0) {
            m = (pointsY.at(1) - pointsY.at(0)) / (pointsX.at(1) - pointsX.at(0));
            if (m >= 0) {
                lineaCentro_X = ((pointsX.at(0) + pointsX.at(1)) / 2) + ui->textSizeSpinBox->value();
                lineaCentro_Y = ((pointsY.at(0) + pointsY.at(1)) / 2) - ui->textSizeSpinBox->value();
            } else {
                lineaCentro_X = ((pointsX.at(0) + pointsX.at(1)) / 2) - ui->textSizeSpinBox->value();
                lineaCentro_Y = ((pointsY.at(0) + pointsY.at(1)) / 2) - ui->textSizeSpinBox->value();
            }
        } else {
            lineaCentro_X = pointsX.at(0) + ui->textSizeSpinBox->value();
            lineaCentro_Y = ((pointsY.at(0) + pointsY.at(1)) / 2) - ui->textSizeSpinBox->value();
        }

        //---------------------------- Definición de etiqueta ---------------------------
        contadorDeshacer.push_back(contadorDeshacer.last() + 1);
        etiquetasDeshacer.push_back(QString("%1").arg(contadorDeshacer.last()));

        //-------------------------- Dibujar linea y anotación --------------------------
        imageFrame = imageDeshacer.last();

        QPainter paint(&imageFrame);
        QPen paintPen(customColorObject);
        paintPen.setWidth(thickness);
        paint.setPen(paintPen);
        paint.drawLine(pointsX.at(0), pointsY.at(0), pointsX.at(1), pointsY.at(1));

        paintPen.setColor(annotationCustomColor);
        paint.setPen(paintPen);
        paint.setFont(annotationFont);
        paint.drawText(lineaCentro_X, lineaCentro_Y, QString("%1").arg(etiquetasDeshacer.last()));

        ui->frameSelectedImageLabel->setPixmap(imageFrame);

        paint.end();

        //--------------------------------- Información ---------------------------------
        imageDeshacer.push_back(imageFrame);
        referencePoints.push_back(QPointF( -1 , -1));
        QLineF dist(pointsX.at(0),pointsY.at(0),pointsX.at(1),pointsY.at(1));

        //----------------------------- Distancia en metros -----------------------------
        QString realDist;
        if (calibrationFlag == true) {
            QVector<double> pointsRealX;
            QVector<double> pointsRealY;

            double x1_p = QString(HTextLines.at(0)).toDouble() * double(pointsX.at(0)) + QString(HTextLines.at(1)).toDouble() * double(pointsY.at(0)) + QString(HTextLines.at(2)).toDouble();
            double y1_p = QString(HTextLines.at(3)).toDouble() * double(pointsX.at(0)) + QString(HTextLines.at(4)).toDouble() * double(pointsY.at(0)) + QString(HTextLines.at(5)).toDouble();
            double z1_p = QString(HTextLines.at(6)).toDouble() * double(pointsX.at(0)) + QString(HTextLines.at(7)).toDouble() * double(pointsY.at(0)) + QString(HTextLines.at(8)).toDouble();

            double x2_p = QString(HTextLines.at(0)).toDouble() * double(pointsX.at(1)) + QString(HTextLines.at(1)).toDouble() * double(pointsY.at(1)) + QString(HTextLines.at(2)).toDouble();
            double y2_p = QString(HTextLines.at(3)).toDouble() * double(pointsX.at(1)) + QString(HTextLines.at(4)).toDouble() * double(pointsY.at(1)) + QString(HTextLines.at(5)).toDouble();
            double z2_p = QString(HTextLines.at(6)).toDouble() * double(pointsX.at(1)) + QString(HTextLines.at(7)).toDouble() * double(pointsY.at(1)) + QString(HTextLines.at(8)).toDouble();

            pointsRealX.push_back( x1_p / z1_p );
            pointsRealX.push_back( x2_p / z2_p );

            pointsRealY.push_back( y1_p / z1_p );
            pointsRealY.push_back( y2_p / z2_p );

            QLineF distReal(pointsRealX.at(0),pointsRealY.at(0),pointsRealX.at(1),pointsRealY.at(1));
            realDist = QString("%1").arg(distReal.length());
        } else {
            realDist = QString("-");
        }

        //------------------------ Presentación de la información -----------------------
        lineInformation.push_back(QString("%1,%2,%3,%4,%5,%6,%7,%8m\r\n").arg(valueFrame).arg(contadorDeshacer.last()).arg(pointsX.at(0)).arg(pointsY.at(0)).arg(pointsX.at(1)).arg(pointsY.at(1)).arg(dist.length()).arg(realDist) );
        QString fullText;
        for (int i = 0; i < lineInformation.length(); i++){
            fullText.append(QString("%1").arg(lineInformation.at(i)));
        }
        fullText.append("\r\n");
        if (rectangleInformation.length() > 1) {
            for (int i = 0; i < rectangleInformation.length(); i++) {
                fullText.append(QString("%1").arg(rectangleInformation.at(i)));
            }
            fullText.append("\r\n");
        }
        if (ellipseInformation.length() > 1) {
            for (int i = 0; i < ellipseInformation.length(); i++) {
                fullText.append(QString("%1").arg(ellipseInformation.at(i)));
            }
        }
        ui->informationTextEdit->setPlainText(fullText);
        textoDeshacer.push_back(ui->informationTextEdit->toPlainText());

        //-------------------------------- Reajustar GUI --------------------------------
        pointsX.clear();
        pointsY.clear();
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
        lineToolState = false;
        ui->lineaButton->setChecked(lineToolState);
        rubberBandState = false;
        rubberBand->~QRubberBand();
        ui->cargarImagenButton->setEnabled(true);
        ui->annotationButton->setEnabled(true);
        ui->elipseButton->setEnabled(true);
        ui->lineaButton->setEnabled(true);
        ui->rectanguloButton->setEnabled(true);
        ui->roiButton->setEnabled(true);
        roiFlag = false;
        ui->deshacerButton->setEnabled(true);
        ui->zoomInButton->setEnabled(true);
        ui->zoomOutButton->setEnabled(false);
        ui->saveImageButton->setEnabled(true);
        ui->saveTextButton->setEnabled(true);
        ui->loadPreviousFrameToolButton->setEnabled(true);
        ui->loadNextFrameToolButton->setEnabled(true);
        ui->calibracionButton->setEnabled(true);
    }
}

void MainWindow::mouseEllipseDraw(QPoint &pos)
{
    //------------------------------- Corrección de puntos ------------------------------
    pointsX.push_back(int(pos.x() * (videoWidth / frameWidth)));
    pointsY.push_back(int(pos.y() * (videoHeight / frameHeight)));

    //------------------------ Procedimiento para dibujar elipse ------------------------
    fixed_point = pos;
    if (pointsX.length() == 1) {
        //---------------------------- Activación de rubber band ----------------------------
        rubberBand = new QRubberBand(QRubberBand::Rectangle, ui->frameSelectedImageLabel);
        rubberBand->setGeometry(QRect( QPoint( fixed_point.x(), fixed_point.y() ) , QPoint( fixed_point.x() + 1, fixed_point.y() + 1 ) ) );
        rubberBand->show();
        rubberBandState = true;

        //----------------------- Bloqueo de botones temporalmente ----------------------
        ui->annotationButton->setEnabled(false);
        ui->lineaButton->setEnabled(false);
        ui->rectanguloButton->setEnabled(false);
        ui->elipseButton->setEnabled(false);
        ui->roiButton->setEnabled(false);
        ui->deshacerButton->setEnabled(false);
        ui->zoomInButton->setEnabled(false);
        ui->zoomOutButton->setEnabled(false);
        ui->saveImageButton->setEnabled(false);
        ui->saveTextButton->setEnabled(false);
        ui->loadPreviousFrameToolButton->setEnabled(false);
        ui->loadNextFrameToolButton->setEnabled(false);
        ui->calibracionButton->setEnabled(false);
        ui->cargarImagenButton->setEnabled(false);
    } else if (pointsX.length() == 2) {
        //--------------------------- Reajustar puntos al zoom --------------------------
        if (scaleFactor > 1) {
            pointsX[0] = int((pointsX.at(0) / scaleFactor) + positionCorrection.at(0));
            pointsY[0] = int((pointsY.at(0) / scaleFactor) + positionCorrection.at(1));
            pointsX[1] = int((pointsX.at(1) / scaleFactor) + positionCorrection.at(0));
            pointsY[1] = int((pointsY.at(1) / scaleFactor) + positionCorrection.at(1));
            scaleFactor = 1;
            positionCorrection.clear();
        }

        //--------------------- Definición de esquina, ancho y alto ---------------------
        int puntoSI_X, puntoSI_Y, anchoSeleccionado, altoSeleccionado;
        if (pointsX.at(0) > pointsX.at(1)) {
            puntoSI_X = pointsX.at(1);
            anchoSeleccionado = pointsX.at(0) - pointsX.at(1);
        } else {
            puntoSI_X = pointsX.at(0);
            anchoSeleccionado = pointsX.at(1) - pointsX.at(0);
        }
        if (pointsY.at(0) < pointsY.at(1)) {
            puntoSI_Y = pointsY.at(0);
            altoSeleccionado = pointsY.at(1) - pointsY.at(0);
        } else {
            puntoSI_Y = pointsY.at(1);
            altoSeleccionado = pointsY.at(0) - pointsY.at(1);
        }

        //---------------------------- Definición de etiqueta ---------------------------
        QPoint windowPosition;
        windowPosition.setX(pos.x());
        windowPosition.setY(pos.y());imageFrame = imageDeshacer.last();
        QPoint framePosition = ui->frameSelectedImageLabel->mapToGlobal(windowPosition);

        LabelWindow labelText;
        labelText.move(framePosition.x(), framePosition.y());
        labelText.exec();

        contadorDeshacer.push_back(contadorDeshacer.last() + 1);
        etiquetasDeshacer.push_back(QString("%1 %2").arg(contadorDeshacer.last()).arg(labelText.anotacion));

        //-------------------------- Dibujar elipse y anotación -------------------------
        imageFrame = imageDeshacer.last();

        QPainter paint(&imageFrame);
        QPen paintPen(customColorObject);
        paintPen.setWidth(thickness);
        paint.setPen(paintPen);
        paint.drawEllipse(puntoSI_X, puntoSI_Y, anchoSeleccionado, altoSeleccionado);
        paint.drawEllipse(puntoSI_X + (anchoSeleccionado / 2) - 2, puntoSI_Y + (altoSeleccionado / 2) - 2, 4, 4);

        paintPen.setColor(annotationCustomColor);
        paint.setPen(paintPen);
        paint.setFont(annotationFont);
        paint.drawText(puntoSI_X, puntoSI_Y, QString("%1").arg(etiquetasDeshacer.last()));

        ui->frameSelectedImageLabel->setPixmap(imageFrame);

        paint.end();

        //--------------------------------- Información ---------------------------------
        imageDeshacer.push_back(imageFrame);
        referencePoints.push_back(QPointF( puntoSI_X + (anchoSeleccionado / 2) , puntoSI_Y + (altoSeleccionado / 2) ));

        //------------------------ Presentación de la información -----------------------
        ellipseInformation.push_back(QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\r\n").arg(valueFrame).arg(contadorDeshacer.last()).arg(labelText.anotacion).arg(puntoSI_X).arg(puntoSI_Y).arg(puntoSI_X + anchoSeleccionado).arg(puntoSI_Y + altoSeleccionado).arg(puntoSI_X + (anchoSeleccionado / 2)).arg(puntoSI_Y + (altoSeleccionado / 2)));
        QString fullText;
        if (lineInformation.length() > 1) {
            for (int i = 0; i < lineInformation.length(); i++) {
                fullText.append(QString("%1").arg(lineInformation.at(i)));
            }
            fullText.append("\r\n");
        }
        if (rectangleInformation.length() > 1) {
            for (int i = 0; i < rectangleInformation.length(); i++) {
                fullText.append(QString("%1").arg(rectangleInformation.at(i)));
            }
            fullText.append("\r\n");
        }
        if (ellipseInformation.length() > 1) {
            for (int i = 0; i < ellipseInformation.length(); i++) {
                fullText.append(QString("%1").arg(ellipseInformation.at(i)));
            }
        }
        ui->informationTextEdit->setPlainText(fullText);
        textoDeshacer.push_back(ui->informationTextEdit->toPlainText());

        //-------------------------------- Reajustar GUI --------------------------------
        pointsX.clear();
        pointsY.clear();
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
        ellipseToolState = false;
        ui->elipseButton->setChecked(ellipseToolState);
        rubberBandState = false;
        rubberBand->~QRubberBand();
        ui->cargarImagenButton->setEnabled(true);
        ui->annotationButton->setEnabled(true);
        ui->elipseButton->setEnabled(true);
        ui->lineaButton->setEnabled(true);
        ui->rectanguloButton->setEnabled(true);
        ui->roiButton->setEnabled(true);
        roiFlag = false;
        ui->deshacerButton->setEnabled(true);
        ui->zoomInButton->setEnabled(true);
        ui->zoomOutButton->setEnabled(false);
        ui->saveImageButton->setEnabled(true);
        ui->saveTextButton->setEnabled(true);
        ui->loadPreviousFrameToolButton->setEnabled(true);
        ui->loadNextFrameToolButton->setEnabled(true);
        ui->calibracionButton->setEnabled(true);
    }
}

// --------------------------------------------------------------------------------------
// OPCIONES
// --------------------------------------------------------------------------------------

void MainWindow::on_zoomInButton_clicked()
{
    pointsX.clear();
    pointsY.clear();

    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
    annotationButtonState = false;
    ui->annotationButton->setChecked(annotationButtonState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
    rectangleToolState = false;
    ui->rectanguloButton->setChecked(rectangleToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
    lineToolState = false;
    ui->lineaButton->setChecked(lineToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
    ellipseToolState = false;
    ui->elipseButton->setChecked(ellipseToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
    roiToolState = false;
    ui->roiButton->setChecked(roiToolState);

    if (zoominToolState == true) {
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
        zoominToolState = false;
    } else {
        ui->frameSelectedImageLabel->setCursor(Qt::CrossCursor);
        connect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
        zoominToolState = true;
    }
    ui->zoomInButton->setChecked(zoominToolState);
}

void MainWindow::on_zoomOutButton_clicked()
{
    //--------------------------------- Reiniciar puntos --------------------------------
    pointsX.clear();
    pointsY.clear();
    positionCorrection.clear();
    scaleFactor = 1;
    roiFlag = false;

    //----------------------------- Presentar última imagen -----------------------------
    ui->frameSelectedImageLabel->setPixmap(imageDeshacer.last());

    //---------------------------------- Reiniciar GUI ----------------------------------
    ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
    connect(ui->frameSelectedImageLabel, SIGNAL(sendMouseMovePos(QPoint&)), this, SLOT(mousePosition(QPoint&)));

    ui->lineaButton->setEnabled(true);
    ui->rectanguloButton->setEnabled(true);
    ui->elipseButton->setEnabled(true);
    ui->roiButton->setEnabled(true);
    ui->annotationButton->setEnabled(true);

    if (imageDeshacer.length() == 1) {
        ui->deshacerButton->setEnabled(false);
    } else {
        ui->deshacerButton->setEnabled(true);
    }
    ui->zoomOutButton->setEnabled(false);
    ui->zoomInButton->setEnabled(true);
    ui->saveImageButton->setEnabled(true);
    ui->saveTextButton->setEnabled(true);
}

void MainWindow::on_deshacerButton_clicked()
{
    //--------------------------------- Deshacer imagen ---------------------------------
    imageDeshacer.removeLast();
    imageFrame = imageDeshacer.last();
    ui->frameSelectedImageLabel->setPixmap(imageDeshacer.last());

    //---------------------------------- Deshacer texto ---------------------------------
    textoDeshacer.removeLast();
    ui->informationTextEdit->setPlainText(textoDeshacer.last());

    //------------------------------- Deshacer información ------------------------------
    int lastLabel = contadorDeshacer.last();
    if (lineInformation.length() > 1) {
        QStringList test = lineInformation.last().split(',');
        if (test.at(1).toInt() == lastLabel) {
            lineInformation.removeLast();
        }
    }
    if (rectangleInformation.length() > 1) {
        QStringList test = rectangleInformation.last().split(',');
        if (test.at(1).toInt() == lastLabel) {
            rectangleInformation.removeLast();
        }
    }
    if (ellipseInformation.length() > 1) {
        QStringList test = ellipseInformation.last().split(',');
        if (test.at(1).toInt() == lastLabel) {
            ellipseInformation.removeLast();
        }
    }

    //--------------------------------- Deshacer listas ---------------------------------
    contadorDeshacer.removeLast();
    etiquetasDeshacer.removeLast();
    referencePoints.removeLast();

    //------------------------- Definición del nombre de archivo ------------------------
    QFileInfo videoFile(videoName);
    QString imageDefaultName = QString("%1_%2").arg(videoFile.baseName()).arg(valueFrame);
    if (etiquetasDeshacer.length() > 1) {
        for (int i = 1; i < etiquetasDeshacer.length(); i++) {
            imageDefaultName = QString("%1_%2").arg(imageDefaultName).arg(etiquetasDeshacer.at(i));
        }
    }
    imageDefaultName = QString("%1.png").arg(imageDefaultName);
    ui->imageNameValueText->setText(imageDefaultName);

    //------------------------------ Deshabilitar deshacer ------------------------------
    if (imageDeshacer.length() == 1) {
        ui->deshacerButton->setEnabled(false);
    }
}

void MainWindow::on_roiButton_clicked()
{
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
    annotationButtonState = false;
    ui->annotationButton->setChecked(annotationButtonState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
    rectangleToolState = false;
    ui->rectanguloButton->setChecked(rectangleToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
    lineToolState = false;
    ui->lineaButton->setChecked(lineToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
    ellipseToolState = false;
    ui->elipseButton->setChecked(ellipseToolState);
    disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
    zoominToolState = false;
    ui->zoomInButton->setChecked(zoominToolState);

    if (roiToolState == true) {
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
        roiToolState = false;
    } else {
        ui->frameSelectedImageLabel->setCursor(Qt::CrossCursor);
        connect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
        roiToolState = true;
    }
    ui->roiButton->setChecked(roiToolState);
}

void MainWindow::mouseROISelection(QPoint &pos)
{
    //------------------------------- Corrección de puntos ------------------------------
    pointsX.push_back(int(pos.x() * (videoWidth / frameWidth)));
    pointsY.push_back(int(pos.y() * (videoHeight / frameHeight)));

    //---------------------- Procedimiento para seleccionar el ROI ----------------------
    fixed_point = pos;
    if (pointsX.length() == 1) {
        //-------------------------- Activación de rubber band --------------------------
        rubberBand = new QRubberBand(QRubberBand::Rectangle, ui->frameSelectedImageLabel);
        rubberBand->setGeometry(QRect( QPoint( fixed_point.x(), fixed_point.y() ) , QPoint( fixed_point.x() + 1, fixed_point.y() + 1 ) ) );
        rubberBand->show();
        rubberBandState = true;

        //----------------------- Bloqueo de botones temporalmente ----------------------
        ui->annotationButton->setEnabled(false);
        ui->lineaButton->setEnabled(false);
        ui->rectanguloButton->setEnabled(false);
        ui->elipseButton->setEnabled(false);
        ui->roiButton->setEnabled(false);
        ui->deshacerButton->setEnabled(false);
        ui->zoomInButton->setEnabled(false);
        ui->zoomOutButton->setEnabled(false);
        ui->saveImageButton->setEnabled(false);
        ui->saveTextButton->setEnabled(false);
        ui->loadPreviousFrameToolButton->setEnabled(false);
        ui->loadNextFrameToolButton->setEnabled(false);
        ui->calibracionButton->setEnabled(false);
        ui->cargarImagenButton->setEnabled(false);
    } else if (pointsX.length() == 2) {
        //--------------------------- Reajustar puntos al zoom --------------------------
        if (scaleFactor > 1) {
            pointsX[0] = int((pointsX.at(0) / scaleFactor) + positionCorrection.at(0));
            pointsY[0] = int((pointsY.at(0) / scaleFactor) + positionCorrection.at(1));
            pointsX[1] = int((pointsX.at(1) / scaleFactor) + positionCorrection.at(0));
            pointsY[1] = int((pointsY.at(1) / scaleFactor) + positionCorrection.at(1));
            scaleFactor = 1;
            positionCorrection.clear();
        }

        //--------------------- Definición de esquina, ancho y alto ---------------------
        double puntoSI_X, puntoSI_Y, anchoSeleccionado, altoSeleccionado;
        if (pointsX.at(0) > pointsX.at(1)) {
            puntoSI_X = pointsX.at(1);
            anchoSeleccionado = pointsX.at(0) - puntoSI_X;
        } else {
            puntoSI_X = pointsX.at(0);
            anchoSeleccionado = pointsX.at(1) - puntoSI_X;
        }
        if (pointsY.at(0) < pointsY.at(1)) {
            puntoSI_Y = pointsY.at(0);
            altoSeleccionado = pointsY.at(1) - puntoSI_Y;
        } else {
            puntoSI_Y = pointsY.at(1);
            altoSeleccionado = pointsY.at(0) - puntoSI_Y;
        }

        //---------------------------- Obtener imagen del ROI ---------------------------
        cv::Rect selection((int(puntoSI_X)), int(puntoSI_Y), int(anchoSeleccionado), int(altoSeleccionado));
        cv::Mat cropped(loadFrame, selection);
        cv::cvtColor(cropped, cropped, cv::COLOR_BGR2RGB);
        QImage croppedframe = matToQImage(cropped);
        imageFrame = QPixmap::fromImage(croppedframe);
        ui->frameSelectedImageLabel->setPixmap(imageFrame);

        //---------------------------- Presentar información ----------------------------
        roiFlag = true;
        ui->informationTextEdit->setPlainText("");
        ui->informationTextEdit->setPlainText(QString("%1,%2,%3,%4\r\n").arg(puntoSI_X).arg(puntoSI_Y).arg(puntoSI_X + anchoSeleccionado).arg(puntoSI_Y + altoSeleccionado));

        //-------------------------------- Reajustar GUI --------------------------------
        pointsX.clear();
        pointsY.clear();
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMouseMovePos(QPoint&)), this, SLOT(mousePosition(QPoint&)));
        roiToolState = false;
        ui->roiButton->setChecked(roiToolState);
        rubberBandState = false;
        rubberBand->~QRubberBand();
        ui->cargarImagenButton->setEnabled(true);
        ui->zoomOutButton->setEnabled(true);
        ui->saveImageButton->setEnabled(true);
        ui->saveTextButton->setEnabled(true);
        ui->loadPreviousFrameToolButton->setEnabled(true);
        ui->loadNextFrameToolButton->setEnabled(true);
    }
}

void MainWindow::mouseZoom(QPoint &pos)
{
    //------------------------------- Corrección de puntos ------------------------------
    pointsX.push_back(int(pos.x() * (videoWidth / frameWidth)));
    pointsY.push_back(int(pos.y() * (videoHeight / frameHeight)));

    //---------------------------- Activación de rubber band ----------------------------
    fixed_point = pos;
    if (pointsX.length() == 1) {
        rubberBand = new QRubberBand(QRubberBand::Rectangle, ui->frameSelectedImageLabel);
        rubberBand->setGeometry(QRect( QPoint( fixed_point.x(), fixed_point.y() ) , QPoint( fixed_point.x() + 1, fixed_point.y() + 1 ) ) );
        rubberBand->show();
        rubberBandState = true;

        //----------------------- Bloqueo de botones temporalmente ----------------------
        ui->annotationButton->setEnabled(false);
        ui->lineaButton->setEnabled(false);
        ui->rectanguloButton->setEnabled(false);
        ui->elipseButton->setEnabled(false);
        ui->roiButton->setEnabled(false);
        ui->deshacerButton->setEnabled(false);
        ui->zoomInButton->setEnabled(false);
        ui->saveImageButton->setEnabled(false);
        ui->saveTextButton->setEnabled(false);
        ui->loadPreviousFrameToolButton->setEnabled(false);
        ui->loadNextFrameToolButton->setEnabled(false);
        ui->calibracionButton->setEnabled(false);
        ui->cargarImagenButton->setEnabled(false);
    } else if (pointsX.length() == 2) {
        //--------------------- Definición de esquina, ancho y alto ---------------------
        int puntoSI_X, puntoSI_Y, anchoSeleccionado, altoSeleccionado, anchoFinal, altoFinal, displayCenter_X, displayCenter_Y;
        if (pointsX.at(0) > pointsX.at(1)) {
            puntoSI_X = pointsX.at(1);
            anchoSeleccionado = pointsX.at(0) - puntoSI_X;
        } else {
            puntoSI_X = pointsX.at(0);
            anchoSeleccionado = pointsX.at(1) - puntoSI_X;
        }
        if (pointsY.at(0) < pointsY.at(1)) {
            puntoSI_Y = pointsY.at(0);
            altoSeleccionado = pointsY.at(1) - puntoSI_Y;
        } else {
            puntoSI_Y = pointsY.at(1);
            altoSeleccionado = pointsY.at(0) - puntoSI_Y;
        }

        //------------------ Definición de esquina, ancho y alto final ------------------
        if (anchoSeleccionado > altoSeleccionado) {
            altoFinal = static_cast<int>(anchoSeleccionado * (videoHeight / videoWidth));
            anchoFinal = anchoSeleccionado;
            displayCenter_X = 0;
            displayCenter_Y = (altoFinal / 2) - (altoSeleccionado / 2);
        } else {
            anchoFinal = static_cast<int>(altoSeleccionado * (videoWidth / videoHeight));
            altoFinal = altoSeleccionado;
            displayCenter_X = (anchoFinal / 2) - (anchoSeleccionado / 2);
            displayCenter_Y = 0;
        }
        puntoSI_X = puntoSI_X - displayCenter_X;
        puntoSI_Y = puntoSI_Y - displayCenter_Y;

        if (puntoSI_X < 1) {
            puntoSI_X = 1;
        } else if (puntoSI_X + anchoFinal > videoWidth) {
            puntoSI_X = static_cast<int>(videoWidth - anchoFinal);
        }
        if (puntoSI_Y < 1) {
            puntoSI_Y = 1;
        } else if (puntoSI_Y + altoFinal > videoHeight) {
            puntoSI_Y = static_cast<int>(videoHeight - altoFinal);
        }

        //--------------------------- Obtener imagen aumentada --------------------------
        cv::Rect selection(puntoSI_X, puntoSI_Y, anchoFinal, altoFinal);
        cv::Mat cropped(loadFrame, selection);
        cv::resize(cropped, cropped, trackFrame.size());
        cv::cvtColor(cropped, cropped, cv::COLOR_BGR2RGB);
        QImage qtrackframe = matToQImage(cropped);
        imageZoomFrame = QPixmap::fromImage(qtrackframe);
        ui->frameSelectedImageLabel->setPixmap(imageZoomFrame);

        //--------------------------- Variables de corrección ---------------------------
        scaleFactor = videoWidth / anchoFinal;
        positionCorrection.push_back(puntoSI_X);
        positionCorrection.push_back(puntoSI_Y);

        //-------------------------------- Reajustar GUI --------------------------------
        pointsX.clear();
        pointsY.clear();
        ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
        disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));
        zoominToolState = false;
        ui->zoomInButton->setChecked(zoominToolState);
        rubberBandState = false;
        rubberBand->~QRubberBand();
        ui->cargarImagenButton->setEnabled(true);
        ui->elipseButton->setEnabled(true);
        ui->lineaButton->setEnabled(true);
        ui->rectanguloButton->setEnabled(true);
        ui->roiButton->setEnabled(true);
        if (imageDeshacer.length() > 1) {
            ui->deshacerButton->setEnabled(true);
        }
        ui->zoomOutButton->setEnabled(true);
        ui->loadPreviousFrameToolButton->setEnabled(true);
        ui->loadNextFrameToolButton->setEnabled(true);
    }
}

// --------------------------------------------------------------------------------------
// MODO
// --------------------------------------------------------------------------------------

void MainWindow::on_loadPreviousFrameToolButton_clicked()
{
    roiFlag = false;
    if (valueFrame > 0) {
        valueFrame = valueFrame - 1;
            // Guardar Imagen del Cuadro Automaticamente --------------------------------
            QString imageDefaultName = QString("%1/%2").arg(videoResultFolder).arg(ui->imageNameValueText->text());
            QFileInfo fileImage(imageDefaultName);
            if (fileImage.exists()) {
                QMessageBox msgBox(QMessageBox::Warning, tr("Advertencia"), tr("¿Quiere sobreescribir la imagen?"), QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();
                switch (ret) {
                    case QMessageBox::Yes: {
                        QFile file(imageDefaultName);
                        file.open(QIODevice::WriteOnly);
                        imageDeshacer.last().save(&file, "PNG");
                        break;
                    }
                    case QMessageBox::No:
                        break;
                    default:
                        break;
                }
            } else {
                QFile file(imageDefaultName);
                file.open(QIODevice::WriteOnly);
                imageDeshacer.last().save(&file, "PNG");
            }

            // --------------------------- Guardar Información --------------------------
            if (ui->informationTextEdit->toPlainText() != "") {
                // -------------------- Guardar Texto Automaticamente -------------------
                QString textName = QString("%1/%2.txt").arg(videoResultFolder).arg(ui->textNameValueText->text());
                QFile fileTxt(textName);
                fileTxt.open(QIODevice::WriteOnly);
                QTextStream out(&fileTxt);

                QString textOut = ui->informationTextEdit->toPlainText();
                textOut.replace("\n","\r\n");
                out << textOut;
                fileTxt.close();

                // -------------------- Guardar JSON Automaticamente --------------------
                QJsonArray jsonArray, rectangleArray, lineArray, ellipseArray;
                QJsonObject rectangleObject, lineObjects, ellipseObjects;

                if (rectangleInformation.length() > 1) {
                    for (int i = 1; i < rectangleInformation.length(); i++) {
                        QStringList elements = rectangleInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("tl_x",elements[2]);
                            carObject.insert("tl_y",elements[3]);
                            carObject.insert("br_x",elements[4]);
                            carObject.insert("br_y",elements[5]);
                            carObject.insert("c_x",elements[6]);
                            carObject.insert("c_y",elements[7]);
                            rectangleArray.append(carObject);
                        }
                    }
                }
                rectangleObject.insert("rectangle",rectangleArray);

                if (lineInformation.length() > 1) {
                    for (int i = 1; i < lineInformation.length(); i++) {
                        QStringList elements = lineInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("p1_x",elements[2]);
                            carObject.insert("p1_y",elements[3]);
                            carObject.insert("p2_x",elements[4]);
                            carObject.insert("p2_y",elements[5]);
                            carObject.insert("distance_px",elements[6]);
                            carObject.insert("distance_m",elements[7]);
                            lineArray.append(carObject);
                        }
                    }
                }
                lineObjects.insert("line",lineArray);

                if (ellipseInformation.length() > 1) {
                    for (int i = 1; i < ellipseInformation.length(); i++) {
                        QStringList elements = ellipseInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("tl_x",elements[2]);
                            carObject.insert("tl_y",elements[3]);
                            carObject.insert("br_x",elements[4]);
                            carObject.insert("br_y",elements[5]);
                            carObject.insert("c_x",elements[6]);
                            carObject.insert("c_y",elements[7]);
                            ellipseArray.append(carObject);
                        }
                    }
                }
                ellipseObjects.insert("ellipse",ellipseArray);

                jsonArray.append(rectangleObject);
                jsonArray.append(lineObjects);
                jsonArray.append(ellipseObjects);

                QString jsonName = QString("%1/%2.json").arg(videoResultFolder).arg(ui->textNameValueText->text());
                QFile jsonFile(jsonName);
                jsonFile.open(QIODevice::WriteOnly);

                QJsonDocument saveDoc(jsonArray);
                jsonFile.write(saveDoc.toJson());
            }

            //----------------------------- Reiniciar listas ----------------------------
            imageDeshacer.clear();
            contadorDeshacer.clear();
            etiquetasDeshacer.clear();
            referencePoints.clear();

            contadorDeshacer.push_back(0);
            etiquetasDeshacer.push_back("0");
            referencePoints.push_back(QPointF(-1,-1));
            scaleFactor = 1;

            pointsX.clear();
            pointsY.clear();

            // ------------------------- Presentar nuevo cuadro -------------------------
            valueFrameMsec = frame_msec * valueFrame;
            video.set(cv::CAP_PROP_POS_MSEC, valueFrameMsec);
            video.read(loadFrame);
            QImage qloadframe = matToQImage(loadFrame);
            imageDeshacer.push_back(QPixmap::fromImage(qloadframe));

            ui->frameSelectedImageLabel->setPixmap(imageDeshacer.last());

            // ----------------- Presentar información del nuevo cuadro -----------------
            ui->frameSelectedValueLabel->setText(QString::number(valueFrame));

            //------------------------------ Reiniciar GUI ------------------------------
            QFileInfo videoFile(videoName);
            ui->imageNameValueText->setText(QString("%1_%2.png").arg(videoFile.baseName()).arg(valueFrame));

            connect(ui->frameSelectedImageLabel, SIGNAL(sendMouseMovePos(QPoint&)), this, SLOT(mousePosition(QPoint&)));
            ui->cargarImagenButton->setEnabled(true);
            ui->annotationButton->setEnabled(true);
            ui->elipseButton->setEnabled(true);
            ui->lineaButton->setEnabled(true);
            ui->rectanguloButton->setEnabled(true);
            ui->roiButton->setEnabled(true);
            ui->deshacerButton->setEnabled(false);
            ui->zoomInButton->setEnabled(true);
            ui->zoomOutButton->setEnabled(false);
            ui->saveImageButton->setEnabled(true);
            ui->saveTextButton->setEnabled(true);
            ui->loadPreviousFrameToolButton->setEnabled(true);
            ui->loadNextFrameToolButton->setEnabled(true);
            ui->calibracionButton->setEnabled(true);
    }
}

void MainWindow::on_loadNextFrameToolButton_clicked()
{
    roiFlag = false;
    if (valueFrame > 0) {
        valueFrame = valueFrame + 1;
            // Guardar Imagen del Cuadro Automaticamente --------------------------------
            QString imageDefaultName = QString("%1/%2").arg(videoResultFolder).arg(ui->imageNameValueText->text());
            QFileInfo fileImage(imageDefaultName);
            if (fileImage.exists()) {
                QMessageBox msgBox(QMessageBox::Warning, tr("Advertencia"), tr("¿Quiere sobreescribir la imagen?"), QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();
                switch (ret) {
                    case QMessageBox::Yes: {
                        QFile file(imageDefaultName);
                        file.open(QIODevice::WriteOnly);
                        imageDeshacer.last().save(&file, "PNG");
                        break;
                    }
                    case QMessageBox::No:
                        break;
                    default:
                        break;
                }
            } else {
                QFile file(imageDefaultName);
                file.open(QIODevice::WriteOnly);
                imageDeshacer.last().save(&file, "PNG");
            }

            // --------------------------- Guardar Información --------------------------
            if (ui->informationTextEdit->toPlainText() != "") {
                // -------------------- Guardar Texto Automaticamente -------------------
                QString textName = QString("%1/%2.txt").arg(videoResultFolder).arg(ui->textNameValueText->text());
                QFile fileTxt(textName);
                fileTxt.open(QIODevice::WriteOnly);
                QTextStream out(&fileTxt);

                QString textOut = ui->informationTextEdit->toPlainText();
                textOut.replace("\n","\r\n");
                out << textOut;
                fileTxt.close();

                // -------------------- Guardar JSON Automaticamente --------------------
                QJsonArray jsonArray, rectangleArray, lineArray, ellipseArray;
                QJsonObject rectangleObject, lineObjects, ellipseObjects;

                if (rectangleInformation.length() > 1) {
                    for (int i = 1; i < rectangleInformation.length(); i++) {
                        QStringList elements = rectangleInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("tl_x",elements[2]);
                            carObject.insert("tl_y",elements[3]);
                            carObject.insert("br_x",elements[4]);
                            carObject.insert("br_y",elements[5]);
                            carObject.insert("c_x",elements[6]);
                            carObject.insert("c_y",elements[7]);
                            rectangleArray.append(carObject);
                        }
                    }
                }
                rectangleObject.insert("rectangle",rectangleArray);

                if (lineInformation.length() > 1) {
                    for (int i = 1; i < lineInformation.length(); i++) {
                        QStringList elements = lineInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("p1_x",elements[2]);
                            carObject.insert("p1_y",elements[3]);
                            carObject.insert("p2_x",elements[4]);
                            carObject.insert("p2_y",elements[5]);
                            carObject.insert("distance_px",elements[6]);
                            carObject.insert("distance_m",elements[7]);
                            lineArray.append(carObject);
                        }
                    }
                }
                lineObjects.insert("line",lineArray);

                if (ellipseInformation.length() > 1) {
                    for (int i = 1; i < ellipseInformation.length(); i++) {
                        QStringList elements = ellipseInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("tl_x",elements[2]);
                            carObject.insert("tl_y",elements[3]);
                            carObject.insert("br_x",elements[4]);
                            carObject.insert("br_y",elements[5]);
                            carObject.insert("c_x",elements[6]);
                            carObject.insert("c_y",elements[7]);
                            ellipseArray.append(carObject);
                        }
                    }
                }
                ellipseObjects.insert("ellipse",ellipseArray);

                jsonArray.append(rectangleObject);
                jsonArray.append(lineObjects);
                jsonArray.append(ellipseObjects);

                QString jsonName = QString("%1/%2.json").arg(videoResultFolder).arg(ui->textNameValueText->text());
                QFile jsonFile(jsonName);
                jsonFile.open(QIODevice::WriteOnly);

                QJsonDocument saveDoc(jsonArray);
                jsonFile.write(saveDoc.toJson());
            }

            //----------------------------- Reiniciar listas ----------------------------
            imageDeshacer.clear();
            contadorDeshacer.clear();
            etiquetasDeshacer.clear();
            referencePoints.clear();

            contadorDeshacer.push_back(0);
            etiquetasDeshacer.push_back("0");
            referencePoints.push_back(QPointF(-1,-1));
            scaleFactor = 1;

            pointsX.clear();
            pointsY.clear();

            // ------------------------- Presentar nuevo cuadro -------------------------
            valueFrameMsec = frame_msec * valueFrame;
            video.set(cv::CAP_PROP_POS_MSEC, valueFrameMsec);
            video.read(loadFrame);
            QImage qloadframe = matToQImage(loadFrame);
            imageDeshacer.push_back(QPixmap::fromImage(qloadframe));

            ui->frameSelectedImageLabel->setPixmap(imageDeshacer.last());

            // ----------------- Presentar información del nuevo cuadro -----------------
            ui->frameSelectedValueLabel->setText(QString::number(valueFrame));

            //------------------------------ Reiniciar GUI ------------------------------
            QFileInfo videoFile(videoName);
            ui->imageNameValueText->setText(QString("%1_%2.png").arg(videoFile.baseName()).arg(valueFrame));

            connect(ui->frameSelectedImageLabel, SIGNAL(sendMouseMovePos(QPoint&)), this, SLOT(mousePosition(QPoint&)));
            ui->cargarImagenButton->setEnabled(true);
            ui->annotationButton->setEnabled(true);
            ui->elipseButton->setEnabled(true);
            ui->lineaButton->setEnabled(true);
            ui->rectanguloButton->setEnabled(true);
            ui->roiButton->setEnabled(true);
            ui->deshacerButton->setEnabled(false);
            ui->zoomInButton->setEnabled(true);
            ui->zoomOutButton->setEnabled(false);
            ui->saveImageButton->setEnabled(true);
            ui->saveTextButton->setEnabled(true);
            ui->loadPreviousFrameToolButton->setEnabled(true);
            ui->loadNextFrameToolButton->setEnabled(true);
            ui->calibracionButton->setEnabled(true);
    }
}

// --------------------------------------------------------------------------------------
// VIDEO
// --------------------------------------------------------------------------------------

void MainWindow::on_calibracionButton_clicked()
{
    CalibracionWindow calibWindow;
    connect(this, SIGNAL(imageCalibPass(cv::Mat &, QString &)), &calibWindow, SLOT(imageCalibReceive(cv::Mat &, QString &)) );
    emit imageCalibPass(loadFrame, videoName);
    calibWindow.exec();

    //----------------------- Verificación de calibración -----------------------
    QFileInfo videoFile(videoName);
    QFileInfo CalibrationFile( QString("%1/%2%3").arg(videoFile.canonicalPath()).arg(videoFile.baseName()).arg("_CalibrationData.txt") );
    if (CalibrationFile.exists()) {
        ui->calibracionStatusLabel->setText(QChar(0x2713));
        ui->calibracionStatusLabel->setStyleSheet(QString("color: rgb(0, 170, 0);"));
        calibrationFlag = true;

        //------------------ Lectura de archivo de calibración ------------------
        QFile file( CalibrationFile.absoluteFilePath() );
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        QString lines = in.readAll();
        HTextLines = lines.split(QRegExp("\t|\n"));
    } else {
        ui->calibracionStatusLabel->setText(QChar(0x2717));
        ui->calibracionStatusLabel->setStyleSheet(QString("color: rgb(255, 0, 0);"));
    }
}

// --------------------------------------------------------------------------------------
// FUNCIONES
// --------------------------------------------------------------------------------------

void MainWindow::on_cargarImagenButton_clicked()
{
    //--------------------------------- Reiniciar listas --------------------------------
    roiFlag = false;
    imageDeshacer.clear();
    contadorDeshacer.clear();
    textoDeshacer.clear();
    etiquetasDeshacer.clear();
    referencePoints.clear();

    contadorDeshacer.push_back(0);
    etiquetasDeshacer.push_back("0");
    referencePoints.push_back(QPointF(-1,-1));
    textoDeshacer.push_back("");
    scaleFactor = 1;

    pointsX.clear();
    pointsY.clear();

    lineInformation.clear();
    rectangleInformation.clear();
    ellipseInformation.clear();
    lineInformation.push_back(lineTitle);
    rectangleInformation.push_back(rectangleTitle);
    ellipseInformation.push_back(ellipseTitle);

    //------------------------- Definición del nombre de archivo ------------------------
    QFileInfo videoFile(videoName);
    QString imageDefaultName = QString("%1_%2.png").arg(videoFile.baseName()).arg(valueFrame);
    ui->imageNameValueText->setText(imageDefaultName);

    QString textDefaultName = videoFile.baseName();
    ui->textNameValueText->setText(textDefaultName);

    //---------------------------------- Reiniciar GUI ----------------------------------
    ui->annotationButton->setEnabled(true);
    ui->lineaButton->setEnabled(true);
    ui->rectanguloButton->setEnabled(true);
    ui->elipseButton->setEnabled(true);
    ui->roiButton->setEnabled(true);
    ui->deshacerButton->setEnabled(false);
    ui->zoomInButton->setEnabled(true);
    ui->zoomOutButton->setEnabled(false);
    ui->saveImageButton->setEnabled(true);
    ui->saveTextButton->setEnabled(true);
    ui->loadPreviousFrameToolButton->setEnabled(true);
    ui->loadNextFrameToolButton->setEnabled(true);
    ui->calibracionButton->setEnabled(true);
    ui->informationTextEdit->setPlainText("");
    rubberBandState = false;

    //--------------------------- Presentar cuadro seleccionado -------------------------
    valueFrameMsec = frame_msec * valueFrame;
    video.set(cv::CAP_PROP_POS_MSEC, valueFrameMsec);
    video.read(loadFrame);
    QImage qloadframe = matToQImage(loadFrame);
    imageDeshacer.push_back(QPixmap::fromImage(qloadframe));

    ui->frameSelectedImageLabel->setPixmap(imageDeshacer.last());
    ui->frameSelectedValueLabel->setText(QString::number(valueFrame));
    frameSelectedState = true;

    //--------------------------- Conexión del cuadro al mouse --------------------------
    connect(ui->frameSelectedImageLabel, SIGNAL(sendMouseMovePos(QPoint&)), this, SLOT(mousePosition(QPoint&)));
}

void MainWindow::on_saveImageButton_clicked()
{
    QString imageDefaultName = QString("%1/%2").arg(videoResultFolder).arg(ui->imageNameValueText->text());
    QString imageName = QFileDialog::getSaveFileName(this, tr("Guardar la imagen"), imageDefaultName, "Portable Network Graphics (*.png)");
    if (imageName != "") {
        QFileInfo fileImage(imageName);
        videoResultFolder = fileImage.absolutePath();

        QFile file(imageName);
        file.open(QIODevice::WriteOnly);
        if (roiFlag == true) {
            imageFrame.save(&file, "PNG");
        } else {
            imageDeshacer.last().save(&file, "PNG");
        }
        ui->statusBar->showMessage(tr("Imagen guardada"), 2000);
    }
}

void MainWindow::on_saveTextButton_clicked()
{
    //------------------------------ Guardar la información -----------------------------
    if (ui->informationTextEdit->toPlainText() != "") {
        QString textName = QString("%1/%2.txt").arg(videoResultFolder).arg(ui->textNameValueText->text());
        textName = QFileDialog::getSaveFileName(this, tr("Guardar el texto"),  textName, tr("Archivo de texto (*.txt)"));
        if (textName != "") {
            QFileInfo fileText(textName);
            videoResultFolder = fileText.absolutePath();

            QFile fileTxt(textName);
            fileTxt.open(QIODevice::WriteOnly);
            QTextStream out(&fileTxt);

            QString textOut = ui->informationTextEdit->toPlainText();
            textOut.replace("\n","\r\n");
            out << textOut;
            fileTxt.close();
            ui->statusBar->showMessage(tr("Texto guardado"), 2000);

            // ---------------------- Guardar JSON ----------------------
            QString jsonName = QString("%1/%2.json").arg(videoResultFolder).arg(ui->textNameValueText->text());
            jsonName = QFileDialog::getSaveFileName(this, tr("Guardar el texto"), jsonName, tr("Archivo JSON (*.json)"));
            if (jsonName != "") {
                QJsonArray jsonArray, rectangleArray, lineArray, ellipseArray;
                QJsonObject rectangleObject, lineObjects, ellipseObjects;

                if (rectangleInformation.length() > 1) {
                    for (int i = 0; i < rectangleInformation.length(); i++) {
                        QStringList elements = rectangleInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("class",elements[2]);
                            carObject.insert("tl_x",elements[3]);
                            carObject.insert("tl_y",elements[4]);
                            carObject.insert("br_x",elements[5]);
                            carObject.insert("br_y",elements[6]);
                            carObject.insert("c_x",elements[7]);
                            carObject.insert("c_y",elements[8]);
                            rectangleArray.append(carObject);
                        }
                    }
                }
                rectangleObject.insert("rectangle",rectangleArray);

                if (lineInformation.length() > 1) {
                    for (int i = 0; i < lineInformation.length(); i++) {
                        QStringList elements = lineInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("p1_x",elements[2]);
                            carObject.insert("p1_y",elements[3]);
                            carObject.insert("p2_x",elements[4]);
                            carObject.insert("p2_y",elements[5]);
                            carObject.insert("distance_px",elements[6]);
                            carObject.insert("distance_m",elements[7]);
                            lineArray.append(carObject);
                        }
                    }
                }
                lineObjects.insert("line",lineArray);

                if (ellipseInformation.length() > 1) {
                    for (int i = 0; i < ellipseInformation.length(); i++) {
                        QStringList elements = ellipseInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                        if (elements.length() > 1) {
                            QJsonObject carObject;
                            carObject.insert("frame",elements[0]);
                            carObject.insert("id",elements[1]);
                            carObject.insert("class",elements[2]);
                            carObject.insert("tl_x",elements[3]);
                            carObject.insert("tl_y",elements[4]);
                            carObject.insert("br_x",elements[5]);
                            carObject.insert("br_y",elements[6]);
                            carObject.insert("c_x",elements[7]);
                            carObject.insert("c_y",elements[8]);
                            ellipseArray.append(carObject);
                        }
                    }
                }
                ellipseObjects.insert("ellipse",ellipseArray);

                jsonArray.append(rectangleObject);
                jsonArray.append(lineObjects);
                jsonArray.append(ellipseObjects);

                QFile jsonFile(jsonName);
                jsonFile.open(QIODevice::WriteOnly);

                QJsonDocument saveDoc(jsonArray);
                jsonFile.write(saveDoc.toJson());
            }
        }
    } else {
        QMessageBox *emptyPanelMsgBox = new QMessageBox(QMessageBox::Critical, "Error", tr("El panel de información está vacío"), QMessageBox::Close);
        emptyPanelMsgBox->exec();
    }
}

void MainWindow::mousePosition(QPoint &move_pos)
{
    int mouseX, mouseY;
    QPixmap imageTemp, imageOriginal;
    //---------------------------- Definición de puntos final ---------------------------
    if (scaleFactor > 1) {
        mouseX = int(( (move_pos.x() * (videoWidth / frameWidth)) / scaleFactor) + positionCorrection.at(0));
        mouseY = int(( (move_pos.y() * (videoHeight / frameHeight)) / scaleFactor) + positionCorrection.at(1));
        imageTemp = imageZoomFrame;
        imageOriginal = imageZoomFrame;
    } else {
        mouseX = int(move_pos.x() * (videoWidth / frameWidth));
        mouseY = int(move_pos.y() * (videoHeight / frameHeight));
        imageTemp = imageDeshacer.last();
        imageOriginal = imageDeshacer.last();
    }
    QToolTip::showText(ui->frameSelectedImageLabel->mapToGlobal(move_pos), QString("%1,%2").arg(mouseX).arg(mouseY));

    //--------------------------- Dimensiones del rubber band ---------------------------
    if (pointsX.length() == 1) {
        int fixPoint_X, fixPoint_Y, anchoSeleccion, altoSeleccion;
        if (fixed_point.x() > move_pos.x()) {
            fixPoint_X = move_pos.x();
            anchoSeleccion = fixed_point.x() - move_pos.x();
        } else {
            fixPoint_X = fixed_point.x();
            anchoSeleccion = move_pos.x() - fixed_point.x();
        }
        if (fixed_point.y() < move_pos.y()) {
            fixPoint_Y = fixed_point.y();
            altoSeleccion = move_pos.y() - fixed_point.y();
        } else {
            fixPoint_Y = move_pos.y();
            altoSeleccion = fixed_point.y() - move_pos.y();
        }
        rubberBand->setGeometry(fixPoint_X, fixPoint_Y, anchoSeleccion, altoSeleccion);
    }

    QPainter paint(&imageTemp);
    QPen paintPen(QColor( 0 , 255, 0 ));
    paintPen.setWidth(thickness);
    paint.setPen(paintPen);

    for (int i = 0 ; i < referencePoints.length() ; i++) {
        QLineF testLine(referencePoints.at(i).x(), referencePoints.at(i).y() , mouseX , mouseY);
        if (testLine.length() < 20) {
            if (scaleFactor > 1) {
                paint.drawEllipse(int((referencePoints.at(i).x() - 2 - positionCorrection.at(0) ) * scaleFactor) , int((referencePoints.at(i).y() - 2 - positionCorrection.at(1)) * scaleFactor) , 4, 4);
            } else {
                paint.drawEllipse(int(referencePoints.at(i).x() - 2) , int(referencePoints.at(i).y() - 2) , 4, 4);
            }
            ui->frameSelectedImageLabel->setPixmap(imageTemp);
            break;
        } else {
            ui->frameSelectedImageLabel->setPixmap(imageOriginal);
        }
    }
    paint.end();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (frameSelectedState == true) {
        //--------------------------------- Evento botón Esc --------------------------------
        if (event->key() == Qt::Key_Escape) {
            //--------------------------- Desconectar herramientas --------------------------
            disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseLineDraw(QPoint&)));
            disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseTextDraw(QPoint&)));
            disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseRectangleDraw(QPoint&)));
            disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseEllipseDraw(QPoint&)));
            disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseROISelection(QPoint&)));
            disconnect(ui->frameSelectedImageLabel, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseZoom(QPoint&)));

            //--------------------------- Desactivar herramientas ---------------------------
            if (annotationButtonState == true || rectangleToolState == true || lineToolState == true || ellipseToolState == true || roiToolState == true || zoominToolState == true) {
                ui->statusBar->showMessage(tr("Herramienta cancelada"), 2000);
                annotationButtonState = false;
                ui->annotationButton->setChecked(annotationButtonState);
                rectangleToolState = false;
                ui->rectanguloButton->setChecked(rectangleToolState);
                lineToolState = false;
                ui->lineaButton->setChecked(lineToolState);
                ellipseToolState = false;
                ui->elipseButton->setChecked(ellipseToolState);
                roiToolState = false;
                ui->roiButton->setChecked(roiToolState);
                zoominToolState = false;
                ui->zoomInButton->setChecked(zoominToolState);

                //-------------------------------- Reajustar GUI --------------------------------
                pointsX.clear();
                pointsY.clear();
                ui->frameSelectedImageLabel->setCursor(Qt::ArrowCursor);
                if (rubberBandState == true) {
                    rubberBand->~QRubberBand();
                    rubberBandState = false;
                }
                ui->cargarImagenButton->setEnabled(true);
                ui->annotationButton->setEnabled(true);
                ui->elipseButton->setEnabled(true);
                ui->lineaButton->setEnabled(true);
                ui->rectanguloButton->setEnabled(true);
                ui->roiButton->setEnabled(true);
                ui->deshacerButton->setEnabled(true);
                if (scaleFactor == 1.0) {
                    ui->zoomInButton->setEnabled(true);
                    ui->zoomOutButton->setEnabled(false);
                } else {
                    ui->zoomInButton->setEnabled(false);
                    ui->zoomOutButton->setEnabled(true);
                }
                ui->saveImageButton->setEnabled(true);
                ui->saveTextButton->setEnabled(true);
                ui->loadPreviousFrameToolButton->setEnabled(true);
                ui->loadNextFrameToolButton->setEnabled(true);
                ui->calibracionButton->setEnabled(true);
            }
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ui->frame->setGeometry(0,0,this->width(),90);
    if (int(videoWidth) != 0) {
        if (videoWidth > videoHeight) {
            frameWidth = (this->width() - 340) / 2;
            frameHeight = videoHeight * frameWidth / videoWidth;
            if (this->height() - 180 < frameHeight) {
                frameHeight = this->height() - 190;
                frameWidth = (frameHeight / videoHeight) * videoWidth;
            }
        } else {
            frameHeight = this->height() - 190;
            frameWidth = (frameHeight / videoHeight) * videoWidth;
        }
    } else {
        frameWidth = (this->width() - 340) / 2;
        frameHeight = this->height() - 190;
    }
    // Sección Video --------------------------------------------------------------------
    ui->infoFrame->setGeometry(10, int(((this->height() - ui->statusBar->height() + 90) / 2) - (frameHeight / 2) - 30), int(frameWidth), 25);
    ui->videoLabel->setGeometry(10, ui->infoFrame->y() + 30, int(frameWidth), int(frameHeight));
    ui->videoFrame->setGeometry(10, ui->videoLabel->y() + int(frameHeight) + 5, int(frameWidth), 25);
    // infoFrame ------------------------------------------------------------------------
    ui->videoNameValueLabel->resize(int(frameWidth) - 320, 25);
    ui->calibracionButton->setGeometry(int(frameWidth) - 25, 0, 25, 25);
    ui->calibracionStatusLabel->setGeometry(int(frameWidth) - 50, 0, 25, 25);
    ui->calibracionLabel->setGeometry(int(frameWidth) - 120, 0, 70, 25);
    ui->videoHeightValueLabel->setGeometry(int(frameWidth) - 160, 0, 40, 25);
    ui->videoHeightLabel->setGeometry(int(frameWidth) - 200, 0, 40, 25);
    ui->videoWidthValueLabel->setGeometry(int(frameWidth) - 240, 0, 40, 25);
    ui->videoWidthLabel->setGeometry(int(frameWidth) - 280, 0, 40, 25);
    // videoFrame -----------------------------------------------------------------------
    ui->sliderTrack->resize(int(frameWidth) - 235, 25);
    ui->lineEditFrame->setGeometry(int(frameWidth) - 50, 0, 50, 25);

    ui->cargarImagenButton->setGeometry(ui->videoLabel->width() + 20, static_cast<int>(ui->videoLabel->y() + (ui->videoLabel->height() / 2) - 12.5), 25, 25);

    // Sección Imagen -------------------------------------------------------------------
    ui->imageFrame->setGeometry(ui->cargarImagenButton->x() + 35, static_cast<int>(((this->height() - ui->statusBar->height() + 90) / 2) - (frameHeight / 2) - 30), static_cast<int>(frameWidth), 25);
    ui->frameSelectedImageLabel->setGeometry(ui->imageFrame->x(), ui->imageFrame->y() + 30, int(frameWidth), int(frameHeight));
    // imageFrame -----------------------------------------------------------------------
    ui->imageNameValueText->resize(int(frameWidth) - 185, 25);
    ui->saveImageButton->setGeometry(int(frameWidth) - 25, 0, 25, 25);

    // Sección Texto --------------------------------------------------------------------
    ui->textFrame->setGeometry(ui->imageFrame->x() + int(frameWidth) + 10, 100, 260, 25);
    ui->informationTextEdit->setGeometry(ui->textFrame->x(), 130, 260, this->height() - 190);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //------------------------------ Guardar la información -----------------------------
    if (ui->informationTextEdit->toPlainText() != "") {
        QMessageBox msgBox(QMessageBox::Warning, tr("Advertencia"), tr("¿Quiere guardar el texto?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Save:
            {
                QString textName = QString("%1/%2.txt").arg(videoResultFolder).arg(ui->textNameValueText->text());
                textName = QFileDialog::getSaveFileName(this, tr("Guardar el texto"),  textName, tr("Archivo de texto (*.txt)"));
                if (textName != "") {
                    QFile fileTxt(textName);
                    fileTxt.open(QIODevice::WriteOnly);
                    QTextStream out(&fileTxt);

                    QString textOut = ui->informationTextEdit->toPlainText();
                    textOut.replace("\n","\r\n");
                    out << textOut;
                    fileTxt.close();

                    // ---------------------- Guardar JSON ----------------------
                    QString jsonName = QString("%1/%2.json").arg(videoResultFolder).arg(ui->textNameValueText->text());
                    jsonName = QFileDialog::getSaveFileName(this, tr("Guardar el texto"), jsonName, tr("Archivo JSON (*.json)"));
                    if (jsonName != "") {
                        QJsonArray jsonArray, rectangleArray, lineArray, ellipseArray;
                        QJsonObject rectangleObject, lineObjects, ellipseObjects;

                        if (rectangleInformation.length() > 1) {
                            for (int i = 0; i < rectangleInformation.length(); i++) {
                                QStringList elements = rectangleInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                                if (elements.length() > 1) {
                                    QJsonObject carObject;
                                    carObject.insert("frame",elements[0]);
                                    carObject.insert("id",elements[1]);
                                    carObject.insert("tl_x",elements[2]);
                                    carObject.insert("tl_y",elements[3]);
                                    carObject.insert("br_x",elements[4]);
                                    carObject.insert("br_y",elements[5]);
                                    carObject.insert("c_x",elements[6]);
                                    carObject.insert("c_y",elements[7]);
                                    rectangleArray.append(carObject);
                                }
                            }
                        }
                        rectangleObject.insert("rectangle",rectangleArray);

                        if (lineInformation.length() > 1) {
                            for (int i = 0; i < lineInformation.length(); i++) {
                                QStringList elements = lineInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                                if (elements.length() > 1) {
                                    QJsonObject carObject;
                                    carObject.insert("frame",elements[0]);
                                    carObject.insert("id",elements[1]);
                                    carObject.insert("p1_x",elements[2]);
                                    carObject.insert("p1_y",elements[3]);
                                    carObject.insert("p2_x",elements[4]);
                                    carObject.insert("p2_y",elements[5]);
                                    carObject.insert("distance_px",elements[6]);
                                    carObject.insert("distance_m",elements[7]);
                                    lineArray.append(carObject);
                                }
                            }
                        }
                        lineObjects.insert("line",lineArray);

                        if (ellipseInformation.length() > 1) {
                            for (int i = 0; i < ellipseInformation.length(); i++) {
                                QStringList elements = ellipseInformation.at(i).split(QRegExp(",|[\r\n]"),QString::SkipEmptyParts);
                                if (elements.length() > 1) {
                                    QJsonObject carObject;
                                    carObject.insert("frame",elements[0]);
                                    carObject.insert("id",elements[1]);
                                    carObject.insert("tl_x",elements[2]);
                                    carObject.insert("tl_y",elements[3]);
                                    carObject.insert("br_x",elements[4]);
                                    carObject.insert("br_y",elements[5]);
                                    carObject.insert("c_x",elements[6]);
                                    carObject.insert("c_y",elements[7]);
                                    ellipseArray.append(carObject);
                                }
                            }
                        }
                        ellipseObjects.insert("ellipse",ellipseArray);

                        jsonArray.append(rectangleObject);
                        jsonArray.append(lineObjects);
                        jsonArray.append(ellipseObjects);

                        QFile jsonFile(jsonName);
                        jsonFile.open(QIODevice::WriteOnly);

                        QJsonDocument saveDoc(jsonArray);
                        jsonFile.write(saveDoc.toJson());
                    }
                }
                event->accept();
                break;
            }
            case QMessageBox::Discard:
                event->accept();
                break;
            case QMessageBox::Cancel:
                event->ignore();
                break;
            default:
                break;
        }
    }
}

QImage MainWindow::matToQImage(cv::Mat mat)
{
    if(mat.channels() == 1) {// if grayscale image
        return QImage(static_cast<uchar*>(mat.data), mat.cols, mat.rows, int(mat.step), QImage::Format_Indexed8);
    } else if(mat.channels() == 3) {// if 3 channel color image
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);     // invert BGR to RGB
        return QImage(static_cast<uchar*>(mat.data), mat.cols, mat.rows, int(mat.step), QImage::Format_RGB888);
    }
    return QImage();
}
