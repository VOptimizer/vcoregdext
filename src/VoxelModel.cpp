#include <VoxelModel.hpp>

namespace VCoreGDExt
{
    void VoxelModel::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("get_name"), &VoxelModel::GetName);
        godot::ClassDB::bind_method(godot::D_METHOD("set_name"), &VoxelModel::SetName);
        godot::ClassDB::add_property("VoxelModel", godot::PropertyInfo(godot::Variant::STRING, "name"), "set_name", "get_name");

        godot::ClassDB::bind_method(godot::D_METHOD("get_bbox"), &VoxelModel::GetBBox);
        godot::ClassDB::bind_method(godot::D_METHOD("clear"), &VoxelModel::Clear);

        godot::ClassDB::bind_method(godot::D_METHOD("get_voxel"), &VoxelModel::GetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("set_voxel"), &VoxelModel::SetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_voxel"), &VoxelModel::RemoveVoxel);

        godot::ClassDB::bind_method(godot::D_METHOD("add_material"), &VoxelModel::AddMaterial);
        godot::ClassDB::bind_method(godot::D_METHOD("get_material"), &VoxelModel::GetMaterial);

        godot::ClassDB::bind_method(godot::D_METHOD("get_block_count"), &VoxelModel::GetBlockCount);
    }

    int VoxelModel::AddMaterial(const godot::Ref<VoxelMaterial> &_Material)
    {
        m_Model->Materials.push_back(_Material->GetMaterial());
        return m_Model->Materials.size() - 1;
    }

    godot::Ref<VoxelMaterial> VoxelModel::GetMaterial(int _Index)
    {
        if(_Index < 0 || _Index > m_Model->Materials.size())
            return nullptr;

        return new VoxelMaterial(m_Model->Materials[_Index]);
    }

    godot::Dictionary VoxelModel::GetVoxel(const godot::Vector3i &_Pos)
    {
        godot::Dictionary ret;
        auto voxel = m_Model->GetVoxel(Convert::ToVVec3(_Pos));

        // Check if a diffuse texture existing.
        auto it = m_Model->Textures.find(VCore::TextureType::DIFFIUSE);
        if(it != m_Model->Textures.end())
        {
            if(voxel->Color < it->second->GetSize().x)
                ret["color"] = godot::Color::hex(it->second->GetPixel(VCore::Math::Vec2ui(voxel->Color, 0)));
        }

        ret["material"] = GetMaterial(voxel->Material);
        ret["transparent"] = voxel->Transparent;

        return ret;
    }

    void VoxelModel::SetVoxel(const godot::Vector3i &_Pos, int _MaterialIdx, const godot::Color &_Color)
    {
        if(_MaterialIdx > m_Model->Materials.size() || _MaterialIdx < 0)
            _MaterialIdx = 0;

        m_Model->SetVoxel(Convert::ToVVec3(_Pos), _MaterialIdx, _Color.to_rgba32(), m_Model->Materials[_MaterialIdx]->Transparency != 0);
    }

    void VoxelModel::RemoveVoxel(const godot::Vector3i &_Pos)
    {
        m_Model->RemoveVoxel(Convert::ToVVec3(_Pos));
    }
}