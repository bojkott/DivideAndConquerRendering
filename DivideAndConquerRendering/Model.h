#pragma once
#include <vector>
#include "vkGroups.h"
#include "ModelHelper.h"

#include <glm\glm.hpp>
class Mesh;
class DeviceGroup;
class Model {
private:
	//vkGroups::MeshGroup meshGroup;
	vkGroups::AdvancedMeshGroup advancedMeshGroup;
	glm::mat4 modelMatrix;
public:
	Model(std::string filepath);

	void submitModel(DeviceGroup* device);

	glm::mat4 getModelMatrix();

	void setPosition(glm::vec3 pos);
};