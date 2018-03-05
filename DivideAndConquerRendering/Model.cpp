#include "Model.h"
#include "Mesh.h"
#include "ModelHelper.h"
#include "DeviceGroup.h"
#include "Renderer.h"
#include "DeviceContext.h"
#include "Renderer.h"
#include <glm\gtx\transform.hpp>
#include "Technique.h"
Model::Model(std::string filepath)
{
	//Renderer::deviceGroup.createMeshGroup(ModelHelper::loadModelFromFile(filepath), this, meshGroup);
	Renderer::deviceGroup.createAdvancedMeshGroup(ModelHelper::loadModelFromFile(filepath), this, advancedMeshGroup);
}

void Model::submitModel(Renderer* renderer)
{
	DeviceGroup* deviceGroup = &renderer->deviceGroup;
	int deviceIndex = 0;
	for (auto& advancedSet : advancedMeshGroup.sets)
	{
		DeviceContext* device = deviceGroup->getDevices()[deviceIndex];
		for (Mesh* mesh : advancedSet.second.sets[device])
		{
			device->submitMesh(mesh);
		}
		if(renderer->getSlaveDevicesEnabled())
			deviceIndex = (deviceIndex + 1) % deviceGroup->getGroupSize();
	}

}

glm::mat4 Model::getModelMatrix()
{
	return modelMatrix;
}

void Model::setPosition(glm::vec3 pos)
{
	modelMatrix = glm::translate(glm::mat4(), pos);
}
