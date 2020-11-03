#pragma once

#include "program.h"
#include "cam_mgr.h"
#include "rend_set.h"
#include "node.h"
#include "light.h"

#include <vector>
#include <chrono>
#include <string>

namespace efiilj
{
	struct buffer_data
	{
		unsigned id;
		unsigned type;
		unsigned slot;
	};	

	class deferred_renderer
	{
	private:

		typedef std::chrono::duration<float> duration;
		typedef std::chrono::high_resolution_clock frame_timer; 
		typedef std::chrono::time_point<frame_timer> frame_timer_point;

		unsigned gbo_, rbo_, ubo_, quad_vao_, quad_vbo_;

		frame_timer_point last_frame_;
		renderer_settings settings_;

		std::shared_ptr<camera_manager> camera_mgr_;
		std::shared_ptr<shader_program> geometry_;
		std::shared_ptr<shader_program> lighting_;

		mesh_resource v_pointlight_;
		mesh_resource v_spotlight_;

		light_source directional_light_;

		std::vector<light_source> light_sources_;
		std::vector<std::shared_ptr<graphics_node>> nodes_;
		std::vector<unsigned> buffers_;

		void gen_buffer(unsigned type);
		void attach_buffers();

		void setup_quad();
		void setup_uniforms();
		
		void set_light_uniforms(const light_source& light);

		float get_attenuation_radius(const light_source& light);

	public:
		deferred_renderer
			(
				std::shared_ptr<camera_manager> camera_manager,	
				const renderer_settings& settings,
				std::shared_ptr<shader_program> geometry, 
				std::shared_ptr<shader_program> lighting
			);

		~deferred_renderer() = default;

		void add_nodes(const std::vector<std::shared_ptr<graphics_node>>& nodes);

		void render();
		void reload_shaders();

	};
}
