#include "Mesh.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ModelHelper.h"

Mesh::Mesh(DeviceContext* context, ModelHelper::MeshInfo& meshInfo)
{
	// Data has been loaded, time to create buffers
	vertexBuffer = new VertexBuffer(meshInfo.vertices, context);
	indexBuffer = new IndexBuffer(meshInfo.indices, context);
	material = meshInfo.material;
}
