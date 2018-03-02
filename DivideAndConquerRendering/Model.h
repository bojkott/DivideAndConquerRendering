#pragma once
#include <vector>
#include "vkGroups.h"
#include "ModelHelper.h"

#include <glm\glm.hpp>

class DeviceContext;
class Mesh;
class Model {
private:
	vkGroups::MeshGroup meshGroup;
	glm::mat4 modelMatrix;
public:
	Model(std::string filepath);

	void submitModel(DeviceContext* device);

	glm::mat4 getModelMatrix();
};