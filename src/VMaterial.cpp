#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/material.hpp"
#include <VMaterial.hpp>
#include <Helper/Resource.hpp>

namespace VCoreGDExt
{
    StaticRID VMaterial::Shader;
    godot::Ref<godot::Shader> VMaterial::ShaderResource;
    uint64_t VMaterial::MaterialCount = 0;

    void VMaterial::_bind_methods()
    {
        // godot::ClassDB::bind_method(godot::D_METHOD("to_standard_material_3d"), &VMaterial::ToStandardMaterial3D);

        godot::ClassDB::bind_method(godot::D_METHOD("get_name"), &VMaterial::GetName);
        godot::ClassDB::bind_method(godot::D_METHOD("set_name"), &VMaterial::SetName);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::STRING, "name"), "set_name", "get_name");

        godot::ClassDB::bind_method(godot::D_METHOD("get_metallic"), &VMaterial::GetMetallic);
        godot::ClassDB::bind_method(godot::D_METHOD("set_metallic"), &VMaterial::SetMetallic);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "metallic"), "set_metallic", "get_metallic");

        godot::ClassDB::bind_method(godot::D_METHOD("get_specular"), &VMaterial::GetSpecular);
        godot::ClassDB::bind_method(godot::D_METHOD("set_specular"), &VMaterial::SetSpecular);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "specular"), "set_specular", "get_specular");

        godot::ClassDB::bind_method(godot::D_METHOD("get_roughness"), &VMaterial::GetRoughness);
        godot::ClassDB::bind_method(godot::D_METHOD("set_roughness"), &VMaterial::SetRoughness);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "roughness"), "set_roughness", "get_roughness");

        godot::ClassDB::bind_method(godot::D_METHOD("get_ior"), &VMaterial::GetIOR);
        godot::ClassDB::bind_method(godot::D_METHOD("set_ior"), &VMaterial::SetIOR);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "ior"), "set_ior", "get_ior");

        godot::ClassDB::bind_method(godot::D_METHOD("get_power"), &VMaterial::GetPower);
        godot::ClassDB::bind_method(godot::D_METHOD("set_power"), &VMaterial::SetPower);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "power"), "set_power", "get_power");

        godot::ClassDB::bind_method(godot::D_METHOD("get_transparency"), &VMaterial::GetTransparency);
        godot::ClassDB::bind_method(godot::D_METHOD("set_transparency"), &VMaterial::SetTransparency);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "transparency"), "set_transparency", "get_transparency");

        godot::ClassDB::bind_method(godot::D_METHOD("set_shader"), &VMaterial::_set_shader);

        godot::ClassDB::bind_method(godot::D_METHOD("_get_material_idx"), &VMaterial::GetMaterialIdx);
        godot::ClassDB::bind_method(godot::D_METHOD("_set_material_idx"), &VMaterial::SetMaterialIdx);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::INT, "_material_idx", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_INTERNAL), "_set_material_idx", "_get_material_idx");
    }

    VMaterial::VMaterial()
    { 
        if(!Shader)
            InitShaderCode();

        m_Material = VCore::MaterialManager::GetMaterial(0);

        // m_MaterialRid = godot::RenderingServer::get_singleton()->material_create();
        // godot::RenderingServer::get_singleton()->material_set_shader(get_rid(), Shader);
        set_shader(ShaderResource);
        InitShaderParameters();
        MaterialCount++;
    }

    VMaterial::VMaterial(const VCore::Material &p_Material, uint8_t p_MaterialIdx) : m_Material(p_Material), m_MaterialIdx(p_MaterialIdx)
    { 
        if(!Shader)
            InitShaderCode();

        // m_MaterialRid = godot::RenderingServer::get_singleton()->material_create();
        // godot::RenderingServer::get_singleton()->material_set_shader(get_rid(), Shader);
        set_shader(ShaderResource);
        InitShaderParameters(); 
        MaterialCount++;
    }

    void VMaterial::DeinitShaderCode()
    {
        ShaderResource.unref();
        // godot::RenderingServer::get_singleton()->free_rid(Shader);
        // Shader.Clear();
    }

    void VMaterial::_validate_property(godot::PropertyInfo &p_property) const
    {
        if(p_property.name.casecmp_to("shader") == 0)
        {
            p_property.usage &= ~godot::PROPERTY_USAGE_EDITOR;
            p_property.usage |= godot::PROPERTY_USAGE_INTERNAL;
        }
    }

    void VMaterial::InitShaderCode()
    {
        ShaderResource.instantiate();
        ShaderResource->set_code(
        R"(
            shader_type spatial;
            render_mode blend_mix, depth_prepass_alpha, depth_draw_opaque, cull_front, diffuse_burley, specular_schlick_ggx;
            uniform float alpha : hint_range(0, 1);
            uniform float roughness : hint_range(0, 1);
            uniform float specular;
            uniform float metallic;
            uniform float emission_energy;

            void vertex() 
            {
                NORMAL *= -1.0;
            }

            void fragment() 
            {
                ALBEDO = COLOR.rgb;
                METALLIC = metallic;
                ROUGHNESS = roughness;
                SPECULAR = specular;
                EMISSION = COLOR.rgb * emission_energy;
                ALPHA = round(alpha * 100.0) / 100.0;
            }
        )"
        );

        // Shader = godot::RenderingServer::get_singleton()->shader_create();          
        // godot::RenderingServer::get_singleton()->shader_set_code(Shader, 
        // R"(
        //     shader_type spatial;
        //     render_mode blend_mix, depth_prepass_alpha, depth_draw_opaque, cull_front, diffuse_burley, specular_schlick_ggx;
        //     uniform float alpha : hint_range(0, 1);
        //     uniform float roughness : hint_range(0, 1);
        //     uniform float specular;
        //     uniform float metallic;
        //     uniform float emission_energy;

        //     void vertex() 
        //     {
        //         NORMAL *= -1.0;
        //     }

        //     void fragment() 
        //     {
        //         ALBEDO = COLOR.rgb;
        //         METALLIC = metallic;
        //         ROUGHNESS = roughness;
        //         SPECULAR = specular;
        //         EMISSION = COLOR.rgb * emission_energy;
        //         ALPHA = round(alpha * 100.0) / 100.0;
        //     }
        // )");
    }

    void VMaterial::InitShaderParameters()
    {
        set_shader_parameter("metallic", m_Material->Metallic);
        set_shader_parameter("specular", m_Material->Specular);
        set_shader_parameter("roughness", m_Material->Roughness);
        set_shader_parameter("emission_energy", m_Material->Power);
        set_shader_parameter("alpha", 1.0 - m_Material->Transparency);

        // godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_, "metallic", m_Material->Metallic);
        // godot::RenderingServer::get_singleton()->material_set_param(m_MaterialRid, "specular", m_Material->Specular);
        // godot::RenderingServer::get_singleton()->material_set_param(m_MaterialRid, "roughness", m_Material->Roughness);
        // godot::RenderingServer::get_singleton()->material_set_param(m_MaterialRid, "emission_energy", m_Material->Power);
        // godot::RenderingServer::get_singleton()->material_set_param(m_MaterialRid, "alpha", 1.0 - m_Material->Transparency);
    }
} // namespace VCoreGDExt