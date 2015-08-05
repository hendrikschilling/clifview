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
CvDatastore lfdataset;
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

  for(uint i=0;i<datasets.size();i++)
    ui->datasetList->addItem(QString(datasets[i].c_str()));
}

void ClifView::setView(int idx)
{
    int flags = 0;
    switch (ui->selViewProc->currentIndex()) {
        case 1 : flags = CLIF_DEMOSAIC; break;
    }

    lfdataset.readCvMat(idx, curview, flags);

    curview *= 1.0/256.0;
    curview.convertTo(curview, CV_8U);

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

    lfdataset = CvDatastore(lffile, group_str);

    ui->datasetSlider->setMaximum(lfdataset.count()-1);
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
