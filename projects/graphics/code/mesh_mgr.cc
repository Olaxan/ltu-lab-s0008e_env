#include "mesh_mgr.h"
#include "imgui.h"

namespace efiilj
{

	mesh_manager::mesh_manager()
	{
		printf("Init mesh instances...\n");
		_name = "Mesh";
	}

	mesh_manager::~mesh_manager()
	{
		printf("Mesh instance manager exit\n");
	}

	void mesh_manager::extend_defaults(mesh_instance_id)
	{
		_data.mesh.emplace_back(-1);
		_data.material.emplace_back(-1);
	}

	void mesh_manager::draw_gui()
	{

	}

	void mesh_manager::draw_gui(mesh_instance_id idx)
	{

		if (!is_valid(idx))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "No mesh selected");
			return;
		}

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh instance %d", idx);

		if (ImGui::TreeNode("Mesh data"))
		{
			ImGui::BulletText("%ld vertices", _meshes->get_vertex_count(idx));
			ImGui::BulletText("%ld indices", _meshes->get_index_count(idx));
			ImGui::BulletText("Material: %d", _data.material[idx]);
			ImGui::BulletText("Loaded: %s", _meshes->get_state(idx) ? "true" : "false");
			ImGui::BulletText("VAO: %d, VBO: %d, IBO: %d", 
					_meshes->get_vao(idx),
					_meshes->get_vbo(idx),
					_meshes->get_ibo(idx));

			ImGui::TreePop();
		}
	}

	void mesh_manager::on_register(std::shared_ptr<manager_host> host)
	{
		_meshes = host->get_manager_from_fcc<mesh_server>('MESR');	
		_materials = host->get_manager_from_fcc<material_server>('MASR');
	}
}