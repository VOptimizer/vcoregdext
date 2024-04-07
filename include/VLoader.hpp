#ifndef VLOADER_HPP
#define VLOADER_HPP

#include <godot_cpp/classes/ref.hpp>
#include <VCore/VCore.hpp>
#include "VSceneNode.hpp"

namespace VCoreGDExt
{
    /**
     * @brief The loader class allows you to load all supported voxel formats. For a complete list of all supported formats, please visit https://github.com/VOptimizer/VCore/tree/master/Docs/Voxelformats.
     */
    class VLoader : public godot::RefCounted
    {
        GDCLASS(VLoader, godot::RefCounted)
        public:
            VLoader() = default;

            /**
             * @brief Loads a given file.
             * 
             * @return Returns either OK on success or Error::ERR_CANT_OPEN on error.
             */
            godot::Error Load(const godot::String &_Path);

            /**
             * @return Returns the scenetree of the file.
             * A scenetree represents the structure of the scene. In short on which position is which model.
             */
            godot::Ref<VSceneNode> GetSceneTree() const
            {
                if(!m_Loader || !m_Loader->GetSceneTree())
                    return nullptr;

                return memnew(VSceneNode(m_Loader->GetSceneTree()));
            }

            /**
             * @return Returns an array of VModels of the last loaded file.
             */
            godot::Array GetModels() const;

            ~VLoader() = default;

        protected:
            VCore::VoxelFormat m_Loader;
	        static void _bind_methods();
    };
} // namespace VCoreGDExt

#endif