#ifndef CALIBRACIONWINDOW_H
#define CALIBRACIONWINDOW_H

#include <QDialog>

#include <opencv2/core/core.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class CalibracionWindow;
}

class CalibracionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CalibracionWindow(QWidget *parent = nullptr);
    ~CalibracionWindow();

public slots:
    void imageCalibReceive(cv::Mat &imageCalibration, QString &videoName);
    void mousePosition(QPoint &move_pos);
    void mouseCalibLineDraw(QPoint &pos);
    void resizeEvent(QResizeEvent *);

private slots:
    void on_largoViaEdit_textChanged();
    void on_anchoViaEdit_textChanged();
    void on_empezarButton_clicked();
    void on_aceptarButton_clicked();
    void on_cancelarButton_clicked();

private:
    Ui::CalibracionWindow *ui;

    double videoWidth, videoHeight, frameWidth, frameHeight;
    QString fileName;
    QVector<float> pointsX, pointsY, xx, yy;
    QPixmap imageCalibTemp, pixmapCorrected;
    QImage qloadframe;
    cv::Mat matImageCalib, H;

    QImage matToQImage(cv::Mat mat);
};

#endif // CALIBRACIONWINDOW_H
