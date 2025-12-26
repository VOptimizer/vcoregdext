#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <VFileRef.hpp>

namespace VCoreGDExt
{
    void VFileRef::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("_set_file"), &VFileRef::SetFile);
        godot::ClassDB::bind_method(godot::D_METHOD("_get_file"), &VFileRef::GetFile);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::STRING, "_file", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_INTERNAL | godot::PROPERTY_USAGE_STORAGE), "_set_file", "_get_file");
    }

    VFileRef::~VFileRef()
    {
        if (!m_File.is_empty())
            CVoxelStorage::GetInstance()->UnRef(m_File);
    }
}