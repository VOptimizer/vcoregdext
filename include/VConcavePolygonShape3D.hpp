#ifndef VCONCAVEPOLYGONSHAPE3D_H
#define VCONCAVEPOLYGONSHAPE3D_H

#include <ProjectSettingsPaths.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <VoxelStorage.hpp>
#include <VCore/VCore.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

using namespace godot;

namespace VCoreGDExt 
{
    class VConcavePolygonShape3D : public ConcavePolygonShape3D
    {
        GDCLASS(VConcavePolygonShape3D, ConcavePolygonShape3D);
        public:
            VConcavePolygonShape3D() = default;
            VConcavePolygonShape3D(
                const VCore::VoxelModel &p_Model, 
                const godot::String &p_File, 
                size_t p_Index, 
                VCore::MesherTypes p_MesherType, 
                double p_ModelScale) : 
                m_Model(p_Model), 
                m_MesherType(p_MesherType), 
                m_File(p_File), 
                m_Index(p_Index), 
                m_ModelScale(p_ModelScale)
            { 
                CVoxelStorage::GetInstance()->AddRef(m_File);
                GenerateColliderMesh();
            }

            void SetModelScale(double p_ModelScale) { m_ModelScale = p_ModelScale; }
            double GetModelScale() const { return m_ModelScale; }

            /** Generates the collider mesh. */
            void GenerateColliderMesh();

            /**
             * @return Returns true, if at p_Pos a voxel exists.
             */
            bool HasVoxel(const godot::Vector3i &p_Pos);

            /**
             * @brief Sets a voxel at a given position.
             */
            void SetVoxel(const godot::Vector3i &p_Pos);

            /**
             * @brief Removes a voxel on a given position.
            */
            void RemoveVoxel(const godot::Vector3i &p_Pos);

            /** Removes voxels relative to the origin. */
            void RemoveVoxels(const godot::Vector3i &p_Origin, godot::PackedVector3Array p_Points);

            /**
             * @brief Clears all voxels.
             */
            void Clear()
            {
                m_Model->Clear();
                set_faces(PackedVector3Array());
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
                GenerateColliderMesh();
            }

            int64_t GetMesherType() const
            {
                return (int64_t)m_MesherType;
            }

            ~VConcavePolygonShape3D()
            {
                if(!m_File.is_empty())
                    CVoxelStorage::GetInstance()->UnRef(m_File);
            }

        protected:
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
            VCore::VoxelModel m_Model{new VCore::CVoxelSpace()};
            VCore::MesherTypes m_MesherType{(VCore::MesherTypes)((int)godot::ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MESHER_PATH, (int)VCore::MesherTypes::GREEDY))};
            bool m_FileLoaded{};
            godot::String m_File;
            size_t m_Index{};
            double m_ModelScale{godot::ProjectSettings::get_singleton()->get_setting(VCORE_CONFIG_MODEL_SCALE_PATH, 1.0)};
    };
}

#endif