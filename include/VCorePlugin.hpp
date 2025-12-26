#ifndef VCOREPLUGIN_H
#define VCOREPLUGIN_H

#include <godot_cpp/classes/editor_import_plugin.hpp>
#include <EditorSceneFormatImporterVCore.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>

using namespace godot;

namespace VCoreGDExt 
{
    class VCorePlugin : public EditorPlugin
    {
        GDCLASS(VCorePlugin, EditorPlugin);
        public:
            VCorePlugin() = default;

        	void _enter_tree() override;
	        void _exit_tree() override;

            ~VCorePlugin() = default;

        protected:
            static void _bind_methods() {}

        private:
            Ref<VCoreGDExt::EditorSceneFormatImporterVCore> m_Importer;
    };
}

#endif