#include "godot_cpp/variant/color.hpp"
#include <VConcavePolygonShape3D.hpp>
#include <Helper/Convert.hpp>
#include <GodotSurface.hpp>

namespace VCoreGDExt 
{
    void VConcavePolygonShape3D::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("clear"), &VConcavePolygonShape3D::Clear);

        godot::ClassDB::bind_method(godot::D_METHOD("has_voxel", "p_Pos"), &VConcavePolygonShape3D::HasVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("set_voxel", "p_Pos"), &VConcavePolygonShape3D::SetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_voxel", "p_Pos"), &VConcavePolygonShape3D::RemoveVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_voxels", "p_origin", "p_points"), &VConcavePolygonShape3D::RemoveVoxels);
        godot::ClassDB::bind_method(godot::D_METHOD("generate_collider_mesh"), &VConcavePolygonShape3D::GenerateColliderMesh);

        godot::ClassDB::bind_method(godot::D_METHOD("_set_model_config"), &VConcavePolygonShape3D::SetModelConfig);
        godot::ClassDB::bind_method(godot::D_METHOD("_get_model_config"), &VConcavePolygonShape3D::GetModelConfig);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::DICTIONARY, "_model_config", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_INTERNAL | godot::PROPERTY_USAGE_STORAGE), "_set_model_config", "_get_model_config");

        godot::ClassDB::bind_method(godot::D_METHOD("set_model_scale", "p_ModelScale"), &VConcavePolygonShape3D::SetModelScale);
        godot::ClassDB::bind_method(godot::D_METHOD("get_model_scale"), &VConcavePolygonShape3D::GetModelScale);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "model_scale"), "set_model_scale", "get_model_scale");
    }

    void VConcavePolygonShape3D::LoadVModel()
    {
        if(!m_File.is_empty() && !m_FileLoaded)
        {
            auto file = CVoxelStorage::GetInstance()->GetVoxelTree(m_File);
            if(file)
                m_Model = file->Tree->GetModels()[m_Index];
            m_FileLoaded = true;
        }
    }

    bool VConcavePolygonShape3D::HasVoxel(const godot::Vector3i &p_Pos)
    {
        LoadVModel();

        auto voxel = m_Model->Find(Convert::ToVVec3(p_Pos));
        return voxel != m_Model->end();
    }

    void VConcavePolygonShape3D::SetVoxel(const godot::Vector3i &p_Pos)
    {
        LoadVModel();
        
        auto color = Convert::ToColor(Color::from_string("#33ccff", Color()));
        m_Model->Insert({Convert::ToVVec3(p_Pos), VCore::CVoxel(color.AsRGBA(), 0)});
    }

    void VConcavePolygonShape3D::RemoveVoxel(const godot::Vector3i &_Pos)
    {
        LoadVModel();

        auto it = m_Model->Find(Convert::ToVVec3(_Pos));
        if(it != m_Model->end())
            m_Model->Erase(it);
    }

    void VConcavePolygonShape3D::RemoveVoxels(const godot::Vector3i &p_Origin, godot::PackedVector3Array p_Points)
    {
        for (int64_t i = 0; i < p_Points.size(); i++) 
            RemoveVoxel(p_Origin + p_Points[i]);
    }

    void VConcavePolygonShape3D::GenerateColliderMesh()
    {
        LoadVModel();
        auto mesher = VCore::IMesher::Create(m_MesherType, VCore::SurfaceFactory([](void* p_Context) -> VCore::ISurface* {
            return new GodotSurface(*reinterpret_cast<double*>(p_Context));
        }, &m_ModelScale));

        PackedVector3Array faces;
        auto mesh = mesher->GenerateMesh(m_Model);

        for (auto &&surface : mesh->Surfaces) 
        {
            for (uint64_t i = 0; i < surface->GetFaceCount(); i++) 
            {
                auto idx1 = surface->GetIndex(i * 3);
                auto idx2 = surface->GetIndex(i * 3 + 1);
                auto idx3 = surface->GetIndex(i * 3 + 2);

                faces.push_back(Convert::ToGDVec3(surface->GetVertex(idx3).Pos));
                faces.push_back(Convert::ToGDVec3(surface->GetVertex(idx2).Pos));
                faces.push_back(Convert::ToGDVec3(surface->GetVertex(idx1).Pos));
            }
        }

        set_faces(faces);
    }

    void VConcavePolygonShape3D::_validate_property(godot::PropertyInfo &p_property) const
    {
        if(p_property.name.casecmp_to("faces") == 0)
        {
            p_property.usage &= ~godot::PROPERTY_USAGE_EDITOR;
            p_property.usage |= godot::PROPERTY_USAGE_INTERNAL;
        }
    }
}