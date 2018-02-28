#pragma once
#ifndef MODEL_H
#define MODEL_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>
#include <vulkan\vulkan.hpp>

#include "Common.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"



class Model
{
private: 
	// Self destroying objects
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;

public:
	Model(DeviceContext* context, std::vector<Vertex>& vertices, std::vector<uint32_t> indices);
	Model() {};



private:


};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}
#endif
