#ifndef VMaterial_HPP
#define VMaterial_HPP

#include <cstdint>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/texture.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <VCore/VCore.hpp>

#include <Helper/StaticType.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/variant/rid.hpp>

namespace VCoreGDExt
{
    using StaticRID = TStaticType<godot::RID>;

    /**
     * @brief Represents a voxel material. This class can be use like a StandardMaterial3D, however if you plan to use this library only at development time you need to convert this material into an actual StandardMaterial3D.
     */
    class VMaterial : public godot::ShaderMaterial
    {
        GDCLASS(VMaterial, godot::Material)
        public:
            VMaterial();
            VMaterial(const VCore::Material &p_Material, uint8_t p_MaterialIdx);

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
                // godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "metallic", _Metallic);
                set_shader_parameter("metallic", _Metallic);
            }

            float GetSpecular() const 
            {
                return m_Material->Specular;
            }

            void SetSpecular(float _Specular) 
            {
                m_Material->Specular = _Specular;
                // godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "specular", _Specular);
                set_shader_parameter("specular", _Specular);
            }

            float GetRoughness() const 
            {
                return m_Material->Roughness;
            }

            void SetRoughness(float _Roughness) 
            {
                m_Material->Roughness = _Roughness;
                // godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "roughness", _Roughness);
                set_shader_parameter("roughness", _Roughness);
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
                // godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "emission_energy", _Power);
                set_shader_parameter("emission_energy", _Power);
            }

            float GetTransparency() const 
            {
                return m_Material->Transparency;
            }

            void SetTransparency(float _Transparency) 
            {
                m_Material->Transparency = _Transparency;
                // godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "alpha", 1.0 - _Transparency);
                set_shader_parameter("alpha", 1.0 - _Transparency);
            }

            void SetMaterialIdx(uint8_t p_MaterialIdx) 
            {
                m_MaterialIdx = p_MaterialIdx; 
                m_Material = VCore::MaterialManager::GetMaterial(m_MaterialIdx);
                InitShaderParameters();
            }
            uint8_t GetMaterialIdx() const { return m_MaterialIdx; }

            /**
             * @return Converts this material to a standard godot material. This is only needed if don't want to deploy this library with your project, and just want to import assets at development time.
            */
            // godot::Ref<godot::StandardMaterial3D> ToStandardMaterial3D() const;

            /**
             * @brief For internal use only!
             */
            VCore::Material GetMaterial()
            {
                return m_Material;
            }

            // godot::RID get_rid() const { godot::print_line(m_MaterialRid.is_valid()); return m_MaterialRid; }
            // godot::RID _get_shader_rid() const override { return Shader; }
            // bool _can_do_next_pass() const override { return false; }
	        // bool _can_use_render_priority() const override { return false; }
            // godot::Shader::Mode _get_shader_mode() const override { return godot::Shader::Mode::MODE_SPATIAL; }

            void _set_shader(const godot::Ref<godot::Shader> &p_shader) {}

            ~VMaterial()
            {
                MaterialCount--;
                if(MaterialCount == 0)
                    DeinitShaderCode();
            }

            static void DeinitShaderCode();
        protected:
            static godot::Ref<godot::Shader> ShaderResource;
            static StaticRID Shader;
            static uint64_t MaterialCount;

            // godot::RID m_MaterialRid;
            VCore::Material m_Material;
            uint8_t m_MaterialIdx{};

	        static void _bind_methods();
            void _get_property_list(godot::List<godot::PropertyInfo> *p_list) const { p_list->clear(); }
            void _validate_property(godot::PropertyInfo &p_property) const;

            static void InitShaderCode();
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