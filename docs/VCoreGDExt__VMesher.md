# Class: VCoreGDExt::VMesher

> To turn a VModel into a useable mesh for godot you need this class.

### void void VCoreGDExt::VMesher::SetMesherType(int _Type)

> Sets the mesher type. Each mesher type produces a more or less optimized vertex mesh.

### int int VCoreGDExt::VMesher::GetMesherType() const

### godot::Array godot::Array VCoreGDExt::VMesher::GenerateScene(const godot::Ref< VSceneNode > _SceneTree, bool _MergeChildren)

> Generates a whole scene.

#### Return:

> Returns a list of dictionaries, in the format {"transform": Transform3D, "mesh": ArrayMesh}

### godot::Ref< godot::ArrayMesh > godot::Ref< godot::ArrayMesh > VCoreGDExt::VMesher::GenerateMesh(godot::Ref< VModel > _Model)

#### Return:

> Unlike GenerateChunks this method will create one ArrayMesh, where all subchunks are merged into.

### godot::Array godot::Array VCoreGDExt::VMesher::GenerateChunks(godot::Ref< VModel > _Model, bool _OnlyDirtyChunks)

> Generates an array of godot::ArrayMesh. Each mesh represents one chunk.

