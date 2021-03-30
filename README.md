# Godot-Planet-Generator-Module

With this module you can create and configure procedurally created planet meshes in Godot.

You need the source code of Godot 3.2 in order to compile this project. 
The contents of this project must be copied to the /modules/ folder inside the Godot source code.

# API Specification

PlanetMeshInstance::CreatePlanet(const StringName& planetGroupName)

A mesh and a static body will be created and added as child nodes. 

# Editor

The PlanetMeshInstance class inherits from MeshInstance. 
You can add a PlanetMeshInstance node yourself, it will be added to the selectable Node Types.

![New Node Type](https://github.com/EternalColor/Godot-Planet-Generator-Module/blob/master/PlanetMeshInstanceNodeType.png?raw=true)
