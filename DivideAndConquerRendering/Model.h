#pragma once
#include <vector>
#include "vkGroups.h"
#include "ModelHelper.h"
class DeviceContext;
class Mesh;
class Model {
private:
	vkGroups::MeshGroup meshGroup;
public:
	Model(std::string filepath);

	void submitModel(DeviceContext* device);
};