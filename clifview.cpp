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
Mat curview;
QImage curview_q;
QGraphicsScene *curview_scene = NULL;
QGraphicsPixmapItem *curview_pmi = NULL;

vector<DatasetRoot*> root_list;
DatasetRoot *root_curr = NULL;

void attachTreeItem(QTreeWidgetItem *w, StringTree *t)
{
    if (t->val.second)
        w->setData(1, Qt::DisplayRole, QString(((Attribute*)t->val.second)->toString().c_str()));

    for(int i=0;i<t->childCount();i++) {
        QTreeWidgetItem *item = new QTreeWidgetItem(w, QStringList(QString(t->childs[i].val.first.c_str())));
        attachTreeItem(item, &t->childs[i]);
    }
}

class DatasetRoot {
public:
    bool expanded = false;
    bool active = false; //current data is this root
    CvDatastore datastore;
    Dataset dataset;
    std::string name;
    H5::H5File *f;

    DatasetRoot(std::string name_) : name(name_) {};

    void openDataset()
    {
        if (!dataset.valid()) {
            string group_str("/clif/");
            group_str.append(name);

            dataset = Dataset(lffile, group_str);
        }
    }

    void expand(QTreeWidgetItem *item)
    {
        openDataset();

        if (!expanded) {
            StringTree tree = dataset.attrs.getTree();

            attachTreeItem(item, &tree);
        }
    }

    void openDatastore()
    {
        openDataset();

        if (!datastore.valid()) {
            datastore = CvDatastore(&dataset, "data");
        }
    }
};

template <class T> class QVP
{  
public:
    static T* asPtr(QVariant v) { return  (T *) v.value<void *>(); }
    static QVariant asQVariant(T* ptr) { return qVariantFromValue((void *) ptr); }
};

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
  
  //FIXME clean root list!

  for(uint i=0;i<datasets.size();i++) {
     QString path(datasets[i].c_str());
     DatasetRoot *root = new DatasetRoot(datasets[i]);
     
     QTreeWidgetItem *item = new QTreeWidgetItem(ui->tree, QStringList(path));
     item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
     
     item->setData(0, Qt::UserRole, QVP<DatasetRoot>::asQVariant(root));
     root_list.push_back(root);
  }
}

void ClifView::setView(DatasetRoot *root, int idx)
{
    if (!root)
      return;
    int flags = 0;
    switch (ui->selViewProc->currentIndex()) {
        case 1 : flags = CLIF_DEMOSAIC; break;
    }

    root->datastore.readCvMat(idx, curview, flags);

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

void ClifView::on_datasetSlider_valueChanged(int value)
{
    setView(root_curr, value);
}

void ClifView::on_selViewProc_currentIndexChanged(int index)
{
    setView(root_curr, ui->datasetSlider->value());
}

void ClifView::on_tree_itemExpanded(QTreeWidgetItem *item)
{
    if (!item->data(0, Qt::UserRole).isValid())
      return;
    
    DatasetRoot *root = QVP<DatasetRoot>::asPtr(item->data(0, Qt::UserRole));
    
    root->expand(item);

    //std::string root = item->data(0, Qt::UserRole).toString().toUtf8().constData();;

    /*string group_str("/clif/");
    group_str.append(root);

    lfdataset = Dataset(lffile, group_str);
    lfdatastore = CvDatastore(&lfdataset, "data");*/

    /*ui->datasetSlider->setMaximum(lfdatastore.count()-1);
    ui->datasetSlider->setValue(0);

    setView(0);

    StringTree tree = lfdataset.attrs.getTree();

    attachTreeItem(item, &tree);*/
}

void ClifView::on_tree_itemActivated(QTreeWidgetItem *item, int column)
{
    if (!item->data(0, Qt::UserRole).isValid())
      return;
    
    DatasetRoot *root = QVP<DatasetRoot>::asPtr(item->data(0, Qt::UserRole));
    root_curr = root;
    
    root->openDatastore();
    setView(root, 0);
}
