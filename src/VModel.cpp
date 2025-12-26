#include <VCore/VCore.hpp>
#include <VModel.hpp>
#include "VCore/Meshing/Mesh/Surface.hpp"
#include "VoxelStorage.hpp"
#include "godot_cpp/classes/array_mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/variant/plane.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include <cstdint>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <GodotSurface.hpp>
#include <Helper/Convert.hpp>
#include <VMaterial.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/memory.hpp>
#include <Helper/Resource.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/camera3d.hpp>

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

        godot::ClassDB::bind_integer_constant(get_class_static(), "ModelTypes", "STATIC", (int)ModelTypes::STATIC);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ModelTypes", "DYNAMIC", (int)ModelTypes::DYNAMIC);

        godot::ClassDB::bind_method(godot::D_METHOD("clear"), &VModel::Clear);
        godot::ClassDB::bind_method(godot::D_METHOD("set_mesh", "p_Mesh"), &VModel::_set_mesh);
        godot::ClassDB::bind_method(godot::D_METHOD("get_aabb"), &VModel::_get_aabb);

        godot::ClassDB::bind_method(godot::D_METHOD("has_voxel", "p_Pos"), &VModel::HasVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("get_voxel", "p_Pos"), &VModel::GetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("set_voxel", "p_Pos", "p_MaterialIdx", "p_Color"), &VModel::SetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_voxel", "p_Pos"), &VModel::RemoveVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_voxels", "p_origin", "p_points"), &VModel::RemoveVoxels);
        godot::ClassDB::bind_method(godot::D_METHOD("generate_mesh"), &VModel::GenerateMesh);

        godot::ClassDB::bind_method(godot::D_METHOD("set_mesher_type", "p_Type"), &VModel::SetMesherType);
        godot::ClassDB::bind_method(godot::D_METHOD("get_mesher_type"), &VModel::GetMesherType);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::INT, "mesher_type", godot::PROPERTY_HINT_ENUM, "SIMPLE,GREEDY,GREEDY_CHUNKED,GREEDY_TEXTURED,SMOOTH"), "set_mesher_type", "get_mesher_type");

        godot::ClassDB::bind_method(godot::D_METHOD("set_model_type", "p_Type"), &VModel::SetModelType);
        godot::ClassDB::bind_method(godot::D_METHOD("get_model_type"), &VModel::GetModelType);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::INT, "model_type", godot::PROPERTY_HINT_ENUM, "STATIC,DYNAMIC"), "set_model_type", "get_model_type");

        godot::ClassDB::bind_method(godot::D_METHOD("set_frustum_culling_enabled", "p_FrustumCullingEnabled"), &VModel::SetFrustumCullingEnabled);
        godot::ClassDB::bind_method(godot::D_METHOD("is_frustum_culling_enabled"), &VModel::IsFrustumCullingEnabled);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::BOOL, "frustum_culling_enabled"), "set_frustum_culling_enabled", "is_frustum_culling_enabled");

        godot::ClassDB::bind_method(godot::D_METHOD("_set_model_config"), &VModel::SetModelConfig);
        godot::ClassDB::bind_method(godot::D_METHOD("_get_model_config"), &VModel::GetModelConfig);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::DICTIONARY, "_model_config", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_INTERNAL | godot::PROPERTY_USAGE_STORAGE), "_set_model_config", "_get_model_config");

        godot::ClassDB::bind_method(godot::D_METHOD("set_model_scale", "p_ModelScale"), &VModel::SetModelScale);
        godot::ClassDB::bind_method(godot::D_METHOD("get_model_scale"), &VModel::GetModelScale);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "model_scale"), "set_model_scale", "get_model_scale");

        godot::ClassDB::bind_method(godot::D_METHOD("get_mesh_aabb"), &VModel::GetMeshAABB);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::AABB, "mesh_aabb", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_EDITOR), "", "get_mesh_aabb");
        


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
        ret["color"] = Convert::ToGDColor(VCore::CColor::CreateFromRGBA(voxel->second.GetColor()));
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

        m_Model->Insert({Convert::ToVVec3(p_Pos), VCore::CVoxel(color.AsRGBA(), p_MaterialIdx)});
    }

    void VModel::RemoveVoxel(const godot::Vector3i &_Pos)
    {
        LoadVModel();

        auto it = m_Model->Find(Convert::ToVVec3i(_Pos));
        if(it != m_Model->end())
            m_Model->Erase(it);
    }

    void VModel::RemoveVoxels(const godot::Vector3i &p_Origin, godot::PackedVector3Array p_Points)
    {
        for (int64_t i = 0; i < p_Points.size(); i++) 
            RemoveVoxel(p_Origin + p_Points[i]);
    }

    void VModel::GenerateMesh()
    {
        LoadVModel();
        auto mesher = VCore::IMesher::Create(m_MesherType, VCore::SurfaceFactory([](void* p_Context) -> VCore::ISurface* {
            return new GodotSurface(*reinterpret_cast<double*>(p_Context));
        }, &m_ModelScale));

        if(m_ModelType == ModelTypes::DYNAMIC)
        {
            if(m_Mesh.is_valid())
            {
                m_Mesh.unref();
                set_mesh(nullptr);
            }

            godot::Camera3D *cam = nullptr;
            if(get_viewport())
                cam = get_viewport()->get_camera_3d();

            if(cam && m_FrustumCullingEnabled)
            {
                godot::TypedArray<godot::Plane> planes = cam->get_frustum();
                VCore::CFrustum frustum(
                    VCore::CPlane(((godot::Plane)planes[0]).d, Convert::ToVVec3(((godot::Plane)planes[0]).normal)),
                    VCore::CPlane(((godot::Plane)planes[1]).d, Convert::ToVVec3(((godot::Plane)planes[1]).normal)),
                    VCore::CPlane(((godot::Plane)planes[2]).d, Convert::ToVVec3(((godot::Plane)planes[2]).normal)),
                    VCore::CPlane(((godot::Plane)planes[4]).d, Convert::ToVVec3(((godot::Plane)planes[4]).normal)),
                    VCore::CPlane(((godot::Plane)planes[3]).d, Convert::ToVVec3(((godot::Plane)planes[3]).normal)),
                    VCore::CPlane(((godot::Plane)planes[5]).d, Convert::ToVVec3(((godot::Plane)planes[5]).normal))
                );

                mesher->SetFrustum(&frustum);
            }
            else
                mesher->SetFrustum(nullptr);

            if(!cam && m_FrustumCullingEnabled)
                return;

            auto chunks = mesher->GenerateChunks(m_Model, true);
            for (auto &chunk: chunks) 
            {
                auto name = godot::String::num_uint64(chunk.UniqueId);
                godot::MeshInstance3D *gdchunk = nullptr;
                if(has_node(name))
                    gdchunk = get_node<godot::MeshInstance3D>(name);
                else
                {
                    gdchunk = memnew(godot::MeshInstance3D());
                    gdchunk->set_name(name);
                    add_child(gdchunk);

                    gdchunk->set_mesh(memnew(godot::ArrayMesh()));
                    gdchunk->set_meta("__vcore_chunk", true);
                }

                gdchunk->set_scale(godot::Vector3(-1, 1, 1));
                godot::Ref<godot::ArrayMesh> mesh = gdchunk->get_mesh();
                FillArrayMesh(mesh, chunk.MeshData);
            }
        }
        else
        {
            for (int64_t i = 0; i < get_child_count(); i++) 
            {
                auto c = get_child(i);
                auto mesh = Object::cast_to<godot::MeshInstance3D>(c);
                if(mesh && mesh->has_meta("__vcore_chunk"))
                {
                    auto value = c->get_meta("__vcore_chunk");
                    if(value.get_type() == godot::Variant::BOOL && (bool)value)
                        c->queue_free();
                }
            }

            mesher->SetFrustum(nullptr);

            auto mesh = mesher->GenerateMesh(m_Model);
            if (!m_Mesh.is_valid())
                m_Mesh.instantiate();
            else
                m_Mesh->clear_surfaces();

            FillArrayMesh(m_Mesh, mesh);
            set_mesh(m_Mesh);
        }
    }

    void VModel::LoadVModel()
    {
        if(!m_File.is_empty() && !m_FileLoaded)
        {
            auto file = CVoxelStorage::GetInstance()->GetVoxelTree(m_File);
            if(file)
                m_Model = file->Tree->GetModels()[m_Index];
            m_FileLoaded = true;
        }
    }

    void VModel::FillArrayMesh(godot::Ref<godot::ArrayMesh> &p_Mesh, const VCore::Mesh &p_MeshData)
    {
        if(p_MeshData)
        {
            for (auto &&surface : p_MeshData->Surfaces) 
            {
                godot::Array meshArray;
                meshArray.resize(godot::ArrayMesh::ARRAY_MAX);

                auto gdSurface = dynamic_cast<GodotSurface*>(surface);
                meshArray[godot::ArrayMesh::ARRAY_VERTEX] = gdSurface->Positions;
                meshArray[godot::ArrayMesh::ARRAY_NORMAL] = gdSurface->Normals;
                meshArray[godot::ArrayMesh::ARRAY_COLOR] = gdSurface->Colors;
                meshArray[godot::ArrayMesh::ARRAY_INDEX] = gdSurface->Indices;

                p_Mesh->add_surface_from_arrays(godot::ArrayMesh::PRIMITIVE_TRIANGLES, meshArray);
                
                auto mat = VCore::MaterialManager::GetMaterial(surface->MaterialHandle);
                auto matIdx = surface->MaterialHandle;
                if(!mat)
                {
                    mat = VCore::MaterialManager::GetMaterial(0);
                    matIdx = 0;
                }

                p_Mesh->surface_set_material(p_Mesh->get_surface_count() - 1, memnew(VMaterial(mat, matIdx)));
            }
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