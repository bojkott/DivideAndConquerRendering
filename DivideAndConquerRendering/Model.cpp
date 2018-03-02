#include "Model.h"
#include "Mesh.h"
#include "ModelHelper.h"
#include "Renderer.h"
#include "DeviceContext.h"
#include <glm\gtx\transform.hpp>
Model::Model(std::string filepath)
{
	Renderer::deviceGroup.createMeshGroup(ModelHelper::loadModelFromFile(filepath), this, meshGroup);
}

void Model::submitModel(DeviceContext * device)
{
	for (Mesh* mesh : meshGroup.sets[device])
	{
		device->submitMesh(mesh);
	}
}

glm::mat4 Model::getModelMatrix()
{
	return modelMatrix;
}
