#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectiondialog.h"

#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->treeWidget->setHeaderLabel("database");
    ui->tableWidget->setAlternatingRowColors(true);

    db = QSqlDatabase::addDatabase("QPSQL");

    connect(this, SIGNAL(TableUpdated(QTreeWidgetItem*,int)),
                         this, SLOT(on_treeWidget_itemActivated(QTreeWidgetItem*,int)));
}

MainWindow::~MainWindow()
{
    delete ui;

    db.close();
}

static void qSetBold(QTreeWidgetItem *item, bool bold)
{
    QFont font = item->font(0);
    font.setBold(bold);
    item->setFont(0, font);
}

void MainWindow::AddConnection(const QString &_user, const QString &_password,
                               const QString &_dbname, const QString &_host)
{
    if (db.isOpen()) {
        db.close();
        ui->treeWidget->clear();
    }

    db.setUserName(_user);
    db.setPassword(_password);
    db.setDatabaseName(_dbname);
    db.setHostName(_host);

    if (db.open()) {
        ui->statusLabel->setText(tr("Successfully connected to the database!"));
    } else {
        QMessageBox::warning(this, tr("Failed connection attempt"),
                             db.lastError().text());
        ui->statusLabel->setText("Failed to connect to the database");
        db = QSqlDatabase::addDatabase("QPSQL");
    }

    if (db.isOpen()) {

        QTreeWidgetItem* treeroot = new QTreeWidgetItem(ui->treeWidget);
        treeroot->setText(0, db.userName().append("@").append(db.databaseName()));
        qSetBold(treeroot, true);

        QStringList tables = db.tables();
        for (int i = 0; i < tables.count(); ++i) {

            QTreeWidgetItem* table = new QTreeWidgetItem(treeroot);
            table->setText(0, tables.at(i));
        }
    }
}

void MainWindow::on_connectButton_clicked()
{
    ConnectionDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        this->AddConnection(dialog.username(), dialog.password(),
                            dialog.databasename(), dialog.hostname());
    }
}

void MainWindow::on_treeWidget_itemActivated(QTreeWidgetItem *item, int column)
{
    if (!item->parent()) { return; }

    this->current_item = item;

    if (this->current_tablename != "") {
        ui->tableWidget->clear();
        table.Clear();
    }

    this->current_tablename = item->text(column);
    table.RetreiveColumnSchema(db, item->text(column));

    qDebug() << "Selected table: " << this->current_tablename;

    ui->tableWidget->setColumnCount(table.ColumnCount());
    ui->tableWidget->setRowCount(table.RowCount());

    QStringList headers;
    headers << "Name" << "DataType" << "DefaultValue" << "Position" << "Nullable"
            << "Is Pkey" << "Is Fkey" << "Unique" << "Has Check" << "Check";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        ColumnSchema* col = table.Column(i);
        QList<QString> colstr;

        colstr.push_back(col->Name());
        colstr.push_back(col->DataType());
        colstr.push_back(col->DefaultValue());
        colstr.push_back(QString::number(col->OrdinalPosition()));
        colstr.push_back(col->IsNullable() ? "YES" : "NO");
        colstr.push_back(col->IsPkey() ? "YES" : "NO");
        colstr.push_back(col->IsFkey() ? "YES" : "NO");
        colstr.push_back(col->IsUnique() ? "YES" : "NO");
        colstr.push_back(col->HasCheck() ? "YES" : "NO");
        colstr.push_back(col->CheckString());

        for (int j = 0; j < ui->tableWidget->columnCount(); ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(colstr[j]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(i, j,
                                     item
                                     );
        }
    }

    ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    ColumnSchema* col = table.Column(row);
    this->column = col;

    this->selectedrow = row;
    this->current_columnname = col->Name();

    qDebug() << "SELECTED ROW: " << row;

    //QModelIndex index = ui->tableWidget->indexAt(QPoint(row, column));

    ui->nullBox->setChecked(col->IsNullable());
    ui->pkeyBox->setChecked(col->IsPkey());
    ui->uniqueBox->setChecked(col->IsUnique());
    ui->fkeyBox->setChecked(col->IsFkey());
    if(ui->fkeyBox->isChecked())
    {
        ui->fkeyList->setEnabled(true);
        populate_dropdown();
    }
    else
    {
        ui->fkeyList->clear();
        ui->fkeyList->setEnabled(false);
    }


//    QSqlQuery q(db);
//    QString querystr = QString(
//            "SELECT column_name, data_type, column_default, "
//                "ordinal_position, is_nullable "
//            "FROM INFORMATION_SCHEMA.COLUMNS "
//            "WHERE table_name = '" + table + "'"
//           );
//    qDebug() << querystr;
//    q.exec(querystr);
//    qDebug() << q.lastError().text();
}

void MainWindow::populate_dropdown()
{
    QStringList tables = db.tables();
    QStringList listy;
    TableSchema ts;
    for(int k = 0; k < tables.size(); ++k)
    {
        qDebug() << tables.at(k) << " AT " << k;
        if(tables.at(k) == this->current_tablename) continue;

        ts.RetreiveColumnSchema(db,tables.at(k));
        ColumnSchema* cs;
        for(int i = 0; i < ts.RowCount(); ++i)
        {
            cs = ts.Column(i);
            if((cs->IsPkey() || cs->IsUnique()) && (this->column->DataType() == cs->DataType()))
            {
                QString str = QString(cs->Name() + "@" + this->current_tablename);
                listy.push_back(str);
            }
        }
    }
    ui->fkeyList->addItems(listy);

}

