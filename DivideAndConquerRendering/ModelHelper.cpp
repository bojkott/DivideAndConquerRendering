#include "ModelHelper.h"
#include "Material.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

std::vector<ModelHelper::MeshInfo> ModelHelper::loadModelFromFile(const std::string & filepath)
{
	std::vector<MeshInfo> meshes;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str())) {
		throw std::runtime_error(err);
	}

	for (int i = 0; i < shapes.size(); i++)
	{
		MeshInfo mesh;
		for (const auto& index : shapes[i].mesh.indices) {
			Vertex vertex = {};
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			vertex.norm = {
				attrib.normals[3 * index.vertex_index + 0],
				attrib.normals[3 * index.vertex_index + 1],
				attrib.normals[3 * index.vertex_index + 2],
			};

			mesh.vertices.push_back(vertex);
		}

		//Material* mat = new ForwardMaterial(materials[i]);
		//mesh.material = mat;
		meshes.push_back(mesh);
	}
	return meshes;
}
