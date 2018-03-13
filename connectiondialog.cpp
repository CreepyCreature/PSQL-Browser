#include "connectiondialog.h"
#include "ui_connectiondialog.h"

#include <QDebug>

ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
}

ConnectionDialog::~ConnectionDialog()
{
    //delete ui;
}

QString ConnectionDialog::username() const
{
    return ui->userEdit->text();
}

QString ConnectionDialog::password() const
{
    return ui->passwordEdit->text();
}

QString ConnectionDialog::databasename() const
{
    return ui->dbnameEdit->text();
}

QString ConnectionDialog::hostname() const
{
    return ui->hostEdit->text();
}

void ConnectionDialog::on_connectButton_clicked()
{
    this->accept();
}
