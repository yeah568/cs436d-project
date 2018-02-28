#pragma once

#include "common.hpp"

class Sprite : public Renderable
{
public:
  Sprite(Texture* texture);
  Sprite(const vec2& position, const vec2& scale, float rotation, Texture* texture);

  bool init();
  void destroy();
  void draw(const mat3& projection) override;

  const vec2& get_position() const;
  void set_position(const vec2& position);

  const vec2& get_scale() const;
  void set_scale(const vec2& scale);

  float get_rotation() const;
  void set_rotation(float rotation);

  bbox get_bounding_box() const;

  void set_texture(Texture* texture);

  void set_color(float r, float g, float b) {m_color[0]=r;m_color[1]=g;m_color[2]=b;};

protected:
  vec2 m_position;
  vec2 m_scale;
  float m_rotation; 
  Texture* m_texture;
  float m_color[3];
};
