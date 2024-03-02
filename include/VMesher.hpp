#ifndef VOXELMESHER_HPP
#define VOXELMESHER_HPP

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/string.hpp>
#include <VCore/VCore.hpp>

#include "VModel.hpp"

namespace VCoreGDExt
{
    class VMesher : public godot::RefCounted
    {
        GDCLASS(VMesher, godot::RefCounted)
        public:
            VMesher() : m_Mesher(VCore::IMesher::Create(VCore::MesherTypes::GREEDY)) {}

            /**
             * @brief Generates an array of godot::ArrayMesh. Each mesh represents one chunk.
             * 
             * @param _Model: Voxel model to meshify.
             * @param _OnlyDirtyChunks: Meshes only dirty chunks.
             */
            godot::Array GenerateChunks(godot::Ref<VModel> _Model, bool _OnlyDirtyChunks);

            ~VMesher() {}

        protected:
            VCore::Mesher m_Mesher;
	        static void _bind_methods();

            godot::Ref<godot::ArrayMesh> CreateMesh(const VCore::Mesh &_Mesh);
            godot::Ref<godot::ImageTexture> ConvertTextureToGodot(const VCore::Texture &_Texture);
    };
} // namespace VCoreGDExt

#endif