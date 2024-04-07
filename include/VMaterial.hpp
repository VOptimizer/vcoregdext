#ifndef VMaterial_HPP
#define VMaterial_HPP

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/texture.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <VCore/VCore.hpp>

#include "Helper/StaticType.hpp"

namespace VCoreGDExt
{
    using StaticRID = TStaticType<godot::RID>;

    /**
     * @brief Represents a voxel material. This class can be use like a StandardMaterial3D, however if you plan to use this library only at development time you need to convert this material into an actual StandardMaterial3D.
     */
    class VMaterial : public godot::Material
    {
        GDCLASS(VMaterial, godot::Material)
        public:
            VMaterial();
            VMaterial(const VCore::Material &_Material);

            godot::String GetName() const 
            {
                return m_Material->Name.c_str();
            }

            void SetName(const godot::String& _Name) 
            {
                m_Material->Name = _Name.utf8().get_data();
            }

            float GetMetallic() const 
            {
                return m_Material->Metallic;
            }

            void SetMetallic(float _Metallic)
            {
                m_Material->Metallic = _Metallic;
                godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "metallic", _Metallic);
            }

            float GetSpecular() const 
            {
                return m_Material->Specular;
            }

            void SetSpecular(float _Specular) 
            {
                m_Material->Specular = _Specular;
                godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "specular", _Specular);
            }

            float GetRoughness() const 
            {
                return m_Material->Roughness;
            }

            void SetRoughness(float _Roughness) 
            {
                m_Material->Roughness = _Roughness;
                godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "roughness", _Roughness);
            }

            float GetIOR() const 
            {
                return m_Material->IOR;
            }

            void SetIOR(float _Ior) 
            {
                m_Material->IOR = _Ior;
            }

            float GetPower() const 
            {
                return m_Material->Power;
            }

            void SetPower(float _Power) 
            {
                m_Material->Power = _Power;
                godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "emission_energy", _Power);
            }

            float GetTransparency() const 
            {
                return m_Material->Transparency;
            }

            void SetTransparency(float _Transparency) 
            {
                m_Material->Transparency = _Transparency;
                godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "alpha", 1.0 - _Transparency);
            }

            void SetAlbedoTexture(const godot::Ref<godot::Texture> &_Texture)
            {
                m_Albedo = _Texture;
                godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "texture_albedo", m_Albedo->get_rid());
            }

            godot::Ref<godot::Texture> GetAlbedoTexture() const
            {
                return m_Albedo;
            }

            void SetEmissionTexture(const godot::Ref<godot::Texture> &_Texture)
            {
                m_Emission = _Texture;
                godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "texture_emission", m_Emission->get_rid());
            }

            godot::Ref<godot::Texture> GetEmissionTexture() const
            {
                return m_Emission;
            }

            /**
             * @return Converts this material to a standard godot material. This is only needed if don't want to deploy this library with your project, and just want to import assets at development time.
            */
            godot::Ref<godot::StandardMaterial3D> ToStandardMaterial3D() const;

            /**
             * @brief For internal use only!
             */
            VCore::Material GetMaterial()
            {
                return m_Material;
            }

            godot::RID _get_shader_rid() const override { return Shader; }
            bool _can_do_next_pass() const override { return true; }
	        bool _can_use_render_priority() const override { return true; }
            godot::Shader::Mode _get_shader_mode() const override { return godot::Shader::Mode::MODE_SPATIAL; }

            ~VMaterial()
            {
                MaterialCount--;
                if(MaterialCount == 0)
                    DeinitShaderCode();
            }

        protected:
            static StaticRID Shader;
            static uint64_t MaterialCount;

            VCore::Material m_Material;
            godot::Ref<godot::Texture> m_Albedo;
            godot::Ref<godot::Texture> m_Emission;

	        static void _bind_methods();

            static void InitShaderCode();
            static void DeinitShaderCode();
            void InitShaderParameters();
    };
} // namespace VCoreGDExt

#endif

// shader_type spatial;
// render_mode blend_mix,depth_draw_opaque,cull_front,diffuse_burley,specular_schlick_ggx;
// uniform vec4 albedo : source_color;
// uniform sampler2D texture_albedo : source_color,filter_linear_mipmap,repeat_enable;
// uniform float roughness : hint_range(0,1);
// uniform float specular;
// uniform float metallic;
// uniform sampler2D texture_emission : source_color, hint_default_black,filter_linear_mipmap,repeat_enable;
// uniform float emission_energy;

// uniform vec3 uv1_scale;
// uniform vec3 uv1_offset;

// void vertex() {
// 	UV=UV*uv1_scale.xy+uv1_offset.xy;
// }

// void fragment() {
// 	vec2 base_uv = UV;
// 	vec4 albedo_tex = texture(texture_albedo,base_uv);
// 	ALBEDO = albedo.rgb * albedo_tex.rgb;
// 	METALLIC = metallic;
// 	vec4 roughness_texture_channel = vec4(1.0,0.0,0.0,0.0);
// 	ROUGHNESS = roughness;
// 	SPECULAR = specular;
// 	vec3 emission_tex = texture(texture_emission,base_uv).rgb;
// 	EMISSION = emission_tex*emission_energy;
// 	ALPHA *= albedo.a * albedo_tex.a;
// }