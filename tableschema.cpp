#include "tableschema.h"

#include <QMessageBox>

TableSchema::TableSchema()
{
}

TableSchema::TableSchema(
        const QSqlDatabase& _db,
        const QString& _tablename
        )
{

    if (!_db.isOpen() || _tablename == "")
        { return; }
}

void TableSchema::RetreiveColumnSchema(
        const QSqlDatabase& _db,
        const QString& _tablename

        )
{
    if (!_db.isOpen() || _tablename == "")
        { return; }

    QSqlQuery q(_db);
    QString querystr = QString(
            "SELECT column_name, data_type, column_default, "
                "ordinal_position, is_nullable "
            "FROM INFORMATION_SCHEMA.COLUMNS "
            "WHERE table_name = '" + _tablename + "'"
           );
    qDebug() << querystr;
    q.exec(querystr);
    qDebug() << q.lastError().text();

//    QSqlRecord record = q.record();
//    for (int i = 0; i < record.count(); ++i) {

//        QSqlField field = record.field(i);
//        qDebug() << field.name() << " ";
//    }

    QSqlQuery conq(_db);
    QString constr =
      "SELECT relname AS table "
            ",attname AS column "
            ",conname "
            ",contype "
            ",consrc "
      "FROM   pg_constraint c "
      "JOIN   pg_namespace n ON n.oid = c.connamespace "
      "JOIN   pg_class cls ON cls.oid = c.conrelid "
      "JOIN   pg_attribute a ON a.attnum = ANY(c.conkey) "
      "AND    a.attrelid = c.conrelid "
      "AND    n.nspname = 'public' "
      "WHERE relname = :table "
      "AND   attname = :column ";
    while (q.next()) {

        ColumnSchema column;
        column.setName(q.value("column_name").toString());
        column.setDataType(q.value("data_type").toString());
        column.setDefaultValue(q.value("column_default").toString());
        column.setOrdinalPosition(q.value("ordinal_position").toInt());
        column.setNullable(q.value("is_nullable").toString() == "YES" ? true : false);

//        qDebug() << "Colname: " << column.Name();
//        qDebug() << "Data Type: " << column.DataType();
//        qDebug() << "Default: " << column.DefaultValue();
//        qDebug() << "Ordinal Position: " << column.OrdinalPosition();
//        qDebug() << "Nullable: " << column.IsNullable();
//        qDebug() << "\n";

        conq.prepare(constr);
        conq.bindValue(":table", _tablename);
        conq.bindValue(":column", column.Name());
        if (!conq.exec()) {
            QMessageBox::warning((QWidget*)0, QString("Unsuccessful Query"),
                                 conq.lastError().text());
        }

        QStringList checkstringlist;
        while (conq.next()) {
            //qDebug() << "contype: " << conq.value("contype").toString();
            switch (conq.value("contype").toString().toStdString().at(0)) {
            case 'p':
                column.setIsPkey(true);
                column.setIsUnique(true);
                column.addConstraint(ColumnSchema::PRIMARY_KEY, conq.value("conname").toString());
                break;
            case 'f':
                column.setIsFkey(true);
                column.addConstraint(ColumnSchema::FOREIGN_KEY, conq.value("conname").toString());
                break;
            case 'u':
                column.setIsUnique(true);
                column.addConstraint(ColumnSchema::UNIQUE, conq.value("conname").toString());
                break;
            case 'c':
                column.setHasCheck(true);
                checkstringlist << conq.value("consrc").toString();
                column.addConstraint(ColumnSchema::CHECK, conq.value("conname").toString());
                break;
            }

        }

        if (column.HasCheck()) {
            QString checkstring;
            checkstring = checkstringlist.at(0);
            for (int i = 1; i < checkstringlist.count(); ++i) {
                checkstring += " | " + checkstringlist.at(i);
            }
            column.setCheckString(checkstring);
        }
//        qDebug() << "Is Pkey: " << column.IsPkey();
//        qDebug() << "Is Fkey: " << column.IsFkey();
//        qDebug() << "Unique: " << column.IsUnique();
//        qDebug() << "Has Check: " << column.HasCheck();
//        qDebug() << "Check: " << column.CheckString();
//        qDebug() << "\n";

        this->AddColumn(column);
    }
}

void TableSchema::AddColumn(const ColumnSchema& _column)
{
    columns.push_back(_column);
    rowcount++;
}

QString TableSchema::ColumnInfo()
{
    QString info;
    for (auto col : columns) {

        info += "Colname: " + col.Name() + "\n";
        info += "Datatype: " + col.DataType() + "\n";
        info += "Default: " + col.DefaultValue() + "\n";
        info += "Position: " + QString::number(col.OrdinalPosition()) + "\n";
        info += "Nullable: " + (col.IsNullable() ? QString("YES") : QString("NO")) + "\n";
    }
    return info;
}

ColumnSchema* TableSchema::Column(int index)
{
    return &columns[index];
}

void TableSchema::Clear()
{
    tablename = "";
    columns.clear();
    rowcount = 0;
}

int TableSchema::RowCount() const
{
    return rowcount;
}

int TableSchema::ColumnCount() const // DOES NOT RETURN NUMBER OF COLUMNS IN TABLE SCHEMA!
{
    return colcount;
}

QString TableSchema::get_name()
{
    return this->tablename;
}
