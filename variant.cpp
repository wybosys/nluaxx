#include "core.h"
#include "variant.h"
#include <sstream>

NLUA_BEGIN

Variant::~Variant() {
    _clear();
}

void Variant::_clear() {
    _type = VariantType::UNKNOWN;
    ::bzero(&_pod, sizeof(_pod));
    _arr = nullptr;
    _str.clear();
}

string Variant::toString() const {
    switch (_type) {
        case VariantType::STRING:
            return _str;
        case VariantType::INTEGER: {
            ::std::stringstream ss;
            ss << _pod.i;
            return ss.str();
        }
        case VariantType::DECIMAL: {
            ::std::stringstream ss;
            ss << _pod.n;
            return ss.str();
        }
        case VariantType::POINTER: {
            ::std::stringstream ss;
            ss << (ptrdiff_t) _pod.ptr;
            return ss.str();
        }
        case VariantType::BOOLEAN: {
            return _pod.b ? "true" : "false";
        }
        default:
            break;
    }

    return _str;
}

number Variant::toNumber() const {
    switch (_type) {
        case VariantType::STRING: {
            ::std::stringstream ss;
            ss << _str;
            number n;
            ss >> n;
            return n;
        }
        case VariantType::INTEGER:
            return _pod.i;
        case VariantType::DECIMAL:
            return _pod.n;
        case VariantType::POINTER:
            return (ptrdiff_t) _pod.ptr;
        case VariantType::BOOLEAN:
            return _pod.b ? 1 : 0;
        default:
            break;
    }

    return _pod.n;
}

integer Variant::toInteger() const {
    switch (_type) {
        case VariantType::STRING: {
            ::std::stringstream ss;
            ss << _str;
            integer n;
            ss >> n;
            return n;
        }
        case VariantType::INTEGER:
            return _pod.i;
        case VariantType::DECIMAL:
            return _pod.n;
        case VariantType::POINTER:
            return (ptrdiff_t) _pod.ptr;
        case VariantType::BOOLEAN:
            return _pod.b ? 1 : 0;
        default:
            break;
    }

    return _pod.i;
}

Variant::array_type Variant::toArray() const {
    return _arr;
}

bool Variant::toBoolean() const {
    switch (_type) {
        case VariantType::STRING:
            return _str != "false";
        case VariantType::INTEGER:
            return _pod.i != 0;
        case VariantType::DECIMAL:
            return _pod.n != 0;
        case VariantType::POINTER:
            return _pod.ptr != nullptr;
        default:
            break;
    }

    return _pod.b;
}

void *Variant::toPointer() const {
    switch (_type) {
        case VariantType::STRING:
            return (void *) _str.c_str();
        default:
            break;
    }

    return _pod.ptr;
}

NLUA_END
