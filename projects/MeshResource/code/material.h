#pragma once

#include "tex_res.h"
#include "program.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace efiilj 
{
	class material_base
	{
	private:
		std::unordered_map<std::string, std::shared_ptr<texture_resource>> textures_;
		std::shared_ptr<shader_program> program_;

	public:

		bool doubleSided;

		material_base(std::shared_ptr<shader_program> program);

		void add_texture(std::string type, std::shared_ptr<texture_resource> texture)
		{
			textures_[type] = std::move(texture);
		}

		const shader_program& program() const { return *this->program_; }
		void program(std::shared_ptr<shader_program>& program) { this->program_ = std::move(program); }

		virtual void apply() = 0;
	};

	class gltf_pbr_base : public material_base
	{
	public:
		gltf_pbr_base(std::shared_ptr<shader_program> program);

		std::vector<double> emissiveFactor;
		std::vector<double> baseColorFactor;
		double metallicFactor;
		double roughnessFactor;
		
		void apply();
	};
}
