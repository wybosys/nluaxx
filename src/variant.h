#ifndef __NLUA_VARIANT_H_INCLUDED
#define __NLUA_VARIANT_H_INCLUDED

#include <vector>

NLUA_BEGIN

enum struct VariantType {
    UNKNOWN = 0,
    NIL = 0x10,
    NUMBER_MASK = 0x20,
    INTEGER = 0x21,
    DECIMAL = 0x22,
    POINTER = 0x23,
    STRING = 0x30,
    BOOLEAN = 0x40,
    ARRAY = 0x50,
};

class Variant {
public:

    Variant() = default;

    Variant(nullptr_t) {
        _type = VariantType::POINTER;
        _pod.ptr = nullptr;
    }

    Variant(void *ptr) {
        _type = VariantType::POINTER;
        _pod.ptr = ptr;
    }

    Variant(integer i) {
        _type = VariantType::INTEGER;
        _pod.i = i;
    }

    Variant(int i) {
        _type = VariantType::INTEGER;
        _pod.i = i;
    }

    Variant(number n) {
        _type = VariantType::DECIMAL;
        _pod.n = n;
    }

    Variant(bool b) {
        _type = VariantType::BOOLEAN;
        _pod.b = b;
    }

    Variant(string const &str) {
        _type = VariantType::STRING;
        _str = str;
    }

    Variant(char const *str) {
        _type = VariantType::STRING;
        _str = str;
    }

    Variant(Variant const &r) {
        _type = r._type;
        _str = r._str;
        _pod = r._pod;
        _arr = r._arr;
    }

    Variant(Variant const *r) {
        _type = r->_type;
        _str = r->_str;
        _pod = r->_pod;
        _arr = r->_arr;
    }

    ~Variant();

    inline void reset(void *ptr) {
        _clear();

        _type = VariantType::POINTER;
        _pod.ptr = ptr;
    }

    inline void reset(integer i) {
        _clear();

        _type = VariantType::INTEGER;
        _pod.i = i;
    }

    inline void reset(number n) {
        _clear();

        _type = VariantType::DECIMAL;
        _pod.n = n;
    }

    inline void reset(bool b) {
        _clear();

        _type = VariantType::BOOLEAN;
        _pod.b = b;
    }

    inline void reset(string const &str) {
        _clear();

        _type = VariantType::STRING;
        _str = str;
    }

    inline VariantType type() const {
        return _type;
    }

    inline operator string() const {
        return toString();
    }

    inline operator number() const {
        return toNumber();
    }

    inline operator integer() const {
        return toInteger();
    }

    string toString() const;

    number toNumber() const;

    integer toInteger() const;

    bool toBoolean() const;

    void *toPointer() const;

    typedef shared_ptr <vector<shared_ptr < Variant>>>
    array_type;

    array_type toArray() const;

protected:

    void _clear();

private:

    VariantType _type = VariantType::UNKNOWN;

    union {
        void *ptr;
        integer i;
        number n;
        bool b;
    } _pod;

    string _str;
    array_type _arr;
};

template<typename _CharT, typename _Traits>
inline basic_ostream <_CharT, _Traits> &operator<<(basic_ostream <_CharT, _Traits> &stm, Variant const &v) {
    switch (v.type()) {
        case VariantType::STRING:
            stm << v.toString();
            break;
        case VariantType::INTEGER:
            stm << v.toInteger();
            break;
        case VariantType::DECIMAL:
            stm << v.toNumber();
            break;
        case VariantType::POINTER:
            stm << v.toPointer();
            break;
        case VariantType::BOOLEAN:
            stm << v.toBoolean();
            break;
        default:
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
