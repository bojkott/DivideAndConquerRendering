#pragma once
#include <vector>
#include "vkGroups.h"
#include "ModelHelper.h"

#include <glm\glm.hpp>
class Mesh;
class Renderer;
class Model {
private:
	std::vector<ModelHelper::MeshInfo> meshInfo;
	vkGroups::AdvancedMeshGroup advancedMeshGroup;
	glm::mat4 modelMatrix;
public:
	Model(std::string filepath);
	Model(Model* otherModel);
	void submitModel(Renderer* renderer);

	glm::mat4 getModelMatrix();

	void setPosition(glm::vec3 pos);
};