#include "trfm_mgr.h"

namespace efiilj
{
	transform_manager::transform_manager()
	{}

	transform_manager::~transform_manager()
	{}

	transform_id transform_manager::register_entity(entity_id id)
	{
		transform_id trf_id = _instances.size();
		_instances.emplace_back(trf_id);
		_instance_mapping.emplace(id, trf_id);

		_data.model.emplace_back(matrix4());
		_data.inverse.emplace_back(matrix4());
		_data.position.emplace_back(vector4());
		_data.scale.emplace_back(vector4());
		_data.rotation.emplace_back(quaternion());
		_data.parent.emplace_back(-1);
		_data.model_updated.emplace_back(true);
		_data.inverse_updated.emplace_back(true);

		return trf_id;
	}

	bool transform_manager::unregister_entity(transform_id idx)
	{
		return false;
	}

	void transform_manager::update_models()
	{
		for (const transform_id& idx : _instances)
		{
			if (_data.model_updated[idx])
				continue;

			get_model(idx);
		}
	}

	const matrix4& transform_manager::get_model(transform_id idx) const
	{

		// TODO: optimize, fix child updates
		if (true || !_data.model_updated[idx])
		{
			matrix4 t = matrix4::get_translation(_data.position[idx]);
			matrix4 r = _data.rotation[idx].get_rotation_matrix();
			matrix4 s = matrix4::get_scale(_data.scale[idx]);

			_data.model[idx] = t * r * s;

			if (_data.parent[idx] >= 0)
			{
				_data.model[idx] = _data.model[_data.parent[idx]] * _data.model[idx];
			}

			_data.model_updated[idx] = true;
			_data.inverse_updated[idx] = false;
		}

		return _data.model[idx];
	}

	const matrix4& transform_manager::get_model_inv(transform_id idx) const
	{
		// TODO: fix child updates
		if (true || !_data.inverse_updated[idx])
		{
			_data.inverse[idx] = _data.model[idx].inverse();
			_data.inverse_updated[idx] = true;
		}

		return _data.inverse[idx];
	}

	transform_id transform_manager::get_parent(transform_id idx) const
	{
		return _data.parent[idx];
	}

	void transform_manager::set_parent(transform_id child_id, transform_id parent_id)
	{
		_data.parent[child_id] = parent_id;
		_data.children[parent_id].emplace_back(child_id);
	}

	vector3 transform_manager::get_position(transform_id idx) const
	{
		return _data.model[idx].col(3).xyz();
	}

	void transform_manager::set_position(transform_id idx, const vector3& pos)
	{
		_data.model[idx].col(3, vector4(pos, 1.0f));
	}

	const quaternion& transform_manager::get_rotation(transform_id idx) const
	{
		return _data.rotation[idx];
	}

	void transform_manager::set_rotation(transform_id idx, const quaternion& rot)
	{
		_data.rotation[idx] = rot;	
	}

	vector3 transform_manager::get_scale(transform_id idx)	const
	{
		return _data.scale[idx].xyz();
	}

	void transform_manager::set_scale(transform_id idx, const vector3& scale)
	{
		_data.scale[idx] = vector4(scale, 1.0f);
	}

	void transform_manager::set_scale(transform_id idx, const float& scale)
	{
		_data.scale[idx] = vector4(scale, scale, scale, 1.0f);
	}

	void transform_manager::add_position(transform_id idx, const vector3& delta)
	{
		_data.position[idx] += vector4(delta, 1.0f);
	}

	void transform_manager::add_scale(transform_id idx, const vector3& delta)
	{
		_data.scale[idx] += vector4(delta, 1.0f);
	}

	void transform_manager::add_rotation(transform_id idx, const vector3& axis, float angle)
	{
		_data.rotation[idx].add_axis_rotation(axis, angle);
	}

	vector3 transform_manager::get_left(transform_id idx) const
	{
		get_model(idx);
		return _data.model[idx].col(0).xyz().norm();
	}

	vector3 transform_manager::get_right(transform_id idx) const
	{
		return get_left(idx) * -1.0f;
	}

	vector3 transform_manager::get_up(transform_id idx) const
	{
		get_model(idx);
		return _data.model[idx].col(1).xyz().norm();
	}

	vector3 transform_manager::get_down(transform_id idx) const
	{
		return get_up(idx) * -1.0f;
	}

	vector3 transform_manager::get_forward(transform_id idx) const
	{
		get_model(idx);
		return _data.model[idx].col(2).xyz().norm();
	}

	vector3 transform_manager::get_backward(transform_id idx) const
	{
		return get_forward(idx) * -1.0f;
	}
}