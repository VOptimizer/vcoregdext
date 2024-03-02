#ifndef VMaterial_HPP
#define VMaterial_HPP

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/variant/string.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
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
                godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "alpha", _Transparency);
            }

            /**
             * @brief For internal use only!
             */
            VCore::Material GetMaterial()
            {
                return m_Material;
            }

            static void InitShaderCode();

            godot::RID _get_shader_rid() const override { return Shader; }
            bool _can_do_next_pass() const override { return true; }
	        bool _can_use_render_priority() const override { return true; }

            ~VMaterial() {}

        protected:
            static godot::RID Shader;

            VCore::Material m_Material;
	        static void _bind_methods();

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