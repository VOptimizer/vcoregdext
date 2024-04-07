#ifndef VSCENENODE_HPP
#define VSCENENODE_HPP

#include <Helper/Convert.hpp>

#include <godot_cpp/classes/ref.hpp>
#include <VCore/VCore.hpp>
#include "VModel.hpp"

namespace VCoreGDExt
{
    /**
     * @brief A scene node represents the Transform of a voxel model in 3d space.
     */
    class VSceneNode : public godot::RefCounted
    {
        GDCLASS(VSceneNode, godot::RefCounted)
        public:
            VSceneNode() : m_Node(new VCore::CSceneNode()) {}
            VSceneNode(const VCore::SceneNode &_Node) : m_Node(_Node) {}

            void SetVisible(bool _Visible)
            {
                m_Node->Visible = _Visible;
            }

            bool GetVisible()
            {
                return m_Node->Visible;
            }

            void SetName(const godot::String &_Name)
            {
                m_Node->Name = _Name.utf8().get_data();
            }

            godot::String GetName() const
            {
                return godot::String(m_Node->Name.c_str());
            }

            void SetPosition(const godot::Vector3 &_Position)
            {
                m_Node->Position = Convert::ToVVec3(_Position);
            }

            godot::Vector3 GetPosition()
            {
                return Convert::ToGDVec3(m_Node->Position);
            }

            void SetRotation(const godot::Vector3 &_Rotation)
            {
                m_Node->Rotation = Convert::ToVVec3(_Rotation);
            }

            godot::Vector3 GetRotation()
            {
                return Convert::ToGDVec3(m_Node->Rotation);
            }

            void SetScale(const godot::Vector3 &_Scale)
            {
                m_Node->Scale = Convert::ToVVec3(_Scale);
            }

            godot::Vector3 GetScale()
            {
                return Convert::ToGDVec3(m_Node->Scale);
            }

            void SetModel(const godot::Ref<VModel> &_Model)
            {
                m_Node->Mesh = _Model->GetModel();
            }

            godot::Ref<VModel> GetModel()
            {
                return memnew(VModel(m_Node->Mesh));
            }

            uint64_t GetChildrenCount() const 
            {
                return (uint64_t)m_Node->GetChildrenCount();
            }

            godot::Ref<VSceneNode> GetChild(uint64_t _Index)
            {
                auto node = (*m_Node)[_Index];
                if(!node)
                    return nullptr;

                return memnew(VSceneNode(node));
            }

            void AddChild(const godot::Ref<VSceneNode> &_Child)
            {
                m_Node->AddChild(_Child->GetSceneNode());
            }

            /**
             * @brief For internal use only!
             */
            VCore::SceneNode GetSceneNode()
            {
                return m_Node;
            }

            ~VSceneNode() = default;
        protected:
            VCore::SceneNode m_Node;
	        static void _bind_methods()
            {
                godot::ClassDB::bind_method(godot::D_METHOD("get_visible"), &VSceneNode::GetVisible);
                godot::ClassDB::bind_method(godot::D_METHOD("set_visible"), &VSceneNode::SetVisible);
                godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::BOOL, "visible"), "set_visible", "get_visible");

                godot::ClassDB::bind_method(godot::D_METHOD("get_name"), &VSceneNode::GetName);
                godot::ClassDB::bind_method(godot::D_METHOD("set_name"), &VSceneNode::SetName);
                godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::STRING, "name"), "set_name", "get_name");

                godot::ClassDB::bind_method(godot::D_METHOD("set_position"), &VSceneNode::SetPosition);
                godot::ClassDB::bind_method(godot::D_METHOD("get_position"), &VSceneNode::GetPosition);
                godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::VECTOR3, "position"), "set_position", "get_position");

                godot::ClassDB::bind_method(godot::D_METHOD("set_rotation"), &VSceneNode::SetRotation);
                godot::ClassDB::bind_method(godot::D_METHOD("get_rotation"), &VSceneNode::GetRotation);
                godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::VECTOR3, "rotation"), "set_rotation", "get_rotation");

                godot::ClassDB::bind_method(godot::D_METHOD("get_scale"), &VSceneNode::GetScale);
                godot::ClassDB::bind_method(godot::D_METHOD("set_scale"), &VSceneNode::SetScale);
                godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::VECTOR3, "scale"), "set_scale", "get_scale");

                godot::ClassDB::bind_method(godot::D_METHOD("set_model"), &VSceneNode::SetModel);
                godot::ClassDB::bind_method(godot::D_METHOD("get_model"), &VSceneNode::GetModel);
                // godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::OBJECT, "model", godot::PROPERTY_HINT_OBJECT_ID, VModel::get_class_static()), "set_model", "get_model");
            
                godot::ClassDB::bind_method(godot::D_METHOD("get_children_count"), &VSceneNode::GetChildrenCount);
                godot::ClassDB::bind_method(godot::D_METHOD("get_child"), &VSceneNode::GetChild);
                godot::ClassDB::bind_method(godot::D_METHOD("add_child"), &VSceneNode::AddChild);
            }
    };
} // namespace VCoreGDExt

#endif