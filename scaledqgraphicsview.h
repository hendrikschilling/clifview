#ifndef SCALEDQGRAPHICSVIEW
#define SCALEDQGRAPHICSVIEW

#include <QGraphicsView>

class ScaledQGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    ScaledQGraphicsView(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *me);
    void wheelEvent(QWheelEvent * event);
private:
    bool fit = true;
};


#endif // SCALEDQGRAPHICSVIEW

