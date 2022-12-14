#include "framedisplay.h"

frameDisplay::frameDisplay(QWidget *parent) : QLabel(parent)
{
    setMouseTracking(false);
}

void frameDisplay::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint mouse_pos = ev->pos();
    emit sendMouseMovePos(mouse_pos);
}

void frameDisplay::mousePressEvent(QMouseEvent *mouse_event)
{
    if (mouse_event->button() == Qt::LeftButton) {
        QPoint mouse_pos = mouse_event->pos();
        emit sendMousePosition(mouse_pos);
    }
}
