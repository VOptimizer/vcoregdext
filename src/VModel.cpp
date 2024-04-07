#include <VModel.hpp>

namespace VCoreGDExt
{
    void VModel::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("get_name"), &VModel::GetName);
        godot::ClassDB::bind_method(godot::D_METHOD("set_name"), &VModel::SetName);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::STRING, "name"), "set_name", "get_name");

        godot::ClassDB::bind_method(godot::D_METHOD("get_bbox"), &VModel::GetBBox);
        godot::ClassDB::bind_method(godot::D_METHOD("clear"), &VModel::Clear);

        godot::ClassDB::bind_method(godot::D_METHOD("has_voxel"), &VModel::HasVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("get_voxel"), &VModel::GetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("set_voxel"), &VModel::SetVoxel);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_voxel"), &VModel::RemoveVoxel);

        godot::ClassDB::bind_method(godot::D_METHOD("add_material"), &VModel::AddMaterial);
        godot::ClassDB::bind_method(godot::D_METHOD("get_material"), &VModel::GetMaterial);

        godot::ClassDB::bind_method(godot::D_METHOD("get_block_count"), &VModel::GetBlockCount);
    }

    int VModel::AddMaterial(const godot::Ref<VMaterial> &_Material)
    {
        m_Model->Materials.push_back(_Material->GetMaterial());
        return m_Model->Materials.size() - 1;
    }

    godot::Ref<VMaterial> VModel::GetMaterial(int _Index)
    {
        if(_Index < 0 || _Index > m_Model->Materials.size())
            return nullptr;

        return new VMaterial(m_Model->Materials[_Index]);
    }

    godot::Dictionary VModel::GetVoxel(const godot::Vector3i &_Pos)
    {
        godot::Dictionary ret;
        auto voxel = m_Model->GetVoxel(Convert::ToVVec3(_Pos));
        if(!voxel)
            return ret;

        // Check if a diffuse texture existing.
        auto it = m_Model->Textures.find(VCore::TextureType::DIFFIUSE);
        if(it != m_Model->Textures.end())
        {
            if(voxel->Color < it->second->GetSize().x)
            {
                auto color = VCore::CColor(it->second->GetPixel(VCore::Math::Vec2ui(voxel->Color, 0)));
                ret["color"] = godot::Color(color.R / 255.f, color.G / 255.f, color.B / 255.f, color.A / 255.f);
            }
        }

        ret["material"] = GetMaterial(voxel->Material);
        ret["transparent"] = voxel->Transparent;

        return ret;
    }

    bool VModel::HasVoxel(const godot::Vector3i &_Pos)
    {
        auto voxel = m_Model->GetVoxel(Convert::ToVVec3(_Pos));
        return voxel != nullptr;
    }

    void VModel::SetVoxel(const godot::Vector3i &_Pos, int _MaterialIdx, const godot::Color &_Color)
    {
        if(_MaterialIdx > m_Model->Materials.size() || _MaterialIdx < 0)
            _MaterialIdx = 0;

        int cidx = 0;
        auto it = m_Model->Textures.find(VCore::TextureType::DIFFIUSE);
        if(it != m_Model->Textures.end())
        {
            auto &pixels = it->second->GetPixels();
            auto cit = std::find(pixels.begin(), pixels.end(), _Color.to_abgr32());
            if(cit != pixels.end())
                cidx = (int)(cit - pixels.begin());
            else
            {
                it->second->AddPixel(_Color.to_abgr32());
                cidx = it->second->GetSize().x - 1;
            }
        }
        else
        {
            m_Model->Textures[VCore::TextureType::DIFFIUSE] = std::make_shared<VCore::CTexture>();
            m_Model->Textures[VCore::TextureType::DIFFIUSE]->AddPixel(_Color.to_abgr32());
        }

        m_Model->SetVoxel(Convert::ToVVec3(_Pos), _MaterialIdx, cidx, m_Model->Materials[_MaterialIdx]->Transparency != 0);
    }

    void VModel::RemoveVoxel(const godot::Vector3i &_Pos)
    {
        m_Model->RemoveVoxel(Convert::ToVVec3(_Pos));
    }
}