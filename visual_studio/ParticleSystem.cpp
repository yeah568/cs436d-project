#include "ParticleSystem.hpp"



ParticleSystem::ParticleSystem(int max_particles)
{
	m_max_particles = max_particles;
	m_particle_pool = new Particle[max_particles];
}

ParticleSystem::~ParticleSystem()
{
	delete[] m_particle_pool;
}

void ParticleSystem::update(float dt) {

}