void MainWindow::on_updateButton_clicked()
{
    if (!column || current_tablename == "" || !db.isOpen()) { return; }

    UpdateNullable();
    update_pKey();
    update_unique();
    update_fKey();

    emit TableUpdated(this->current_item);
}

void MainWindow::update_fKey()
{
        //- !fkey -> fkey
        if(ui->fkeyBox->isChecked())
        {
            if(!column->IsFkey())
            {
                QStringList strl = ui->fkeyList->currentText().split("@");

                QSqlQuery q(this->db);
                q.exec(QString(
                           "ALTER TABLE " + this->current_tablename
                           + " ADD FOREIGN KEY (" + this->current_columnname + ")"
                           + " REFERENCES " + strl.at(1) + " (" + strl.at(0) + ");"
                           ));
                qDebug() << q.lastError().text() << "\n" << q.lastQuery();
            }

        }
        //- fkey -> !fkey
        else if(!ui->fkeyBox->isChecked())
        {
            if(column->IsFkey())
            {

                QSqlQuery q(this->db);
                q.exec(QString(
                           "ALTER TABLE " + this->current_tablename
                           + " DROP CONSTRAINT " + this->current_tablename + "_" + this->current_columnname + "_fkey;"
                           ));
                qDebug() << q.lastError().text() << "\n" << q.lastQuery();
            }
        }
}

//- The convention for unique constraints will be unique_<column>
void MainWindow::update_unique()
{

    //- add unique constraint to column
    if (ui->uniqueBox->isChecked())
    {
        if (!column->IsUnique())
        {
            //Create index
            QSqlQuery q(this->db);
            q.exec(QString(
                       "CREATE UNIQUE INDEX CONCURRENTLY " + this->current_tablename + "_"
                       + this->current_columnname + " ON " + this->current_tablename + "("
                       + this->current_columnname + ");"
                       ));
            qDebug() << q.lastError().text() << "\n" << q.lastQuery();

            //Add constraint
            QSqlQuery q2(this->db);
            q2.exec(QString(
                       "ALTER TABLE " + this->current_tablename
                       + " ADD CONSTRAINT unique_" + this->current_columnname
                       + " UNIQUE USING INDEX " + this->current_tablename + "_" + this->current_columnname + ";"
                       ));
            qDebug() << q2.lastError().text() << "\n" << q2.lastQuery();
        }
    }
    // drop unique constraint from column
    else if (!ui->uniqueBox->isChecked())
    {
        if (column->IsUnique())
        {
            QSqlQuery q(this->db);
            q.exec(QString(
                       "ALTER TABLE " + this->current_tablename
                       + " DROP CONSTRAINT unique_" + this->current_columnname + ";"
                       ));
            qDebug() << q.lastError().text() << "\n" << q.lastQuery();
        }
    }
}

void MainWindow::update_pKey()
{
    //- !pkey -> pkey
    if(ui->pkeyBox->isChecked())
    {
        if(!column->IsPkey())
        {
            if (TableHasPkey(this->table)) {
                QMessageBox::warning(this,
                                     "Multiple PKey",
                                     "Table already has a Primary Key!"
                                     );
                return;
            }

            QSqlQuery q(this->db);
            q.exec(QString(
                       "ALTER TABLE " + this->current_tablename
                       + " ADD PRIMARY KEY (" + this->current_columnname + ");"
                       ));
            qDebug() << q.lastError().text() << "\n" << q.lastQuery();

        }
    }
    //- pkey -> !pkey
    else if(!ui->pkeyBox->isChecked()) {
        if(column->IsPkey()) {
            QStringList cname = column->ConstraintName(column->PRIMARY_KEY);
            QSqlQuery q(this->db);

            for (auto pks : cname) {
                q.exec(QString(
                    "ALTER TABLE " + this->current_tablename
                    + " DROP CONSTRAINT " + pks
                    ));
                qDebug() << q.lastError().text() << "\n" << q.lastQuery();
                if (q.lastError().text().contains("other objects depend on it", Qt::CaseInsensitive)) {
                    QMessageBox::warning(this,
                                         "Cannot Drop Pkey",
                                         "The key you are trying to drop is referenced in another table!"
                                         );
                    return;
                }
            }
        }
    }
}

bool MainWindow::TableHasPkey(TableSchema& _table)
{
    for (int i = 0; i < _table.RowCount(); ++i)
    {
        ColumnSchema* col = _table.Column(i);
        if (col->IsPkey())
        { return true; }
    }
    return false;
}

void MainWindow::UpdateNullable()
{
    if (ui->nullBox->isChecked()) {
        if (!column->IsNullable()) {
            // QUERY SA SE FACA NULLABLE
            QSqlQuery q(this->db);
            q.exec(QString(
                       "ALTER TABLE " + this->current_tablename
                       + " ALTER COLUMN " + this->current_columnname + " DROP NOT NULL"
                       ));
            qDebug() << q.lastError().text() << "\n" << q.lastQuery();
        }
    }
    else if (!ui->nullBox->isChecked()) {
        if (column->IsNullable()) {
            // QUERY SA SE FACA NOT NULLABLE
            QSqlQuery q(this->db);
            q.exec(QString(
                       "ALTER TABLE " + this->current_tablename
                       + " ALTER COLUMN " + this->current_columnname + " SET NOT NULL"
                       ));
            qDebug() << q.lastError().text() << "\n" << q.lastQuery();
        }
    }
}

void MainWindow::on_fkeyBox_clicked()
{
    if(ui->fkeyBox->isChecked())
    {
        ui->fkeyList->setEnabled(true);
        populate_dropdown();
    }
    else
    {
        ui->fkeyList->clear();
        ui->fkeyList->setEnabled(false);
    }
}
