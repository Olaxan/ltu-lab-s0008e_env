#include "gltf_loader.h"

#define TINYGLTF_IMPLEMENTATION

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include "tiny_gltf.h"
#include "node.h"

#include <stdio.h>
#include <vector>
#include <GL/glew.h>

namespace efiilj
{
	gltf_model_loader::gltf_model_loader()
	{

	}

	gltf_model_loader::gltf_model_loader(std::string path, bool is_binary)
	{
		tinygltf::Model model;
		if (load_from_file(model, path, is_binary))
		{
			printf("GLTF file successfully loaded\n");
			get_meshes(model);
		}
		else
		{
			printf("Failed to load GLTF\n");
		}
	}

	bool gltf_model_loader::load_from_file(tinygltf::Model& model, std::string path, bool is_binary)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		bool ret;
		
		if (is_binary)
			ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
		else
			ret = loader.LoadASCIIFromFile(&model, &err, &warn, path); // for binary glTF(.glb)

		if (!warn.empty()) {
			printf("Warn: %s\n", warn.c_str());
		}

		if (!err.empty()) {
			printf("Err: %s\n", err.c_str());
		}

		return ret;	
	
	}

	mesh_resource gltf_model_loader::build_mesh(tinygltf::Model& model, tinygltf::Mesh& mesh)
	{
		printf("Constructing mesh %s\n", mesh.name.c_str());
		
		mesh_resource m_res = mesh_resource();

		tinygltf::Primitive prim = mesh.primitives[0];

		for (auto &attrib : prim.attributes)
		{
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			tinygltf::BufferView view = model.bufferViews[accessor.bufferView];

			int stride = accessor.ByteStride(view);
			printf("Attribute: %s, accessor %d, stride %d, view %d (%s)\n", 
					attrib.first.c_str(), attrib.second, stride, accessor.bufferView, view.name.c_str());

			if (attrib.first.compare("POSITION") == 0)
			{
				//m_res.buffer(GL_ARRAY_BUFFER);
			}
		}
		
	//	std::cout << "buffer.data.size = " << buf.data.size()
      	//	<< ", bufferview.byteOffset = " << view.byteOffset
      	//	<< std::endl;

	//	m_res.buffer(view.target, view.byteLength, &buf.data.at(0) + view.byteOffset, GL_STATIC_DRAW);

	//	for (int i = 0; i < mesh.primitives.size(); i++)
	//	{
	//		tinygltf::Primitive prim = mesh.primitives[i];
	//		tinygltf::Accessor i_a = model.accessors[prim.indices];

	//		for (auto &attrib : prim.attributes)
	//		{
	//		
	//		}
	//	}
	}

	void gltf_model_loader::parse_node(tinygltf::Model& model, tinygltf::Node& node)
	{
		if (node.mesh >= 0 && node.mesh < model.meshes.size())
		{
			build_mesh(model, model.meshes[node.mesh]);
		}

		for (size_t i = 0; i < node.children.size(); i++) 
		{
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			parse_node(model, model.nodes[node.children[i]]);
		}
	}

	unsigned gltf_model_loader::get_meshes(tinygltf::Model& model)
	{
		const tinygltf::Scene scene = model.scenes[model.defaultScene];

		printf("Parsing GLTF file, %lu nodes in default scene %d\n", scene.nodes.size(), model.defaultScene);

		for (int i = 0; i < scene.nodes.size(); i++)
		{
			tinygltf::Node& node = model.nodes[scene.nodes[i]];
			parse_node(model, node);
		}

		return 0;
	}

	gltf_model_loader::~gltf_model_loader()
	{
		printf("GLTF loader exited");
	}
}

