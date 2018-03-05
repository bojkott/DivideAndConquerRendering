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


	float device0Percentage = 0.5f;
	float device1Percentage = 0.5f;

	int numberOfSetsOnDevice0 = advancedMeshGroup.sets.size()*device0Percentage;
	int setIndex = 0;
	for (auto& advancedSet : advancedMeshGroup.sets)
	{
		setIndex++;
		DeviceContext* device = deviceGroup->getDevices()[deviceIndex];
		for (Mesh* mesh : advancedSet.second.sets[device])
		{
			device->submitMesh(mesh);
		}
		if (setIndex > numberOfSetsOnDevice0)
		{
			if (renderer->getSlaveDevicesEnabled())
				deviceIndex = deviceGroup->getGroupSize()-1;
		}

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
