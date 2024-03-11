#ifndef VModel_HPP
#define VModel_HPP

#include "VMaterial.hpp"
#include <Helper/Convert.hpp>

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/string.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
    /**
     * @brief A voxel model represents a collection of voxels, textures and materials.
     */
    class VModel : public godot::RefCounted
    {
        GDCLASS(VModel, godot::RefCounted)
        public:
            VModel() : m_Model(new VCore::CVoxelModel()) 
            {
                m_Model->Materials.push_back(std::make_shared<VCore::CMaterial>());
            }

            VModel(const VCore::VoxelModel &_Model) : m_Model(_Model) { }

            godot::String GetName() const
            {
                return godot::String(m_Model->Name.c_str());
            }

            void SetName(const godot::String &_Name)
            {
                m_Model->Name = _Name.utf8().get_data();
            }

            /**
             * @return Returns the bounding box of a voxel model inside the voxel grid.
             */
            godot::AABB GetBBox() const
            {
                return godot::AABB(Convert::ToGDVec3(m_Model->BBox.Beg), Convert::ToGDVec3(m_Model->BBox.GetSize()));
            }

            /**
             * @brief Must be called for each new material to be used in the model.
             * The default material has the index 0.
             */
            int AddMaterial(const godot::Ref<VMaterial> &_Material);

            /**
             * @return Gets the reference to a voxel material by its index.
             */
            godot::Ref<VMaterial> GetMaterial(int _Index);

            /**
             * @return Gets a voxel a a given position. An empty dictionary means air, otherwise the format is as follows {"color": Color, "material": VMaterial, "transparent": bool}
            */
            godot::Dictionary GetVoxel(const godot::Vector3i &_Pos);

            /**
             * @brief Sets a voxel at a given position. If _MaterialIdx is invalid, the material will be fallback to the default one.
             */
            void SetVoxel(const godot::Vector3i &_Pos, int _MaterialIdx, const godot::Color &_Color);

            /**
             * @brief Removes a voxel on a given position.
            */
            void RemoveVoxel(const godot::Vector3i &_Pos);

            /**
             * @brief Clears all voxels.
             */
            void Clear()
            {
                m_Model->Clear();
            }

            uint64_t GetBlockCount()
            {
                return (uint64_t)m_Model->GetBlockCount();
            }

            /**
             * @brief For internal use only!
             */
            VCore::VoxelModel GetModel()
            {
                return m_Model;
            }

            ~VModel() {}

        protected:
            VCore::VoxelModel m_Model;
	        static void _bind_methods();
    };
            // // Texturing
            // TexturingTypes TexturingType;
            // CVoxelTextureMap TextureMapping;           //!< Contains information about the texture atlas mapping.

            // std::vector<Material> Materials;           //!< Used materials
            // std::map<TextureType, Texture> Textures;   //!< Used colors / texture atlas

            // /**
            //  * List of voxels. The size of the list is always the size of the voxel space.
            //  * Voxels which are null are empty space.
            //  * 
            //  * @return Returns the list of voxels.
            //  */
            // inline VoxelData &GetVoxels() //const
            // {
            //     return m_Voxels;
            // }

            // /**
            //  * @brief Sets a voxel with an given material index.
            //  * 
            //  * @param Pos: The position of the voxel inside the voxel space.
            //  * @param Material: Material index of the voxels material.
            //  * @param Color: Color index.
            //  * @param Transparent: Is the block transparent?
            //  */
            // void SetVoxel(const Math::Vec3i &Pos, int Material, int Color, bool Transparent);

            // /**
            //  * @brief Removes a voxel on a given position
            //  * 
            //  * @param Pos: Position of the voxel to remove.
            //  */
            // void RemoveVoxel(const Math::Vec3i &Pos);

            // /**
            //  * @brief Clears all voxeldata.
            //  */
            // void Clear();

            // /**
            //  * @return Gets a voxel on a given position.
            //  */
            // Voxel GetVoxel(const Math::Vec3i &Pos);

            // /**
            //  * @brief Gets a voxel on a given position.
            //  * 
            //  * @param pos: Position of the voxel
            //  * @param OpaqueOnly: If true than only opaque voxels are returned, otherwise all transparent one.
            //  */
            // Voxel GetVoxel(const Math::Vec3i &Pos, bool OpaqueOnly);

            // /**
            //  * @return Gets a visible voxel on a given position.
            //  */
            // Voxel GetVisibleVoxel(const Math::Vec3i &Pos);

            // /**
            //  * @brief Gets a visible voxel on a given position.
            //  * 
            //  * @param pos: Position of the voxel
            //  * @param OpaqueOnly: If true than only opaque voxels are returned, otherwise all transparent one.
            //  */
            // Voxel GetVisibleVoxel(const Math::Vec3i &Pos, bool OpaqueOnly);

            // /**
            //  * @brief Gets the count of all setted blocks.
            //  */
            // inline size_t GetBlockCount() const
            // {
            //     return m_Voxels.size();
            // }

            // /**
            //  * @brief Queries all visible voxels.
            //  * @param opaque: If true only opaque voxels are returned, otherwise only none opaque voxels are returned.
            //  */
            // VectoriMap<Voxel> QueryVisible(bool opaque) const;

            // /**
            //  * @return Gets a list of all chunks which has been modified.
            //  * @note Marks all chunks as processed.
            //  */
            // VoxelData::querylist QueryDirtyChunks();

            // /**
            //  * @return Returns all chunks.
            //  */
            // VoxelData::querylist QueryChunks() const;

            // /**
            //  * @return Returns all chunks inside of the frustum
            //  */
            // VoxelData::querylist QueryChunks(const CFrustum *_Frustum) const;
} // namespace VCoreGDExt

#endif