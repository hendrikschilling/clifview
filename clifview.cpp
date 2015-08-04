#include "clifview.h"
#include "ui_clifview.h"

#include "clif.hpp"

#include <QFileDialog>

using namespace clif;
using namespace std;

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

  H5::H5File lffile(filename.toLocal8Bit().constData(), H5F_ACC_RDONLY);

  vector<string> datasets = Datasets(lffile);

  for(uint i=0;i<datasets.size();i++)
    ui->datasetList->addItem(QString(datasets[i].c_str()));
}

void ClifView::on_datasetList_clicked(const QModelIndex &index)
{

}
