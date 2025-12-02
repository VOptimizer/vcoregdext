#include <VoxelStorage.hpp>
#include <VPackedScene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include <VExporter.hpp>
#include <VMaterial.hpp>
#include <VModel.hpp>
#include <VLoader.hpp>

using namespace godot;

static Ref<VCoreGDExt::VLoader> g_VLoader;

void InitializeVCoreGDExtension(ModuleInitializationLevel _Level)
{
    if(_Level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    // Register VCore Bindings
    ClassDB::register_class<VCoreGDExt::VExporter>();
    ClassDB::register_class<VCoreGDExt::VMaterial>();
    ClassDB::register_class<VCoreGDExt::VModel>();
    ClassDB::register_class<VCoreGDExt::VPackedScene>();
    ClassDB::register_class<VCoreGDExt::VLoader>();
    ClassDB::register_internal_class<VCoreGDExt::CVoxelTreeStorage>();

    g_VLoader.instantiate();
    ResourceLoader::get_singleton()->add_resource_format_loader(g_VLoader);

    // VCoreGDExt::VMaterial::InitShaderCode();
}

void DeinitializeVCoreGDExtension(ModuleInitializationLevel _Level)
{
    if(_Level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    delete VCoreGDExt::CVoxelStorage::GetInstance();

    ResourceLoader::get_singleton()->remove_resource_format_loader(g_VLoader);
    g_VLoader.unref();

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