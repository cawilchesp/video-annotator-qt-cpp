#include "calibracionwindow.h"
#include "ui_calibracionwindow.h"
#include "framedisplay.h"

#include "opencv2/opencv.hpp"

#include <QSettings>
#include <QToolTip>
#include <QtCore>
#include <QtGui>
#include <QFileDialog>

CalibracionWindow::CalibracionWindow(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::CalibracionWindow)
{
    ui->setupUi(this);

    QRegExp regExp2 ("[0-9.,]{1,5}");
    ui->anchoViaEdit->setValidator(new QRegExpValidator (regExp2, this));
    ui->largoViaEdit->setValidator(new QRegExpValidator (regExp2, this));
}

CalibracionWindow::~CalibracionWindow()
{
    delete ui;
}

void CalibracionWindow::imageCalibReceive(cv::Mat &imageCalibration, QString &videoName)
{
    fileName = videoName;
    matImageCalib = imageCalibration;
    qloadframe = matToQImage(imageCalibration);
    imageCalibTemp = QPixmap::fromImage(qloadframe);
    videoWidth = qloadframe.width();
    videoHeight = qloadframe.height();

    // AJUSTE DE WIDGETS DE LA VENTANA ----------------------------------------------
    //-------------------- Ajuste de las ventanas de imágenes -------------------

    if (videoWidth > videoHeight) {
        frameWidth = (this->width() - 30) / 2;
        frameHeight = videoHeight * frameWidth / videoWidth;
    } else {
        frameHeight = this->height() - 50;
        frameWidth = (frameHeight / videoHeight) * videoWidth;
    }
    ui->lblCalibracion->setMaximumWidth(frameWidth);
    ui->lblCalibracion->setMinimumWidth(frameWidth);
    ui->lblCorreccion->setMaximumWidth(frameWidth);
    ui->lblCorreccion->setMinimumWidth(frameWidth);
    ui->lblCalibracion->setMaximumHeight(frameHeight);
    ui->lblCalibracion->setMinimumHeight(frameHeight);
    ui->lblCorreccion->setMaximumHeight(frameHeight);
    ui->lblCorreccion->setMinimumHeight(frameHeight);

    this->setMinimumHeight(50 + frameHeight);
    this->setMaximumHeight(50 + frameHeight);
    ui->lblCalibracion->setPixmap(imageCalibTemp);
    connect(ui->lblCalibracion, SIGNAL(sendMouseMovePos(QPoint&)), this, SLOT(mousePosition(QPoint&)));
}

void CalibracionWindow::mousePosition(QPoint &move_pos)
{
    int mouseX = move_pos.x() * (videoWidth / frameWidth);
    int mouseY = move_pos.y() * (videoHeight / frameHeight);

    QToolTip::showText(ui->lblCalibracion->mapToGlobal(move_pos), QString("%1,%2").arg(mouseX).arg(mouseY));
}

void CalibracionWindow::mouseCalibLineDraw(QPoint &pos)
{
    //------------------------------- Corrección de puntos ------------------------------
    pointsX.push_back(pos.x() * (videoWidth / frameWidth));
    pointsY.push_back(pos.y() * (videoHeight / frameHeight));

    //---------------------------------- Dibujar puntos ---------------------------------
    QPainter paint(&imageCalibTemp);
    QPen paintPen(QColor( 0, 255, 0 ));
    paintPen.setWidth(4);
    paint.setPen(paintPen);

    if (pointsX.length() == 1) {
        paint.drawEllipse(pointsX.at(0) - 1 , pointsY.at(0) - 1 , 2, 2);
        ui->lblCalibracion->setPixmap(imageCalibTemp);
    } else if (pointsX.length() == 2) {
        paint.drawEllipse(pointsX.at(1) - 1 , pointsY.at(1) - 1 , 2, 2);
        ui->lblCalibracion->setPixmap(imageCalibTemp);
    } else if (pointsX.length() == 3) {
        paint.drawEllipse(pointsX.at(2) - 1 , pointsY.at(2) - 1 , 2, 2);
        ui->lblCalibracion->setPixmap(imageCalibTemp);
    } else if (pointsX.length() == 4) {
        paint.drawEllipse(pointsX.at(3) - 1 , pointsY.at(3) - 1 , 2, 2);
        ui->lblCalibracion->setPixmap(imageCalibTemp);

        float anchoDist = ui->anchoViaEdit->text().toDouble();
        float largoDist = ui->largoViaEdit->text().toDouble();

        std::vector<cv::Point2f> pts_src, pts_dst, pts_dst_image;
        pts_src.push_back(cv::Point2f( pointsX[0] , pointsY[0] ));
        pts_src.push_back(cv::Point2f( pointsX[1] , pointsY[1] ));
        pts_src.push_back(cv::Point2f( pointsX[2] , pointsY[2] ));
        pts_src.push_back(cv::Point2f( pointsX[3] , pointsY[3] ));

        pts_dst.push_back(cv::Point2f( 0, 0 ));
        pts_dst.push_back(cv::Point2f( largoDist, 0 ));
        pts_dst.push_back(cv::Point2f( largoDist, anchoDist ));
        pts_dst.push_back(cv::Point2f( 0, anchoDist ));

        pts_dst_image.push_back(cv::Point2f( 0, 0 ));
        pts_dst_image.push_back(cv::Point2f( videoWidth, 0 ));
        pts_dst_image.push_back(cv::Point2f( videoWidth, videoHeight ));
        pts_dst_image.push_back(cv::Point2f( 0, videoHeight ));

        cv::Mat outputImage;
        H = cv::getPerspectiveTransform(pts_src, pts_dst);
        cv::Mat H2 = cv::getPerspectiveTransform(pts_src, pts_dst_image);
        cv::warpPerspective(matImageCalib, outputImage, H2, cv::Size(videoWidth,videoHeight));

        QImage imageCorrected = matToQImage(outputImage);
        ui->lblCorreccion->setPixmap(QPixmap::fromImage(imageCorrected));

        ui->lblCalibracion->setCursor(Qt::ArrowCursor);
        disconnect(ui->lblCalibracion, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseCalibLineDraw(QPoint&)));
        pointsX.clear();
        pointsY.clear();
        paint.end();
        ui->anchoViaEdit->setEnabled(true);
        ui->largoViaEdit->setEnabled(true);
        ui->empezarButton->setEnabled(false);
    }
}

