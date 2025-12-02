#ifndef CONVERT_HPP
#define CONVERT_HPP

#include "godot_cpp/variant/color.hpp"
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
    namespace Convert
    {
        inline godot::Vector3 ToGDVec3(const VCore::Math::Vec3f &_V)
        {
            return godot::Vector3(_V.x, _V.y, _V.z);
        }

        inline godot::Vector3 ToGDVec3i(const VCore::Math::Vec3i &_V)
        {
            return godot::Vector3i(_V.x, _V.y, _V.z);
        }

        inline godot::Vector2 ToGDVec2(const VCore::Math::Vec2f &_V)
        {
            return godot::Vector2(_V.x, _V.y);
        }

        inline VCore::Math::Vec3f ToVVec3(const godot::Vector3 &_V)
        {
            return VCore::Math::Vec3f(_V.x, _V.y, _V.z);
        }

        inline VCore::Math::Vec3f ToVVec3i(const godot::Vector3i &_V)
        {
            return VCore::Math::Vec3i(_V.x, _V.y, _V.z);
        }

        inline VCore::Math::Vec2f ToVVec2(const godot::Vector2 &_V)
        {
            return VCore::Math::Vec2f(_V.x, _V.y);
        }

        inline VCore::Math::Vec2ui ToVVec2ui(const godot::Vector2i &_V)
        {
            return VCore::Math::Vec2ui(_V.x, _V.y);
        }

        inline godot::Color ToGDColor(const VCore::CColor &p_Color)
        {
            return godot::Color(p_Color.R / 255.f, p_Color.G / 255.f, p_Color.B / 255.f, p_Color.A / 255.f);
        }

        inline VCore::CColor ToColor(const godot::Color &p_Color)
        {
            return VCore::CColor(p_Color.r * 255.f, p_Color.g * 255.f, p_Color.b * 255.f, p_Color.a * 255.f);
        }

        inline godot::Ref<godot::ImageTexture> TextureToGodot(const VCore::Texture &_Texture)
        {
            // Converts the colorpalette to a texture for godot.
            godot::Ref<godot::Image> img = godot::Image::create(_Texture->GetSize().x, _Texture->GetSize().y, false, godot::Image::Format::FORMAT_RGBA8);
            for (size_t x = 0; x < _Texture->GetSize().x; x++)
            {
                for (size_t y = 0; y < _Texture->GetSize().y; y++)
                {
                    VCore::CColor c;
                    c.FromARGB(_Texture->GetPixel(VCore::Math::Vec2ui(x, y)));

                    img->set_pixel(x, y, godot::Color(c.R / 255.f, c.G / 255.f, c.B / 255.f, c.A / 255.f));
                }
            }

            return godot::ImageTexture::create_from_image(img);
        }

        inline VCore::Texture GodotToTexture(const godot::Ref<godot::ImageTexture> &_Texture)
        {
            auto img = _Texture->get_image();
            if(img->get_format() != godot::Image::FORMAT_RGBA8)
                return nullptr;

            return std::make_shared<VCore::CTexture>(Convert::ToVVec2ui(img->get_size()), (uint32_t*)img->get_data().ptr());
        }
    } // namespace Convert
} // namespace VCoreGDExt


#endif