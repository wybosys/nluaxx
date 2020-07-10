#include "nlua++.hpp"
#define __NLUA_PRIVATE__
#include "android-prv.hpp"

#include <cross/cross.hpp>
#include <cross/stringbuilder.hpp>
#include <cross/logger.hpp>

USE_AJNI;
USE_CROSS;

NLUA_BEGIN

// 保存返回给Java层的C++对象
GsObject<Object> gs_objects;

shared_ptr<JVariant> toAjni(shared_ptr<Variant> const& var)
{
    if (!var || var->isnil())
        return make_shared<JVariant>(); // 不能返回空

    switch (var->vt)
    {
    case Variant::VT::INTEGER:
        return make_shared<JVariant>(var->toInteger());
    case Variant::VT::NUMBER:
        return make_shared<JVariant>(var->toNumber());
    case Variant::VT::BOOLEAN:
        return make_shared<JVariant>(var->toBool());
    case Variant::VT::STRING:
        return make_shared<JVariant>(var->toString());
    default:
        break;
    }

    // 其他类型均属于复杂类型，无法从lua直接转换到Java
    NLUA_ERROR("无法从类型 " << (int)var->vt << " 转换成Ajni的JVariant");

    return make_shared<JVariant>();
}

shared_ptr<Variant> fromAjni(shared_ptr<JVariant> const& var)
{
    if (!var || var->isnil())
        return make_shared<Variant>();

    switch (var->vt)
    {
    case JVariant::VT::INTEGER:
        return make_shared<Variant>(var->toInteger());
    case JVariant::VT::NUMBER:
        return make_shared<Variant>(var->toNumber());
    case JVariant::VT::BOOLEAN:
        return make_shared<Variant>(var->toBool());
    case JVariant::VT::STRING:
        return make_shared<Variant>(var->toString());
    default:
        break;
    }

    // 其他类型均属于复杂类型，无法从lua直接转换到Java
    NLUA_ERROR("无法从Ajni类型 " << (int)var->vt << " 转换成Variant");

    return make_shared<Variant>();
}

void LOG_WARN(string const& msg)
{
    Logger::Warn(msg);
}

void LOG_ERROR(string const& msg)
{
    Logger::Error(msg);
}

NLUA_END
