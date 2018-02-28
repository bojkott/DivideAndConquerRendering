#pragma once

#include "ModelHelper.h"

class DeviceContext;
class VertexBuffer;
class IndexBuffer;
class Material;
class Mesh
{
private: 
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	Material* material;
	
public:
	Mesh(DeviceContext* context, ModelHelper::MeshInfo& meshInfo);
	Mesh() {};
private:
};


