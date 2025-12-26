#ifndef VLOADER_HPP
#define VLOADER_HPP

#include "GodotSurface.hpp"
#include "Helper/Convert.hpp"
#include "VConcavePolygonShape3D.hpp"
#include "VCore/Meshing/IMesher.hpp"
#include "VCore/Meshing/Mesh/Surface.hpp"
#include "VModel.hpp"
#include "godot_cpp/classes/collision_shape3d.hpp"
#include "godot_cpp/classes/concave_polygon_shape3d.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/memory.hpp"
#include <godot_cpp/variant/packed_string_array.hpp>
#include <cctype>
#include <cstdint>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/array.hpp>
#include <VCore/VCore.hpp>
#include <string>
#include <string_view>
#include <vector>

using namespace godot;

namespace VCoreGDExt
{
    class CSceneTreeConverter : public VCore::ISceneTreeVisitor<VCore::VoxelModel>
    {
        public:
            CSceneTreeConverter(const String &p_File, VCore::MesherTypes p_MesherType, ModelTypes p_ModelType, bool p_FrustumCullingEnabled, double p_ModelScale) : VCore::ISceneTreeVisitor<VCore::VoxelModel>() 
            {
                m_File = p_File;
                m_MesherType = p_MesherType;
                m_ModelType = p_ModelType;
                m_FrustumCullingEnabled = p_FrustumCullingEnabled;
                m_ModelScale = p_ModelScale;
            }

            Node3D *Convert(const VCore::VoxelSceneTree &p_Tree)
            {
                m_SceneTree = p_Tree;
                m_Owner = memnew(Node3D());
                CopyProperties(m_Owner, p_Tree.get());

                m_Nodes.push_back(m_Owner);
                TraverseTree();

                return m_Owner;
            }

        protected:
            void EnterSceneNode(const VCore::CSceneNodeBase *p_Node) override
            {
                if(p_Node == m_SceneTree.get())
                    return;

                Node3D *result = nullptr;
                Node3D *parent = m_Nodes.back();

                const auto model = dynamic_cast<const VCore::CSceneModelNode*>(p_Node);
                if(model)
                {
                    // Generates a collisionshape instead of a VModel
                    if(EndsWith(model->Name, "-col") || EndsWith(model->Name, "_col"))
                    {
                        result = GetStaticBody();
                        CollisionShape3D *shape = memnew(CollisionShape3D());
                        if(!result)
                            result = memnew(godot::StaticBody3D());
                        else
                            shape->set_owner(m_Owner);
                        
                        VConcavePolygonShape3D *shape3d = memnew(VConcavePolygonShape3D(
                            m_SceneTree->GetModels()[model->ModelId],
                            m_File, 
                            model->ModelId, 
                            m_MesherType, 
                            m_ModelScale
                        ));

                        if(!p_Node->Name.empty())
                            shape->set_name(p_Node->Name.c_str());
                        shape->set_shape(shape3d);
                        result->add_child(shape, true);
                    }
                    else
                        result = memnew(VModel(
                        m_SceneTree->GetModels()[model->ModelId], 
                        m_File, 
                        model->ModelId, 
                        m_MesherType, 
                        m_ModelType, 
                        m_FrustumCullingEnabled, 
                        m_ModelScale
                        ));
                }
                else
                    result = memnew(Node3D());

                if(!result->get_parent())
                {
                    CopyProperties(result, p_Node);
                    
                    parent->add_child(result, true);
                    result->set_owner(m_Owner);

                    // Sets the owner for all chunk nodes, if there are any.
                    for (int64_t i = 0; i < result->get_child_count(); i++) 
                        result->get_child(i)->set_owner(m_Owner);
                }

                m_Nodes.push_back(result);
            }

            void LeaveSceneNode(const VCore::CSceneNodeBase *) override 
            {
                m_Nodes.pop_back();
            }
        
        private:
            bool EndsWith(const std::string &p_String, std::string_view p_Ending) const
            {
                bool endsWith = false;
                if (p_String.length() >= p_Ending.length())
                {
                    std::string_view ending(p_String.data() + (p_String.size() - p_Ending.size()), p_Ending.size());
                    for (int i = 0; i < p_Ending.size(); i++) 
                    {
                        if(::tolower(ending[i]) == ::tolower(p_Ending[i]))
                            endsWith = true;
                        else
                        {
                            endsWith = false;
                            break;
                        }
                    }
                }

                return endsWith;
            }

            godot::StaticBody3D *GetStaticBody() const
            {
                Node3D *parent = m_Nodes.back();
                for (int32_t i = 0; i < parent->get_child_count(); i++) 
                {
                    godot::StaticBody3D *body = Object::cast_to<StaticBody3D>(parent->get_child(i));
                    if(body)
                        return body;
                }

                return nullptr;
            }

            void CopyProperties(Node3D *p_Node, const VCore::CSceneNodeBase *p_VNode)
            {
                if(!p_VNode->Name.empty())
                    p_Node->set_name(p_VNode->Name.c_str());
                else
                    p_Node->set_name(p_Node->get_class());

                p_Node->set_visible(p_VNode->Visible);
                p_Node->set_position(Convert::ToGDVec3(p_VNode->GetPosition() * m_ModelScale));
                p_Node->set_scale(Convert::ToGDVec3(p_VNode->GetScale()));
                p_Node->set_rotation(Convert::ToGDVec3(p_VNode->GetRotation()));
            }

            Node3D *m_Owner;
            std::vector<Node3D*> m_Nodes;
            String m_File;
            VCore::MesherTypes m_MesherType;
            bool m_FrustumCullingEnabled;
            ModelTypes m_ModelType;
            double m_ModelScale{1};
    };

    /**
     * @brief The loader class allows you to load all supported voxel formats. For a complete list of all supported formats, please visit https://github.com/VOptimizer/VCore/tree/master/Docs/Voxelformats.
     */
    class VLoader : public ResourceFormatLoader
    {
        GDCLASS(VLoader, ResourceFormatLoader)
        public:
            VLoader() = default;

            static PackedStringArray get_recognized_extensions();

            godot::Error LoadTree(const String &p_path, Node3D *&p_Tree, VCore::MesherTypes p_MesherType, ModelTypes p_ModelType, bool p_IsFrustumCullingEnabled, double p_ModelScale) const;

            Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
            PackedStringArray _get_recognized_extensions() const override;
            bool _handles_type(const StringName &p_type) const override;
            String _get_resource_type(const String &p_path) const override;
            bool _exists(const String &p_path) const override;
            bool _recognize_path(const String &p_path, const StringName &p_type) const override;

            ~VLoader() = default;
        protected:
	        static void _bind_methods() {}
    };
} // namespace VCoreGDExt

#endif