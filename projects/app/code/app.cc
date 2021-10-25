//------------------------------------------------------------------------------
// exampleapp.cc // (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <filesystem>

#include "imgui.h"

#include "app.h"
#include "loader.h"
#include "gltf_loader.h"
#include "quat.h"

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768
#define CAMERA_SPEED 0.05f
#define PI 3.14159f

namespace fs = std::filesystem;


namespace efiilj
{

	application::application()
	: window_(nullptr), time_(0), 
		mouse_x(0), mouse_y(0), mouse_down_x(0), mouse_down_y(0), mouse_norm_x(0), mouse_norm_y(0), 
		is_dragging_mouse(false), is_mouse_captured(true) { }

	application::~application() = default;

	bool application::open()
	{
		app::open();

		this->window_ = new Display::Window(WINDOW_WIDTH, WINDOW_HEIGHT);

		if (this->window_->Open())
		{

			//enable face culling
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			//lock mouse to window
			glfwSetInputMode(window_->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			// set clear color
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			this->window_->SetUiRender([this]()
					{
						ImGui::Begin("Managers");
						editor->show_entity_gui();
						ImGui::End();
					});

			return true;
		}
		return false;
	}

	void application::run()
	{

		renderer_settings set;
		set.width = WINDOW_WIDTH;
		set.height = WINDOW_HEIGHT;
		set.default_fallback_path = "../res/shaders/default_color.sdr";
		set.default_fallback_path_primary = "../res/shaders/default_primary.sdr";
		set.default_fallback_path_secondary = "../res/shaders/default_secondary.sdr";

		managers = std::make_shared<manager_host>();

		entities = std::make_shared<entity_manager>();

		shaders = std::make_shared<shader_server>();
		textures = std::make_shared<texture_server>();
		meshes = std::make_shared<mesh_server>();
		materials = std::make_shared<material_server>();

		metadata = std::make_shared<meta_manager>();
		transforms = std::make_shared<transform_manager>();
		cameras = std::make_shared<camera_manager>();
		lights = std::make_shared<light_manager>();
		graphics = std::make_shared<graphics_manager>();
		material_instances = std::make_shared<material_manager>();
		mesh_instances = std::make_shared<mesh_manager>();

		rdef = std::make_shared<deferred_renderer>(set);
		rfwd = std::make_shared<forward_renderer>(set);
		rdbg = std::make_shared<debug_renderer>();
		colliders = std::make_shared<collider_manager>();
		sim = std::make_shared<simulator>();
		gltf = std::make_shared<gltf_model_server>();

		managers->register_manager(entities, 'ENTS');
		managers->register_manager(transforms, 'TRFM');
		managers->register_manager(metadata, 'META');

		managers->register_manager(shaders, 'SHDR');
		managers->register_manager(textures, 'TXSR');
		managers->register_manager(meshes, 'MESR');
		managers->register_manager(materials, 'MASR');

		managers->register_manager(cameras, 'CAMS');
		managers->register_manager(lights, 'LGHT');
		managers->register_manager(graphics, 'GRFX');
		managers->register_manager(mesh_instances, 'MEMR');
		managers->register_manager(material_instances, 'MAMR');

		managers->register_manager(rfwd, 'RFWD');
		managers->register_manager(rdef, 'RDEF');
		managers->register_manager(colliders, 'RAYS');
		managers->register_manager(sim, 'PHYS');
		managers->register_manager(gltf, 'GLTF');

		managers->register_manager(rdbg, 'RDBG');

		editor = std::make_shared<entity_editor>(entities, managers);

		// Camera entity

		entity_id cam_ent = entities->create();
		transform_id cam_trf_id = transforms->register_entity(cam_ent);
		camera_id cam_cam_id = cameras->register_entity(cam_ent);
		meta_id cam_meta_id = metadata->register_entity(cam_ent);

		cameras->set_transform(cam_cam_id, cam_trf_id);
		cameras->set_size(cam_cam_id, WINDOW_WIDTH, WINDOW_HEIGHT);
		cameras->set_fov(cam_cam_id, 1.2f);
		cameras->set_camera(cam_cam_id);

		metadata->set_name(cam_meta_id, "Main camera");
		metadata->set_description(cam_meta_id, "This is the main camera of the application.");

		// Ground
	
		object_loader cube("../res/meshes/cube.obj", meshes);
		mesh_id mesh_cube = cube.get_mesh();

		entity_id e_cube = entities->create();
		transform_id trf_cube = transforms->register_entity(e_cube);
		transforms->set_position(trf_cube, vector3(0, -15.0f, 0));
		transforms->set_scale(trf_cube, vector3(10.0f, 10.0f, 10.0f));

		mesh_instance_id miid_cube = mesh_instances->register_entity(e_cube);
		mesh_instances->set_mesh(miid_cube, mesh_cube);

		material_id mtrl_cube = materials->create();
		materials->set_base_color(mtrl_cube, vector4(1, 1, 1, 1));
		meshes->set_material(mesh_cube, mtrl_cube);
		mesh_instances->set_material(miid_cube, mtrl_cube);

		rfwd->register_entity(e_cube);
		colliders->register_entity(e_cube);
		physics_id rb_cube = sim->register_entity(e_cube);

		sim->recalculate_com(rb_cube);
		sim->set_static(rb_cube, true);

		// Testcube

		object_loader testcube("../res/meshes/cube.obj", meshes);
		mesh_id mesh_testcube = testcube.get_mesh();

		entity_id e_testcube = entities->create();
		transform_id trf_testcube = transforms->register_entity(e_testcube);
		transforms->set_position(trf_testcube, vector3(0, 0, 0));
		transforms->set_scale(trf_testcube, 0.5f);

		mesh_instance_id miid_testcube = mesh_instances->register_entity(e_testcube);
		mesh_instances->set_mesh(miid_testcube, mesh_testcube);

		material_id mtrl_testcube = materials->create();
		materials->set_base_color(mtrl_testcube, vector4(1, 0, 0, 1));
		meshes->set_material(mesh_testcube, mtrl_testcube);
		mesh_instances->set_material(miid_testcube, mtrl_testcube);

		rfwd->register_entity(e_testcube);
		colliders->register_entity(e_testcube);
		physics_id rb_testcube = sim->register_entity(e_testcube);

		sim->recalculate_com(rb_testcube);

		// GLTF
		
		model_id test_mdl = gltf->create();
		model_id test_mdl2 = gltf->create();

		gltf->set_uri(test_mdl, "../res/gltf/Avocado/Avocado.gltf");
		gltf->set_uri(test_mdl2, "../res/gltf/Avocado/Avocado.gltf");

		gltf->load(test_mdl);
		gltf->load(test_mdl2);

		gltf->get_nodes(test_mdl);
		gltf->get_nodes(test_mdl2);

		auto node = gltf->get_scene(test_mdl).nodes[0];
		auto node2 = gltf->get_scene(test_mdl2).nodes[0];

		transform_id test_trf = transforms->get_component(node);
		transform_id test_trf2 = transforms->get_component(node2);

		transforms->set_scale(test_trf, 3.0f);
		transforms->set_scale(test_trf2, 1.0f);

		transforms->set_position(test_trf, vector3(-2.0f, 0, 0));
		transforms->set_position(test_trf2, vector3(2.0f, 0, 0));

		graphics_id test_gfx = rdef->register_entity(node);
		graphics_id test_gfx2 = rdef->register_entity(node2);

		collider_id test_col = colliders->register_entity(node);
		collider_id test_col2 = colliders->register_entity(node2);

		physics_id test_rb = sim->register_entity(node);
		physics_id test_rb2 = sim->register_entity(node2);

		sim->recalculate_com(test_rb);
		sim->recalculate_com(test_rb2);

		gltf->unload(test_mdl);
		gltf->unload(test_mdl2);

		// Debug drawing
		rdbg->set_shader(rfwd->get_fallback_shader());

		// Hitmarker
		object_loader obj("../res/volumes/v_pointlight.obj", meshes);

		entity_id e_hitmarker = entities->create();

		transform_id trf_hitmarker = transforms->register_entity(e_hitmarker);
		transforms->set_scale(trf_hitmarker, 0.01f);

		if (obj.is_valid())
		{

			mesh_id mesh_hitmarker = obj.get_mesh();
			mesh_instance_id miid_hitmarker = mesh_instances->register_entity(e_hitmarker);
			mesh_instances->set_mesh(miid_hitmarker, mesh_hitmarker);

			material_id mtrl_hitmarker = materials->create();
			materials->set_base_color(mtrl_hitmarker, vector4(1, 0, 0, 1));
			meshes->set_material(mesh_hitmarker, mtrl_hitmarker);
			mesh_instances->set_material(miid_hitmarker, mtrl_hitmarker);

			rfwd->register_entity(e_hitmarker);
		}

		// Lights
		
		entity_id e_sun = entities->create();
		light_id sun_light = lights->register_entity(e_sun);
		lights->set_base(sun_light, vector3(1.0f, 1.0f, 1.0f), 0.5f, 0.5f);

		transform_id sun_trf = transforms->register_entity(e_sun);
		transforms->add_rotation(sun_trf, vector3(1, 1, 0).norm(), PI / 2.0f);
		lights->set_transform(sun_light, sun_trf);

		std::set<int> keys;
		
		window_->SetKeyPressFunction([&](const int key, int, const int action, int)
		{
			if (action == 1)
			{
				keys.emplace(key);

				if (key == GLFW_KEY_TAB)
				{
					is_mouse_captured = !is_mouse_captured;

					//if (is_mouse_captured_)
					//	window_->SetCursorMode(GLFW_CURSOR_DISABLED);
					//else
					//	window_->SetCursorMode(GLFW_CURSOR_NORMAL);

				}
			}
			else if (action == 0)
			{
				const auto it = keys.find(key);
				if (it != keys.end())
					keys.erase(it);
			}
		});

			window_->SetMouseMoveFunction([&](const double x, const double y)
		{
			mouse_x = x;
			mouse_y = y;

			mouse_norm_x = x / WINDOW_WIDTH - 0.5f;
			mouse_norm_y = y / WINDOW_HEIGHT - 0.5f;

		});

		window_->SetMousePressFunction([&](const int button, const int action, int mods)  //NOLINT
		{
			if (action == 1)
			{
				mouse_down_x = mouse_norm_x;
				mouse_down_y = mouse_norm_y;

				is_dragging_mouse = true;

				camera_id active = cameras->get_camera();
				ray r = cameras->get_ray_from_camera(active, mouse_x, mouse_y);
				trace_hit hit;

				if (colliders->test_hit(r, hit))
				{
					if (button == GLFW_MOUSE_BUTTON_RIGHT)
					{
						transforms->set_position(trf_hitmarker, hit.position);
						editor->set_selected(hit.entity);
					}
					else if (button == GLFW_MOUSE_BUTTON_LEFT)
					{
						PointForce impulse;
						impulse.force = r.direction * 0.1f;
						impulse.p = hit.position;

						physics_id idp = sim->get_component(hit.entity);

						if (sim->is_valid(idp))
							sim->add_impulse(idp, impulse);
					}
				}
			}
			else
			{
				is_dragging_mouse = false;
			}
		});

		while (this->window_->IsOpen())
		{

			//transform_id selected_trf = is_mouse_captured ? cam_trf_id : e1_trf;
			transform_id selected_trf = cam_trf_id;

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			this->window_->Update();
	
			if (keys.find(GLFW_KEY_W) != keys.end())
				transforms->add_position(selected_trf, transforms->get_forward(selected_trf) * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_S) != keys.end())
				transforms->add_position(selected_trf, transforms->get_backward(selected_trf) * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_A) != keys.end())
				transforms->add_position(selected_trf, transforms->get_left(selected_trf) * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_D) != keys.end())
				transforms->add_position(selected_trf, transforms->get_right(selected_trf) * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_SPACE) != keys.end())
				transforms->add_position(selected_trf, transforms->get_up(selected_trf) * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_LEFT_SHIFT) != keys.end())
				transforms->add_position(selected_trf, transforms->get_down(selected_trf) * CAMERA_SPEED);

			if (keys.find(GLFW_KEY_UP) != keys.end())
				transforms->add_rotation(selected_trf, transforms->get_right(selected_trf), -0.1f);

			if (keys.find(GLFW_KEY_DOWN) != keys.end())
				transforms->add_rotation(selected_trf, transforms->get_right(selected_trf), 0.1f);

			if (keys.find(GLFW_KEY_RIGHT) != keys.end())
				transforms->add_rotation(selected_trf, transforms->get_up(selected_trf), -0.1f);

			if (keys.find(GLFW_KEY_LEFT) != keys.end())
				transforms->add_rotation(selected_trf, transforms->get_up(selected_trf), 0.1f);

			if (keys.find(GLFW_KEY_PAGE_UP) != keys.end())
				transforms->add_rotation(selected_trf, transforms->get_forward(selected_trf), -0.1f);

			if (keys.find(GLFW_KEY_PAGE_DOWN) != keys.end())
				transforms->add_rotation(selected_trf, transforms->get_forward(selected_trf), 0.1f);
			
			if (keys.find(GLFW_KEY_ESCAPE) != keys.end())
				window_->Close();

			cameras->update();
			colliders->update();

			sim->begin_frame();
			rdef->begin_frame();
		    rfwd->begin_frame();
			rdbg->begin_frame();

			sim->simulate();
			rdef->render_frame();
		    rfwd->render_frame();
			rdbg->render_frame();

			sim->end_frame();
			rdef->end_frame();
			rfwd->end_frame();
			rdbg->end_frame();

			this->window_->SwapBuffers();
			
		}
	}
}
