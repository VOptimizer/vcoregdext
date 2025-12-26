#include <VCorePlugin.hpp>

namespace VCoreGDExt 
{
    void VCorePlugin::_enter_tree()
    {
        EditorPlugin::_enter_tree();
        m_Importer.instantiate();
        add_scene_format_importer_plugin(m_Importer);
    }

	void VCorePlugin::_exit_tree()
    {
        EditorPlugin::_exit_tree();
        remove_scene_format_importer_plugin(m_Importer);
    }
}