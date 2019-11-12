#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Particles.h"
#include "SDL/include/SDL_timer.h"
#include <math.h>

j1Particles::j1Particles()
{

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;
}

j1Particles::~j1Particles()
{}

bool j1Particles::Start()
{
	
	return true;
}

// Unload assets
bool j1Particles::CleanUp()
{
	App->tex->UnLoad(graphics);

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			delete active[i];
			active[i] = nullptr;
		}
	}

	return true;
}

bool  j1Particles::Update()
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = active[i];

		if (p == nullptr)
			continue;

		if (p->Update() == false)
		{
			delete p;
			active[i] = nullptr;
		}
		else if (SDL_GetTicks() >= p->born)
		{
			
			if (p->fx_played == false)
			{
				p->fx_played = true;
				// Play particle fx here
			}
		}
	}

	return true;
}

void j1Particles::AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type, Uint32 delay)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] == nullptr)
		{
			Particle* p = new Particle(particle);
			p->born = SDL_GetTicks() + delay;
			if (collider_type != COLLIDER_NONE)
				p->collider = App->collision->AddCollider(p->anim.GetCurrentFrame(), collider_type, this);
			active[i] = p;
			break;
		}
	}
	//active[last_particle++] = p;                   we arenot sure why this was here
}

void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			delete active[i];
			active[i] = nullptr;
			if (c1->type == COLLIDER_PLAYER_SHOT && c2->type == COLLIDER_ENEMY)
			{

			}
			if (c1->type == COLLIDER_ENEMY_SHOT && c2->type == COLLIDER_PLAYER)
			{

			}
			break;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), position(p.position), speed(p.speed),
	fx(p.fx), born(p.born), life(p.life)
{}

Particle::~Particle()
{
	if (collider != nullptr)
		collider->to_delete = true;
}

bool Particle::Update()
{
	bool ret = true;

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life)
			ret = false;
	}
	else
		if (anim.Finished())
			ret = false;

	position.x += speed.x;
	position.y += speed.y;
	collider->SetPos(position.x, position.y);

	return ret;
}

