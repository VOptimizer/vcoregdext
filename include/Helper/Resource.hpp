
#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <godot_cpp/variant/variant.hpp>

#define MAKE_RESOURCE_TYPE_HINT(m_type) godot::vformat("%s/%s:%s", godot::Variant::OBJECT, godot::PROPERTY_HINT_RESOURCE_TYPE, m_type)

#endif