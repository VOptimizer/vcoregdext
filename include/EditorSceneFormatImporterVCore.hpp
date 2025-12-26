#ifndef EDITORSCENEFORMATIMPORTERVCORE_H
#define EDITORSCENEFORMATIMPORTERVCORE_H

#include <godot_cpp/classes/editor_scene_format_importer.hpp>

using namespace godot;

namespace VCoreGDExt 
{
    class EditorSceneFormatImporterVCore : public EditorSceneFormatImporter
    {
        GDCLASS(EditorSceneFormatImporterVCore, EditorSceneFormatImporter);
        public:
            EditorSceneFormatImporterVCore() = default;

            PackedStringArray _get_extensions() const override;
            Object *_import_scene(const String &p_path, uint32_t p_flags, const Dictionary &p_options) override;
            void _get_import_options(const String &p_path) override;
            Variant _get_option_visibility(const String &p_path, bool p_for_animation, const String &p_option) const override;

            ~EditorSceneFormatImporterVCore() = default;

        protected:
            static void _bind_methods() {}
    };
}

#endif