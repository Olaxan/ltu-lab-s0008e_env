#pragma once

#include <string>
#include <unordered_map>

#include "matrix4.h"

namespace efiilj
{

	/// <summary>
	/// Class to create a shader on the GPU, as well as to hold handles which enables binding.
	/// </summary>
	class ShaderResource
	{
	private:

		enum ShaderDebugType
		{
			SHADER,
			PROGRAM
		};

		unsigned int program_id;

		std::unordered_map<std::string, int> locations;

	public:

		ShaderResource();
		ShaderResource(const char* vertex, const char* fragment);
		ShaderResource(std::string vertex, std::string fragment);

		static std::string LoadShader(const char* filePath);
		static std::string LoadShader(std::string filePath);

		unsigned int CompileShader(unsigned int type, const char* shader);
		unsigned int CreateProgram(const char* vertex, const char* fragment);
		bool DebugShader(unsigned int id, ShaderDebugType type, unsigned int status, std::ostream& stream, const char* header) const;

		void Use() const;
		void Drop() const;

		int FindUniformLocation(const char* name);
		bool SetUniform1i(const char* name, int val);
		bool SetUniformVector4fv(const char* name, const Vector4& vec);
		bool SetUniformMatrix4fv(const char* name, const Matrix4& mat);

		~ShaderResource();
	};
}