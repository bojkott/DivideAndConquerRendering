#include "Mesh.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ModelHelper.h"
#include "Technique.h"
#include "Model.h"
Mesh::Mesh(DeviceContext* context, ModelHelper::MeshInfo& meshInfo)
{
	// Data has been loaded, time to create buffers
	vertexBuffer = new VertexBuffer(meshInfo.vertices, context);
	indexBuffer = new IndexBuffer(meshInfo.indices, context);
	technique = Technique::createOrGetTechnique(context, meshInfo.material, new RenderState());
}

Technique * Mesh::getTechnique()
{
	return technique;
}

void Mesh::bind(vk::CommandBuffer commandBuffer)
{
	vertexBuffer->bind(commandBuffer);
}
