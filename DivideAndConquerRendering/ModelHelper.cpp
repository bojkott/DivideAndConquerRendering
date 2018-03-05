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
	std::vector<Material*> mats(materials.size());
	for (int i = 0; i < mats.size(); i++)
	{
		mats[i] = new ForwardMaterial(materials[i]);
	}
	Material* blankMat = new ForwardMaterial(tinyobj::material_t());

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
			mesh.material = mats[matIndex];
		}
		else
		{
			mesh.material = blankMat;
		}
		
		meshes.push_back(mesh);

	}

	return meshes;
}