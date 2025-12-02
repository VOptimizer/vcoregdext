#include "VoxelStorage.hpp"
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <GodotSurface.hpp>
#include <Helper/Convert.hpp>
#include <VMaterial.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/memory.hpp>
#include <VModel.hpp>
#include <Helper/Resource.hpp>

// #define MAKE_RESOURCE_TYPE_HINT(m_type) vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, m_type)

// template <typename... VarArgs>
// godot::String vformat(const godot::String &p_text, const VarArgs... p_args) {
// 	godot::Variant args[sizeof...(p_args) + 1] = { p_args..., godot::Variant() }; // +1 makes sure zero sized arrays are also supported.
// 	godot::Array args_array;
// 	args_array.resize(sizeof...(p_args));
// 	for (uint32_t i = 0; i < sizeof...(p_args); i++) {
// 		args_array[i] = args[i];
// 	}

// 	return p_text % args_array;
// }

namespace VCoreGDExt
{
    void VModel::_bind_methods()
    {
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "SIMPLE", (int)VCore::MesherTypes::SIMPLE);
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "GREEDY", (int)VCore::MesherTypes::GREEDY);
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "GREEDY_CHUNKED", (int)VCore::MesherTypes::GREEDY_CHUNKED);
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "GREEDY_TEXTURED", (int)VCore::MesherTypes::GREEDY_TEXTURED);
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "SMOOTH", (int)VCore::MesherTypes::SMOOTH);

        godot::ClassDB::bind_method(godot::D_METHOD("clear"), &VModel::Clear);
        godot::ClassDB::bind_method(godot::D_METHOD("set_mesh"), &VModel::_set_mesh);

        godot::ClassDB::bind_method(godot::D_METHOD("has_voxel"), &VModel::HasVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("get_voxel"), &VModel::GetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("set_voxel"), &VModel::SetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_voxel"), &VModel::RemoveVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("generate_mesh"), &VModel::GenerateMesh);

        godot::ClassDB::bind_method(godot::D_METHOD("set_mesher_type"), &VModel::SetMesherType);
        godot::ClassDB::bind_method(godot::D_METHOD("get_mesher_type"), &VModel::GetMesherType);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::INT, "mesher_type", godot::PROPERTY_HINT_ENUM, "SIMPLE,GREEDY,GREEDY_CHUNKED,GREEDY_TEXTURED,SMOOTH"), "set_mesher_type", "get_mesher_type");

        godot::ClassDB::bind_method(godot::D_METHOD("_set_model_config"), &VModel::SetModelConfig);
        godot::ClassDB::bind_method(godot::D_METHOD("_get_model_config"), &VModel::GetModelConfig);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::DICTIONARY, "_model_config", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_INTERNAL | godot::PROPERTY_USAGE_STORAGE), "_set_model_config", "_get_model_config");

        // godot::ClassDB::bind_method(godot::D_METHOD("get_block_count"), &VModel::GetBlockCount);
    }

    godot::Dictionary VModel::GetVoxel(const godot::Vector3i &p_Pos)
    {
        LoadVModel();

        godot::Dictionary ret;
        auto voxel = m_Model->Find(Convert::ToVVec3(p_Pos));
        if(voxel == m_Model->end())
            return ret;

        godot::Ref<VMaterial> material = memnew(VMaterial(VCore::MaterialManager::GetMaterial(voxel->second.GetMaterial()), voxel->second.GetMaterial()));
        ret["material"] = material;
        ret["color"] = Convert::ToGDColor(VCore::CColor(voxel->second.GetColor()));
        ret["transparent"] = material.is_valid() ? (material->GetTransparency() != 0.0) : false;

        return ret;
    }

    bool VModel::HasVoxel(const godot::Vector3i &p_Pos)
    {
        LoadVModel();

        auto voxel = m_Model->Find(Convert::ToVVec3(p_Pos));
        return voxel != m_Model->end();
    }

    void VModel::SetVoxel(const godot::Vector3i &p_Pos, int p_MaterialIdx, const godot::Color &p_Color)
    {
        LoadVModel();

        auto mat = VCore::MaterialManager::GetMaterial(p_MaterialIdx);
        if(!mat)
            p_MaterialIdx = 0;
        
        auto color = Convert::ToColor(p_Color);

        m_Model->Insert({Convert::ToVVec3(p_Pos), VCore::CVoxel(color.AsABGR(), p_MaterialIdx)});
    }

    void VModel::RemoveVoxel(const godot::Vector3i &_Pos)
    {
        LoadVModel();

        auto it = m_Model->Find(Convert::ToVVec3(_Pos));
        if(it != m_Model->end())
            m_Model->Erase(it);
    }

    void VModel::GenerateMesh()
    {
        auto mesher = VCore::IMesher::Create<GodotSurface>(m_MesherType);
        auto mesh = mesher->GenerateMesh(m_Model);

        if (!m_Mesh.is_valid())
            m_Mesh.instantiate();
        else
            m_Mesh->clear_surfaces();

        if(mesh)
        {
            for (auto &&surface : mesh->Surfaces) 
            {
                godot::Array meshArray;
                meshArray.resize(godot::ArrayMesh::ARRAY_MAX);

                auto gdSurface = dynamic_cast<GodotSurface*>(surface);
                meshArray[godot::ArrayMesh::ARRAY_VERTEX] = gdSurface->Positions;
                meshArray[godot::ArrayMesh::ARRAY_NORMAL] = gdSurface->Normals;
                meshArray[godot::ArrayMesh::ARRAY_COLOR] = gdSurface->Colors;
                meshArray[godot::ArrayMesh::ARRAY_INDEX] = gdSurface->Indices;

                m_Mesh->add_surface_from_arrays(godot::ArrayMesh::PRIMITIVE_TRIANGLES, meshArray);
                
                auto mat = VCore::MaterialManager::GetMaterial(surface->MaterialHandle);
                auto matIdx = surface->MaterialHandle;
                if(!mat)
                {
                    mat = VCore::MaterialManager::GetMaterial(0);
                    matIdx = 0;
                }

                m_Mesh->surface_set_material(m_Mesh->get_surface_count() - 1, memnew(VMaterial(mat, matIdx)));
            }
        }

        set_mesh(m_Mesh);
    }

    void VModel::LoadVModel()
    {
        if(!m_File.is_empty() && !m_Model)
        {
            auto file = CVoxelStorage::GetInstance()->GetVoxelTree(m_File);
            if(file)
                m_Model = file->Tree->GetModels()[m_Index];
        }
    }

    void VModel::_notification(int p_what)
    {
        switch (p_what) 
        {
            case NOTIFICATION_TRANSLATION_CHANGED: 
            {
                if (m_Mesh.is_valid())
                    m_Mesh->notification(NOTIFICATION_TRANSLATION_CHANGED);
            } break;
        }
    }

    void VModel::_validate_property(godot::PropertyInfo &p_property) const
    {
        if(p_property.name.casecmp_to("mesh") == 0)
        {
            p_property.usage &= ~godot::PROPERTY_USAGE_EDITOR;
            p_property.usage |= godot::PROPERTY_USAGE_INTERNAL;
        }
    }
}