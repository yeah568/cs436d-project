// Header
#include "Bullet.hpp"

#include <cmath>
#include <cfloat>

Bullet::Bullet()
  : Sprite(nullptr)
{
  m_scale.x = 0.5f;
  m_scale.y = 0.5f;
  m_position.x = -50;
  m_position.y = 50;
}

void Bullet::update(float ms)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	float BULLET_SPEED;
	if (m_scale.x > 1.1)
		BULLET_SPEED = 3200.f;
	else
		BULLET_SPEED = 800.f;
	float step = BULLET_SPEED * (ms / 1000);
	force nf;
	if (added_forces.size() > 0)
		nf = get_net_force();
	else {
		nf.dir = {0,0};
		nf.mag = 0;
	}
	vec2 full_force = nf.mag * nf.dir;
	printf("%f, %f\n", m_movement_dir.x, full_force.x);
	m_position.x += (m_movement_dir.x + full_force.x)*step;
	m_position.y += (m_movement_dir.y + full_force.y)*step;
	added_forces.clear();
}

force Bullet::get_net_force() {
	force nf;
	vec2 weighted_vec_sum;
	for (auto& f : added_forces) {
		printf("Mag: %f\n",f.mag);
		weighted_vec_sum = weighted_vec_sum + f.mag * f.dir;
	}
	nf.dir = normalize(weighted_vec_sum);
	nf.mag = length(weighted_vec_sum);
	return nf;
}