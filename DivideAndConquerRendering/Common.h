#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <glm\glm.hpp>

#include "VulkanHelpers.h"


const std::string MODEL_PATH = "../assets/models/";
const std::string TEXTURE_PATH = "../assets/textures/";

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}


};