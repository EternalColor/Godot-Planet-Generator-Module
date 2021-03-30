#include "PlanetMeshInstance.h"

PlanetMeshInstance::PlanetMeshInstance() 
    :  	resolution { 2 },
		noiseInfluence { 1 },
        shaderMaterial { nullptr },
		noise { nullptr }
{
	//Ensure Random is seeded
	Math::randomize();
}

void PlanetMeshInstance::_bind_methods()
{   
	ClassDB::bind_method(D_METHOD("CreatePlanet", "planetGroupName"), &PlanetMeshInstance::CreatePlanet);

    ClassDB::bind_method(D_METHOD("SetResolution", "resolution"), &PlanetMeshInstance::SetResolution);
    ClassDB::bind_method(D_METHOD("GetResolution"), &PlanetMeshInstance::GetResolution);

	ClassDB::bind_method(D_METHOD("SetShaderMaterial", "shaderMaterial"), &PlanetMeshInstance::SetShaderMaterial);
    ClassDB::bind_method(D_METHOD("GetShaderMaterial"), &PlanetMeshInstance::GetShaderMaterial);

	ClassDB::bind_method(D_METHOD("SetNoise", "noise"), &PlanetMeshInstance::SetNoise);
    ClassDB::bind_method(D_METHOD("GetNoise"), &PlanetMeshInstance::GetNoise);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "resolution"), "SetResolution", "GetResolution");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shaderMaterial", PROPERTY_HINT_RESOURCE_TYPE, "ShaderMaterial"), "SetShaderMaterial", "GetShaderMaterial");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise", PROPERTY_HINT_RESOURCE_TYPE, "OpenSimplexNoise"), "SetNoise", "GetNoise");
}

