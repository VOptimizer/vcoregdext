#include <VMaterial.hpp>

namespace VCoreGDExt
{
    StaticRID VMaterial::Shader;

    void VMaterial::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("to_standard_material_3d"), &VMaterial::ToStandardMaterial3D);

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
    
        godot::ClassDB::bind_method(godot::D_METHOD("get_albedo_texture"), &VMaterial::GetAlbedoTexture);
        godot::ClassDB::bind_method(godot::D_METHOD("set_albedo_texture"), &VMaterial::SetAlbedoTexture);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::OBJECT, "albedo_texture", godot::PROPERTY_HINT_RESOURCE_TYPE), "set_albedo_texture", "get_albedo_texture");
    
        godot::ClassDB::bind_method(godot::D_METHOD("get_emission_texture"), &VMaterial::GetEmissionTexture);
        godot::ClassDB::bind_method(godot::D_METHOD("set_emission_texture"), &VMaterial::SetEmissionTexture);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::OBJECT, "emission_texture", godot::PROPERTY_HINT_RESOURCE_TYPE), "set_emission_texture", "get_emission_texture");
    }

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
        godot::RenderingServer::get_singleton()->shader_set_code(Shader, 
        R"(
            shader_type spatial;
            render_mode blend_mix, depth_prepass_alpha, depth_draw_opaque, cull_front, diffuse_burley, specular_schlick_ggx;
            uniform float alpha : hint_range(0, 1);
            uniform sampler2D texture_albedo : source_color, filter_nearest, repeat_enable;
            uniform float roughness : hint_range(0, 1);
            uniform float specular;
            uniform float metallic;
            uniform float emission_energy;
            uniform sampler2D texture_emission : source_color, hint_default_black, filter_nearest, repeat_enable;

            void vertex() 
            {
                UV = UV * vec2(1, -1) + vec2(0, 1);
                NORMAL *= -1.0;
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

    godot::Ref<godot::StandardMaterial3D> VMaterial::ToStandardMaterial3D() const
    {
        godot::Ref<godot::StandardMaterial3D> result = memnew(godot::StandardMaterial3D());

        if(!m_Albedo.is_null())
            result->set_texture(godot::StandardMaterial3D::TextureParam::TEXTURE_ALBEDO, m_Albedo);

        result->set_metallic(m_Material->Metallic);
        result->set_specular(m_Material->Specular);
        result->set_roughness(m_Material->Roughness);
        result->set_cull_mode(godot::StandardMaterial3D::CULL_FRONT);
        result->set_uv1_scale(godot::Vector3(1, -1, 1));
        result->set_uv1_offset(godot::Vector3(0, 1, 0));
        result->set_texture_filter(godot::StandardMaterial3D::TextureFilter::TEXTURE_FILTER_NEAREST);
        result->set_cull_mode(godot::StandardMaterial3D::CullMode::CULL_FRONT);

        if(m_Material->Transparency != 0.0)
        {
            result->set_transparency(godot::StandardMaterial3D::Transparency::TRANSPARENCY_ALPHA_DEPTH_PRE_PASS);
            result->set_albedo(godot::Color(1, 1, 1, 1 - m_Material->Transparency));
        }

        if(m_Material->Power != 0.0)
        {
            result->set_feature(godot::StandardMaterial3D::Feature::FEATURE_EMISSION, true);
            result->set_emission_energy_multiplier(m_Material->Power);

            if(!m_Emission.is_null())
                result->set_texture(godot::StandardMaterial3D::TextureParam::TEXTURE_EMISSION, m_Emission);
        }

        return result;
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