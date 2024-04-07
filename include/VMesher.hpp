#ifndef VOXELMESHER_HPP
#define VOXELMESHER_HPP

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/string.hpp>
#include <VCore/VCore.hpp>
#include "VSceneNode.hpp"

#include "VModel.hpp"

namespace VCoreGDExt
{
    /**
     * @brief To turn a VModel into a useable mesh for godot you need this class.
    */
    class VMesher : public godot::RefCounted
    {
        GDCLASS(VMesher, godot::RefCounted)
        public:
            VMesher() : m_Mesher(VCore::IMesher::Create(VCore::MesherTypes::GREEDY)), m_MesherType(VCore::MesherTypes::GREEDY) {}

            /**
             * @brief Sets the mesher type. Each mesher type produces a more or less optimized vertex mesh.
             * Following types are possible:
             *      - SIMPLE (Lots of faces)
             *      - GREEDY (Less faces)
             *      - MARCHING_CUBES (Lots of faces, but smoother)
             *      - GREEDY_CHUNKED (Less faces but more than just greedy)
             *      - GREEDY_TEXTURED (Less faces, if you have a lot of texture)
             */
            void SetMesherType(int _Type)
            {
                m_MesherType = (VCore::MesherTypes)_Type;
                m_Mesher = VCore::IMesher::Create(m_MesherType);
            }

            int GetMesherType() const
            {
                return (int)m_MesherType;
            }

            /**
             * @brief Generates a whole scene.
             * 
             * @param _MergeChildren: Merges all meshes into one.
             * 
             * @return Returns a list of dictionaries, in the format {"name": String, "transform": Transform3D, "mesh": ArrayMesh, "frameTime": Time how long this frame should last in ms. 0 means no animation}
            */
            godot::Array GenerateScene(const godot::Ref<VSceneNode> _SceneTree, bool _MergeChildren);

            // /**
            //  * @return Returns a list of all frames, of the animation.
            //  */
            // std::vector<Mesh> GenerateAnimation(VoxelAnimation _Anim);

            // /**
            //  * @return Returns the voxel mesh as triangulated vertices mesh.
            //  */
            // Mesh GenerateMesh(VoxelModel m);

            // /**
            //  * @brief Sets a frustum, for culling.
            //  */
            // void SetFrustum(const CFrustum *_Frustum);

            /**
             * @return Unlike ::GenerateChunks this method will create one ArrayMesh, where all subchunks are merged into.
             */
            godot::Ref<godot::ArrayMesh> GenerateMesh(godot::Ref<VModel> _Model);

            /**
             * @brief Generates an array of godot::ArrayMesh. Each mesh represents one chunk.
             * 
             * @param _Model: Voxel model to meshify.
             * @param _OnlyDirtyChunks: Meshes only dirty chunks.
             * 
             * @return Returns a list of dictionaries, in the format {"mesh": ArrayMesh, "uniqueId": int}
             */
            godot::Array GenerateChunks(godot::Ref<VModel> _Model, bool _OnlyDirtyChunks);

            ~VMesher() = default;

        protected:
            VCore::Mesher m_Mesher;
            VCore::MesherTypes m_MesherType;
	        static void _bind_methods();

            godot::Ref<godot::ArrayMesh> CreateMesh(const VCore::Mesh &_Mesh);
            godot::Ref<godot::ImageTexture> ConvertTextureToGodot(const VCore::Texture &_Texture);
    };
} // namespace VCoreGDExt

#endif