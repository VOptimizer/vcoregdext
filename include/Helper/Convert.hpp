#ifndef CONVERT_HPP
#define CONVERT_HPP

#include <godot_cpp/variant/vector3.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
    namespace Convert
    {
        inline godot::Vector3 ToGDVec3(const VCore::Math::Vec3f &_V)
        {
            return godot::Vector3(_V.x, _V.y, _V.z);
        }

        inline VCore::Math::Vec3f ToVVec3(const godot::Vector3 &_V)
        {
            return VCore::Math::Vec3f(_V.x, _V.y, _V.z);
        }

        inline VCore::Math::Vec2ui ToVVec2ui(const godot::Vector2i &_V)
        {
            return VCore::Math::Vec2ui(_V.x, _V.y);
        }
    } // namespace Convert
} // namespace VCoreGDExt


#endif