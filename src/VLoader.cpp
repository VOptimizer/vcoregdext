#include "godot_cpp/variant/string.hpp"
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <VFileRef.hpp>
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
#include <ProjectSettingsPaths.hpp>

namespace VCoreGDExt
{
    PackedStringArray VLoader::get_recognized_extensions()
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

    godot::Error VLoader::LoadTree(const String &p_path, Node3D *&p_Tree, VCore::MesherTypes p_MesherType, ModelTypes p_ModelType, bool p_IsFrustumCullingEnabled, double p_ModelScale) const
    {
        auto vtree = CVoxelStorage::GetInstance()->GetVoxelTree(p_path);
        if(!vtree)
            return godot::Error::ERR_CANT_OPEN;
        
        CSceneTreeConverter converter(p_path, p_MesherType, p_ModelType, p_IsFrustumCullingEnabled, p_ModelScale);
        p_Tree = converter.Convert(vtree->Tree);
        return godot::Error::OK;
    }

    Variant VLoader::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
    {
        Node3D *tree = nullptr;

        auto path = p_path;
        auto idx = path.rfind("?");
        VCore::MesherTypes mesherType = (VCore::MesherTypes)((int)ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MESHER_PATH, (int)VCore::MesherTypes::GREEDY));
        ModelTypes modelType = (ModelTypes)((int)ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_TYPE_PATH, (int)ModelTypes::STATIC));
        bool isFrustumCullingEnabled = ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_FRUSTUM_CULLING_ENABLED_PATH, false);
        double modelScale = ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_SCALE_PATH, 1.f);

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
                            mesherType = (VCore::MesherTypes)godot::ClassDB::class_get_integer_constant(VModel::get_class_static(), option[1]);
                    }
                    else if(option[0] == "modelType")
                    {
                        auto values = godot::ClassDB::class_get_enum_constants(VModel::get_class_static(), "ModelTypes");
                        if(values.find(option[1]) != -1)
                            modelType = (ModelTypes)godot::ClassDB::class_get_integer_constant(VModel::get_class_static(), option[1]);
                    }
                    else if(option[0] == "frustumCullingEnabled")
                        isFrustumCullingEnabled = option[1] == "true";
                    else if(option[0] == "modelScale")
                        modelScale = option[1].to_float();
                }
            }
        }

        godot::Error err = LoadTree(path, tree, mesherType, modelType, isFrustumCullingEnabled, modelScale);
        if(err != godot::OK)
            return err;

        Ref<PackedScene> scene = memnew(PackedScene());
        scene->set_meta("__vcore_file_ref", memnew(VFileRef(path)));
        err = scene->pack(tree);
        memfree(tree);
        if(err != godot::Error::OK)
            return err;
        
        return scene;
    }

    PackedStringArray VLoader::_get_recognized_extensions() const
    {
        return get_recognized_extensions();
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
