#pragma once
#include <vector>
#include "ModelHelper.h"
class DeviceContext;
class Mesh;
class Model {
private:
	std::vector<Mesh*> meshes;
	DeviceContext* device;
public:
	Model(DeviceContext* device, std::vector<ModelHelper::MeshInfo> meshes);
};