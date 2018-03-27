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
	meshInfo = ModelHelper::loadModelFromFile(filepath);
	Renderer::deviceGroup.createAdvancedMeshGroup(meshInfo, this, advancedMeshGroup);
}

Model::Model(Model * otherModel)
{
	meshInfo = otherModel->meshInfo;
	Renderer::deviceGroup.createAdvancedMeshGroup(meshInfo, this, advancedMeshGroup);
}

void Model::submitModel(Renderer* renderer)
{
	DeviceGroup* deviceGroup = &renderer->deviceGroup;
	int deviceIndex = 0;


	std::map<DeviceContext*, int> numberOfMaterialsOnDevices;
	for (auto& device : deviceGroup->getDevices())
	{
		numberOfMaterialsOnDevices[device] = advancedMeshGroup.sets.size() * device->getLoadPercentage();
	}

	int setIndex = 0;
	for (auto& advancedSet : advancedMeshGroup.sets)
	{
		setIndex++;
		DeviceContext* device = deviceGroup->getDevices()[deviceIndex];
		int numberOfMaterialsOnDevice = numberOfMaterialsOnDevices[device];
		for (Mesh* mesh : advancedSet.second.sets[device])
		{
			device->submitMesh(mesh);
		}
		if (setIndex > numberOfMaterialsOnDevice)
		{
			if (renderer->getSlaveDevicesEnabled())
				deviceIndex = deviceGroup->getGroupSize()-1;
			setIndex = 0;
		}

	}

}

void Model::submitModel(DeviceContext * device)
{
}

glm::mat4 Model::getModelMatrix()
{
	return modelMatrix;
}

void Model::setPosition(glm::vec3 pos)
{
	modelMatrix = glm::translate(glm::mat4(), pos);
}
