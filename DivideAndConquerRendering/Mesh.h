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
	Mesh(DeviceContext* context, Model* model, ModelHelper::MeshInfo& meshInfo);
	Mesh() {};

	Technique* getTechnique();

	void draw(vk::CommandBuffer commandBuffer);
private:
};


