#include "boss.hpp"

#include "common.hpp"
#include "BeatList.hpp"
#include "bullet.hpp"

#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <cfloat>

Texture Boss::boss_texture;

bool Boss::init(float health, std::vector<LittleEnemy>* little_enemies)
{
	// Load shared texture
	if (!boss_texture.is_valid())
	{
		if (!boss_texture.load_from_file(textures_path("boss0.png")))
		{
			fprintf(stderr, "Failed to load boss texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = boss_texture.width * 0.5f;
	float hr = boss_texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f, };
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
	
	// Setting initial values
	m_scale.x = 0.75;
	m_scale.y = 0.75;
	m_position = { 600.f, 80.f };
	m_rotation = 0.f;
    m_health = health;
	m_little_enemies = little_enemies;

	return true;
}

void Boss::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Boss::update(float ms)
{

}

void Boss::draw(const mat3& projection)
{
	transform_begin();
	transform_translate({ m_position.x, m_position.y });
	transform_scale(m_scale);
    transform_rotate(m_rotation);
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
	glBindTexture(GL_TEXTURE_2D, boss_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Boss::on_beat(Beat* beat, vec2 screen) {
	int action = rand() % 3;

	switch (action) {
	case 0:
		move({ -20.f, 0.f });
		break;
	case 1:
		move({ 20.f, 0.f });
		break;
	case 2:
		LittleEnemy little_enemy;
		if (little_enemy.init()) {

			little_enemy.set_position(
				{ ((64.f + (float)beat->x) / 640.f) * screen.x, ((48.f + (float)beat->y) / 480.f) * screen.y });
			m_little_enemies->emplace_back(little_enemy);
		}
		break;
	}
}

bool Boss::collides_with(const Bullet& bullet)
{
	bbox boss_bbox = get_bounding_box();
	bbox bullet_bbox = bullet.get_bounding_box();
	return bullet_bbox.min_x <= boss_bbox.max_x && bullet_bbox.max_x >= boss_bbox.min_x &&
		bullet_bbox.min_y <= boss_bbox.max_y && bullet_bbox.max_y >= boss_bbox.min_y;
}

vec2 Boss::get_position()const
{
	return m_position;
}

float Boss::get_rotation()const
{
	return m_rotation;
}

void Boss::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

void Boss::set_rotation(float radians)
{
	m_rotation = radians;
}

void Boss::set_scale(vec2 scale)
{
	m_scale = scale;
}

vec2 Boss::get_scale() {
	return m_scale;
}

float Boss::get_health()const
{
    return m_health;
}

void Boss::set_health(float delta)
{
    m_health += delta;
}

bbox Boss::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	float width = std::fabs(m_scale.x) * boss_texture.width;
	float height = std::fabs(m_scale.y) * boss_texture.height;
	float wr = width * 0.5f;
	float hr = height * 0.5f;
	vec2 points[4];
	points[0] = { -wr, -hr };
	points[1] = { +wr, -hr };
	points[2] = { +wr, +hr };
	points[3] = { -wr, +hr };
	float min_x = FLT_MAX;
	float min_y = FLT_MAX;
	float max_x = FLT_MIN;
	float max_y = FLT_MIN; 
	for (vec2 point : points) {
		point = rotate(point, m_rotation) + m_position;
		min_x = point.x < min_x ? point.x : min_x;
		min_y = point.y < min_y ? point.y : min_y;
		max_x = point.x > max_x ? point.x : max_x;
		max_y = point.y > max_y ? point.y : max_y;
	}
	return { min_x, min_y, max_x, max_y };
};
