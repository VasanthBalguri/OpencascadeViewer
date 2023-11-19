#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qOsg = new QtOSGWidget(1,1,this);
    ui->setupUi(this);
    ui->verticalLayout->addWidget(qOsg);

    connect(ui->actionBottle,SIGNAL(triggered(bool)),qOsg,SLOT(slotCreateBottle()));
    connect(ui->actionSample,SIGNAL(triggered(bool)),qOsg,SLOT(slotSampleProject()));
    connect(ui->actionRead_Step_File,SIGNAL(triggered(bool)),this,SLOT(slotStepFileDialog()));
    connect(ui->actionOCAF_Example,SIGNAL(triggered(bool)),qOsg,SLOT(slotOCAFExample()));
    connect(this,SIGNAL(signalLoadStep(std::string)),qOsg,SLOT(slotLoadStepFile(std::string)));
    this->setWindowTitle(QString("Opencascade Viewer"));
    resize(1024,768);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotStepFileDialog()
{
    QFileDialog fileDialog;
    QStringList fileNames;
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilter(tr("STEP (*.stp *.step)"));
    if(fileDialog.exec())
    {
        fileNames = fileDialog.selectedFiles();
        emit signalLoadStep(((QString)fileNames.first()).toStdString());
    }
    else
    {

    }

}
