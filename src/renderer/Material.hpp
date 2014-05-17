#ifndef KIRAN_MATERIAL_HDR
#define KIRAN_MATERIAL_HDR

#include <string>
#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"

struct IMaterialReflectionModel;
class Material {
public:
	Material():
		matID(-1),
		refractionIndex(1.0f),
		beerCoefficient(0.0f),
		specularExponent(1.0f),
		reflectionModel(0) {
	}

	void SetID(int id) { matID = id; }
	int GetID() const { return matID; }

	void SetReflectionModel(const IMaterialReflectionModel* m) { reflectionModel = m; }
	const IMaterialReflectionModel* GetReflectionModel() const { return reflectionModel; }

	void SetTexture(const std::string& s) { texture = s; }
	const std::string& GetTexture() const { return texture; }


	void SetRefractionIndex(float c) { refractionIndex = c; }
	float GetRefractionIndex() const { return refractionIndex; }

	void SetSpecularExponent(float c) { specularExponent = c; }
	float GetSpecularExponent() const { return specularExponent; }

	void SetBeerCoefficient(float b) { beerCoefficient = b; }
	float GetBeerCoefficient() const { return beerCoefficient; }

	bool IsSpecularlyReflective() const { return (specularReflectiveness.sqLen3D() > 0.0f); }
	bool IsSpecularlyRefractive() const { return (specularRefractiveness.sqLen3D() > 0.0f); }


	void SetDiffuseReflectiveness(const math::vec3f& v) { diffuseReflectiveness = v; }
	const math::vec3f& GetDiffuseReflectiveness() const { return diffuseReflectiveness; }

	void SetSpecularReflectiveness(const math::vec3f& v) { specularReflectiveness = v; }
	const math::vec3f& GetSpecularReflectiveness() const { return specularReflectiveness; }

	void SetSpecularRefractiveness(const math::vec3f& v) { specularRefractiveness = v; }
	const math::vec3f& GetSpecularRefractiveness() const { return specularRefractiveness; }


private:
	int matID;

	float refractionIndex;              // how much slower/faster light travels inside this material than outside
	float beerCoefficient;              // acts as multiplier for the absorbance factor of refracted rays; in [0, 1]

	float diffuseCoefficient;           // acts as multiplier for the diffuse color of this material; in [0, 1]
	float specularCoefficient;          // acts as multiplier for the specular color of this material; in [0, 1]
	float specularExponent;

	math::vec3f diffuseReflectiveness;  // probability (per color-band) that this material will generate a diffuse reflection
	math::vec3f specularReflectiveness; // probability (per color-band) that this material will generate a specular reflection
	math::vec3f specularRefractiveness; // probability (per color-band) that this material will generate a refraction

	std::string texture;

	const IMaterialReflectionModel* reflectionModel;
};

#endif
