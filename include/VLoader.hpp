#ifndef VLOADER_HPP
#define VLOADER_HPP

#include <godot_cpp/classes/ref.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
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