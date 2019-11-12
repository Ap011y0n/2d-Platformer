#ifndef __j1Particles_H__
#define __j1Particles_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1ModuleCollision.h"

struct SDL_Texture;
#define MAX_ACTIVE_PARTICLES 1000

struct SDL_Texture;

struct Particle
{
	Collider* collider = nullptr;
	Animation anim;
	uint fx = 0;
	iPoint position;
	iPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;
	bool fx_played = false;

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool Update();
};

class j1Particles : public j1Module
{
public:
	j1Particles();
	~j1Particles();

	bool Start();
	bool Update();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

	void AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type = COLLIDER_NONE, Uint32 delay = 0);

private:

	SDL_Texture* graphics = nullptr;
	Particle* active[MAX_ACTIVE_PARTICLES];
	uint last_particle = 0;

public:

	Particle arrow;

};

#endif