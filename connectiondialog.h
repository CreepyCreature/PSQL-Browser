#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = 0);
    ~ConnectionDialog();

    QString username() const;
    QString password() const;
    QString databasename() const;
    QString hostname() const;

private slots:
    void on_connectButton_clicked();
    void on_cancelButton_clicked() { this->reject(); }

private:
    Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
