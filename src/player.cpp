// Header
#include "player.hpp"

// internal
#include "turtle.hpp"
#include "bullet.hpp"
#include "common.hpp"

// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>

Texture Player::player_texture;

bool Player::init()
{
	// Load shared texture
	if (!player_texture.is_valid())
	{
		if (!player_texture.load_from_file(textures_path("character.png")))
		{
			fprintf(stderr, "Failed to load turtle texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = player_texture.width * 0.5f;
	float hr = player_texture.height * 0.5f;

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
	m_scale.x = -1.f;
	m_scale.y = 1.f;
	m_is_alive = true;
	//m_num_indices = indices.size();
	m_position = { 600.f, 700.f };
	m_rotation = 0.f;
	m_light_up_countdown_ms = -1.f;
	m_movement_dir = { 0.f, 0.f };
	bullet_type = false;

	return true;
}

// Releases all graphics resources
void Player::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

// Called on each frame by World::update()
void Player::update(float ms)
{
	const float SALMON_SPEED = 200.f;
	float step = SALMON_SPEED * (ms / 1000);
	if (exploding_timer > 0 && exploding_timer < 1.25) {
		m_scale.y = 0.4f;
		player_texture.load_from_file(textures_path("explosion_1.png"));
		exploding_timer += ms/1000;
	}
	if (exploding_timer > 1.25 && exploding_timer < 1.5) {
		player_texture.load_from_file(textures_path("explosion_2.png"));
		exploding_timer += ms/1000;
	}
	if (exploding_timer > 1.5 && exploding_timer < 1.75) {
		player_texture.load_from_file(textures_path("explosion_3.png"));
		exploding_timer += ms/1000;
	}
	if (exploding_timer > 1.75 && exploding_timer < 2) {
		player_texture.load_from_file(textures_path("explosion_4.png"));
		exploding_timer += ms/1000;
	}
	if (exploding_timer > 2 && exploding_timer < 2.25) {
		player_texture.load_from_file(textures_path("explosion_5.png"));
		exploding_timer += ms / 1000;
	}
	if (exploding_timer > 2.25 && exploding_timer < 2.5) {
		player_texture.load_from_file(textures_path("explosion_6.png"));
		exploding_timer += ms / 1000;
	}
	if (exploding_timer > 2.5 && exploding_timer < 3) {
		player_texture.load_from_file(textures_path("explosion_7.png"));
		exploding_timer += ms / 1000;
	}
	if (exploding_timer > 3 && exploding_timer <3.5) {
		player_texture.load_from_file(textures_path("explosion_8.png"));
		exploding_timer += ms / 1000;
	}
	if (exploding_timer > 3.5 && exploding_timer < 4) {
		player_texture.load_from_file(textures_path("explosion_9.png"));
		exploding_timer += ms / 1000;
	}
	if (m_is_alive)
	{
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// UPDATE SALMON POSITION HERE BASED ON KEY PRESSED (World::on_key())
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		vec2 normalized_movement = m_movement_dir;
		if (m_movement_dir.x != 0 && m_movement_dir.y != 0) {
			normalized_movement = normalize(normalized_movement);
		}
		
		move({ normalized_movement.x * 10, normalized_movement.y * 10 });

		// Set player to face mouse
		float delta_x = m_mouse.x - m_position.x;
		float delta_y = m_position.y - m_mouse.y;
		float angle = (float)atan2(delta_y, delta_x);
		set_rotation(3.14/2);
	}
	else
	{
		// If dead we make it face upwards and sink deep down
		set_rotation(3.1415f);
		move({ 0.f, step });
	}

	if (abs(m_scale.x * 0.95) < 1.f) {
		m_scale.x = -1.f;
	}
	else {
		m_scale.x *= 0.95;
	}

	if (abs(m_scale.y * 0.95) < 1.f) {
		m_scale.y = 1.f;
	}
	else {
		m_scale.y *= 0.95;
	}

	if (m_light_up_countdown_ms > 0.f)
		m_light_up_countdown_ms -= ms;
}

void Player::draw(const mat3& projection)
{
	transform_begin();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// SALMON TRANSFORMATION CODE HERE

	// see Transformations and Rendering in the specification pdf
	// the following functions are available:
	// transform_translate()
	// transform_rotate()
	// transform_scale()

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// REMOVE THE FOLLOWING LINES BEFORE ADDING ANY TRANSFORMATION CODE
	transform_translate({ m_position.x, m_position.y });
	transform_scale(m_scale);
	if (exploding_timer > 0) {
		transform_rotate(0);
	}
	else {
		transform_rotate(m_rotation - 3.14/2);
	}
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


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
	glBindTexture(GL_TEXTURE_2D, player_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

// Simple bounding box collision check, 
bool Player::collides_with(const Turtle& turtle)
{
	float dx = m_position.x - turtle.get_position().x;
	float dy = m_position.y - turtle.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(turtle.get_bounding_box().x, turtle.get_bounding_box().y);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

bool Player::collides_with(const Bullet& fish)
{
	float dx = m_position.x - fish.get_position().x;
	float dy = m_position.y - fish.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(fish.get_bounding_box().x, fish.get_bounding_box().y);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

vec2 Player::get_position()const
{
	return m_position;
}

float Player::get_rotation()const
{
	return m_rotation - 1.57;
}

void Player::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

void Player::set_rotation(float radians)
{
	m_rotation = radians;
}

void Player::set_scale(vec2 scale)
{
	m_scale = scale;
}

vec2 Player::get_scale() {
	return m_scale;
}

void Player::set_mouse(float x, float y)
{
	m_mouse = { x, y };
}

bool Player::is_alive()const
{
	return m_is_alive;
}

// Called when the salmon collides with a turtle
void Player::kill()
{
	m_is_alive = false;
}

// Called when the salmon collides with a fish
void Player::light_up()
{
	m_light_up_countdown_ms = 1500.f;
}

void Player::dash() {
	move({ m_movement_dir.x*60, m_movement_dir.y *60});
}

void Player::set_movement_dir(vec2 dir) {
	m_movement_dir = dir;
}

void Player::add_movement_dir(vec2 dir) {
	m_movement_dir.x += dir.x;
	m_movement_dir.y += dir.y;
}

void Player::scale_by(float scale) {
	m_scale.x *= scale;
	m_scale.y *= scale;
}