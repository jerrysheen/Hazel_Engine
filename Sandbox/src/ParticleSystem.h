#pragma once
#include "Hazel.h"
#include "glm/glm.hpp"

struct ParticleProps  {
	glm::vec2 Position;
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime;
};
class ParticleSystem {
public:
	ParticleSystem();
	void Update(Hazel::Timestep ts);
	void Render();

	void Emit(const ParticleProps& particleProps);

private:

	struct Particle {
		glm::vec2 Position;
		glm::vec2 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float SizeBegin, SizeEnd;
		float Rotation = 0.0f;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;

	};

	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex = 999;

	uint32_t m_QuadVA = 0;
	//Hazel::Ref<Hazel::Shader> m_ParticleShader;
	uint32_t m_ParticleShaderViewProj = 0, m_ParticleShaderTransform = 0, m_ParticleShaderColor = 0;
};