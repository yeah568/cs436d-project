#include <cmath>
#include "Sprite.hpp"

Sprite::Sprite(Texture* texture)
  : m_texture(texture) {
}

Sprite::Sprite(const vec2& position, const vec2& scale, float rotation, Texture* texture)
  : m_position(position),
    m_scale(scale),
    m_rotation(rotation),
    m_texture(texture) {
}

bool Sprite::init()
{
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
  float color[] = { 1.f, 1.f, 1.f };
  glUniform3fv(color_uloc, 1, color);
  glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

  // Drawing!
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
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

bbox Sprite::get_bounding_box() const
{
  // fabs is to avoid negative scale due to the facing direction
	float width = std::fabs(m_scale.x) * m_texture->width;
	float height = std::fabs(m_scale.y) * m_texture->height;
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
}

void Sprite::set_texture(Texture* texture)
{
  m_texture = texture;
}
