#ifndef PLANET
#define PLANET

#include "core/typedefs.h"
#include "core/class_db.h"
#include "core/reference.h"
#include "core/pool_vector.h"
#include "core/array.h"
#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/math/vector3.h"
#include "core/math/vector2.h"
#include "core/string_name.h"
#include "scene/resources/mesh.h"
#include "scene/resources/material.h"
#include "scene/resources/shader.h"
#include "scene/3d/mesh_instance.h"
#include "scene/3d/physics_body.h"
#include "scene/3d/collision_shape.h"
#include "modules/opensimplex/open_simplex_noise.h"

class PlanetMeshInstance final : public MeshInstance 
{
    GDCLASS(PlanetMeshInstance, MeshInstance);

    private:
        //Can only be a square shape (width = height)
        int resolution;
        int noiseInfluence;

        Ref<ShaderMaterial> shaderMaterial;
        Ref<OpenSimplexNoise> noise;
    protected:
        static void _bind_methods();

    public:
        PlanetMeshInstance();
        ~PlanetMeshInstance() = default;

        void CreatePlanet(const StringName& planetGroupName);

        void SetResolution(const int& resolution);
        int GetResolution() const;

        void SetShaderMaterial(const Ref<ShaderMaterial>& shaderMaterial);
        Ref<ShaderMaterial> GetShaderMaterial() const;

        void SetNoise(const Ref<OpenSimplexNoise>& noise);
        Ref<OpenSimplexNoise> GetNoise() const; 
};

#endif