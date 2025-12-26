#include "ProjectSettingsPaths.hpp"
#include "VLoader.hpp"
#include "VModel.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/variant/string.hpp"
#include <VCore/VCore.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <EditorSceneFormatImporterVCore.hpp>

namespace VCoreGDExt 
{
    static constexpr const char *MESHER_OPTION = "v_core/mesher";
    static constexpr const char *MODEL_TYPE = "v_core/model_type";
    static constexpr const char *FRUSTUM_CULLING_ENABLED = "v_core/frustum_culling_enabled";
    static constexpr const char *MODEL_SCALE = "v_core/model_scale";

    PackedStringArray EditorSceneFormatImporterVCore::_get_extensions() const
    {
        return VLoader::get_recognized_extensions();
    }

    Object *EditorSceneFormatImporterVCore::_import_scene(const String &p_path, uint32_t p_flags, const Dictionary &p_options)
    {
        Ref<VLoader> loader;
        loader.instantiate();

        Node3D *tree = nullptr;
        VCore::MesherTypes mesherType = (VCore::MesherTypes)((int)ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MESHER_PATH, (int)VCore::MesherTypes::GREEDY));
        ModelTypes modelType = (ModelTypes)((int)ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_TYPE_PATH, (int)ModelTypes::STATIC));
        bool isFrustumCullingEnabled = ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_FRUSTUM_CULLING_ENABLED_PATH, false);
        double modelScale = ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_SCALE_PATH, 1.f);

        if(p_options.has(MESHER_OPTION))
            mesherType = (VCore::MesherTypes)((int)p_options[MESHER_OPTION]);

        if(p_options.has(MODEL_TYPE))
            modelType = (ModelTypes)((int)p_options[MODEL_TYPE]);

        if(p_options.has(FRUSTUM_CULLING_ENABLED))
            isFrustumCullingEnabled = (bool)p_options[FRUSTUM_CULLING_ENABLED];

        if(p_options.has(MODEL_SCALE))
            modelScale = (double)p_options[MODEL_SCALE];

        auto err = loader->LoadTree(p_path, tree, mesherType, modelType, isFrustumCullingEnabled, modelScale);
        if(err != OK)
            return nullptr;

        return tree;
    }

    void EditorSceneFormatImporterVCore::_get_import_options(const String &p_path)
    {
        String ext = p_path.get_extension().to_lower();
        if(_get_extensions().find(ext) != -1)
        {
            add_import_option_advanced(Variant::INT, MESHER_OPTION, ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MESHER_PATH, (int)VCore::MesherTypes::GREEDY), PROPERTY_HINT_ENUM, "SIMPLE,GREEDY,GREEDY_CHUNKED,GREEDY_TEXTURED,SMOOTH");
            add_import_option_advanced(Variant::INT, MODEL_TYPE, (int)ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_TYPE_PATH, (int)ModelTypes::STATIC), PROPERTY_HINT_ENUM, "STATIC,DYNAMIC");
            add_import_option_advanced(Variant::BOOL, FRUSTUM_CULLING_ENABLED, ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_FRUSTUM_CULLING_ENABLED_PATH, false));
            add_import_option_advanced(Variant::FLOAT, MODEL_SCALE, ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_SCALE_PATH, 1.f));
        }
    }

    Variant EditorSceneFormatImporterVCore::_get_option_visibility(const String &p_path, bool p_for_animation, const String &p_option) const
    {
        String ext = p_path.get_extension().to_lower();
        if(_get_extensions().find(ext) != -1)
            return !p_option.begins_with("skins/") && !p_option.begins_with("animation/");
        
        return true;
    }
}