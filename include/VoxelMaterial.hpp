#ifndef VOXELMATERIAL_HPP
#define VOXELMATERIAL_HPP

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
    class VoxelMaterial : public godot::RefCounted
    {
        GDCLASS(VoxelMaterial, godot::RefCounted)
        public:
            VoxelMaterial() : m_Material(new VCore::CMaterial()) {}
            VoxelMaterial(const VCore::Material &_Material) : m_Material(_Material) {}

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
            }

            float GetSpecular() const 
            {
                return m_Material->Specular;
            }

            void SetSpecular(float _Specular) 
            {
                m_Material->Specular = _Specular;
            }

            float GetRoughness() const 
            {
                return m_Material->Roughness;
            }

            void SetRoughness(float _Roughness) 
            {
                m_Material->Roughness = _Roughness;
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
            }

            float GetTransparency() const 
            {
                return m_Material->Transparency;
            }

            void SetTransparency(float _Transparency) 
            {
                m_Material->Transparency = _Transparency;
            }

            /**
             * @brief For internal use only!
             */
            VCore::Material GetMaterial()
            {
                return m_Material;
            }

            ~VoxelMaterial() {}

        protected:
            VCore::Material m_Material;
	        static void _bind_methods()
            {
                godot::ClassDB::bind_method(godot::D_METHOD("get_name"), &VoxelMaterial::GetName);
                godot::ClassDB::bind_method(godot::D_METHOD("set_name"), &VoxelMaterial::SetName);
                godot::ClassDB::add_property("VoxelMaterial", godot::PropertyInfo(godot::Variant::STRING, "name"), "set_name", "get_name");

                godot::ClassDB::bind_method(godot::D_METHOD("get_metallic"), &VoxelMaterial::GetMetallic);
                godot::ClassDB::bind_method(godot::D_METHOD("set_metallic"), &VoxelMaterial::SetMetallic);
                godot::ClassDB::add_property("VoxelMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "metallic"), "set_metallic", "get_metallic");

                godot::ClassDB::bind_method(godot::D_METHOD("get_specular"), &VoxelMaterial::GetSpecular);
                godot::ClassDB::bind_method(godot::D_METHOD("set_specular"), &VoxelMaterial::SetSpecular);
                godot::ClassDB::add_property("VoxelMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "specular"), "set_specular", "get_specular");

                godot::ClassDB::bind_method(godot::D_METHOD("get_roughness"), &VoxelMaterial::GetRoughness);
                godot::ClassDB::bind_method(godot::D_METHOD("set_roughness"), &VoxelMaterial::SetRoughness);
                godot::ClassDB::add_property("VoxelMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "roughness"), "set_roughness", "get_roughness");

                godot::ClassDB::bind_method(godot::D_METHOD("get_ior"), &VoxelMaterial::GetIOR);
                godot::ClassDB::bind_method(godot::D_METHOD("set_ior"), &VoxelMaterial::SetIOR);
                godot::ClassDB::add_property("VoxelMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "ior"), "set_ior", "get_ior");

                godot::ClassDB::bind_method(godot::D_METHOD("get_power"), &VoxelMaterial::GetPower);
                godot::ClassDB::bind_method(godot::D_METHOD("set_power"), &VoxelMaterial::SetPower);
                godot::ClassDB::add_property("VoxelMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "power"), "set_power", "get_power");

                godot::ClassDB::bind_method(godot::D_METHOD("get_transparency"), &VoxelMaterial::GetTransparency);
                godot::ClassDB::bind_method(godot::D_METHOD("set_transparency"), &VoxelMaterial::SetTransparency);
                godot::ClassDB::add_property("VoxelMaterial", godot::PropertyInfo(godot::Variant::FLOAT, "transparency"), "set_transparency", "get_transparency");
            }
    };
} // namespace VCoreGDExt

#endif