#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/print_string.hpp"
#include <VPackedScene.hpp>
#include <VoxelStorage.hpp>
#include <cstdint>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <VLoader.hpp>
#include <VModel.hpp>
#include <GodotFileStream.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
    Variant VLoader::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
    {
        Node3D *tree = nullptr;

        auto path = p_path;
        auto idx = path.rfind("?");
        VCore::MesherTypes mesherType = VCore::MesherTypes::GREEDY;
        if(idx != -1)
        {
            auto options = path.substr(idx + 1).split("&");
            path = path.substr(0, idx);

            for (int64_t i = 0; i < options.size(); i++) 
            {
                auto option = options[i].split("=");
                if(option.size() == 2)
                {
                    if(option[0] == "mesher")
                    {
                        auto values = godot::ClassDB::class_get_enum_constants(VModel::get_class_static(), "MesherTypes");
                        if(values.find(option[1]) != -1)
                        {
                            mesherType = (VCore::MesherTypes)godot::ClassDB::class_get_integer_constant(VModel::get_class_static(), option[1]);
                        }
                    }
                }
            }
        }
        
        try
        {
            auto loader = VCore::IVoxelFormat::CreateAndOpen<CGodotIOHandler>(path.utf8().get_data(), VCore::FileMode::READ);
            loader->Load();

            CVoxelStorage::GetInstance()->AddVoxelFile(path, loader->SceneTree);

            CSceneTreeConverter converter(path, mesherType);
            tree = converter.Convert(loader->SceneTree);
        }
        catch(const std::exception& e)
        {
            ERR_PRINT(e.what());
            return godot::Error::ERR_CANT_OPEN;
        }

        Ref<VPackedScene> scene = memnew(VPackedScene(path));
        auto err = scene->pack(tree);
        memfree(tree);
        if(err != godot::Error::OK)
            return err;
        
        return scene;
    }

    PackedStringArray VLoader::_get_recognized_extensions() const
    {
        PackedStringArray ext;
        ext.append("vox");
        ext.append("gox");
        ext.append("kenshape");
        ext.append("qb");
        ext.append("qbt");
        ext.append("qbcl");
        ext.append("qef");
        return ext;
    }

    bool VLoader::_exists(const String &p_path) const
    {
        auto path = p_path;
        auto idx = path.rfind("?");
        if(idx != -1)
            path = path.substr(0, idx);

        return FileAccess::file_exists(path);
    }
    
    bool VLoader::_recognize_path(const String &p_path, const StringName &p_type) const
    {
        auto path = p_path;
        auto idx = path.rfind("?");
        if(idx != -1)
            path = path.substr(0, idx);

        if(!p_type.is_empty() && !_handles_type(p_type))
            return false;

        auto exts = _get_recognized_extensions();
        for (int64_t i = 0; i < exts.size(); i++) 
        {
            const String ext = !exts[i].begins_with(".") ? "." + exts[i] : exts[i];
            if (path.right(ext.length()).nocasecmp_to(ext) == 0)
                return true;
		}

        return false;
	}

    bool VLoader::_handles_type(const StringName &p_type) const
    {
        return p_type.casecmp_to("PackedScene") == 0;
    }

    String VLoader::_get_resource_type(const String &p_path) const
    {
        return "PackedScene";
    }
} // namespace VCoreGDExt
