#ifndef COLUMNSCHEMA_H
#define COLUMNSCHEMA_H

#include <QMap>

class ColumnSchema
{    
public:
    enum ConstraintType {
        PRIMARY_KEY, FOREIGN_KEY, UNIQUE, CHECK
    };

public:
    ColumnSchema();
    ColumnSchema(const QString& _name,
                 const QString& _datatype,
                 const QString& _defaultvalue,
                 int _ordinalposition,
                 bool _isnullable
                 );

    void setName(const QString& _name);
    void setDataType(const QString& _datatype);
    void setDefaultValue(const QString& _defaultvalue);
    void setOrdinalPosition(int _ordinalposition);
    void setNullable(bool _isnullable);

    void setIsPkey(bool _is_pkey);
    void setIsFkey(bool _is_fkey);
    void setIsUnique(bool _is_unique);
    void setHasCheck(bool _has_check);
    void setCheckString(const QString& _checkstr);

    void addConstraint(ConstraintType _contype, const QString& _conname);
    QStringList ConstraintName(ConstraintType _contype);

    QString Name();
    QString DataType();
    QString DefaultValue();
    int OrdinalPosition();
    bool IsNullable();

    bool IsPkey();
    bool IsFkey();
    bool IsUnique();
    bool HasCheck();
    QString CheckString();


public:


private:
    QString name = "";
    QString datatype = "";
    QString defaultvalue = "";
    int ordinalposition = -1;
    bool isnullable = false;

    bool is_pkey = false;
    bool is_fkey = false;
    bool is_unique = false;
    bool has_check = false;
    QString checkstr = "";

    QMultiMap<ConstraintType, QString> connames; // <tip-constraint, nume-constraint>
};

#endif // COLUMNSCHEMA_H
