#ifndef __j1PARTICLES_H__
#define __j1PARTICLES_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1ModuleCollision.h"

#define MAX_ACTIVE_PARTICLES 10

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
	int angle = 0;
	bool fx_played = false;

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool Update(float);
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
	void AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type = COLLIDER_NONE, Uint32 delay = 0, int SpeedX = 1, int SpeedY = 1, int angle = 0, float dt = 0.033f);

private:
	// Create particle
	SDL_Texture* graphics = nullptr;
	Particle* active[MAX_ACTIVE_PARTICLES];

public:
	// Projectile
	Particle arrow;

};

#endif