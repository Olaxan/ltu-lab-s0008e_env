#include "transform.h"

namespace efiilj
{

	transform_model::transform_model(const vector3& pos, const vector3& rot, const vector3& scale)
	: model_(true), position_(pos, 1), scale_(scale, 1), rotation_(rot, 1),
	m_rotation_(matrix4()), model_dirty_(true), inverse_dirty_(true) 
	{ }

	const matrix4& transform_model::get_model() const 
	{
		if (model_dirty_)
		{
			const matrix4 t = matrix4::get_translation(position_);
			const matrix4 r = matrix4::get_rotation_euler(rotation_);
			const matrix4 s = matrix4::get_scale(scale_);

			m_rotation_ = r;
			model_ = t * r * s;

			model_dirty_ = false;
			inverse_dirty_ = true;
		}

		return model_;
	}

	const matrix4& transform_model::get_model_inv() const
	{
		if (inverse_dirty_)
		{
			inverse_ = model_.inverse();
			inverse_dirty_ = false;
		}

		return inverse_;
	}

	vector3 transform_model::left() const
	{
		get_model();
		return m_rotation_.col(0).xyz();
	}

	vector3 transform_model::right() const
	{
		return left() * -1;
	}

	vector3 transform_model::up() const
	{
		get_model();
		return m_rotation_.col(1).xyz();
	}

	vector3 transform_model::down() const
	{
		return up() * -1;
	}

	vector3 transform_model::forward() const
	{
		get_model();
		return m_rotation_.col(2).xyz();
	}

	vector3 transform_model::backward() const
	{
		return forward() * -1;
	}
}
