#include <scaledqgraphicsview.h>

#include <QMouseEvent>


ScaledQGraphicsView::ScaledQGraphicsView(QWidget *parent)
    : QGraphicsView(parent) {};


void ScaledQGraphicsView::resizeEvent(QResizeEvent * event)
{
    if (fit)
        fitInView(sceneRect(), Qt::KeepAspectRatio);

    QGraphicsView::resizeEvent(event);
}

void ScaledQGraphicsView::mousePressEvent(QMouseEvent *me)
{
 if(me->button()==Qt::MiddleButton) {
    fit = !fit;

    if (fit)
        fitInView(sceneRect(), Qt::KeepAspectRatio);
    else {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setTransform(QTransform::fromScale(1,1));
    }
  }
 else
    QGraphicsView::mousePressEvent(me);
}

void ScaledQGraphicsView::wheelEvent(QWheelEvent * event)
{
    fit = false;

    //FIXME strange unit? No fixed "step" possible?
    int steps = event->angleDelta().y() / 120;

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    scale(pow(2, steps),pow(2, steps));

    event->accept();
}
