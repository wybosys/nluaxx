#ifndef __NLUA_VARIANT_H_INCLUDED
#define __NLUA_VARIANT_H_INCLUDED

NLUA_BEGIN

typedef ptrdiff_t integer;
typedef double number;

class Variant {
public:

    enum struct VT {
        NIL,
        INTEGER,
        NUMBER,
        BOOLEAN,
        POINTER,
        STRING
    };

    Variant();

    Variant(integer);

    Variant(number);

    Variant(bool);

    Variant(string const &);

    Variant(void *);

    Variant(char const *);

    Variant(nullptr_t);

    Variant(com::Variant const &);

    VT const vt;

    integer toInteger() const;

    number toNumber() const;

    bool toBool() const;

    void *toPointer() const;

    string const &toString() const;

    operator com::Variant const &() const;

private:
    com::Variant const _var;
};

inline Variant::operator com::Variant const &() const {
    return _var;
}

template<typename _CharT, typename _Traits>
inline basic_ostream <_CharT, _Traits> &operator<<(basic_ostream <_CharT, _Traits> &stm, Variant const &v) {
    switch (v.vt) {
        case Variant::VT::STRING:
            stm << v.toString();
            break;
        case Variant::VT::INTEGER:
            stm << v.toInteger();
            break;
        case Variant::VT::NUMBER:
            stm << v.toNumber();
            break;
        case Variant::VT::POINTER:
            stm << v.toPointer();
            break;
        case Variant::VT::BOOLEAN:
            stm << v.toBool();
            break;
    }
    return stm;
}

template<typename _CharT, typename _Traits>
inline basic_ostream <_CharT, _Traits> &operator<<(basic_ostream <_CharT, _Traits> &stm, shared_ptr <Variant> const &v) {
    if (!v)
        return stm;
    return stm << *v;
}

NLUA_END

#endif
