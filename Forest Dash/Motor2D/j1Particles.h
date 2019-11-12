#ifndef __j1Particles_H__
#define __j1Particles_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1ModuleCollision.h"

#define MAX_ACTIVE_PARTICLES 1

struct SDL_Texture;
struct Collider;
enum COLLIDER_TYPE;

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

	// Destructor
	~j1Particles();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Collisions
	void OnCollision(Collider* c1, Collider* c2);

	// Add particle
	void AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type = COLLIDER_NONE, Uint32 delay = 0);

private:
	// Create particle
	SDL_Texture* graphics = nullptr;
	Particle* active[MAX_ACTIVE_PARTICLES];

public:
	// Projectile
	Particle arrow;

};

#endif