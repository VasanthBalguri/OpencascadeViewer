#include "ocaftreedialog.h"
#include "ui_ocaftreedialog.h"

OcafTreeDialog::OcafTreeDialog(OcafDelegateModel* model,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OcafTreeDialog),model(model)
{
    ui->setupUi(this);
    ui->treeView->setModel(model);
    this->setWindowTitle("OCAF tree");
}

OcafTreeDialog::~OcafTreeDialog()
{
    delete model;
    delete ui;
}
