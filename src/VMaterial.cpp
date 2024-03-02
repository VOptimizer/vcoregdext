#include <VMaterial.hpp>

namespace VCoreGDExt
{
    StaticRID VMaterial::Shader;

    VMaterial::VMaterial() : m_Material(new VCore::CMaterial()) 
    { 
        if(!Shader)
            InitShaderCode();

        godot::RenderingServer::get_singleton()->material_set_shader(godot::Material::get_rid(), Shader);
        InitShaderParameters();
    }

    VMaterial::VMaterial(const VCore::Material &_Material) : m_Material(_Material) 
    { 
        if(!Shader)
            InitShaderCode();

        godot::RenderingServer::get_singleton()->material_set_shader(godot::Material::get_rid(), Shader);
        InitShaderParameters(); 
    }

    void VMaterial::InitShaderCode()
    {
        Shader = godot::RenderingServer::get_singleton()->shader_create();
// 
//                 
        godot::RenderingServer::get_singleton()->shader_set_code(Shader, 
        R"(
            shader_type spatial;
            render_mode blend_mix, depth_draw_always, cull_front, diffuse_burley, specular_schlick_ggx;
            uniform float alpha : hint_range(0, 1);
            uniform sampler2D texture_albedo : source_color, filter_linear_mipmap, repeat_enable;
            uniform float roughness : hint_range(0, 1);
            uniform float specular;
            uniform float metallic;
            uniform float emission_energy;
            uniform sampler2D texture_emission : source_color, hint_default_black, filter_linear_mipmap, repeat_enable;

            void vertex() 
            {
                UV = UV * vec2(1, -1) + vec2(0, 1);
            }

            void fragment() 
            {
                vec4 albedo_tex = texture(texture_albedo, UV);
                ALBEDO = albedo_tex.rgb;
                METALLIC = metallic;
                ROUGHNESS = roughness;
                SPECULAR = specular;
                vec3 emission_tex = texture(texture_emission, UV).rgb;
                EMISSION = emission_tex * emission_energy;
                ALPHA = round(alpha * 100.0) / 100.0;
            }
        )");
    }

    void VMaterial::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("get_name"), &VMaterial::GetName);
        godot::ClassDB::bind_method(godot::D_METHOD("set_name"), &VMaterial::SetName);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::STRING, "name"), "set_name", "get_name");

        godot::ClassDB::bind_method(godot::D_METHOD("get_metallic"), &VMaterial::GetMetallic);
        godot::ClassDB::bind_method(godot::D_METHOD("set_metallic"), &VMaterial::SetMetallic);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "metallic"), "set_metallic", "get_metallic");

        godot::ClassDB::bind_method(godot::D_METHOD("get_specular"), &VMaterial::GetSpecular);
        godot::ClassDB::bind_method(godot::D_METHOD("set_specular"), &VMaterial::SetSpecular);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "specular"), "set_specular", "get_specular");

        godot::ClassDB::bind_method(godot::D_METHOD("get_roughness"), &VMaterial::GetRoughness);
        godot::ClassDB::bind_method(godot::D_METHOD("set_roughness"), &VMaterial::SetRoughness);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "roughness"), "set_roughness", "get_roughness");

        godot::ClassDB::bind_method(godot::D_METHOD("get_ior"), &VMaterial::GetIOR);
        godot::ClassDB::bind_method(godot::D_METHOD("set_ior"), &VMaterial::SetIOR);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "ior"), "set_ior", "get_ior");

        godot::ClassDB::bind_method(godot::D_METHOD("get_power"), &VMaterial::GetPower);
        godot::ClassDB::bind_method(godot::D_METHOD("set_power"), &VMaterial::SetPower);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "power"), "set_power", "get_power");

        godot::ClassDB::bind_method(godot::D_METHOD("get_transparency"), &VMaterial::GetTransparency);
        godot::ClassDB::bind_method(godot::D_METHOD("set_transparency"), &VMaterial::SetTransparency);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "transparency"), "set_transparency", "get_transparency");
    
        godot::ClassDB::bind_method(godot::D_METHOD("get_albedo_texture"), &VMaterial::GetAlbedoTexture);
        godot::ClassDB::bind_method(godot::D_METHOD("set_albedo_texture"), &VMaterial::SetAlbedoTexture);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::OBJECT, "albedo_texture", godot::PROPERTY_HINT_RESOURCE_TYPE), "set_albedo_texture", "get_albedo_texture");
    
        godot::ClassDB::bind_method(godot::D_METHOD("get_emission_texture"), &VMaterial::GetEmissionTexture);
        godot::ClassDB::bind_method(godot::D_METHOD("set_emission_texture"), &VMaterial::SetEmissionTexture);
        godot::ClassDB::add_property("VMaterial", godot::PropertyInfo(godot::Variant::OBJECT, "emission_texture", godot::PROPERTY_HINT_RESOURCE_TYPE), "set_emission_texture", "get_emission_texture");
    }

    void VMaterial::InitShaderParameters()
    {
        godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "metallic", m_Material->Metallic);
        godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "specular", m_Material->Specular);
        godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "roughness", m_Material->Roughness);
        godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "emission_energy", m_Material->Power);
        godot::RenderingServer::get_singleton()->material_set_param(godot::Material::get_rid(), "alpha", 1.0 - m_Material->Transparency);
    }
} // namespace VCoreGDExt