#ifndef OCAFTREEDIALOG_H
#define OCAFTREEDIALOG_H

#include <QDialog>
#include "ocafdelegatemodel.h"

namespace Ui {
class OcafTreeDialog;
}

class OcafTreeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OcafTreeDialog(OcafDelegateModel* model,QWidget *parent = nullptr);
    ~OcafTreeDialog();

private:
    Ui::OcafTreeDialog *ui;
    OcafDelegateModel* model;
};

#endif // OCAFTREEDIALOG_H
