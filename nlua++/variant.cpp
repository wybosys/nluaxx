#include "nlua++.hpp"
#define __NLUA_PRIVATE__
#include "variant.hpp"
#include <cmath>

#include <cross/cross.hpp>
#include <cross/str.hpp>

NLUA_BEGIN

USE_CROSS

Variant::VT FromCom(Variant::com_variant_type::VT vt)
{
    switch (vt) {
        case Variant::com_variant_type::VT::INT:
        case Variant::com_variant_type::VT::UINT:
        case Variant::com_variant_type::VT::LONG:
        case Variant::com_variant_type::VT::ULONG:
        case Variant::com_variant_type::VT::SHORT:
        case Variant::com_variant_type::VT::USHORT:
        case Variant::com_variant_type::VT::LONGLONG:
        case Variant::com_variant_type::VT::ULONGLONG:
        case Variant::com_variant_type::VT::CHAR:
        case Variant::com_variant_type::VT::UCHAR:
            return Variant::VT::INTEGER;
        case Variant::com_variant_type::VT::FLOAT:
        case Variant::com_variant_type::VT::DOUBLE:
            return Variant::VT::NUMBER;
        case Variant::com_variant_type::VT::OBJECT:
        case Variant::com_variant_type::VT::POINTER:
            return Variant::VT::POINTER;
        case Variant::com_variant_type::VT::BOOLEAN:
            return Variant::VT::BOOLEAN;
        case Variant::com_variant_type::VT::STRING:
            return Variant::VT::STRING;
        default:
            break;
    }
    return Variant::VT::NIL;
}

Variant::Variant()
    : vt(VT::NIL)
{
}

Variant::Variant(integer v)
    : vt(VT::INTEGER), _var(v)
{
}

Variant::Variant(number v)
    : vt(VT::NUMBER), _var(v)
{
}

Variant::Variant(bool v)
    : vt(VT::BOOLEAN), _var(v)
{
}

Variant::Variant(string const &v)
    : vt(VT::STRING), _var(v)
{
}

Variant::Variant(char const *v)
    : vt(VT::STRING), _var(v)
{
}

Variant::Variant(void *v)
    : vt(VT::POINTER), _var(v)
{
}

Variant::Variant(nullptr_t)
    : vt(VT::POINTER), _var(nullptr)
{
}

Variant::Variant(com_variant_type const &v)
    : vt(FromCom(v.vt)), _var(v)
{
}

Variant::Variant(shared_ptr<Object> const &r)
    : vt(VT::OBJECT), _obj(r)
{
    // 兼容android
    const_cast<com_variant_type &>(_var) = *r->toComVariant();
}

integer Variant::toInteger() const
{
    switch (_var.vt) {
        case com_variant_type::VT::INT:
            return _var.toInt();
        case com_variant_type::VT::UINT:
            return _var.toUInt();
        case com_variant_type::VT::LONG:
            return _var.toLong();
        case com_variant_type::VT::ULONG:
            return _var.toULong();
        case com_variant_type::VT::SHORT:
            return _var.toShort();
        case com_variant_type::VT::USHORT:
            return _var.toUShort();
        case com_variant_type::VT::LONGLONG:
            return (integer) _var.toLonglong();
        case com_variant_type::VT::ULONGLONG:
            return (integer) _var.toULonglong();
        case com_variant_type::VT::CHAR:
            return _var.toChar();
        case com_variant_type::VT::UCHAR:
            return _var.toUChar();
        case com_variant_type::VT::BOOLEAN:
            return _var.toBool();
        case com_variant_type::VT::FLOAT:
            return (integer) round(_var.toFloat());
        case com_variant_type::VT::DOUBLE:
            return (integer) round(_var.toDouble());
        case com_variant_type::VT::STRING:
            return toint(_var.toString());
        default:
            break;
    }
    return 0;
}

number Variant::toNumber() const
{
    switch (_var.vt) {
        case com_variant_type::VT::FLOAT:
            return _var.toFloat();
        case com_variant_type::VT::DOUBLE:
            return _var.toDouble();
        case com_variant_type::VT::STRING:
            return todouble(_var.toString());
        case com_variant_type::VT::NIL:
            return 0;
        default:
            break;
    }
    return toInteger();
}

bool Variant::toBool() const
{
    if (_var.vt == com_variant_type::VT::BOOLEAN)
        return _var.toBool();
    return toNumber() != 0;
}

string Variant::toString() const
{
    if (_var.vt == com_variant_type::VT::STRING)
        return _var.toString();

    if (_var.vt == com_variant_type::VT::FLOAT ||
        _var.vt == com_variant_type::VT::DOUBLE) {
        return tostr(toNumber());
    }

    return tostr((int) toInteger());
}

void *Variant::toPointer() const
{
    switch (_var.vt) {
        case com_variant_type::VT::OBJECT:
            return _var.toObject();
        case com_variant_type::VT::POINTER:
            return _var.toPointer();
        default:
            break;
    }
    return nullptr;
}

NLUA_END
