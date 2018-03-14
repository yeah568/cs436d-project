// Header
#include "CenterBeatCircle.hpp"
#include <cmath>

std::shared_ptr<Player> CenterBeatCircle::player;

bool CenterBeatCircle::init(bool type)
{
	// Load shared texture
	if (!center_beat_circle_texture.is_valid())
	{	if (type) {
			if (!center_beat_circle_texture.load_from_file(textures_path("orange_moving_beat.png")))
			{
				fprintf(stderr, "Failed to load turtle texture!");
				return false;
			}
		} else {
			if (!center_beat_circle_texture.load_from_file(textures_path("blue_moving_beat.png")))
			{
				fprintf(stderr, "Failed to load turtle texture!");
				return false;
			}
		}
	}
	if (!type) {
		m_scale.x = 2.0f;
		m_scale.y = 2.0f;
	} else {
		m_scale.x = 1.5f;
		m_scale.y = 1.5f;
	}
	// The position corresponds to the center of the texture
	float wr = center_beat_circle_texture.width * 0.5f;
	float hr = center_beat_circle_texture.height * 0.5f;

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
	m_position.x = -50;
	m_position.y = 50;
	beat_circle_type = type;


	return true;
}

// Call if init() was successful
// Releases all graphics resources
void CenterBeatCircle::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void CenterBeatCircle::update(float ms)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	// float step = SPEED * (ms / 1);
	
	// m_position.x += m_movement_dir.x*step;
	// m_position.y += m_movement_dir.y*step;
	// m_scale.x -= ms/1000;
	// m_scale.y -= ms/1000;
}

void CenterBeatCircle::set_scale(vec2 scale) {
	m_scale = scale;
}

void CenterBeatCircle::draw(const mat3& projection)
{
	
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	// vec2 rotated = rotate(m_position, -player->get_rotation());
	// vec2 final_pos = rotated+player->get_position();
	//printf("BC.x=%f,BC.y=%f\n", final_pos.x, final_pos.y);
	transform_begin();
	transform_translate(player->get_position());
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
	glBindTexture(GL_TEXTURE_2D, center_beat_circle_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	
}

vec2 CenterBeatCircle::get_position()const
{
	return m_position;
}

void CenterBeatCircle::set_position(vec2 position)
{
	
	m_position = position;
}

void CenterBeatCircle::set_rotation(float angle)
{
	m_rotation = angle;
}

// Returns the local bounding coordinates scaled by the current size of the fish 
vec2 CenterBeatCircle::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * center_beat_circle_texture.width, std::fabs(m_scale.y) * center_beat_circle_texture.height };
}