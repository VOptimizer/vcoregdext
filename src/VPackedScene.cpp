#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <VPackedScene.hpp>

namespace VCoreGDExt
{
    void VPackedScene::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("_set_file"), &VPackedScene::SetFile);
        godot::ClassDB::bind_method(godot::D_METHOD("_get_file"), &VPackedScene::GetFile);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::STRING, "_file", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_INTERNAL | godot::PROPERTY_USAGE_STORAGE), "_set_file", "_get_file");
    }

    VPackedScene::~VPackedScene()
    {
        if (!m_File.is_empty())
            CVoxelStorage::GetInstance()->AddRef(m_File);
    }
}