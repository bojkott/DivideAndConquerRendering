#include "Model.h"
#include "Mesh.h"
#include "ModelHelper.h"
#include "Renderer.h"
#include "DeviceContext.h"

Model::Model(std::string filepath)
{
	Renderer::deviceGroup.createMeshGroup(ModelHelper::loadModelFromFile(filepath), meshGroup);
}

void Model::submitModel(DeviceContext * device)
{
	for (Mesh* mesh : meshGroup.sets[device])
	{
		device->submitMesh(mesh);
	}
}
