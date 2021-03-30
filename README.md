# Godot-Planet-Generator-Module

With this module you can create and configure procedurally created planet meshes in Godot.

You need the source code of Godot 3.2 in order to compile this project. 
The contents of this project must be copied to the /modules/ folder inside the Godot source.

# API Specification

PlanetMeshInstance::CreatePlanet(const StringName& planetGroupName)

A mesh and a static body will be created and added as child nodes. 

# Editor

You can add a PlanetMeshInstance node yourself, it will be added to the selectable Node Types.

![PlanetMeshInstanceNodeType](https://user-images.githubusercontent.com/58613850/112960420-af502180-9144-11eb-9ea4-b56588618ba7.png)

# In Game Screenshot

![Planet](https://user-images.githubusercontent.com/58613850/112960586-db6ba280-9144-11eb-9ed0-7db7afe26b41.png)
