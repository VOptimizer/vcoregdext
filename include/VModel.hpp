#ifndef VModel_HPP
#define VModel_HPP

#include "VoxelStorage.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <cstddef>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/geometry_instance3d.hpp>
#include <Helper/Convert.hpp>

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/string.hpp>
#include <VCore/VCore.hpp>

namespace VCoreGDExt
{
    /**
     * @brief A voxel model represents a collection of voxels.
     */
    class VModel : public godot::MeshInstance3D
    {
        GDCLASS(VModel, godot::MeshInstance3D)

        public:
            VModel() : m_Model(new VCore::CVoxelSpace()), m_MesherType(VCore::MesherTypes::GREEDY) { }

            VModel(const VCore::VoxelModel &p_Model, const godot::String &p_File, size_t p_Index, VCore::MesherTypes p_MesherType) : 
            m_Model(p_Model), m_MesherType(p_MesherType), m_File(p_File), m_Index(p_Index) 
            { 
                CVoxelStorage::GetInstance()->AddRef(m_File);
                GenerateMesh(); 
            }

            /**
             * @return Calculates and returns the bounding box of a voxel model.
             */
            godot::AABB _get_aabb() const override
            {
                auto bbox = m_Model->CalculateBBox();
                return godot::AABB(Convert::ToGDVec3(bbox.Beg), Convert::ToGDVec3(bbox.GetSize()));
            }

            /**
             * @return Gets a voxel a a given position. An empty dictionary means air, otherwise the format is as follows {"color": Color, "material": VMaterial, "transparent": bool}
            */
            godot::Dictionary GetVoxel(const godot::Vector3i &p_Pos);

            /**
             * @return Returns true, if at _Pos a voxel exists.
             */
            bool HasVoxel(const godot::Vector3i &p_Pos);

            /**
             * @brief Sets a voxel at a given position. If _MaterialIdx is invalid, the material will be fallback to the default one.
             */
            void SetVoxel(const godot::Vector3i &p_Pos, int p_MaterialIdx, const godot::Color &p_Color);

            /**
             * @brief Removes a voxel on a given position.
            */
            void RemoveVoxel(const godot::Vector3i &p_Pos);

            /** Generate the mesh. */
            void GenerateMesh();

            /**
             * @brief Clears all voxels.
             */
            void Clear()
            {
                m_Model->Clear();
                if(m_Mesh.is_valid())
                    m_Mesh->clear_surfaces();
            }

            /**
             * @brief Sets the mesher type. Each mesher type produces a more or less optimized vertex mesh.
             * Following types are possible:
             *      - SIMPLE (Lots of faces)
             *      - GREEDY (Less faces)
             *      - GREEDY_CHUNKED (Less faces but more than just greedy)
             *      - GREEDY_TEXTURED (Less faces, if you have a lot of texture)
             *      - SMOOTH (Lots of faces, but smoother)
             */
            void SetMesherType(int _Type)
            {
                if((VCore::MesherTypes)_Type == m_MesherType)
                    return;

                m_MesherType = (VCore::MesherTypes)_Type;
                GenerateMesh();
            }

            int GetMesherType() const
            {
                return (int)m_MesherType;
            }

            // uint64_t GetBlockCount()
            // {
            //     return (uint64_t)m_Model->GetBlockCount();
            // }

            /**
             * @brief For internal use only!
             */
            VCore::VoxelModel GetModel()
            {
                LoadVModel();
                return m_Model;
            }

            void _set_mesh(const godot::Ref<godot::Mesh> &) {}

            ~VModel()
            {
                if(!m_File.is_empty())
                    CVoxelStorage::GetInstance()->UnRef(m_File);
            }

        protected:
            void _notification(int p_what);
            void _validate_property(godot::PropertyInfo &p_property) const;

	        static void _bind_methods();
        private:
            godot::Dictionary GetModelConfig() const
            {
                godot::Dictionary result;
                result["file"] = m_File;
                result["index"] = m_Index;
                return result;
            }

            void SetModelConfig(godot::Dictionary p_Config)
            {
                if(!m_File.is_empty())
                    CVoxelStorage::GetInstance()->UnRef(m_File);

                if(p_Config.has("file"))
                    m_File = p_Config["file"];

                if(p_Config.has("index"))
                    m_Index = p_Config["index"];

                if(!m_File.is_empty())
                    CVoxelStorage::GetInstance()->AddRef(m_File);
            }

            void LoadVModel();

            godot::Ref<godot::ArrayMesh> m_Mesh;
            VCore::VoxelModel m_Model;
            VCore::MesherTypes m_MesherType;
            godot::String m_File;
            size_t m_Index{};
    };
} // namespace VCoreGDExt

#endif