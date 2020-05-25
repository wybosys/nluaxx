#include "core.h"
#include "variant.h"
#include <math.h>

NLUA_BEGIN

Variant::VT FromCom(com::Variant::VT vt)
{
    switch (vt) {
    case com::Variant::VT::INT:
    case com::Variant::VT::UINT:
    case com::Variant::VT::LONG:
    case com::Variant::VT::ULONG:
    case com::Variant::VT::SHORT:
    case com::Variant::VT::USHORT:
    case com::Variant::VT::LONGLONG:
    case com::Variant::VT::ULONGLONG:
    case com::Variant::VT::CHAR:
    case com::Variant::VT::UCHAR:
        return Variant::VT::INTEGER;
    case com::Variant::VT::FLOAT:
    case com::Variant::VT::DOUBLE:
        return Variant::VT::NUMBER;
    case com::Variant::VT::OBJECT:
    case com::Variant::VT::POINTER:
        return Variant::VT::POINTER;
    case com::Variant::VT::BOOLEAN:
        return Variant::VT::BOOLEAN;
    case com::Variant::VT::STRING:
        return Variant::VT::STRING;
    }
    return Variant::VT::NIL;
}

Variant::Variant()
    :vt(VT::NIL)
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

Variant::Variant(string const& v)
    : vt(VT::STRING), _var(v)
{
}

Variant::Variant(char const* v)
    : vt(VT::STRING), _var(v)
{
}

Variant::Variant(void* v)
    : vt(VT::POINTER), _var(v)
{
}

Variant::Variant(nullptr_t)
    : vt(VT::POINTER), _var(nullptr)
{
}

Variant::Variant(com::Variant const& v)
    : vt(FromCom(v.vt)), _var(v)
{
}

Variant::operator integer() const
{
    switch (_var.vt) {
    case com::Variant::VT::INT:
        return (int)_var;
    case com::Variant::VT::UINT:
        return (unsigned int)_var;
    case com::Variant::VT::LONG:
        return (long)_var;
    case com::Variant::VT::ULONG:
        return (unsigned long)_var;
    case com::Variant::VT::SHORT:
        return (short)_var;
    case com::Variant::VT::USHORT:
        return (unsigned short)_var;
    case com::Variant::VT::LONGLONG:
        return (integer)(long long)_var;
    case com::Variant::VT::ULONGLONG:
        return (integer)(unsigned long long)_var;
    case com::Variant::VT::CHAR:
        return (char)_var;
    case com::Variant::VT::UCHAR:
        return (unsigned char)_var;
    case com::Variant::VT::BOOLEAN:
        return (bool)_var;
    case com::Variant::VT::FLOAT:
        return (integer)round((float)_var);
    case com::Variant::VT::DOUBLE:
        return (integer)round((double)_var);
    }
    return 0;
}

Variant::operator number() const
{
    switch (_var.vt) {
    case com::Variant::VT::FLOAT:
        return (float)_var;
    case com::Variant::VT::DOUBLE:
        return (double)_var;
    }
    return (integer)(*this);
}

Variant::operator bool() const 
{
    if (_var.vt == com::Variant::VT::BOOLEAN)
        return (bool)_var;
    return (number)(*this) != 0;
}

Variant::operator string const& () const
{
    return (string const&)_var;
}

Variant::operator void*() const
{
    switch (_var.vt) {
    case com::Variant::VT::OBJECT:
        return (com::IObject*)_var;
    case com::Variant::VT::POINTER:
        return (void*)_var;
    }
    return NULL;
}

NLUA_END
