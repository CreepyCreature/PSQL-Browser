#ifndef TABLESCHEMA_H
#define TABLESCHEMA_H

#include "columnschema.h"

#include <QList>

#include <QtSql>

class TableSchema
{
public:
    TableSchema();
    TableSchema(const QSqlDatabase& _db,
                const QString& _tablename);

    void RetreiveColumnSchema(const QSqlDatabase& _db,
                         const QString& _tablename);

    ColumnSchema* Column(int index);
    void Clear();

    int RowCount() const;
    int ColumnCount() const;

    QString ColumnInfo();
    QString get_name();
private:
    void AddColumn(const ColumnSchema& _column);
private:
    QString tablename;
    QList<ColumnSchema> columns;

    int rowcount = 0;
    const int colcount = 10;
};

#endif // TABLESCHEMA_H
