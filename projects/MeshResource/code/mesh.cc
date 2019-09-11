#include "mesh.h"

namespace efiilj
{
	MeshResource::MeshResource()
	{
		ibo = 0;
		vao = 0;
		vbo = 0;
		vertexCount = 0;
		indexCount = 0;
	}

	MeshResource::MeshResource(float* vertexList, unsigned int* indexList, int vertexCount, int indexCount) : MeshResource()
	{
		this->vertexCount = vertexCount;
		this->indexCount = indexCount;
		InitVertexBuffer(vertexList, vertexCount);
		InitIndexBuffer(indexList, indexCount);
		InitArrayObject();
	}

	void MeshResource::InitVertexBuffer(float* vertexList, int count)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertexList, GL_STATIC_DRAW);
	}

	void MeshResource::InitIndexBuffer(unsigned int* indexList, int count)
	{
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indexList, GL_STATIC_DRAW);
	}

	void MeshResource::InitArrayObject()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	void MeshResource::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindVertexArray(vao);
	}

	void MeshResource::Draw(const GLuint shader)
	{
		glUseProgram(shader);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);
	}
}