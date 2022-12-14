#ifndef FRAMEDISPLAY_H
#define FRAMEDISPLAY_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class frameDisplay : public QLabel
{
    Q_OBJECT
public:
    frameDisplay(QWidget *parent = nullptr);

protected:

    void mousePressEvent(QMouseEvent *mouse_event);

    void mouseMoveEvent(QMouseEvent *ev);

signals:

    void sendMousePosition(QPoint&);

    void sendMouseMovePos(QPoint&);

};

#endif // FRAMEDISPLAY_H
