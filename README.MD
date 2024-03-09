# vcoregdext

Official VCore extension for Godot 4, which allows to load and mesh voxel models. This library is not a complete add-on for Godot, but a base for your add-ons, games or tools.

## Getting started

Download the latest build of this library from the [release](https://github.com/VOptimizer/vcoregdext/releases) tab.

Copy the library into your godot project or addon and create a `vcoregdext.gdextension` file.

Put the following content into the file.
```
[configuration]

entry_symbol = "VCoreGDExtensionEntry"
compatibility_minimum = "4.2"

[libraries]

windows.release.x86_64 = "res://vcoregdext.dll"
linux.release.x86_64 = "res://libvcoregdext.so"
macos.release.x86_64 = "res://libvcoregdext.framework"
```

## How to build

### Prerequirements

Ensure you have the following prerequisites before getting started:

- [cmake](https://cmake.org/)
- C++ compiler supporting at least C++17
- git
- At least Python 3.4

### Building

1. Clone this repo <br>
`git clone --recursive https://github.com/VOptimizer/vcoregdext.git`

2. Build the library<br>
```bash
cd vcoregdext
mkdir build
cd build
cmake ..
cmake --build .
```
3. Just drink some coffee or touch grass, it will take a while at first.
4. Copy the library into your project and create the `.gdextension` file as described above.

## License

This library is under the [MIT License](LICENSE.txt)