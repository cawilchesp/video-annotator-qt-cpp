#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QGraphicsScene>
#include <QSlider>
#include <QLineEdit>
#include <QRubberBand>
#include <QDockWidget>
#include <QTranslator>

#include <opencv2/core/core.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;

public slots:
    // Video Funciones
    void drawVideoFrame();
    void playVideo();
    void playBackwardsVideo();

    // Herramientas Funciones
    void mouseLineDraw(QPoint &pos);
    void mouseRectangleDraw(QPoint &pos);
    void mouseEllipseDraw(QPoint &pos);
    void mouseROISelection(QPoint &pos);
    void mouseTextDraw(QPoint &pos);

    // Varios
    void mouseZoom(QPoint &pos);
    void mousePosition(QPoint &move_pos);
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

signals:
    void imageCalibPass(cv::Mat &, QString &);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    // Archivo
    void on_abrirButton_clicked();
    void on_recientesComboBox_activated(const QString &arg1);
    void on_settingsButton_clicked();
    void on_languageComboBox_activated(int index);

    // Texto Menu
    void on_textoColorToolButton_clicked();
    void on_actionTextCustomColor_triggered();
    void on_actionTextoNegro_triggered();
    void on_actionTextoBlanco_triggered();
    void on_actionTextoAmarillo_triggered();
    void on_actionTextoAzul_triggered();
    void on_actionTextoRojo_triggered();
    void on_actionTextoVerde_triggered();
    void on_textFontComboBox_currentFontChanged();
    void on_textSizeSpinBox_valueChanged(int arg1);
    void on_boldFontButton_clicked();
    void on_ItalicFontButton_clicked();
    void on_annotationButton_clicked();

    // Herramientas Menu
    void on_thicknessToolButton_clicked();
    void on_action1_px_triggered();
    void on_action2_px_triggered();
    void on_action3_px_triggered();
    void on_action4_px_triggered();
    void on_action5_px_triggered();
    void on_ObjectColorToolButton_clicked();
    void on_actionObjetoNegro_triggered();
    void on_actionObjetoBlanco_triggered();
    void on_actionObjetoAmarillo_triggered();
    void on_actionObjetoAzul_triggered();
    void on_actionObjetoRojo_triggered();
    void on_actionObjetoVerde_triggered();
    void on_actionObjectCustomColor_triggered();
    void on_rectanguloButton_clicked();
    void on_roiButton_clicked();
    void on_lineaButton_clicked();
    void on_elipseButton_clicked();

    // Opciones Menu
    void on_zoomInButton_clicked();
    void on_zoomOutButton_clicked();
    void on_saveImageButton_clicked();
    void on_saveTextButton_clicked();
    void on_deshacerButton_clicked();

    // Video Menu
    void on_loadPreviousFrameToolButton_clicked();
    void on_loadNextFrameToolButton_clicked();
    void on_calibracionButton_clicked();

    // Ayuda
    void on_manualButton_clicked();
    void on_aboutButton_clicked();
    void on_aboutQtButton_clicked();

    // GUI
    void on_cargarImagenButton_clicked();

    // Video Bar
    void on_SlowPlayButton_clicked();
    void on_BackwardFrameButton_clicked();
    void on_BackwardPlayButton_clicked();
    void on_PauseButton_clicked();
    void on_ForwardPlayButton_clicked();
    void on_ForwardFrameButton_clicked();
    void on_FastPlayButton_clicked();
    void on_sliderTrack_sliderMoved(int position);
    void on_lineEditFrame_returnPressed();

    void on_temaComboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    //------------------------------------- Settings ------------------------------------
    QString videoFolder;                    // Ubicación de la carpeta de los archivos de video
    QString resultsFolder;                  // Ubicación de la carpeta donde se guardan los resultados
    int languageSet;                        // Idioma
    QStringList recentVideos;               // Lista de las ubicaciones de los archivos de video recientes
    QStringList classList;                  // Lista de las clases para las etiquetas
    QString annotationFontDefault;          // Fuente predeterminada
    int fontSizeDefault;                    // Tamaño de la fuente predeterminada
    QString videoNameRecent;                // Ubicación del archivo de video reciente seleccionado
    int temaDefault;                        // Tema de la aplicación

    QTranslator T;

    //-------------------------------------- Video --------------------------------------
    QTimer *timerForwardVideo;              // Timer de reproducción
    QTimer *timerBackwardVideo;             // Timer de reproducción inversa

    QString videoResultFolder;              // Ubicación de los resultados del archivo de video seleccionado
    QString videoName;                      // Ubicación del archivo de video seleccionado
    cv::VideoCapture video;                 // Captura del archivo de video seleccionado
    int totalFrames;                        // Propiedad del archivo de video - Número de cuadros totales
    double framesPerSecond;                 // Propiedad del archivo de video - Cuadros por segundo
    double videoWidth;                      // Propiedad del archivo de video - Ancho del video
    double videoHeight;                     // Propiedad del archivo de video - Alto del video

    int valueFrame;                         // Número del cuadro seleccionado
    int speedMultiplier;                    // Multiplicador de velocidad de reproducción
    double frameWidth;
    double frameHeight;
    double frame_msec;
    double valueFrameMsec;

    cv::Mat trackFrame;
    cv::Mat loadFrame;
    QStringList HTextLines;

    //-------------------------------------- Ayuda --------------------------------------
    QDockWidget *helpWidget;

    //------------------------------------ Anotación ------------------------------------
    QFont annotationFont;
    QFont annotationButtonFont;
    QColor annotationCustomColor;
    bool annotationBold;
    bool annotationItalic;
    bool annotationButtonState;

    //------------------------------------- Objetos -------------------------------------
    QColor customColorObject;
    QString objectLabel;
    double scaleFactor;
    bool calibrationFlag;
    bool roiFlag;
    bool rectangleToolState;
    bool lineToolState;
    bool ellipseToolState;
    int thickness;

    //------------------------------------ Opciones -------------------------------------
    bool roiToolState;
    bool zoominToolState;

    //----------------------------------- Información -----------------------------------
    QString rectangleTitle;
    QString lineTitle;
    QString ellipseTitle;

    QPixmap imageFrame;
    QPixmap imageZoomFrame;
    QPoint fixed_point;
    QRubberBand *rubberBand;
    bool rubberBandState;
    bool frameSelectedState;
    QVector<int> pointsX;
    QVector<int> pointsY;
    QVector<int> positionCorrection;
    QVector<int> contadorDeshacer;
    QVector<QPointF> referencePoints;
    QVector<QPixmap> imageDeshacer;
    QVector<QString> textoDeshacer;
    QVector<QString> lineInformation;
    QVector<QString> rectangleInformation;
    QVector<QString> ellipseInformation;
    QVector<QString> etiquetasDeshacer;

    QImage matToQImage(cv::Mat mat);
};

#endif // MAINWINDOW_H
