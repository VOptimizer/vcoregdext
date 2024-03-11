# Examples

## How to load a file?

```gdscript
func load_model(path: string) -> VModel:
    # The loader can load various voxel format.
    var loader : VLoader = VLoader.new()
    if loader.load(path) == OK:
        # Models contains a list of all voxel model of the given file
        # In this example I return the first model.
        var models : Array = loader.get_models()
        if !models.is_empty():
            return models[0]
    else:
        # TODO: Handle error
        pass

    return null
```

## How to generate a mesh?

```gdscript
func generate_mesh(model: VModel) -> void:
    # A mesher turns the voxel mode into an ArrayMesh, so godot can render it.
    # There are different mesher type, please visit the docs page of VMesher for more informations.
    var mesher : VMesher = VMesher.new()
    var mesh : ArrayMesh = mesher.generate_mesh(model)
    if mesh:
		$MeshInstance3D.mesh = mesh
```

## How to manipulate a model?

```gdscript
func alter_model(model: VModel) -> void:
    model.set_voxel(Vector3i(1, 1, 1), 0, Color.red)
    model.remove_voxel(Vector3i(1, 1, 1))
```