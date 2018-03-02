#include "Mesh.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ModelHelper.h"
#include "Technique.h"
#include "Model.h"
Mesh::Mesh(DeviceContext* context, Model* model, ModelHelper::MeshInfo& meshInfo)
{
	this->model = model;
	// Data has been loaded, time to create buffers
	vertexBuffer = new VertexBuffer(meshInfo.vertices, context);
	//indexBuffer = new IndexBuffer(meshInfo.indices, context);
	technique = Technique::createOrGetTechnique(context, meshInfo.material, new RenderState());
}

Technique * Mesh::getTechnique()
{
	return technique;
}

void Mesh::bind(vk::CommandBuffer commandBuffer)
{
	commandBuffer.pushConstants(technique->getPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(&model->getModelMatrix()), &model->getModelMatrix());
	vertexBuffer->bind(commandBuffer);
}