void CalibracionWindow::on_empezarButton_clicked()
{
    imageCalibTemp = QPixmap::fromImage(qloadframe);
    ui->lblCalibracion->setPixmap(imageCalibTemp);
    ui->lblCorreccion->clear();

    ui->anchoViaEdit->setEnabled(false);
    ui->largoViaEdit->setEnabled(false);

    pointsX.clear();
    pointsY.clear();

    ui->lblCalibracion->setCursor(Qt::CrossCursor);
    connect(ui->lblCalibracion, SIGNAL(sendMousePosition(QPoint&)), this, SLOT(mouseCalibLineDraw(QPoint&)));
}

void CalibracionWindow::on_aceptarButton_clicked()
{
    fileName = fileName.left(fileName.length() - 4);
    fileName.append("_CalibrationData.txt");

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << QString("%1\t%2\t%3\r\n"
                   "%4\t%5\t%6\r\n"
                   "%7\t%8\t%9\r\n")
       .arg(H.at<double>(0,0)).arg(H.at<double>(0,1)).arg(H.at<double>(0,2))
       .arg(H.at<double>(1,0)).arg(H.at<double>(1,1)).arg(H.at<double>(1,2))
       .arg(H.at<double>(2,0)).arg(H.at<double>(2,1)).arg(H.at<double>(2,2));
    this->close();
}

void CalibracionWindow::on_cancelarButton_clicked()
{
    this->close();
}

void CalibracionWindow::on_largoViaEdit_textChanged()
{
    if ((ui->anchoViaEdit->text() != "") && (ui->largoViaEdit->text() != "")) {
        ui->empezarButton->setEnabled(true);
    } else {
        ui->empezarButton->setEnabled(false);
    }
}

void CalibracionWindow::on_anchoViaEdit_textChanged()
{
    if ((ui->anchoViaEdit->text() != "") && (ui->largoViaEdit->text() != "")) {
        ui->empezarButton->setEnabled(true);
    } else {
        ui->empezarButton->setEnabled(false);
    }
}

void CalibracionWindow::resizeEvent(QResizeEvent *)
{
    ui->lblCalibracion->setMaximumHeight(9999);
    ui->lblCalibracion->setMaximumWidth(9999);
    ui->lblCorreccion->setMaximumHeight(9999);
    ui->lblCorreccion->setMaximumWidth(9999);
    if (videoWidth > videoHeight) {
        frameWidth = (this->width() - 30) / 2;
        frameHeight = videoHeight * frameWidth / videoWidth;
    } else {
        frameHeight = this->height() - 50;
        frameWidth = (frameHeight / videoHeight) * videoWidth;
    }
    ui->lblCalibracion->setMaximumWidth(frameWidth);
    ui->lblCalibracion->setMinimumWidth(frameWidth);
    ui->lblCorreccion->setMaximumWidth(frameWidth);
    ui->lblCorreccion->setMinimumWidth(frameWidth);
    ui->lblCalibracion->setMaximumHeight(frameHeight);
    ui->lblCalibracion->setMinimumHeight(frameHeight);
    ui->lblCorreccion->setMaximumHeight(frameHeight);
    ui->lblCorreccion->setMinimumHeight(frameHeight);

    this->setMinimumHeight(50 + frameHeight);
    this->setMaximumHeight(50 + frameHeight);
}

QImage CalibracionWindow::matToQImage(cv::Mat mat)
{
    if(mat.channels() == 1) {// if grayscale image
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
    } else if(mat.channels() == 3) {// if 3 channel color image
//        cv::cvtColor(mat, mat, CV_BGR2RGB);     // invert BGR to RGB
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    }
    return QImage();
}
