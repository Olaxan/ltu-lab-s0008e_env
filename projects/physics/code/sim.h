#pragma once

#include "manager.h"
#include "ifmgr.h"

#include <chrono>
#include <vector>
#include <memory>
#include <queue>

#include "phys_data.h"

namespace efiilj
{
	typedef int physics_id;

	typedef int physics_state;

	typedef std::chrono::duration<float> duration;
	typedef std::chrono::high_resolution_clock frame_timer;
	typedef std::chrono::time_point<frame_timer> frame_time;

	struct PointForce
	{
		vector3 p;
		vector3 force;

		PointForce()
			: p(vector3(0)), force(vector3(0)) { }

		PointForce(const vector3& p, const vector3& force)
			: p(p), force(force) { }

	};

	class simulator : public manager<physics_id>
	{
		private:

			struct Derivative
			{
				vector3 velocity; 	// dx/dt (position change over time)
				vector3 force;		// dv/dt (velocity change over time)
				quaternion spin;	// dr/dt (orientation change over time)
				vector3 torque;		// dw/dt (angular velocity change over time)
			};

			struct PhysicsState
			{
				// PRIMARY
				// === Linear
				vector3 position;
				vector3 momentum;

				// === Angular
				quaternion orientation;
				vector3 angular_momentum;

				// SECONDARY
				// === Linear
				vector3 velocity;

				// === Angular
				quaternion spin;
				vector3 angular_velocity;
			};

			struct PhysicsData
			{
				std::vector<PhysicsState> current;
				std::vector<PhysicsState> previous;

				std::vector<std::queue<PointForce>> impulses;
				std::vector<std::vector<PointForce>> forces;

				std::vector<vector3> com;
				std::vector<float> mass;
				std::vector<float> inverse_mass;
				std::vector<float> inertia;
				std::vector<float> inverse_inertia;
				std::vector<float> restitution;
				std::vector<float> friction;
			} _data;

			float t = 0.0f;
			float dt = 0.01f;
			float gravity_mult = 0.0f;
			float air_drag_mult = 0.0f;

			bool collision_rejection = false;

			frame_time current_time = frame_timer::now();

			float accumulator = 0.0f;

			std::shared_ptr<transform_manager> _transforms;
			std::shared_ptr<collider_manager> _colliders;
			std::shared_ptr<mesh_manager> _mesh_instances;
			std::shared_ptr<mesh_server> _meshes;

			void apply_impulses(physics_id, PhysicsState& state);

	public:

		simulator();
		~simulator();

		void extend_defaults(physics_id) override;
		void draw_gui() override;
		void draw_gui(physics_id) override;

		void on_register(std::shared_ptr<manager_host> host) override;

		vector3 calculate_com(entity_id eid) const;
		void recalculate_com(physics_id idx);
		void recalculate_state(physics_id idx, PhysicsState& state);

		bool step(physics_id idx, PhysicsState& state, const float t, const float dt);

		void read_transform(physics_id, PhysicsState& state);
		void write_transform(physics_id, PhysicsState& state);

		void begin_frame();
		void simulate();
		void end_frame();

		Derivative evaluate(physics_id idx, const PhysicsState& state, const Derivative& d, float t, float dt);
		void integrate(physics_id idx, PhysicsState& state, const float t, const float dt);

		void add_impulse(physics_id idx, const PointForce& force)
		{
			_data.impulses[idx].push(force);
		}

		void add_force(physics_id idx, const PointForce& force)
		{
			_data.forces[idx].emplace_back(force);
		}

		vector3 acceleration(physics_id idx, const PhysicsState& state, float t);
		vector3 torque(physics_id idx, const PhysicsState& state, float t);

		const vector3& get_com(physics_id idx) const
		{
			return _data.com[idx];
		}
		
		void set_com(physics_id idx, const vector3& com);

		void set_mass(physics_id idx, float mass)
		{
			_data.mass[idx] = mass;
			_data.inverse_mass[idx] = 1.0f / mass;
		}

		void set_inertia(physics_id idx, float inertia)
		{
			_data.inertia[idx] = inertia;
			_data.inverse_inertia[idx] = 1.0f / inertia;
		}

		void set_static(physics_id idx, bool is_static)
		{
			// TODO: fix
			_data.inverse_mass[idx] = is_static ? 0 : 1.0f / _data.mass[idx];
			_data.inverse_inertia[idx] = is_static ? 0 : 1.0f / _data.inertia[idx];
		}

		void set_inertia_as_cube(physics_id idx, float length)
		{
			set_inertia(idx, 1.0f / 6.0f * (length * length) * _data.mass[idx]);
		};
	};
}
