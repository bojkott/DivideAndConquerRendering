#pragma once


#include <glm\glm.hpp>
#include <vector>
class Material;
class ModelHelper
{
public:
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 norm;

		bool operator==(const Vertex& other) const {
			return pos == other.pos && texCoord == other.texCoord && norm == other.norm;
		}
	};

	struct MeshInfo
	{
		Material* material;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	static std::vector<MeshInfo> loadModelFromFile(const std::string& filePath);
};

//namespace std {
//	template<> struct hash<ModelHelper::Vertex> {
//		size_t operator()(ModelHelper::Vertex const& vertex) const {
//			return ((hash<glm::vec3>()(vertex.pos) ^
//				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
//				(hash<glm::vec2>()(vertex.texCoord) << 1);
//		}
//	};
//}