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

bool Bullet::init(float dmg, float spd)
{
	m_damage = dmg;
	m_speed = spd;
	return Sprite::init();
}

void Bullet::update(float ms)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	float step = m_speed * (ms / 1000);
	
	m_position.x += m_movement_dir.x*step;
	m_position.y += m_movement_dir.y*step;
}

float Bullet::get_damage()
{
	return m_damage;
}

void Bullet::set_velocity(vec2 v) {
	velocity = v;
}

void Bullet::set_damage(float dmg)
{
	m_damage = dmg;
}

float Bullet::get_speed()
{
	return m_speed;
}

void Bullet::set_speed(float spd)
{
	m_speed = spd;
}

void PlayerBullet::update(float ms)
{
	float step = m_speed * (ms / 1000);
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
	vec2 displacement = step * velocity;
	m_rotation = (float)atan2(displacement.y, displacement.x);//+ 3.14/2;
	//printf("\n rotation: %f %f", m_movement_dir.x, m_movement_dir.y);
	m_position = m_position + displacement;
	added_forces.clear();
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
