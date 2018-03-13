#include "columnschema.h"
#include <QDebug>

ColumnSchema::ColumnSchema()
{
}

ColumnSchema::ColumnSchema(const QString& _name,
                           const QString& _datatype,
                           const QString& _defaultvalue,
                           int _ordinalposition,
                           bool _isnullable)
{
    name            = _name;
    datatype        = _datatype;
    defaultvalue    = _defaultvalue;
    ordinalposition = _ordinalposition;
    isnullable      = _isnullable;
}

// Setters : start

void ColumnSchema::setName(const QString& _name)
{
    name = _name;
}

void ColumnSchema::setDataType(const QString& _datatype)
{
    datatype = _datatype;
}

void ColumnSchema::setDefaultValue(const QString& _defaultvalue)
{
    defaultvalue = _defaultvalue;
}

void ColumnSchema::setOrdinalPosition(int _ordinalposition)
{
    ordinalposition = _ordinalposition;
}

void ColumnSchema::setNullable(bool _isnullable)
{
    isnullable = _isnullable;
}

void ColumnSchema::setIsPkey(bool _is_pkey)
{
    is_pkey = _is_pkey;
}

void ColumnSchema::setIsFkey(bool _is_fkey)
{
    is_fkey = _is_fkey;
}

void ColumnSchema::setIsUnique(bool _is_unique)
{
    is_unique = _is_unique;
}

void ColumnSchema::setHasCheck(bool _has_check)
{
    has_check = _has_check;
}

void ColumnSchema::setCheckString(const QString &_checkstr)
{
    checkstr = _checkstr;
}

// Setters : end

void ColumnSchema::addConstraint(ConstraintType _contype, const QString &_conname)
{
    auto it = connames.insert(_contype, _conname);
    //qDebug() << "ADDED CONSTRAINT: " << it.key() << it.value();
}

QStringList ColumnSchema::ConstraintName(ConstraintType _contype)
{
    QList<QString> list;
    list = connames.values(_contype);

    QStringList cnames(list);

    qDebug() << "Returned constraint names: ";
    for (auto s : cnames) {
        qDebug() << s;
    }

    return cnames;
}

// Getters : start

QString ColumnSchema::Name()
{
    return name;
}

QString ColumnSchema::DataType()
{
    return datatype;
}

QString ColumnSchema::DefaultValue()
{
    return defaultvalue;
}

int ColumnSchema::OrdinalPosition()
{
    return ordinalposition;
}

bool ColumnSchema::IsNullable()
{
    return isnullable;
}

bool ColumnSchema::IsPkey()
{
    return is_pkey;
}

bool ColumnSchema::IsFkey()
{
    return is_fkey;
}

bool ColumnSchema::IsUnique()
{
    return is_unique;
}

bool ColumnSchema::HasCheck()
{
    return has_check;
}

QString ColumnSchema::CheckString()
{
    return checkstr;
}

// Getters : end
