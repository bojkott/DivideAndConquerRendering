#include "Model.h"
#include "Mesh.h"
#include "ModelHelper.h"
Model::Model(DeviceContext * device, std::vector<ModelHelper::MeshInfo> meshesInfo)
{
	for (auto & meshInfo : meshesInfo)
	{
		Mesh* mesh = new Mesh(device, meshInfo);
		meshes.push_back(mesh);
	}
}
