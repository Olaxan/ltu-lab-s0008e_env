#include "node.h"

#include <memory>
#include <utility>
#include <limits>
#include <algorithm>

#include <imgui.h>

namespace efiilj
{

	graphics_node::graphics_node(
		std::shared_ptr<mesh_resource> mesh_ptr,
		std::shared_ptr<material_base> material_ptr,
		std::shared_ptr<transform_model> transform_ptr)
			: mesh_(std::move(mesh_ptr)), material_(std::move(material_ptr)), transform_(std::move(transform_ptr)),
			is_absolute_(false), name("Object")
	{}

	graphics_node::graphics_node(
			std::shared_ptr<mesh_resource> mesh_ptr,
			std::shared_ptr<material_base> material_ptr)
		: graphics_node(mesh_ptr, material_ptr, std::make_shared<transform_model>())
	{}

	void graphics_node::bind() const
	{
		mesh_->bind();

		if (material_ != nullptr)
			material_->apply();
	}

	void graphics_node::unbind() const
	{
		mesh_->unbind();
	}

	void graphics_node::draw() const
	{
		matrix4 model = is_absolute_ ? matrix4() : transform_->get_model();
		material_->get_program()->set_uniform("model", model);
		mesh_->draw_elements();
	}

	bounds graphics_node::get_bounds() const
	{
		return mesh_->get_bounds(transform_->get_model());
	}

	bool graphics_node::point_inside_bounds(const vector3& point) const
	{
		bounds b = get_bounds();

		const float e = 0.0001f;

		return point.x > (b.min.x - e)
			&& point.x < (b.max.x + e)
			&& point.y > (b.min.y - e)
			&& point.y < (b.max.y + e)
			&& point.z > (b.min.z - e)
			&& point.z < (b.max.z + e);
	}

	bool graphics_node::ray_intersect_bounds(const ray& test, vector3& hit) const
	{
		float tmax = 100000000.0f;
		float tmin = -tmax;

		bounds bounds = get_bounds();

		const vector3 dir = vector3(
            1.0f / ((std::fabs(test.direction.x) < 1e-6) ? 0.0001f : test.direction.x),
            1.0f / ((std::fabs(test.direction.y) < 1e-6) ? 0.0001f : test.direction.y),
            1.0f / ((std::fabs(test.direction.z) < 1e-6) ? 0.0001f : test.direction.z)
        );

        const float tx1 = (bounds.min.x - test.origin.x) * dir.x;
        const float tx2 = (bounds.max.x - test.origin.x) * dir.x;

        tmin = std::max(tmin, std::min(tx1, tx2));
        tmax = std::min(tmax, std::max(tx1, tx2));
        
        const float ty1 = (bounds.min.y - test.origin.y) * dir.y;
        const float ty2 = (bounds.max.y - test.origin.y) * dir.y;

        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
       
        const float tz1 = (bounds.min.z - test.origin.z) * dir.z;
        const float tz2 = (bounds.max.z - test.origin.z) * dir.z;

        tmin = std::max(tmin, std::min(tz1, tz2));
        tmax = std::min(tmax, std::max(tz1, tz2));
        
        if (tmin > 0.0f && tmax >= tmin) 
		{
			hit = test.origin + test.direction * tmin;
            return true;
        }

        return false;
	}

	bool graphics_node::ray_intersect_triangle(const ray& test, vector3& hit, vector3& norm) const
	{

		if (!ray_intersect_bounds(test, hit))
			return false;

		if (!mesh_->has_mesh_data())
			return false;

		bool is_hit = false;
		float nearest = std::numeric_limits<float>::max();
		vector3 nearest_hit;

		vector3 ra = transform_->get_model_inv() * test.origin;
		vector3 rb = transform_->get_model_inv() * (test.origin + test.direction * 1000.0f);

		ray r = ray(ra, (rb - ra).norm());

		const vector3 d = r.direction;
		const vector3 o = r.origin;

		auto data = mesh_->get_mesh_data();

		for (size_t i = 0; i < data->indices.size();)
		{
			const vector3& a = data->positions[data->indices[i++]];
			const vector3& b = data->positions[data->indices[i++]];
			const vector3& c = data->positions[data->indices[i++]];

			const vector3 e1 = b - a;
			const vector3 e2 = c - a;
			const vector3 n = vector3::cross(e1, e2);

			const float det = -vector3::dot(d, n);
			const float invdet = 1.0f / det;

			const vector3 ao = o - a;
			const vector3 dao = vector3::cross(ao, d);

			const float u = vector3::dot(e2, dao) * invdet;
			const float v = -vector3::dot(e1, dao) * invdet;
			const float t = vector3::dot(ao, n) * invdet;

			if (det >= 1e-6 && t >= 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f)
			{
				is_hit = true;

				if (t < nearest)
				{
					nearest_hit = o + d * t;
					nearest = t;
					norm = n;
				}
			}
		}

		hit = (transform_->get_model() * vector4(nearest_hit, 1.0f)).xyz();
		norm = (transform_->get_model() * vector4(norm, 1.0f)).xyz();

		return is_hit;
	}

	void graphics_node::draw_node_gui()
	{
		ImGui::Text("Node");
		transform_->draw_transform_gui();
	}
}