void PlanetMeshInstance::CreatePlanet(const StringName& planetGroupName)
{
	static const int FACE_COUNT { 6 };

	static const Vector3 DIRECTIONS[FACE_COUNT]
	{						//						^ y
		Vector3(-1, 0, 0), 	//LEFT X				|
		Vector3(1, 0, 0),	//RIGHT X				|
		Vector3(0, -1, 0),	//DOWN Y			    |_________> x
		Vector3(0, 1, 0),	//UP Y				   /
		Vector3(0, 0, -1),	//BACKWARD Z		  /	
		Vector3(0, 0, 1),	//FORWARD Z			 O z	
	};

	const int RESOLUTION_SQUARED { this->resolution * this->resolution };
	const int RESOLUTION_CUBED { this->resolution * this->resolution * this->resolution };
	const int RESOLUTION_MINUS_ONE { this->resolution - 1 }; 

	//Overwrites any seed set from outside! we always want a random seed when creating a new planet mesh.
	//But we keep the other noise settings
	noise->set_seed(Math::rand());

	int vertexIndex { 0 };
	int triangleIndex { 0 };

	Ref<ArrayMesh> planetArrayMesh;
    planetArrayMesh.instance();

    Array planetMeshArray = Array();
	planetMeshArray.resize(VS::ARRAY_MAX);

	PoolVector<Vector3> vertices;
	PoolVector<Vector3> normals;
	PoolVector<Vector2> uvs;
	PoolVector<int> triangles;

	vertices.resize(RESOLUTION_SQUARED * FACE_COUNT);
	normals.resize(RESOLUTION_SQUARED * FACE_COUNT);
	uvs.resize(RESOLUTION_SQUARED * FACE_COUNT);
	//RESOLUTION_MINUS_ONE * RESOLUTION_MINUS_ONE * 2 triangles * 3 vertices * 6 faces
	triangles.resize(RESOLUTION_MINUS_ONE * RESOLUTION_MINUS_ONE * 6 * FACE_COUNT);

	//Generate Quadsphere
	for(int faceIndex = 0; faceIndex < FACE_COUNT; ++faceIndex)
	{
		const Vector3 AXIS_A { DIRECTIONS[faceIndex].y, DIRECTIONS[faceIndex].z, DIRECTIONS[faceIndex].x };
		const Vector3 AXIS_B { DIRECTIONS[faceIndex].cross(AXIS_A).normalized() };

		for(int x = 0; x < this->resolution; ++x)
		{
			for(int y = 0; y < this->resolution; ++y)	
			{
				const Vector2 PERCENT { Vector2(x, y) / RESOLUTION_MINUS_ONE };

				const Vector3 POINT_ON_UNIT_CUBE { DIRECTIONS[faceIndex] + (PERCENT.x - 0.5f) * 2.0f * AXIS_A + (PERCENT.y - 0.5f) * 2.0f * AXIS_B };

				const Vector3 POINT_ON_UNIT_SPHERE { POINT_ON_UNIT_CUBE.normalized() };
				
				//Bring noise value from [-1, 1] to [0, 1] range. 
				const float NOISE_VALUE { (this->noise->get_noise_3dv(POINT_ON_UNIT_SPHERE * RESOLUTION_CUBED) + 1) * 0.5f };

				const Vector3 POINT_ON_PLANET { POINT_ON_UNIT_SPHERE * NOISE_VALUE };

				//Two triangles that make up the quad of this face
				if(x != RESOLUTION_MINUS_ONE && y != RESOLUTION_MINUS_ONE)
				{
					triangles.set(triangleIndex, vertexIndex);
					triangles.set(triangleIndex + 1, vertexIndex + this->resolution + 1);
					triangles.set(triangleIndex + 2, vertexIndex + this->resolution);

					triangles.set(triangleIndex + 3, vertexIndex);
					triangles.set(triangleIndex + 4, vertexIndex + 1);
					triangles.set(triangleIndex + 5, vertexIndex + this->resolution + 1);

					triangleIndex += 6;
				}

				vertices.set(vertexIndex, POINT_ON_PLANET);
				++vertexIndex;
			}
		}
	}

	for(int i = 0; i < triangles.size(); i += 3)
	{
		const int VERTEX_INDEX_1 { triangles[i] };
		const int VERTEX_INDEX_2 { triangles[i + 1] };
		const int VERTEX_INDEX_3 { triangles[i + 2] };

		const Vector3 VECTOR_1 { vertices[VERTEX_INDEX_1] };
		const Vector3 VECTOR_2 { vertices[VERTEX_INDEX_2] };
		const Vector3 VECTOR_3 { vertices[VERTEX_INDEX_3] };

		const Vector3 NORMAL { (-(VECTOR_2 - VECTOR_1)).normalized().cross((VECTOR_3 - VECTOR_1).normalized()).normalized() };
		
		normals[VERTEX_INDEX_1] = NORMAL;
		normals[VERTEX_INDEX_2] = NORMAL;
		normals[VERTEX_INDEX_3] = NORMAL;
	}

	for(int i = 0; i < uvs.size(); ++i)
	{
		uvs.set(i, Vector2(Math::range_lerp(uvs[i].x, 0, 10, 0, 1), 0));
	}

	planetMeshArray[VS::ARRAY_VERTEX] = vertices;
	planetMeshArray[VS::ARRAY_NORMAL] = normals;
	planetMeshArray[VS::ARRAY_TEX_UV] = uvs;
	planetMeshArray[VS::ARRAY_INDEX] = triangles;

	planetArrayMesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, planetMeshArray);

	this->set_mesh(planetArrayMesh);

	//Is not a Ref<> type, hence memnew
	StaticBody* staticBody = memnew(StaticBody);
	staticBody->add_to_group(planetGroupName);

	//Is not a Ref<> type, hence memnew
	CollisionShape* collisionShape = memnew(CollisionShape); 
	collisionShape->set_shape(this->mesh->create_trimesh_shape());

	staticBody->add_child(collisionShape);

	this->add_child(staticBody);

	//Assuming there is only one surface for a planet
	this->set_surface_material(0, this->shaderMaterial);
}

void PlanetMeshInstance::SetResolution(const int& resolution)
{
	this->resolution = resolution;
}

int PlanetMeshInstance::GetResolution() const
{
	return this->resolution;
}

void PlanetMeshInstance::SetShaderMaterial(const Ref<ShaderMaterial>& shaderMaterial)
{
    this->shaderMaterial = shaderMaterial;
}

Ref<ShaderMaterial> PlanetMeshInstance::GetShaderMaterial() const
{
    return this->shaderMaterial;
}

void PlanetMeshInstance::SetNoise(const Ref<OpenSimplexNoise>& noise)
{
    this->noise = noise;
}

Ref<OpenSimplexNoise> PlanetMeshInstance::GetNoise() const
{
    return this->noise;
}