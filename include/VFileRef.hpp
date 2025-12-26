#ifndef VPACKEDSCENE_H
#define VPACKEDSCENE_H

#include "VoxelStorage.hpp"
#include "godot_cpp/classes/resource.hpp"
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/wrapped.hpp>

using namespace godot;

namespace VCoreGDExt 
{
    class VFileRef : public Resource
    {
        GDCLASS(VFileRef, Resource)
        public:
            VFileRef() = default;
            VFileRef(const String &p_File) : m_File(p_File) { }

            String GetFile() const { return m_File; }
            void SetFile(const String &p_File) 
            { 
                if(!m_File.is_empty())
                    CVoxelStorage::GetInstance()->UnRef(p_File);

                m_File = p_File;
                CVoxelStorage::GetInstance()->AddRef(p_File);
            }

            ~VFileRef();
        protected:
            static void _bind_methods();

        private:
            String m_File;
    };
}

#endif