//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"

#include "app.h"
#include "loader.h"
#include "light.h"
#include "material.h"
#include "node.h"
#include "swrast.h"
#include "bufrend.h"
#include "gltf_loader.h"
#include "color.h"
#include "program.h"
#include "def_rend.h"
#include "fwd_rend.h"
#include "cam_mgr.h"
#include "imgui.h"
#include "rect.h"
#include "line.h"
#include "math_test.h"
#include "cube.h"

#include "gui_matedit.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <set>

#define flase false
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1200
#define CAMERA_SPEED 0.5f
#define NUM_LIGHTS 10
#define PI 3.14159f

float randf(float max = 1.0f)
{
	return static_cast<float>(rand()) / static_cast <float> (static_cast<float>(RAND_MAX) / max);
}

float randf(float min, float max)
{
	return min + static_cast<float>(rand()) / ( static_cast<float>(static_cast<float>(RAND_MAX) / (max - min)));
}

namespace efiilj
{

	application::application()
	: window_(nullptr), time_(0), 
		mouse_x_(0), mouse_y_(0), mouse_down_x_(0), mouse_down_y_(0), mouse_norm_x_(0), mouse_norm_y_(0), 
		is_dragging_mouse_(false), is_mouse_captured_(true), is_software_renderer_(flase) { }

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

