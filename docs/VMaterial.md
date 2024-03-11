# Class: VMaterial

> Represents a voxel material. This class can be use like a StandardMaterial3D, however if you plan to use this library only at development time you need to convert this material into an actual StandardMaterial3D.

### String get_name() const

### void set_name(String _Name)

### float get_metallic() const

### void set_metallic(float _Metallic)

### float get_specular() const

### void set_specular(float _Specular)

### float get_roughness() const

### void set_roughness(float _Roughness)

### float get_i_o_r() const

### void set_i_o_r(float _Ior)

### float get_power() const

### void set_power(float _Power)

### float get_transparency() const

### void set_transparency(float _Transparency)

### void set_albedo_texture( Texture  _Texture)

### Texture  get_albedo_texture() const

### void set_emission_texture( Texture  _Texture)

### Texture  get_emission_texture() const

### StandardMaterial3D  to_standard_material3_d() const

#### Return:

> Converts this material to a standard godot material. This is only needed if don't want to deploy this library with your project, and just want to import assets at development time.

### Material get_material()

> For internal use only!

### RID get_shader_rid() override

### bool can_do_next_pass() override

### bool can_use_render_priority() override

### Mode get_shader_mode() override

