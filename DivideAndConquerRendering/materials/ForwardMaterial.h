#pragma once
#include "../Material.h"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <tiny_obj_loader.h>

class ForwardMaterial : public Material
{
private:
	struct UniformBufferObject
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	tinyobj::material_t material;
public:
	ForwardMaterial(tinyobj::material_t material);
	tinyobj::material_t* getMaterial();
};