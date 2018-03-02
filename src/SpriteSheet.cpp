#include "SpriteSheet.hpp"

SpriteSheet::SpriteSheet()
	: Sprite(nullptr)
{
}

bool SpriteSheet::init(int pictures) {
	m_color[0] = 1.0f;
	m_color[1] = 1.0f;
	m_color[2] = 1.0f;
	m_pictures = pictures;
	time_until_update = 0;
	// The position corresponds to the center of the texture
	float wr = m_texture->width * 0.5f;
	float hr = m_texture->height * 0.5f;

	vertices[0].position = { -wr/m_pictures, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr/m_pictures, +hr, -0.01f };
	vertices[1].texcoord = { 1.f/m_pictures, 1.f, };
	vertices[2].position = { +wr/m_pictures, -hr, -0.01f };
	vertices[2].texcoord = { 1.f/m_pictures, 0.f };
	vertices[3].position = { -wr/m_pictures, -hr, -0.01f };
	vertices[3].texcoord = { 0.f, 0.f };
	
	
	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	return true;
}

void SpriteSheet::update(float ms) {
	time_until_update += ms;
	if (time_until_update > 250) {
		time_until_update = 0;
		vertices[0].texcoord.x += 1.f / m_pictures;
		printf("Left Vertex: %f ", vertices[0].texcoord.x);
		vertices[1].texcoord.x += 1.f / m_pictures;
		vertices[2].texcoord.x += 1.f/m_pictures;
		vertices[3].texcoord.x += 1.f/m_pictures;
		glGenBuffers(1, &mesh.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);
	}

}
void SpriteSheet::update() {
	vertices[0].texcoord.x += 1.f / m_pictures;
	printf("Left Vertex: %f ", vertices[0].texcoord.x);
	vertices[1].texcoord.x += 1.f / m_pictures;
	vertices[2].texcoord.x += 1.f / m_pictures;
	vertices[3].texcoord.x += 1.f / m_pictures;
	if (vertices[0].texcoord.x < 1) {
		glGenBuffers(1, &mesh.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);
	}
}

