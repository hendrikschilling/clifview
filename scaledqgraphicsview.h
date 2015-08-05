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

};


#endif // SCALEDQGRAPHICSVIEW

