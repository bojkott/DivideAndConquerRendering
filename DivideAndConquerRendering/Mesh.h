#pragma once

#include <vulkan\vulkan.hpp>
#include "ModelHelper.h"

class DeviceContext;
class VertexBuffer;
class IndexBuffer;
class Technique;
class Model;
class Mesh
{
private: 
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	Technique* technique;
	Model* model;
public:
	Mesh(DeviceContext* context, ModelHelper::MeshInfo& meshInfo);
	Mesh() {};

	Technique* getTechnique();

	void bind(vk::CommandBuffer commandBuffer);
private:
};


