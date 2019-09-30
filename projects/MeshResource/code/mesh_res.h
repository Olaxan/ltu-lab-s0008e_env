#pragma once

#include "matrix4.h"

namespace efiilj
{
	/// <summary>
	/// Struct to represent a single vertex with position and color.
	/// </summary>
	struct Vertex
	{
		Vertex() : xyzw(0, 0, 0, 0), normal(0, 0, 0, 0), rgba(1, 1, 1, 1), uv(0, 0) { }
		Vertex(Vector3 xyzw, Vector3 normal, Vector4 rgba, Vector2 uv) : xyzw(xyzw, 1), normal(normal, 1), rgba(rgba), uv(uv) { }

		Vector4 xyzw;
		Vector4 normal;
		Vector4 rgba;
		Vector2 uv;
	};

	/// <summary>
	/// Class to create a mesh on the GPU, as well as to hold buffer handles which enables binding.
	/// </summary>
	class MeshResource
	{
	private:

		/// <summary>
		/// Vertex Buffer Object
		/// </summary>
		unsigned int vbo;

		/// <summary>
		/// Index Buffer Object
		/// </summary>
		unsigned int ibo;

		/// <summary>
		/// Vertex Array Object
		/// </summary>
		unsigned int vao;

		int vertexCount;
		int indexCount;

		/// <summary>
		/// Creates and initializes the Vertex Buffer, configures vertex attribute pointers, and enables attribute arrays.
		/// Ensure a Vertex Array Object has been configured and bound before running this function.
		/// </summary>
		/// <param name="vertexList">The list of vertices to buffer</param>
		/// <param name="count">Size of vertex list</param>
		void InitVertexBuffer(Vertex* vertexList, int count);

		/// <summary>
		/// Creates and initializes the Index Buffer.
		/// </summary>
		/// <param name="indexList">The list of indices to buffer</param>
		/// <param name="count">Size of index list</param>
		void InitIndexBuffer(unsigned int* indexList, int count);

		/// <summary>
		/// Creates and initializes the Vertex Array Object.
		/// </summary>
		void InitArrayObject();

	public:

		/// <summary>
		/// Creates an empty MeshResource object.
		/// </summary>
		MeshResource();

		/// <summary>
		/// Creates a new MeshResource instance with the specified vertex and index lists.
		/// </summary>
		/// <param name="vertexList">List of vertices to buffer</param>
		/// <param name="vertexCount">Size of the vertex list</param>
		/// <param name="indexList">List of indices to buffer</param>
		/// <param name="indexCount">Size of the index list</param>
		MeshResource(Vertex* vertexList, int vertexCount, unsigned int* indexList, int indexCount);

		MeshResource(MeshResource& copy);
		MeshResource(MeshResource&& move);

		/// <summary>
		/// Static method to generate a cube of the specified size.
		/// </summary>
		/// <param name="size">The size across the entire cube</param>
		/// <param name="c">Vertex color</param>
		/// <returns></returns>
		static MeshResource Cube(float size, float c = 1);

		/// <summary>
		/// Gets mesh vertex count
		/// </summary>
		/// <returns>The number of vertices in the mesh</returns>
		int VertexCount()
		{
			return vertexCount;
		}

		/// <summary>
		/// Gets the mesh index count
		/// </summary>
		/// <returns>The number of index in the mesh</returns>
		int IndexCount()
		{
			return indexCount;
		}

		/// <summary>
		/// Binds Vertex Array Object and Index Buffer to prepare OpenGL for drawing this mesh.
		/// </summary>
		void Bind();

		/// <summary>
		/// Unbinds the resource from the OpenGL state machine.
		/// </summary>
		void Unbind();

		/// <summary>
		/// Pushes a new vertex list (of the same size) to the Vertex Buffer.
		/// </summary>
		/// <param name="vertexList">The updated vertex list</param>
		void UpdateVertexBuffer(Vertex* vertexList);

		/// <summary>
		/// Performs a draw call with the correct index specifications.
		/// </summary>
		void DrawElements();

		~MeshResource();
	};
}