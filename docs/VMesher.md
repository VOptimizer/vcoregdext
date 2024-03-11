# Class: VMesher

> To turn a VModel into a useable mesh for godot you need this class.

### void set_mesher_type(int _Type)

> Sets the mesher type. Each mesher type produces a more or less optimized vertex mesh. Following types are possible:

### int get_mesher_type() const

### Array generate_scene( VSceneNode  _SceneTree, bool _MergeChildren)

> Generates a whole scene.

#### Param: _MergeChildren
> Merges all meshes into one.
#### Return:

> Returns a list of dictionaries, in the format {"name": String, "transform": Transform3D, "mesh": ArrayMesh, "frameTime": Time how long this frame should last in ms. 0 means no animation}

### ArrayMesh  generate_mesh( VModel  _Model)

#### Return:

> Unlike GenerateChunks this method will create one ArrayMesh, where all subchunks are merged into.

### Array generate_chunks( VModel  _Model, bool _OnlyDirtyChunks)

> Generates an array of godot::ArrayMesh. Each mesh represents one chunk.

#### Param: _Model
> Voxel model to meshify. 
#### Param: _OnlyDirtyChunks
> Meshes only dirty chunks.
#### Return:

> Returns a list of dictionaries, in the format {"mesh": ArrayMesh, "uniqueId": int}

