#ifndef VModel_HPP
#define VModel_HPP

#include <ProjectSettingsPaths.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <VoxelStorage.hpp>
#include <cstdint>
#include <godot_cpp/variant/dictionary.hpp>
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
    enum class ModelTypes
    {
        STATIC, //!< The complete voxel model is generated everytime, if something changed.
        DYNAMIC //!< Only chunks which had been changed are remeshed.
    };

    /**
     * @brief A voxel model represents a collection of voxels.
     */
    class VModel : public godot::MeshInstance3D
    {
        GDCLASS(VModel, godot::MeshInstance3D)

        public:
            VModel() : 
            m_Model(new VCore::CVoxelSpace()), 
            m_MesherType((VCore::MesherTypes)((int)godot::ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MESHER_PATH, (int)VCore::MesherTypes::GREEDY))), 
            m_ModelType((ModelTypes)((int)godot::ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_TYPE_PATH, (int)ModelTypes::STATIC))),
            m_FrustumCullingEnabled(godot::ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_FRUSTUM_CULLING_ENABLED_PATH, false)) { }

            VModel(
                const VCore::VoxelModel &p_Model, 
                const godot::String &p_File, 
                size_t p_Index, 
                VCore::MesherTypes p_MesherType, 
                ModelTypes p_ModelType, 
                bool p_FrustumCullingEnabled,
                double p_ModelScale) : 
                m_Model(p_Model), 
                m_MesherType(p_MesherType), 
                m_ModelType(p_ModelType), 
                m_FrustumCullingEnabled(p_FrustumCullingEnabled), 
                m_File(p_File), 
                m_Index(p_Index), 
                m_ModelScale(p_ModelScale)
            { 
                CVoxelStorage::GetInstance()->AddRef(m_File);
                GenerateMesh();
            }

            /**
             * @return Calculates and returns the bounding box of a voxel model.
             */
            virtual godot::AABB _get_aabb() const override
            {
                const_cast<VModel*>(this)->LoadVModel();
                auto bbox = m_Model->CalculateBBox();
                return godot::AABB(Convert::ToGDVec3(bbox.Beg), Convert::ToGDVec3(bbox.GetSize()));
            }

            /**
             * @return Gets a voxel a a given position. An empty dictionary means air, otherwise the format is as follows {"color": Color, "material": VMaterial, "transparent": bool}
            */
            godot::Dictionary GetVoxel(const godot::Vector3i &p_Pos);

            /**
             * @return Returns true, if at p_Pos a voxel exists.
             */
            bool HasVoxel(const godot::Vector3i &p_Pos);

            /**
             * @brief Sets a voxel at a given position. If p_MaterialIdx is invalid, the material will be fallback to the default one.
             */
            void SetVoxel(const godot::Vector3i &p_Pos, int p_MaterialIdx, const godot::Color &p_Color);

            bool IsFrustumCullingEnabled() const { return m_FrustumCullingEnabled; }

            /** If frustum culling enabled is set to true, only chunks inside the frustum of the camera are meshed. */
            void SetFrustumCullingEnabled(bool p_FrustumCullingEnabled) { m_FrustumCullingEnabled = p_FrustumCullingEnabled; }

            /**
             * @brief Removes a voxel on a given position.
            */
            void RemoveVoxel(const godot::Vector3i &p_Pos);

            /** Removes voxels relative to the origin. */
            void RemoveVoxels(const godot::Vector3i &p_Origin, godot::PackedVector3Array p_Points);

            /** Generates the vertex based mesh from the voxel data. */
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
             *      - SIMPLE Generates for each visible voxel at least one face (Lots of faces)
             *      - GREEDY Similar voxels are grouped into one big face (Less faces)
             *      - GREEDY_CHUNKED Similar to greedy but only works on one chunk at the time. (Less faces but more than just greedy)
             *      - GREEDY_TEXTURED Voxels with similar material are grouped into one big face. Each voxel color is rendered onto a texture. (Less faces, if you have a lot of texture)
             *      - SMOOTH Generates a smooth surface using the Transvoxel algorithm (Lots of faces, but smoother)
             */
            void SetMesherType(VCore::MesherTypes p_Type)
            {
                if(p_Type == m_MesherType)
                    return;

                m_MesherType = p_Type;
                GenerateMesh();
            }

            int64_t GetMesherType() const
            {
                return (int64_t)m_MesherType;
            }

            /** Sets the model type, which determines how the mesh of this model should be updated, when voxels are changed. */
            void SetModelType(ModelTypes p_Type)
            {
                if(p_Type == m_ModelType)
                    return;

                m_ModelType = p_Type;
                GenerateMesh();
            }


            int64_t GetModelType() const
            {
                return (int64_t)m_ModelType;
            }

            void SetModelScale(double p_ModelScale) { m_ModelScale = p_ModelScale; }
            double GetModelScale() const { return m_ModelScale; }

            /** Levelplacement helper. I need to know the size! */
            godot::AABB GetMeshAABB() const 
            {
                auto mesh = get_mesh();
                if(mesh.is_valid())
                    return mesh->get_aabb();

                return godot::AABB();
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
            static void FillArrayMesh(godot::Ref<godot::ArrayMesh> &p_Mesh, const VCore::Mesh &p_MeshData);

            godot::Ref<godot::ArrayMesh> m_Mesh;
            VCore::VoxelModel m_Model;
            VCore::MesherTypes m_MesherType;
            ModelTypes m_ModelType;
            bool m_FrustumCullingEnabled;
            bool m_FileLoaded{};
            godot::String m_File;
            size_t m_Index{};
            double m_ModelScale{godot::ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_SCALE_PATH, 1.0)};
    };
} // namespace VCoreGDExt

VARIANT_ENUM_CLASS_CAST(VCore::MesherTypes);
VARIANT_ENUM_CLASS_CAST(VCoreGDExt::ModelTypes);

#endif