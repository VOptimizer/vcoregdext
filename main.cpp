#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include <VExporter.hpp>
#include <VMaterial.hpp>
#include <VModel.hpp>
#include <VMesher.hpp>
#include <VLoader.hpp>
#include <VSceneNode.hpp>

using namespace godot;

void InitializeVCoreGDExtension(ModuleInitializationLevel _Level)
{
    if(_Level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    // Register VCore Bindings
    ClassDB::register_class<VCoreGDExt::VExporter>();
    ClassDB::register_class<VCoreGDExt::VModel>();
    ClassDB::register_class<VCoreGDExt::VMaterial>();
    ClassDB::register_class<VCoreGDExt::VSceneNode>();
    ClassDB::register_class<VCoreGDExt::VMesher>();
    ClassDB::register_class<VCoreGDExt::VLoader>();

    // VCoreGDExt::VMaterial::InitShaderCode();
}

void DeinitializeVCoreGDExtension(ModuleInitializationLevel _Level)
{
    if(_Level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE)
        return;
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