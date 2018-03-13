#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tableschema.h"

#include <QMainWindow>
#include <QtSql>
#include <QTreeWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void AddConnection(const QString&, const QString&,
                        const QString&, const QString&);

signals:
    void TableUpdated(QTreeWidgetItem* item, int column = 0);

private:
    void UpdateNullable();
    void update_pKey();
    void update_unique();
    void update_fKey();

    void populate_dropdown();

    bool TableHasPkey(TableSchema& table);

private slots:
    void on_connectButton_clicked();

    void on_treeWidget_itemActivated(QTreeWidgetItem *item, int column);

    void on_tableWidget_cellClicked(int row, int column);

    void on_updateButton_clicked();

    void on_fkeyBox_clicked();

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;

    TableSchema table;
    ColumnSchema* column = 0;
    QString current_tablename = "";
    QString current_columnname = "";
    int selectedrow = -1;

    QTreeWidgetItem* current_item = 0;
};

#endif // MAINWINDOW_H
