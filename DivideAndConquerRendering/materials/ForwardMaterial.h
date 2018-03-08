#pragma once
#include "../Material.h"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <tiny_obj_loader.h>

class ForwardMaterial : public Material
{
private:
	struct MaterialBuffer
	{
		glm::vec4 ambient;
		glm::vec4 objColor;
		glm::vec4 emission;
		glm::vec4 specular;

	}buffer;

	tinyobj::material_t material;
public:
	ForwardMaterial(tinyobj::material_t material);
	tinyobj::material_t* getMaterial();

	size_t getMaterialBufferSize();

	std::string getAmbientTexname();
	std::string getDiffuseTexname();
	std::string getSpecularTexname();
	std::string getSpecularHighlightTexname();
	std::string getBumpTexname();
	std::string getDisplacementTexname();
	std::string getAlphaTexname();
	std::string getReflectionTexname();

	void* getMaterialBufferData();

};