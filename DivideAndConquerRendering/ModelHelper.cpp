#include "ModelHelper.h"


#define TINYOBJLOADER_IMPLEMENTATION

#include "materials\ForwardMaterial.h"

#define MTL_BASEDIR std::string("../assets/models/")

std::vector<ModelHelper::MeshInfo> ModelHelper::loadModelFromFile(const std::string & filepath)
{
	std::vector<MeshInfo> meshes;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	std::string fullpath = MTL_BASEDIR + filepath;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fullpath.c_str(), MTL_BASEDIR.c_str())) {
		throw std::runtime_error(err);
	}


	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		MeshInfo mesh;
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {

				Vertex vertex;
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				vertex.pos = {
					attrib.vertices[3 * idx.vertex_index + 0],
					attrib.vertices[3 * idx.vertex_index + 1],
					attrib.vertices[3 * idx.vertex_index + 2]
				};

				vertex.norm = {
					attrib.normals[3 * idx.normal_index + 0],
					attrib.normals[3 * idx.normal_index + 1],
					attrib.normals[3 * idx.normal_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * idx.texcoord_index + 0],
					attrib.texcoords[2 * idx.texcoord_index + 1]
				};
				mesh.vertices.push_back(vertex);
			}
			index_offset += fv;
		}
		int matIndex = shapes[s].mesh.material_ids[0];
		if (matIndex != -1)
		{
			mesh.material = new ForwardMaterial(materials[matIndex]);
		}
		
		meshes.push_back(mesh);

	}



	//for (int i = 0; i < shapes.size(); i++)
	//{
	//	MeshInfo mesh;

	//	for (int j = 0; j < shapes[i].mesh.indices.size(); j += 3)
	//	{
	//		Vertex vertex;
	//		vertex.pos = {
	//			attrib.vertices[shapes[i].mesh.indices[j].vertex_index],
	//			attrib.vertices[shapes[i].mesh.indices[j+1].vertex_index],
	//			attrib.vertices[shapes[i].mesh.indices[j+2].vertex_index],
	//		};

	//		vertex.texCoord = {
	//			attrib.texcoords[2 * index.texcoord_index + 0],
	//			1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
	//		};

	//		vertex.color = { 1.0f, 1.0f, 1.0f };

	//		vertex.norm = {
	//			attrib.normals[3 * index.normal_index + 0],
	//			attrib.normals[3 * index.normal_index + 1],
	//			attrib.normals[3 * index.vertex_index + 2],
	//		};

	//		mesh.vertices.push_back(vertex);
	//	}

	//	//Material* mat = new ForwardMaterial(materials[i]);
	//	//mesh.material = mat;
	//	//meshes.push_back(mesh);
	//}
	return meshes;
}