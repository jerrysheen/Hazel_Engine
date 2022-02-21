#include "ParticleSystem.h"

#include "glm/gtc/constants.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/compatibility.hpp"

ParticleSystem::ParticleSystem()
{
	m_ParticlePool.resize(1000);
}

void ParticleSystem::Update(Hazel::Timestep ts)
{
	// update里面其实就是每次都去减少射出来的粒子
	// 如果池子里面的某个粒子本来就是不激活的，就不去调整
	// 如果他的lifeTime已经为0了，那么就设为false
	for (auto& particle : m_ParticlePool) {
		if (particle.Active == false) continue;
		if (particle.LifeRemaining < 0.0f) {
			particle.Active = false;
			continue;
		}
		particle.LifeRemaining -= ts;
		particle.Position += particle.Velocity * (float)ts ;
		particle.Rotation += 0.01 * ts;
	}
}

void ParticleSystem::Render()
{
	for (auto& particle : m_ParticlePool) 
	{
		if (!particle.Active) continue;

		// fade away particles
		float life = particle.LifeRemaining / particle.LifeTime;
		glm::vec4 color = glm::lerp(particle.ColorBegin, particle.ColorEnd, life);
		color.a = color.a * life;

		float size = glm::lerp(particle.SizeBegin, particle.SizeEnd, life);

		Hazel::Renderer3D::DrawQuad(particle.Position, {size, size}, color);
	}
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
	Particle& particle = m_ParticlePool[m_PoolIndex];
	particle.Active = true;
	particle.Position = particleProps.Position;
	particle.Rotation = Hazel::Random::Float() * 2.0f * glm::pi<float>();

	// Speed
	particle.Velocity = particleProps.Velocity;
	particle.Velocity.x += particleProps.VelocityVariation.x * (Hazel::Random::Float() - 0.5f);
	particle.Velocity.y += particleProps.VelocityVariation.y * (Hazel::Random::Float() - 0.5f);

	// Color
	particle.ColorBegin = particleProps.ColorBegin;
	particle.ColorEnd = particleProps.ColorEnd;

	particle.LifeTime = particleProps.LifeTime;
	particle.LifeRemaining = particleProps.LifeTime;
	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Hazel::Random::Float() - 0.5f);
	particle.SizeEnd = particleProps.SizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}
