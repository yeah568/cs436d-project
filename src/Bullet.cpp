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
  velocity = {0,-1};
}

void PlayerBullet::update(float ms)
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
	velocity = velocity + step * nf.mag  * nf.dir;
	//printf("Fullforce x: %f y: %f", full_force.x, full_force.y);
	//printf("%f, %f\n", m_movement_dir.x, full_force.x);
	
	m_rotation = -(float)atan2(m_movement_dir.x + velocity.x, m_movement_dir.y + 2*velocity.y) + 3.14/2;
	//printf("\n rotation: %f %f", m_movement_dir.x, m_movement_dir.y);
	m_position.x += (velocity.x)*step;
	// TODO: when the bullet was above the BH the fullforce was equal to the movement_dir so i added a 2 for no so that they don't just stop
	m_position.y += (0*m_movement_dir.y + velocity.y)*step;
	added_forces.clear();
}

void EnemyBullet::update(float ms)
{
	float step =  (ms / 1000);
	m_position = m_position + 200*step * m_movement_dir;
}

force PlayerBullet::get_net_force() {
	force nf;
	vec2 weighted_vec_sum = { 0,0 };
	for (auto& f : added_forces) {
		//printf("Direction: %f\n",f.dir.x);
		weighted_vec_sum = weighted_vec_sum + (f.mag * f.dir);
	}
	
	nf.dir = normalize(weighted_vec_sum);
	//printf("Normalized force direction %f",nf.dir.x);
	nf.mag = length(weighted_vec_sum);
	return nf;
}