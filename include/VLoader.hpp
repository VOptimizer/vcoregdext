#ifndef VLOADER_HPP
#define VLOADER_HPP

#include "Helper/Convert.hpp"
#include "VModel.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/string.hpp"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/array.hpp>
#include <VCore/VCore.hpp>
#include <vector>

using namespace godot;

namespace VCoreGDExt
{
    class CSceneTreeConverter : public VCore::ISceneTreeVisitor<VCore::VoxelModel>
    {
        public:
            CSceneTreeConverter(const String &p_File, VCore::MesherTypes p_MesherType) : VCore::ISceneTreeVisitor<VCore::VoxelModel>() 
            {
                m_File = p_File;
                m_MesherType = p_MesherType;
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
                Node3D *result = nullptr;
                const auto model = dynamic_cast<const VCore::CSceneModelNode*>(p_Node);
                if(model)
                    result = memnew(VModel(m_SceneTree->GetModels()[model->ModelId], m_File, model->ModelId, m_MesherType));
                else
                    result = memnew(Node3D());

                CopyProperties(result, p_Node);
                
                Node3D *parent = m_Nodes.back();
                parent->add_child(result);
                result->set_owner(m_Owner);
                m_Nodes.push_back(result);
            }

            void LeaveSceneNode(const VCore::CSceneNodeBase *) override 
            {
                m_Nodes.pop_back();
            }
        
        private:
            void CopyProperties(Node3D *p_Node, const VCore::CSceneNodeBase *p_VNode)
            {
                if(!p_VNode->Name.empty())
                    p_Node->set_name(p_VNode->Name.c_str());
                else
                    p_Node->set_name(p_Node->get_class());

                p_Node->set_visible(p_VNode->Visible);
                p_Node->set_position(Convert::ToGDVec3(p_VNode->GetPosition()));
                p_Node->set_scale(Convert::ToGDVec3(p_VNode->GetScale()));
                p_Node->set_rotation(Convert::ToGDVec3(p_VNode->GetRotation()));
            }

            Node3D *m_Owner;
            std::vector<Node3D*> m_Nodes;
            String m_File;
            VCore::MesherTypes m_MesherType;
    };

    /**
     * @brief The loader class allows you to load all supported voxel formats. For a complete list of all supported formats, please visit https://github.com/VOptimizer/VCore/tree/master/Docs/Voxelformats.
     */
    class VLoader : public ResourceFormatLoader
    {
        GDCLASS(VLoader, ResourceFormatLoader)
        public:
            VLoader() = default;

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