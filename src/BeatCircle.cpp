// Header
#include "BeatCircle.hpp"

#include <cmath>

Texture BeatCircle::beatCircle_texture;
Texture BeatCircle::beatCircle_texture2;
Player* BeatCircle::player;

bool BeatCircle::init(float speed)
{
	SPEED = speed;
	// Load shared texture
	if (!beatCircle_texture.is_valid())
	{
		if (!beatCircle_texture.load_from_file(textures_path("orange_moving_beat.png")))
		{
			fprintf(stderr, "Failed to load turtle texture!");
			return false;
		}
	}
	if (!beatCircle_texture2.is_valid())
	{
		if (!beatCircle_texture2.load_from_file(textures_path("blue_moving_beat.png")))
		{
			fprintf(stderr, "Failed to load turtle texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = beatCircle_texture.width * 0.5f;
	float hr = beatCircle_texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f,  };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
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

	// Setting initial values,
	// 1.0 would be as big as the original texture
	m_scale.x = 1.1f;
	m_scale.y = 1.1f;
	m_position.x = -50;
	m_position.y = 50;
	beat_circle_type = false;


	return true;
}

void BeatCircle::set_dir(int direction) {
	dir = direction;
	switch (dir) {
		case 1:
			m_movement_dir = {1.0f, 0.0f};
			break;
		case 2:
			m_movement_dir = {0.0f, -1.0f};
			break;
		case 3:
			m_movement_dir = {-1.0f, 0.0f};
			break;
		case 4:
			m_movement_dir = {0.0f, 1.0f};
			break;
	}
	beat_circle_type = ((dir % 2) == 1);
}

// Call if init() was successful
// Releases all graphics resources
void BeatCircle::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void BeatCircle::update(float ms)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	float step = SPEED * (ms / 1);
	
	m_position.x += m_movement_dir.x*step;
	m_position.y += m_movement_dir.y*step;
	m_scale.x -= ms/1000;
	m_scale.y -= ms/1000;
}

void BeatCircle::set_scale(vec2 scale) {
	m_scale = scale;
}

void BeatCircle::draw(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	vec2 rotated = rotate(m_position, -player->get_rotation());
	vec2 final_pos = rotated+player->get_position();
	//printf("BC.x=%f,BC.y=%f\n", final_pos.x, final_pos.y);
	transform_begin();
	transform_translate(final_pos);
	
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	if (beat_circle_type) {
		glBindTexture(GL_TEXTURE_2D, beatCircle_texture.id);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, beatCircle_texture2.id);
	}

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 BeatCircle::get_position()const
{
	return m_position;
}

void BeatCircle::set_position(vec2 position)
{
	printf("BC.x=%f,BC.y=%f\n", position.x, position.y);
	m_position = position;
}

void BeatCircle::set_rotation(float angle)
{
	m_rotation = angle;
}

// Returns the local bounding coordinates scaled by the current size of the fish 
vec2 BeatCircle::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * beatCircle_texture.width, std::fabs(m_scale.y) * beatCircle_texture.height };
}