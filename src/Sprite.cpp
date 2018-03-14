#include <cmath>
#include "Sprite.hpp"
#include <cfloat>

Sprite::Sprite(std::shared_ptr<Texture> texture)
  : m_texture(texture) {
}

Sprite::Sprite(const vec2& position, const vec2& scale, float rotation, std::shared_ptr<Texture> texture)
  : m_position(position),
    m_scale(scale),
    m_rotation(rotation),
    m_texture(texture) {
}

bool Sprite::init()
{
  m_color[0] = 1.0f;
  m_color[1] = 1.0f;
  m_color[2] = 1.0f;
  // The position corresponds to the center of the texture
  float wr = m_texture->width * 0.5f;
  float hr = m_texture->height * 0.5f;

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

  return true;
}

void Sprite::destroy()
{
  glDeleteBuffers(1, &mesh.vbo);
  glDeleteBuffers(1, &mesh.ibo);
  glDeleteBuffers(1, &mesh.vao);

  glDeleteShader(effect.vertex);
  glDeleteShader(effect.fragment);
  glDeleteShader(effect.program);
}

void Sprite::draw(const mat3& projection)
{
  // Transformation code, see Rendering and Transformation in the template specification for more info
  // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
  //printf("BC.x=%f,BC.y=%f\n", final_pos.x, final_pos.y);
  transform_begin();
  transform_translate(m_position);	
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
  glBindTexture(GL_TEXTURE_2D, m_texture->id);

  // Setting uniform values to the currently bound program
  glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
  glUniform3fv(color_uloc, 1, m_color);
  glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

  // Drawing!
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Sprite::draw_hitboxes(const mat3& projection)
{
	if (m_hitboxes.size() == 0) return;
	for (auto& hitbox : m_hitboxes) {
		hitbox->draw(projection);
	}
}

const vec2& Sprite::get_position() const
{
  return m_position;
}

void Sprite::set_position(const vec2& position) {
  m_position = position;
}

const vec2& Sprite::get_scale() const
{
  return m_scale;
}

void Sprite::set_scale(const vec2& scale) {
  m_scale = scale;
}

float Sprite::get_rotation() const
{
  return m_rotation;
}

void Sprite::set_rotation(float rotation)
{
  m_rotation = rotation;
}

std::array<vec2, 4> Sprite::get_vertices() const
{
	float width = std::fabs(m_scale.x) * m_texture->width;
	float height = std::fabs(m_scale.y) * m_texture->height;
	float wr = width * 0.5f;
	float hr = height * 0.5f;
	std::array<vec2, 4> points;
	points[0] = rotate({ -wr, -hr }, m_rotation);
	points[1] = rotate({ +wr, -hr }, m_rotation);
	points[2] = rotate({ +wr, +hr }, m_rotation);
	points[3] = rotate({ -wr, +hr }, m_rotation);
	return points;
}

std::vector<std::array<vec2, 4>> Sprite::get_hitbox_points() const
{
	std::array<vec2, 4> vertices = get_vertices();
	std::vector<std::array<vec2, 4>> ret;
	std::vector<mat3> hitbox_matrices;
	if (m_hitboxes.size() == 0) {
		hitbox_matrices.push_back(identity());
	}

	for (auto& hitbox : m_hitboxes) {
		hitbox_matrices.push_back(hitbox->get_matrix());
	}

	for (auto& matrix : hitbox_matrices) {
		std::array<vec2, 4> points = vertices;
		for (int i = 0; i < 4; i++) {
			vec3 point_3 = { points[i].x, points[i].y, 1.f };
			point_3 = mul(matrix, point_3);
			points[i] = { point_3.x, point_3.y };
			points[i] = points[i] + m_position;
		}
		ret.push_back(points);
	}

	return ret;
}

bbox Sprite::get_bounding_box() const
{
	std::array<vec2, 4> vertices = get_vertices();
	float min_x = FLT_MAX;
	float min_y = FLT_MAX;
	float max_x = -FLT_MAX;
	float max_y = -FLT_MAX;
	for (vec2 point : vertices) {
		point = point + m_position;
		min_x = point.x < min_x ? point.x : min_x;
		min_y = point.y < min_y ? point.y : min_y;
		max_x = point.x > max_x ? point.x : max_x;
		max_y = point.y > max_y ? point.y : max_y;
	}

	return { min_x, min_y, max_x, max_y };
}

bbox Sprite::get_hitbox_bbox(std::vector<std::array<vec2, 4>> hitboxes) const
{
	float min_x = FLT_MAX;
	float min_y = FLT_MAX;
	float max_x = -FLT_MAX;
	float max_y = -FLT_MAX;
	for (auto& hitbox : hitboxes) {
		for (vec2 point : hitbox) {
			min_x = point.x < min_x ? point.x : min_x;
			min_y = point.y < min_y ? point.y : min_y;
			max_x = point.x > max_x ? point.x : max_x;
			max_y = point.y > max_y ? point.y : max_y;
		}
	}
	
	return { min_x, min_y, max_x, max_y };
}

void Sprite::set_texture(std::shared_ptr<Texture> texture)
{
  m_texture = texture;
}

// Basic axis-aligned bounding box collisions
bool Sprite::collides_with(const Sprite& other)
{
	std::vector<std::array<vec2, 4>> hitboxes = get_hitbox_points();
	std::vector<std::array<vec2, 4>> other_hitboxes = other.get_hitbox_points();
	bbox bbox1 = get_hitbox_bbox(hitboxes);
	bbox bbox2 = other.get_hitbox_bbox(other_hitboxes);
	if (bbox2.min_x <= bbox1.max_x && bbox2.max_x >= bbox1.min_x &&
		bbox2.min_y <= bbox1.max_y && bbox2.max_y >= bbox1.min_y) {
		// If neither sprite has explicitly defined hitboxes, the bounding box is the hitbox
		// so we return true.
		if (get_hitboxes().size() == 0 && other.get_hitboxes().size() == 0) {
			return true;
		}
		return hitboxes_collide(hitboxes, other_hitboxes);
	}
	return false;
}

std::vector<std::shared_ptr<Hitbox>> Sprite::get_hitboxes() const
{
	return m_hitboxes;
}

bool Sprite::hitboxes_collide(std::vector<std::array<vec2, 4>> hitboxes, std::vector<std::array<vec2, 4>> other_hitboxes)
{
	return hitbox_sat_collision(hitboxes, other_hitboxes) && hitbox_sat_collision(other_hitboxes, hitboxes);
}

bool Sprite::hitbox_sat_collision(std::vector<std::array<vec2, 4>> hitboxes, std::vector<std::array<vec2, 4>> other_hitboxes)
{
	for (auto& hitbox : hitboxes) {
		// Calculate all 4 normals for this hitbox
		vec2 normals[4];
		for (int i = 0, j = 1; i < 4; i++, j = (j + 1) % 4) {
			vec2 edge = hitbox[i] - hitbox[j];
			normals[i] = normalize({ -edge.y, edge.x });
		}

		for (auto& other : other_hitboxes) {
			for (int i = 0; i < 4; i++) {
				vec2 normal = normals[i];
				// Project each shape onto this normal, and keep min/max for each shape
				float h1_min = dot(normal, hitbox[0]);
				float h1_max = h1_min;
				float h2_min = dot(normal, other[0]);
				float h2_max = h2_min;
				for (int j = 1; j < 4; j++) {
					float p1 = dot(normal, hitbox[j]);
					float p2 = dot(normal, other[j]);
					h1_min = p1 < h1_min ? p1 : h1_min;
					h1_max = p1 > h1_max ? p1 : h1_max;
					h2_min = p2 < h2_min ? p2 : h2_min;
					h2_max = p2 > h2_max ? p2 : h2_max;
				}

				// If mins/maxes don't overlap, the hitboxes don't collide
				if (h2_min > h1_max || h2_max < h1_min) {
					break;
				}
				// If they overlap for all 4 normals, hitboxes collide
				else if (i == 3) {
					return true;
				}
			}
		}
	}
	return false;
}
