#ifndef VOXELSTORAGE_HPP
#define VOXELSTORAGE_HPP

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <cstdint>
#include <godot_cpp/variant/typed_dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
    class CVoxelTreeStorage : public godot::Object
    {
        GDCLASS(CVoxelTreeStorage, godot::Object)
        public:
            const godot::String File;
            const VCore::VoxelSceneTree Tree;
            uint64_t References{};

            CVoxelTreeStorage() = default;
            CVoxelTreeStorage(const godot::String &p_File, const VCore::VoxelSceneTree &p_Tree) : File(p_File), Tree(p_Tree) {}
            ~CVoxelTreeStorage() = default;
        protected:
            static void _bind_methods() {}
    };

    class CVoxelStorage
    {
        public:
            CVoxelStorage() = default;

            static CVoxelStorage *GetInstance()
            {
                if(!s_Instance)
                    s_Instance = new CVoxelStorage();

                return s_Instance;
            }

            /**
             * @brief Adds a voxel file to the storage.
             *
             * @param p_File: File path of the voxeldata.
             * @param p_Tree: Voxel scene tree.
             */
            void AddVoxelFile(godot::String p_File, VCore::VoxelSceneTree p_Tree);

            /** @return Gets a voxel tree or null. */
            CVoxelTreeStorage *GetVoxelTree(godot::String p_File) const;

            /** Adds a reference to a voxel file */
            void AddRef(const godot::String &p_File);

            /** Removes a reference to a voxel file */
            void UnRef(const godot::String &p_File);

            ~CVoxelStorage() = default;
        private:
            godot::Dictionary m_Storage;

            static CVoxelStorage *s_Instance;
    };
}

#endif