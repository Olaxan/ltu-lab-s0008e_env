#include "mesh_res.h"

#include <GL/glew.h>

namespace efiilj
{

	MeshResource::MeshResource() : ibo(0), vao(0), vbo(0), indexCount(0), vertexCount(0) { }

	MeshResource::MeshResource(Vertex* vertexList, int vertexCount, unsigned int* indexList, int indexCount) : ibo(0), vao(0), vbo(0)
	{
		this->vertexCount = vertexCount;
		this->indexCount = indexCount;

		InitArrayObject();
		InitVertexBuffer(vertexList, vertexCount);
		InitIndexBuffer(indexList, indexCount);
	}

	MeshResource::MeshResource(MeshResource& copy) 
		: ibo(copy.ibo), vao(copy.vao), vbo(copy.vbo), indexCount(copy.indexCount), vertexCount(copy.vertexCount) { }

	MeshResource MeshResource::Cube(float size, float c)
	{
		size /= 2;

		Vertex vertices[24] = {

			// Front = S
			Vertex(Vector3(-size, -size, size), Vector3(0, 0, 1), Vector4(c, c, c, c), Vector2(0.25f, 0.33f)),		// 0
			Vertex(Vector3( size, -size, size), Vector3(0, 0, 1), Vector4(c, c, c, c), Vector2(0.5f, 0.33f)),		// 1
			Vertex(Vector3( size,  size, size), Vector3(0, 0, 1), Vector4(c, c, c, c), Vector2(0.5f, 0.66f)),		// 2
			Vertex(Vector3(-size,  size, size), Vector3(0, 0, 1), Vector4(c, c, c, c), Vector2(0.25f, 0.66f)),		// 3

			// Back = N
			Vertex(Vector3( size, -size, -size), Vector3(0, 0, -1), Vector4(c, c, c, c), Vector2(0.75, 0.33f)),		// 4
			Vertex(Vector3(-size, -size, -size), Vector3(0, 0, -1), Vector4(c, c, c, c), Vector2(1.0f, 0.33f)),		// 5
			Vertex(Vector3(-size,  size, -size), Vector3(0, 0, -1), Vector4(c, c, c, c), Vector2(1.0f, 0.66f)),		// 6
			Vertex(Vector3( size,  size, -size), Vector3(0, 0, -1), Vector4(c, c, c, c), Vector2(0.75, 0.66f)),		// 7

			// Top
			Vertex(Vector3(-size, size,  size), Vector3(0, 1, 0), Vector4(c, c, c, c), Vector2(0.25f, 0.66f)),		// 8
			Vertex(Vector3( size, size,  size), Vector3(0, 1, 0), Vector4(c, c, c, c), Vector2(0.5f, 0.66f)),		// 9
			Vertex(Vector3( size, size, -size), Vector3(0, 1, 0), Vector4(c, c, c, c), Vector2(0.5f, 1.0f)),		// 10
			Vertex(Vector3(-size, size, -size), Vector3(0, 1, 0), Vector4(c, c, c, c), Vector2(0.25f, 1.0f)),		// 11

			// Bottom
			Vertex(Vector3( size, -size,  size), Vector3(0, -1, 0), Vector4(c, c, c, c), Vector2(0.25f, 0.0f)),		// 12
			Vertex(Vector3(-size, -size,  size), Vector3(0, -1, 0), Vector4(c, c, c, c), Vector2(0.5f, 0.0f)),		// 13
			Vertex(Vector3(-size, -size, -size), Vector3(0, -1, 0), Vector4(c, c, c, c), Vector2(0.5f, 0.33f)),		// 14
			Vertex(Vector3( size, -size, -size), Vector3(0, -1, 0), Vector4(c, c, c, c), Vector2(0.25f, 0.33f)),	// 15

			// Left = W
			Vertex(Vector3(-size, -size, -size), Vector3(-1, 0, 0), Vector4(c, c, c, c), Vector2(0.0f, 0.33f)),		// 16
			Vertex(Vector3(-size, -size,  size), Vector3(-1, 0, 0), Vector4(c, c, c, c), Vector2(0.25f, 0.33f)),	// 17
			Vertex(Vector3(-size,  size,  size), Vector3(-1, 0, 0), Vector4(c, c, c, c), Vector2(0.25f, 0.66f)),	// 18
			Vertex(Vector3(-size,  size, -size), Vector3(-1, 0, 0), Vector4(c, c, c, c), Vector2(0.0f, 0.66f)),		// 19

			// Right = E
			Vertex(Vector3(size, -size,  size), Vector3(1, 0, 0), Vector4(c, c, c, c), Vector2(0.5f, 0.33f)),		// 20
			Vertex(Vector3(size, -size, -size), Vector3(1, 0, 0), Vector4(c, c, c, c), Vector2(0.75, 0.33f)),		// 21
			Vertex(Vector3(size,  size, -size), Vector3(1, 0, 0), Vector4(c, c, c, c), Vector2(0.75, 0.66f)),		// 22
			Vertex(Vector3(size,  size,  size), Vector3(1, 0, 0), Vector4(c, c, c, c), Vector2(0.5f, 0.66f)),		// 23
		};

		unsigned int indices[36] = {
			0, 1, 3, 2, 3, 1,			// Front
			4, 5, 7, 6, 7, 5,			// Back
			8, 9, 11, 10, 11, 9,		// Top
			12, 13, 15, 14, 15, 13,		// Bottom
			16, 17, 19, 18, 19, 17,		// Left
			20, 21, 23, 22, 23, 21		// Right
		};

		return MeshResource(vertices, 24, indices, 36);
	}

	void MeshResource::InitVertexBuffer(Vertex* vertexList, int count)
	{
		if (vbo != 0)
			return;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertexList, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rgba));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}

	void MeshResource::InitIndexBuffer(unsigned int* indexList, int count)
	{
		if (ibo != 0) // Ollad
			return;

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indexList, GL_STATIC_DRAW);
	}

	void MeshResource::InitArrayObject()
	{
		if (vao != 0)
			return;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	}

	void MeshResource::UpdateVertexBuffer(Vertex* vertexList)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), vertexList);
	}

	void MeshResource::Bind()
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}

	void MeshResource::Unbind()
	{
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void MeshResource::DrawElements()
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);
	}

	MeshResource::~MeshResource()
	{
		std::cout << "Deleting mesh resource " << this << std::endl;

		Unbind();
		glDeleteBuffers(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}
}