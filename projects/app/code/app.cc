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
#include "gltf_loader.h"
#include "program.h"
#include "def_rend.h"
#include "fwd_rend.h"
#include "cam_mgr.h"
#include "imgui.h"
#include "rect.h"
#include "line.h"
#include "math_test.h"
#include "cube.h"

#include "quat.h"

#include "gui_matedit.h"

#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/common.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/fwd.hpp"

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <set>

#define flase false
#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768
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
						if (_selected_node != nullptr)
						{
							auto mat = _selected_node->get_material();
							if (mat != nullptr)
							{
								ImGui::Begin("Node");
								_selected_node->draw_node_gui();
								ImGui::End();

								ImGui::Begin("Material");
								mat->draw_material_gui();
								ImGui::End();
							}
						}

						ImGui::Begin("Light");
						_def_renderer->draw_renderer_gui();
						ImGui::End();

						ImGui::Begin("Camera");
						_cam_mgr->get_active_camera()->get_transform()->draw_transform_gui();
						ImGui::End();
					});

			return true;
		}
		return false;
	}

	void application::run()
	{

		MathTest();

		glm::quat q1a = glm::angleAxis(0.25f, glm::vec3(1, 0, 0));
		glm::quat q1b = glm::angleAxis(-0.5f, glm::vec3(0, 1, 0));
		glm::quat q1c = q1a * q1b;

		quaternion q2a(vector3(1, 0, 0), 0.25f);
		quaternion q2b(vector3(0, 1, 0), -0.5f);
		quaternion q2c = q2a * q2b;

		printf("A: %s\nB: %s\n SqrA: %s\n SqrB: %s\n\n",
				glm::to_string(q1a).c_str(),
				q2a.xyzw.to_mem_string().c_str(),
				glm::to_string(q1c).c_str(),
				q2c.xyzw.to_mem_string().c_str());

		glm::mat4 mat1 = glm::mat4_cast(q1c);
		matrix4 mat2 = q2c.get_rotation_matrix();

		printf("%s\n%s\n\n",
				glm::to_string(mat1).c_str(),
				mat2.to_mem_string().c_str());

		glm::vec3 r1 = mat1[0];
		vector3 r2 = mat2.col(0).xyz();

		printf("GLM: %s (mag: %f)\nefiilj: %s (mag: %f)\n\n",
				glm::to_string(r1).c_str(),
				glm::length(r1),
				r2.to_mem_string().c_str(),
				r2.length());

		glm::quat qr1 = glm::angleAxis(0.25f, r1);
		quaternion qr2 = quaternion(r2, 0.25f);

		printf("GLM: %s (mag: %f)\nefiilj: %s (mag: %f)\n\n",
				glm::to_string(qr1).c_str(),
				glm::length(qr1),
				qr2.xyzw.to_mem_string().c_str(),
				qr2.magnitude());

		//return;

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

		_cam_mgr = std::make_shared<camera_manager>(WINDOW_WIDTH, WINDOW_HEIGHT);
		
		renderer_settings set;

		set.width = WINDOW_WIDTH;
		set.height = WINDOW_HEIGHT;

		_def_renderer = std::make_shared<deferred_renderer>(_cam_mgr, set, g_prog_ptr, l_prog_ptr);
		_fwd_renderer = std::make_shared<forward_renderer>(_cam_mgr, set);
		_simulator = std::make_shared<simulator>();
		
		object_loader obj_sphere("../res/volumes/v_pointlight.obj");
		auto sphere_mesh_ptr = obj_sphere.get_resource();
		auto cube_mesh_ptr = std::make_shared<cube>();
		auto rect_mesh_ptr = std::make_shared<rect>();
	
		gltf_model_loader gltf_helmet("../res/gltf/FlightHelmet/FlightHelmet.gltf");
		auto helmet_trans_ptr = std::make_shared<transform_model>(vector3(0, 10, 0), vector3(0), vector3(5.0f, 5.0f, 5.0f));
		auto helmet_scene_ptr = gltf_helmet.get_scene(g_prog_ptr, helmet_trans_ptr, "Helmet");
		_def_renderer->add_scene(helmet_scene_ptr);

		for (auto node : helmet_scene_ptr->nodes)
		{
			auto phys_node_ptr = std::make_shared<physics_node>(node);
			_simulator->add_rigidbody(phys_node_ptr);
		}

		std::vector<std::shared_ptr<light_source>> lights;
		std::vector<std::shared_ptr<transform_model>> light_transforms;

		auto sun_ptr = std::make_shared<light_source>(std::make_shared<transform_model>(vector3(0), vector3(PI / 2, PI / 2, 0)), light_type::directional);
		sun_ptr->set_base(vector3(1.0f, 1.0f, 1.0f), 0.01f, 0.01f);
		_def_renderer->add_light(sun_ptr);

		bool spotlight_on = false;
		auto spotlight_trf_ptr = std::make_shared<transform_model>(vector3(20.0f, 20.0f, 20.0f), vector3(PI / 2, PI / 2));
		//spotlight_trf_ptr->set_parent(helmet_trans_ptr);
		auto spotlight_ptr = std::make_shared<light_source>(spotlight_trf_ptr, light_type::spotlight);
		spotlight_ptr->set_base(vector3(1.0f, 1.0f, 1.0f), 0.5f, 1);
		spotlight_ptr->set_falloff(0, 0, 0.1f);
		spotlight_ptr->set_cutoff(0.91f, 0.82f);
		_def_renderer->add_light(spotlight_ptr);

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

			_def_renderer->add_light(light);
			_fwd_renderer->add_node(node_ptr);
		}

		auto rect_mat_ptr = std::make_shared<material_base>(color_prog_ptr);
		rect_mat_ptr->color = vector4(randf(), randf(), randf(), 1.0f);
		rect_mat_ptr->double_sided = true;
		rect_mat_ptr->wireframe = true;

		auto hit_sphere_trf_ptr = std::make_shared<transform_model>(vector3(), vector3(), vector3(0.05f, 0.05f, 0.05f));
		auto hit_sphere_node_ptr = std::make_shared<graphics_node>(sphere_mesh_ptr, rect_mat_ptr, hit_sphere_trf_ptr);
		_fwd_renderer->add_node(hit_sphere_node_ptr);

		auto parent_test_trf = std::make_shared<transform_model>(vector3(0, 0, 1.0f));
		parent_test_trf->set_parent(helmet_trans_ptr);
		auto parent_test_node = std::make_shared<graphics_node>(sphere_mesh_ptr, rect_mat_ptr, parent_test_trf);
		_fwd_renderer->add_node(parent_test_node);

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
					_def_renderer->reload_shaders();
					_fwd_renderer->reload_shaders();
				}
				else if (key == GLFW_KEY_B)
				{
					for (auto& body : _simulator->get_rigidbodies())
					{
						bounds b = body->get_bounds();
						auto bbox_mesh = std::make_shared<cube>(b.min, b.max);
						auto bbox_node = std::make_shared<graphics_node>(bbox_mesh, rect_mat_ptr);
						_fwd_renderer->add_node(bbox_node);
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

				auto camera_ptr = _cam_mgr->get_active_camera();
				ray r = camera_ptr->get_ray_from_camera(mouse_x_, mouse_y_);
				float nearest = std::numeric_limits<float>::max();
				vector3 hit, norm;

				for (auto body : _simulator->get_rigidbodies())
				{
					if (body->ray_intersect_triangle(r, hit, norm))
					{
						float len = (hit - r.origin).length();
						if (len < nearest)
						{
							hit_sphere_trf_ptr->set_position(hit);
							nearest = len;
							//_selected_node = node;
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
			auto camera_ptr = _cam_mgr->get_active_camera();
			auto camera_trans_ptr = camera_ptr->get_transform(); 
			
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
				camera_trans_ptr->add_rotation(camera_trans_ptr->right(), 0.1f);

			if (keys.find(GLFW_KEY_DOWN) != keys.end())
				camera_trans_ptr->add_rotation(camera_trans_ptr->right(), -0.1f);

			if (keys.find(GLFW_KEY_LEFT) != keys.end())
				camera_trans_ptr->add_rotation(camera_trans_ptr->up(), 0.1f);

			if (keys.find(GLFW_KEY_RIGHT) != keys.end())
				camera_trans_ptr->add_rotation(camera_trans_ptr->up(), -0.1f);

			if (keys.find(GLFW_KEY_PAGE_DOWN) != keys.end())
				camera_trans_ptr->add_rotation(camera_trans_ptr->forward(), 0.1);

			if (keys.find(GLFW_KEY_PAGE_UP) != keys.end())
				camera_trans_ptr->add_rotation(camera_trans_ptr->forward(), -0.1);
			
			if (keys.find(GLFW_KEY_ESCAPE) != keys.end())
				window_->Close();

			if (keys.find(GLFW_KEY_LEFT_CONTROL) != keys.end())
			{
				_def_renderer->toggle_debug();
				_fwd_renderer->toggle_debug();
			}

			_cam_mgr->update_camera();

			float dt = _def_renderer->get_delta_time();
			float d = _def_renderer->get_frame_index() / 100.0f;

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

			srand(_def_renderer->get_frame_index());

			_def_renderer->begin_frame();
			_fwd_renderer->begin_frame();

			_def_renderer->render();
			_fwd_renderer->render();

			_def_renderer->end_frame();
			_fwd_renderer->end_frame();

			this->window_->SwapBuffers();
			
		}
	}
}

#undef flase
