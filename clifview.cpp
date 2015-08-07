#include "clifview.h"
#include "ui_clifview.h"

#include "clif.hpp"

#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <opencv2/opencv.hpp>

using namespace clif_cv;
using namespace std;
using namespace cv;

H5::H5File lffile;
CvDatastore lfdatastore;
Dataset lfdataset;
Mat curview;
QImage curview_q;
QGraphicsScene *curview_scene = NULL;
QGraphicsPixmapItem *curview_pmi = NULL;

QImage  cvMatToQImage( const cv::Mat &inMat )
{
  switch ( inMat.type() )
  {
     // 8-bit, 4 channel
     case CV_8UC4:
     {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

        return image;
     }

     // 8-bit, 3 channel
     case CV_8UC3:
     {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

        return image.rgbSwapped();
     }

     // 8-bit, 1 channel
     case CV_8UC1:
     {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
           for ( int i = 0; i < 256; ++i )
              sColorTable.push_back( qRgb( i, i, i ) );
        }

        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

        image.setColorTable( sColorTable );

        return image;
     }

     default:
        printf("conversion error!\n");
        //std::cout << "::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
  }

  return QImage();
}

ClifView::ClifView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClifView)
{
    ui->setupUi(this);
}

ClifView::~ClifView()
{
    delete ui;
}

void ClifView::on_actionOpen_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this,
        tr("Open clif File"));

  lffile = H5::H5File(filename.toLocal8Bit().constData(), H5F_ACC_RDONLY);

  vector<string> datasets = Datasets(lffile);

  for(uint i=0;i<datasets.size();i++) {
     QString path(datasets[i].c_str());
     ui->datasetList->addItem(path);
     QTreeWidgetItem *item = new QTreeWidgetItem(ui->tree, QStringList(path));
     item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
     item->setData(0, Qt::UserRole, QVariant(path));
  }
}

void ClifView::setView(int idx)
{
    int flags = 0;
    switch (ui->selViewProc->currentIndex()) {
        case 1 : flags = CLIF_DEMOSAIC; break;
    }

    lfdatastore.readCvMat(idx, curview, flags);

    curview *= 1.0/256.0;
    curview.convertTo(curview, CV_8U);

    imwrite("debug.tif", curview);

    curview_q = cvMatToQImage(curview);

    if (!curview_scene)
        curview_scene = new QGraphicsScene();

    ui->viewer->setScene(curview_scene);

    if (curview_pmi)
        delete curview_pmi;
    curview_pmi = curview_scene->addPixmap(QPixmap::fromImage(curview_q));
    ui->viewer->show();

}

void ClifView::on_datasetList_itemActivated(QListWidgetItem *item)
{
    string group_str("/clif/");
    group_str.append(item->text().toUtf8().constData());

    //printf("selected %s\n", group_str.c_str());

    lfdataset = Dataset(lffile, group_str);
    lfdatastore = CvDatastore(&lfdataset, "data");

    lfdataset.attrs.getTree().print();

    ui->datasetSlider->setMaximum(lfdatastore.count()-1);
    ui->datasetSlider->setValue(0);

    setView(0);
}

void ClifView::on_datasetSlider_valueChanged(int value)
{
    setView(value);
}

void ClifView::on_selViewProc_currentIndexChanged(int index)
{
    setView(ui->datasetSlider->value());
}

void attachTreeItem(QTreeWidgetItem *w, StringTree *t)
{
    if (t->val.second)
        w->setData(1, Qt::DisplayRole, QString(((Attribute*)t->val.second)->toString().c_str()));

    for(int i=0;i<t->childCount();i++) {
        QTreeWidgetItem *item = new QTreeWidgetItem(w, QStringList(QString(t->childs[i].val.first.c_str())));
        attachTreeItem(item, &t->childs[i]);
    }
}

void ClifView::on_tree_itemExpanded(QTreeWidgetItem *item)
{
  if (!item->data(0, Qt::UserRole).isValid())
      return;

  std::string root = item->data(0, Qt::UserRole).toString().toUtf8().constData();;

  string group_str("/clif/");
  group_str.append(root);

  //printf("selected %s\n", group_str.c_str());

  lfdataset = Dataset(lffile, group_str);

  StringTree tree = lfdataset.attrs.getTree();

  attachTreeItem(item, &tree);
}
