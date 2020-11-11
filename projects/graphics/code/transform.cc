#include "transform.h"

namespace efiilj
{

	transform_model::transform_model(const vector3& pos, const vector3& rot, const vector3& scale)
	: model_(true), position_(pos, 1), scale_(scale, 1), rotation_(rot, 1), model_dirty_(true), inverse_dirty_(true) 
	{ }

	const matrix4& transform_model::get_model() const 
	{
		if (model_dirty_)
		{
			const matrix4 t = matrix4::get_translation(position_);
			const matrix4 r = matrix4::get_rotation_xyz(rotation_);
			const matrix4 s = matrix4::get_scale(scale_);

			model_ = s * r * t;

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

	// PITCH, YAW, ROLL
	vector4 transform_model::forward() const
	{
		return vector4(
				sin(rotation_.y()),
				-(sin(rotation_.x()) * cos(rotation_.y())),
				-(cos(rotation_.x()) * cos(rotation_.y())),
				1.0f).norm();


		return vector4(
			cos(rotation_.x()) * cos(rotation_.z()),
			sin(rotation_.x()),
			cos(rotation_.x()) * sin(rotation_.z()), 1).norm();
	}

	vector4 transform_model::backward() const
	{
		return forward() * -1;
	}

	vector4 transform_model::left() const
	{
		return vector4::cross(vector4(0, 1, 0, 1), forward()).norm();
	}

	vector4 transform_model::right() const
	{
		return left() * -1;
	}

	vector4 transform_model::up() const
	{
		return vector4::cross(forward(), left());
	}

	vector4 transform_model::down() const
	{
		return up() * -1;
	}
}