			//lock mouse to window
			glfwSetInputMode(window_->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			// set clear color
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			this->window_->SetUiRender([this]()
					{
						if (selected_node_ != nullptr)
						{
							auto mat = selected_node_->get_material();
							if (mat != nullptr)
							{
								ImGui::Begin("Node");
								selected_node_->draw_node_gui();
								ImGui::End();

								ImGui::Begin("Material");
								mat->draw_material_gui();
								ImGui::End();
							}
						}

						ImGui::Begin("Light");
						def_renderer_->draw_renderer_gui();
						ImGui::End();
					});

			return true;
		}
		return false;
	}

	void application::run()
	{

		MathTest();

		auto g_vs = std::make_shared<shader_resource>(GL_VERTEX_SHADER, "../res/shaders/dvs_geometry.glsl");
		auto g_fs = std::make_shared<shader_resource>(GL_FRAGMENT_SHADER, "../res/shaders/dfs_geometry.glsl");
		auto g_prog_ptr = std::make_shared<shader_program>(g_vs, g_fs);

		auto l_vs = std::make_shared<shader_resource>(GL_VERTEX_SHADER, "../res/shaders/dvs_lighting.glsl");
		auto l_fs = std::make_shared<shader_resource>(GL_FRAGMENT_SHADER, "../res/shaders/dfs_lighting.glsl");
		auto l_prog_ptr = std::make_shared<shader_program>(l_vs, l_fs);

		auto gltf_vs = std::make_shared<shader_resource>(GL_VERTEX_SHADER, "../res/shaders/vs_gltf.glsl");
		auto gltf_fs = std::make_shared<shader_resource>(GL_FRAGMENT_SHADER, "../res/shaders/fs_gltf.glsl");
		auto gltf_prog_ptr = std::make_shared<shader_program>(gltf_vs, gltf_fs);

		auto color_vs = std::make_shared<shader_resource>(GL_VERTEX_SHADER, "../res/shaders/vs_color.glsl");
		auto color_fs = std::make_shared<shader_resource>(GL_FRAGMENT_SHADER, "../res/shaders/fs_color.glsl");
		auto color_prog_ptr = std::make_shared<shader_program>(color_vs, color_fs);

		cam_mgr_ptr = std::make_shared<camera_manager>(WINDOW_WIDTH, WINDOW_HEIGHT);
		
		renderer_settings set;

		set.width = WINDOW_WIDTH;
		set.height = WINDOW_HEIGHT;

		def_renderer_ = std::make_shared<deferred_renderer>(cam_mgr_ptr, set, g_prog_ptr, l_prog_ptr);
		fwd_renderer_ = std::make_shared<forward_renderer>(cam_mgr_ptr, set);
		
		object_loader obj_sphere("../res/volumes/v_pointlight.obj");
		auto sphere_mesh_ptr = obj_sphere.get_resource();
		auto cube_mesh_ptr = std::make_shared<cube>();
		auto rect_mesh_ptr = std::make_shared<rect>();
	
		gltf_model_loader gltf_sponza("../res/gltf/Sponza/Sponza.gltf");
		gltf_model_loader gltf_helmet("../res/gltf/FlightHelmet/FlightHelmet.gltf");
		gltf_model_loader gltf_beater("../res/gltf/visp.gltf");

		auto sponza_trans_ptr = std::make_shared<transform_model>(vector3(0, 0, 0), vector3(0), vector3(0.05f, 0.05f, 0.05f));
		auto helmet_trans_ptr = std::make_shared<transform_model>(vector3(0, 10, 0), vector3(0), vector3(5.0f, 5.0f, 5.0f));
		auto beater_trans_ptr = std::make_shared<transform_model>(vector3(5, 5, 5), vector3(0, PI / 2, PI / 2), vector3(0.5f, 0.5f, 0.5f));

		auto sponza_scene_ptr = gltf_sponza.get_scene(g_prog_ptr, sponza_trans_ptr, "Sponza");
		auto helmet_scene_ptr = gltf_helmet.get_scene(g_prog_ptr, helmet_trans_ptr, "Helmet");
		auto beater_scene_ptr = gltf_beater.get_scene(g_prog_ptr, beater_trans_ptr, "Beater");

		def_renderer_->add_scene(sponza_scene_ptr);
		def_renderer_->add_scene(helmet_scene_ptr);
		def_renderer_->add_scene(beater_scene_ptr);

		std::vector<std::shared_ptr<light_source>> lights;
		std::vector<std::shared_ptr<transform_model>> light_transforms;

		auto sun_ptr = std::make_shared<light_source>(std::make_shared<transform_model>(vector3(0), vector3(PI / 2, PI / 2, 0)), light_type::directional);
		sun_ptr->set_base(vector3(1.0f, 1.0f, 1.0f), 0.01f, 0.01f);
		def_renderer_->add_light(sun_ptr);

		bool spotlight_on = false;
		auto spotlight_trf_ptr = std::make_shared<transform_model>(vector3(20.0f, 20.0f, 20.0f), vector3(PI / 2, PI / 2));
		//spotlight_trf_ptr->set_parent(helmet_trans_ptr);
		auto spotlight_ptr = std::make_shared<light_source>(spotlight_trf_ptr, light_type::spotlight);
		spotlight_ptr->set_base(vector3(1.0f, 1.0f, 1.0f), 0.5f, 1);
		spotlight_ptr->set_falloff(0, 0, 0.1f);
		spotlight_ptr->set_cutoff(0.91f, 0.82f);
		def_renderer_->add_light(spotlight_ptr);

		for (size_t i = 0; i < NUM_LIGHTS; i++)
		{
			auto light = std::make_shared<light_source>();

			light_base base;
			base.color = vector3(randf(1.0f), randf(1.0f), randf(1.0f));
			base.ambient_intensity = randf(0.5f);
			base.diffuse_intensity = randf(1.0f);
			light->set_base(base);

			attenuation falloff;
			falloff.constant = 0;
			falloff.linear = 0;
			falloff.exponential = 0.3f;
			light->set_falloff(falloff);

			lights.push_back(light);

			auto mat_ptr = std::make_shared<material_base>(color_prog_ptr);
			mat_ptr->color = vector4(base.color, 1.0f);

			float size = 0.5f - falloff.exponential;
			auto trf_ptr = std::make_shared<transform_model>(vector3(), vector3(), vector3(size, size, size));
			light_transforms.push_back(trf_ptr);

			auto node_ptr = std::make_shared<graphics_node>(sphere_mesh_ptr, mat_ptr, trf_ptr);

			def_renderer_->add_light(light);
			fwd_renderer_->add_node(node_ptr);
		}

		auto rect_mat_ptr = std::make_shared<material_base>(color_prog_ptr);
		rect_mat_ptr->color = vector4(randf(), randf(), randf(), 1.0f);
		rect_mat_ptr->double_sided = true;
		rect_mat_ptr->wireframe = true;

		auto hit_sphere_trf_ptr = std::make_shared<transform_model>(vector3(), vector3(), vector3(0.05f, 0.05f, 0.05f));
		auto hit_sphere_node_ptr = std::make_shared<graphics_node>(sphere_mesh_ptr, rect_mat_ptr, hit_sphere_trf_ptr);
		fwd_renderer_->add_node(hit_sphere_node_ptr);

		auto parent_test_trf = std::make_shared<transform_model>(vector3(0, 0, 1.0f));
		parent_test_trf->set_parent(helmet_trans_ptr);
		auto parent_test_node = std::make_shared<graphics_node>(sphere_mesh_ptr, rect_mat_ptr, parent_test_trf);
		fwd_renderer_->add_node(parent_test_node);

		std::set<int> keys;
		
		window_->SetKeyPressFunction([&](const int key, int, const int action, int)
		{
			if (action == 1)
			{
				keys.emplace(key);

				if (key == GLFW_KEY_TAB)
				{
					if (is_mouse_captured_)
						window_->SetCursorMode(GLFW_CURSOR_NORMAL);
					else
						window_->SetCursorMode(GLFW_CURSOR_DISABLED);

					is_mouse_captured_ = !is_mouse_captured_;
				}
				else if (key == GLFW_KEY_LEFT_CONTROL)
				{
					is_software_renderer_ = !is_software_renderer_;
				}
				else if (key == GLFW_KEY_R)
				{
					def_renderer_->reload_shaders();
					fwd_renderer_->reload_shaders();
				}
				else if (key == GLFW_KEY_B)
				{
					for (auto& node : def_renderer_->get_nodes())
					{
						bounds b = node->get_bounds();
						auto bbox_mesh = std::make_shared<cube>(b.min, b.max);
						auto bbox_node = std::make_shared<graphics_node>(bbox_mesh, rect_mat_ptr);
						fwd_renderer_->add_node(bbox_node);
					}
				}
				else if (key == GLFW_KEY_V)
				{
					for (auto node : beater_scene_ptr->nodes)
					{
						auto mesh = node->get_mesh()->get_mesh_data();

						for (auto v : mesh->positions)
						{
							auto vnode_trf = std::make_shared<transform_model>(beater_trans_ptr->get_model() * v, vector3(0), vector3(0.01f, 0.01, 0.01f));
							auto vnode = std::make_shared<graphics_node>(sphere_mesh_ptr, rect_mat_ptr, vnode_trf);
							fwd_renderer_->add_node(vnode);
						}
					}
				}
				else if (key == GLFW_KEY_F)
				{
					if (spotlight_on)
						spotlight_ptr->set_base(vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
					else
						spotlight_ptr->set_base(vector3(1.0f, 1.0f, 1.0f), 0.5f, 0.5f);

					spotlight_on = !spotlight_on;
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
			mouse_x_ = x;
			mouse_y_ = y;

			mouse_norm_x_ = x / WINDOW_WIDTH - 0.5f;
			mouse_norm_y_ = y / WINDOW_HEIGHT - 0.5f;

		});

		window_->SetMousePressFunction([&](const int button, const int action, int mods) 
		{
			if (action == 1)
			{
				mouse_down_x_ = mouse_norm_x_;
				mouse_down_y_ = mouse_norm_y_;

				is_dragging_mouse_ = true;

				auto camera_ptr = cam_mgr_ptr->get_active_camera();
				ray r = camera_ptr->get_ray_from_camera(mouse_x_, mouse_y_);
				float nearest = std::numeric_limits<float>::max();
				vector3 hit, norm;

				for (auto node : def_renderer_->get_nodes())
				{
					if (node->ray_intersect_triangle(r, hit, norm))
					{
						float len = (hit - r.origin).length();
						if (len < nearest)
						{
							hit_sphere_trf_ptr->set_position(hit);
							nearest = len;
							selected_node_ = node;
						}
					}
				}
			}
			else
			{
				is_dragging_mouse_ = false;
			}
		});

		while (this->window_->IsOpen())
		{
			auto camera_ptr = cam_mgr_ptr->get_active_camera();
			auto camera_trans_ptr = camera_ptr->get_transform(); 

			if (is_mouse_captured_)
				camera_trans_ptr->set_rotation(vector3(mouse_norm_y_ * 15, -mouse_norm_x_ * 15, 0));
			else if (is_dragging_mouse_)
				helmet_trans_ptr->add_rotation(vector3(mouse_norm_y_ - mouse_down_y_, mouse_norm_x_ - mouse_down_x_, 0) * 0.5f);
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			this->window_->Update();
	
			if (keys.find(GLFW_KEY_W) != keys.end())
				camera_trans_ptr->add_position(camera_trans_ptr->forward() * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_S) != keys.end())
				camera_trans_ptr->add_position(camera_trans_ptr->backward() * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_A) != keys.end())
				camera_trans_ptr->add_position(camera_trans_ptr->left() * -CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_D) != keys.end())
				camera_trans_ptr->add_position(camera_trans_ptr->right() * -CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_SPACE) != keys.end())
				camera_trans_ptr->add_position(camera_trans_ptr->up() * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_LEFT_SHIFT) != keys.end())
				camera_trans_ptr->add_position(camera_trans_ptr->down() * CAMERA_SPEED);

			if (keys.find(GLFW_KEY_UP) != keys.end())
				helmet_trans_ptr->add_position(helmet_trans_ptr->forward() * CAMERA_SPEED);

			if (keys.find(GLFW_KEY_DOWN) != keys.end())
				helmet_trans_ptr->add_position(helmet_trans_ptr->backward() * CAMERA_SPEED);

			if (keys.find(GLFW_KEY_LEFT) != keys.end())
				helmet_trans_ptr->add_position(helmet_trans_ptr->left() * CAMERA_SPEED);

			if (keys.find(GLFW_KEY_RIGHT) != keys.end())
				helmet_trans_ptr->add_position(helmet_trans_ptr->right() * CAMERA_SPEED);
			
			if (keys.find(GLFW_KEY_ESCAPE) != keys.end())
				window_->Close();

			if (keys.find(GLFW_KEY_LEFT_CONTROL) != keys.end())
			{
				def_renderer_->toggle_debug();
				fwd_renderer_->toggle_debug();
			}

			cam_mgr_ptr->update_camera();

			//spotlight_trf_ptr->set_position(camera_trans_ptr->get_position());

			float dt = def_renderer_->get_delta_time();
			float d = def_renderer_->get_frame_index() / 100.0f;
			
			//spotlight_trf_ptr->set_rotation(vector3(0, sinf(d), 0));

			for (size_t i = 0; i < NUM_LIGHTS; i++)
			{
				auto& light = lights[i];
				auto& trf = light_transforms[i];

				srand(i + 13);

				float x = sinf(randf(PI) + d) * randf(-25, 25);
				float y = cosf(randf(PI) + d) * randf(-25, 25);
				float z = sinf(randf(PI) + d) * randf(-25, 25);

				light->get_transform()->set_position(vector3(x, 25 + y, z));
				trf->set_position(light->get_transform()->get_position());
			}

			srand(def_renderer_->get_frame_index());

			def_renderer_->begin_frame();
			fwd_renderer_->begin_frame();

			def_renderer_->render();
			fwd_renderer_->render();

			def_renderer_->end_frame();
			fwd_renderer_->end_frame();

			this->window_->SwapBuffers();
			
		}
	}
}

#undef flase
