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
            VLoader() {}

            /**
             * @brief Loads a given file.
             * 
             * @return Returns either OK on success or an error code otherwise.
             */
            godot::Error Load(const godot::String &_Path);

            /**
             * @return Returns the scenetree of the file. 
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

            ~VLoader() {}

        protected:
            VCore::VoxelFormat m_Loader;
	        static void _bind_methods();
    };
} // namespace VCoreGDExt

#endif