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
		/*
		Get stuff from tiny obj
		Like difuse and color
		*/
	};

	tinyobj::material_t material;
public:
	ForwardMaterial(tinyobj::material_t material);
	tinyobj::material_t* getMaterial();
};