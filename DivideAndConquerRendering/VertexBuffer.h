#pragma once
#include "Buffer.h"
#include "ModelHelper.h"
class DeviceContext;
class VertexBuffer : public Buffer
{
public:
	
private:
	uint32_t nrOfVerts;	
public:
	VertexBuffer(std::vector<ModelHelper::Vertex>& verts, DeviceContext * context);
	void bind(vk::CommandBuffer& commandBuffer) const;
	uint32_t getNrOfVerts() const;

private:

};