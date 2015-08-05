#include <scaledqgraphicsview.h>


ScaledQGraphicsView::ScaledQGraphicsView(QWidget *parent)
    : QGraphicsView(parent) {};


void ScaledQGraphicsView::resizeEvent(QResizeEvent * event)
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);

    QGraphicsView::resizeEvent(event);
}
