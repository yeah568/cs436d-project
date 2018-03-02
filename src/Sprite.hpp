#pragma once

#include "common.hpp"
#include "Hitbox.hpp"

#include <vector>
#include <array>

class Sprite : public Renderable
{
public:
  Sprite(Texture* texture);
  Sprite(const vec2& position, const vec2& scale, float rotation, Texture* texture);

  bool init();
  void destroy();
  void draw(const mat3& projection) override;
  void draw_hitboxes(const mat3& projection);

  const vec2& get_position() const;
  void set_position(const vec2& position);

  const vec2& get_scale() const;
  void set_scale(const vec2& scale);

  float get_rotation() const;
  void set_rotation(float rotation);

  bbox get_bounding_box() const;

  bbox get_hitbox_bbox(std::vector<std::array<vec2, 4>> hitboxes) const;

  void set_texture(Texture* texture);

  void set_color(float r, float g, float b) {m_color[0]=r;m_color[1]=g;m_color[2]=b;};

  bool collides_with(const Sprite& other);

  std::array<vec2, 4> get_vertices() const;

  std::vector<Hitbox*> get_hitboxes() const;

  std::vector<std::array<vec2, 4>> get_hitbox_points() const;

private:
  bool hitboxes_collide(std::vector<std::array<vec2, 4>> hitboxes, std::vector<std::array<vec2, 4>> other_hitboxes);

  bool hitbox_sat_collision(std::vector<std::array<vec2, 4>> hitboxes, std::vector<std::array<vec2, 4>> other_hitboxes);


protected:
  vec2 m_position;
  vec2 m_scale;
  float m_rotation; 
  Texture* m_texture;
  float m_color[3];
  // Hitboxes represented as a transformation vector from the sprite
  std::vector<Hitbox*> m_hitboxes;
};
