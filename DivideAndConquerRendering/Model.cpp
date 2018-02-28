#include "Model.h"
#include <glm\glm.hpp>

Model::Model(DeviceContext* context, std::vector<Vertex>& vertices, std::vector<uint32_t> indices)
{
	// Data has been loaded, time to create buffers
	vertexBuffer = VertexBuffer(vertices, context);
	indexBuffer = IndexBuffer(indices, context);
}
