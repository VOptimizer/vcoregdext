#include <EditorSceneFormatImporterVCore.hpp>
#include <VCore/VCore.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include "VConcavePolygonShape3D.hpp"
#include "VCorePlugin.hpp"
#include "godot_cpp/classes/editor_plugin_registration.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include <godot_cpp/classes/project_settings.hpp>
#include <VoxelStorage.hpp>
#include <VFileRef.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include <VExporter.hpp>
#include <VMaterial.hpp>
#include <VModel.hpp>
#include <VLoader.hpp>
#include <ProjectSettingsPaths.hpp>

using namespace godot;

void DefineProperty(const PropertyInfo &p_Info, const Variant &p_Default)
{
    if(!ProjectSettings::get_singleton()->has_setting(p_Info.name))
    {
        ProjectSettings::get_singleton()->set(p_Info.name, p_Default);
    }

    ProjectSettings::get_singleton()->set_initial_value(p_Info.name, p_Default);
	ProjectSettings::get_singleton()->set_as_basic(p_Info.name, true);
	ProjectSettings::get_singleton()->set_restart_if_changed(p_Info.name, false);
	ProjectSettings::get_singleton()->set_as_internal(p_Info.name, false);

    Dictionary propertyInfo;
    propertyInfo["name"] = p_Info.name;
    propertyInfo["type"] = p_Info.type;
    propertyInfo["hint"] = p_Info.hint;
    propertyInfo["hint_string"] = p_Info.hint_string;
    ProjectSettings::get_singleton()->add_property_info(propertyInfo);
}

void InitializeVCoreGDExtension(ModuleInitializationLevel _Level)
{
// #ifdef TOOLS_ENABLED
	if (_Level == MODULE_INITIALIZATION_LEVEL_EDITOR) 
    {
		GDREGISTER_INTERNAL_CLASS(VCoreGDExt::VCorePlugin);
		GDREGISTER_INTERNAL_CLASS(VCoreGDExt::EditorSceneFormatImporterVCore);
		EditorPlugins::add_by_type<VCoreGDExt::VCorePlugin>();
	}
// #endif

    if(_Level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    // Register VCore Bindings
    ClassDB::register_class<VCoreGDExt::VExporter>();
    ClassDB::register_class<VCoreGDExt::VMaterial>();
    ClassDB::register_class<VCoreGDExt::VModel>();
    ClassDB::register_internal_class<VCoreGDExt::VFileRef>();
    ClassDB::register_class<VCoreGDExt::VLoader>();
    ClassDB::register_class<VCoreGDExt::VConcavePolygonShape3D>();
    ClassDB::register_internal_class<VCoreGDExt::CVoxelTreeStorage>();

    Ref<VCoreGDExt::VLoader> vloader;
    vloader.instantiate();
    ResourceLoader::get_singleton()->add_resource_format_loader(vloader);

    DefineProperty(PropertyInfo(godot::Variant::INT, VCORE_CONFIG_MESHER_PATH, godot::PROPERTY_HINT_ENUM, "SIMPLE,GREEDY,GREEDY_CHUNKED,GREEDY_TEXTURED,SMOOTH"), (int)VCore::MesherTypes::GREEDY);
    DefineProperty(PropertyInfo(godot::Variant::INT, VCORE_CONFIG_MODEL_TYPE_PATH, godot::PROPERTY_HINT_ENUM, "STATIC,DYNAMIC"), 0);
    DefineProperty(PropertyInfo(godot::Variant::BOOL, VCORE_CONFIG_MODEL_FRUSTUM_CULLING_ENABLED_PATH), false);
    DefineProperty(PropertyInfo(godot::Variant::FLOAT, VCORE_CONFIG_MODEL_SCALE_PATH), 1.0);
}

void DeinitializeVCoreGDExtension(ModuleInitializationLevel _Level)
{
	if (_Level == MODULE_INITIALIZATION_LEVEL_EDITOR)
		EditorPlugins::remove_by_type<VCoreGDExt::VCorePlugin>();

    if(_Level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    delete VCoreGDExt::CVoxelStorage::GetInstance();
    VCoreGDExt::VMaterial::DeinitShaderCode();
}

extern "C"
{
    // Defines the entry point for godot.
    GDExtensionBool GDE_EXPORT VCoreGDExtensionEntry(
        GDExtensionInterfaceGetProcAddress _GetProcAddr,
        const GDExtensionClassLibraryPtr _Library,
        GDExtensionInitialization *_Initialization
    )
    {
        GDExtensionBinding::InitObject initObj(_GetProcAddr, _Library, _Initialization);

        initObj.register_initializer(InitializeVCoreGDExtension);
        initObj.register_terminator(DeinitializeVCoreGDExtension);
        initObj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return initObj.init();
    }
}