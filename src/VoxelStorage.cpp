#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/string.hpp"
#include <GodotFileStream.hpp>
#include <chrono>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/memory.hpp>
#include <VoxelStorage.hpp>

namespace VCoreGDExt 
{
    CVoxelStorage *CVoxelStorage::s_Instance = nullptr;

    void CVoxelStorage::AddVoxelFile(godot::String p_File, VCore::VoxelSceneTree p_Tree)
    {
        m_Storage[p_File] = memnew(CVoxelTreeStorage(p_File, p_Tree));
    }

    CVoxelTreeStorage *CVoxelStorage::GetVoxelTree(godot::String p_File)
    {
        if(m_Storage.has(p_File))
            return dynamic_cast<CVoxelTreeStorage*>((godot::Object*)m_Storage[p_File]);

        try
        {
            auto start = std::chrono::high_resolution_clock::now();

            auto loader = VCore::IVoxelFormat::CreateAndOpen<CGodotIOHandler>(p_File.utf8().get_data(), VCore::FileMode::READ);
            loader->Load();

            auto end = std::chrono::high_resolution_clock::now();
            auto loaderduration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            ERR_PRINT(godot::String::num_real(loaderduration.count() / 1000000.0));

            AddVoxelFile(p_File, loader->SceneTree);
        }
        catch(const std::exception& e)
        {
            ERR_PRINT(e.what());
            return nullptr;
        }

        return dynamic_cast<CVoxelTreeStorage*>((godot::Object*)m_Storage[p_File]);
    }

    void CVoxelStorage::AddRef(const godot::String &p_File)
    {
        if(m_Storage.has(p_File))
        {
            auto t = m_Storage[p_File];
            auto type = t.get_type();

            CVoxelTreeStorage *ref = dynamic_cast<CVoxelTreeStorage*>((godot::Object*)m_Storage[p_File]);
            ref->References++;
        }
    }

    void CVoxelStorage::UnRef(const godot::String &p_File)
    {
        if(m_Storage.has(p_File))
        {
            CVoxelTreeStorage *ref = dynamic_cast<CVoxelTreeStorage*>((godot::Object*)m_Storage[p_File]);
            ref->References--;
            if(ref->References == 0)
            {
                memfree(ref);
                m_Storage.erase(p_File);
            }
        }
    }
}