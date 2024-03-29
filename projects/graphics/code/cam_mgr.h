#pragma once

#include "entity.h"
#include "trfm_mgr.h"
#include "ifmgr.h"
#include "ray.h"
#include "mgrdata.h"

#include <vector>
#include <memory>

namespace efiilj
{
	typedef int camera_id;

	class camera_manager : public manager<camera_id>
	{
	private:

		unsigned _ubo;
		camera_id _current;

		struct CameraData
		{
			ComponentData<transform_id> transform { -1 };

			ComponentData<float> width 	{ 800 };
			ComponentData<float> height { 600 };
			ComponentData<float> fov 	{ 1.2f };
			ComponentData<float> near 	{ 0.1f };
			ComponentData<float> far 	{ 100.0f };

			ComponentData<matrix4> perspective;
			ComponentData<matrix4> p_inverse;
			ComponentData<matrix4> view;
		} _data;
		
		std::shared_ptr<transform_manager> _transforms;

		void setup_ubo();

		void update_perspective(camera_id idx);
		void update_view(camera_id idx);

		void push_perspective();
		void push_view();

	public:

		camera_manager();
		~camera_manager() = default;

		void on_editor_gui() override;
		void on_editor_gui(camera_id idx) override;
		
		void on_register(std::shared_ptr<manager_host> host) override;
		void on_setup() override;

		void on_activate(camera_id) override;

		void on_begin_frame() override;

		ray get_ray_from_camera(camera_id idx, int mouse_x, int mouse_y) const;

		camera_id get_camera() const;
		bool set_camera(camera_id active);

		float get_width(camera_id idx) const;
		void set_width(camera_id idx, float width);

		float get_height(camera_id idx) const;
		void set_height(camera_id idx, float height);

		void set_size(camera_id idx, float width, float height);

		float get_fov(camera_id idx) const;
		void set_fov(camera_id idx, float fov);

		float get_near(camera_id idx) const;
		void set_near(camera_id idx, float near);

		float get_far(camera_id idx) const;
		void set_far(camera_id idx, float fov);

		const matrix4& get_perspective(camera_id idx) const;

		const matrix4& get_view(camera_id idx) const;
		transform_id get_transform(camera_id idx) const;
		void set_transform(camera_id idx, transform_id trf);

		vector3 get_position(camera_id idx = -1)
		{
			return _transforms->get_position(_data.transform[idx == -1 ? _current : idx]);
		}
	};
}
