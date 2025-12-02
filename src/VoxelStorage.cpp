#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/ref.hpp"
#include <godot_cpp/core/memory.hpp>
#include <VoxelStorage.hpp>

namespace VCoreGDExt 
{
    CVoxelStorage *CVoxelStorage::s_Instance = nullptr;

    void CVoxelStorage::AddVoxelFile(godot::String p_File, VCore::VoxelSceneTree p_Tree)
    {
        m_Storage[p_File] = memnew(CVoxelTreeStorage(p_File, p_Tree));
    }

    CVoxelTreeStorage *CVoxelStorage::GetVoxelTree(godot::String p_File) const
    {
        if(m_Storage.has(p_File))
            return dynamic_cast<CVoxelTreeStorage*>((godot::Object*)m_Storage[p_File]);

        return nullptr;
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