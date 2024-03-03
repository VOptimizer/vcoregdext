#include <VLoader.hpp>
#include <VModel.hpp>
#include <GodotFileStream.hpp>
#include <godot_cpp/core/error_macros.hpp>

namespace VCoreGDExt
{
    godot::Error VLoader::Load(const godot::String &_Path)
    {
        try
        {
            m_Loader = VCore::IVoxelFormat::CreateAndLoad<CGodotIOHandler>(_Path.utf8().get_data());
        }
        catch(const std::exception& e)
        {
            ERR_PRINT(e.what());
            return godot::Error::ERR_CANT_OPEN;
        }
        
        return godot::Error::OK;
    }

    godot::Array VLoader::GetModels() const
    {
        godot::Array result;
        if(m_Loader)
        {
            for (auto &&model : m_Loader->GetModels())
                result.push_back(memnew(VModel(model)));
        }

        return result;
    }

    void VLoader::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("load"), &VLoader::Load);
        godot::ClassDB::bind_method(godot::D_METHOD("get_models"), &VLoader::GetModels);
        godot::ClassDB::bind_method(godot::D_METHOD("get_scene_tree"), &VLoader::GetSceneTree);
    }
} // namespace VCoreGDExt
