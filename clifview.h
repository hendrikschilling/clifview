#ifndef CLIFVIEW_H
#define CLIFVIEW_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui {
class ClifView;
}

class ClifView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClifView(QWidget *parent = 0);
    ~ClifView();

private slots:
    void on_actionOpen_triggered();

    void on_datasetList_itemActivated(QListWidgetItem *item);

    void on_pushFit_clicked();

private:
    Ui::ClifView *ui;
};

#endif // CLIFVIEW_H
