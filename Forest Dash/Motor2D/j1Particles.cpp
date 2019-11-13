#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "SDL/include/SDL_timer.h"
#include <math.h>

j1Particles::j1Particles()
{
	name.create("particles");

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;
}

j1Particles::~j1Particles()
{}

bool j1Particles::Awake(pugi::xml_node& config)
{
	

	return true;
}

bool j1Particles::Start()
{
	LOG("Loading particles");

	// Load spritesheet
	graphics = App->tex->Load("textures/icepick.png");

	// Arrow
	arrow.anim.PushBack({ 4, 26, 62, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 67, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 132, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 198, 26, 58, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 260, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 323, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 386, 26, 62, 14 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 449, 26, 63, 14 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 512, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 576, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 640, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 704, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 772, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 835, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 961, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1028, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1092, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1155, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1155, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1219, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1283, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1348, 26, 60, 14 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1477, 26, 59, 12 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1540, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1603, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1667, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1733, 26, 59, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1796, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	arrow.anim.PushBack({ 1860, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	arrow.life = 2000;
	arrow.speed.x = 8;
	arrow.anim.loop = true;

	return true;
}

// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");

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

bool j1Particles::Update(float dt)
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
			App->render->Blit(graphics, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));
			if (p->fx_played == false)
			{
				p->fx_played = true;
				//App->audio->PlayFx(p->fx);
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
			p->position.x = x;
			p->position.y = y;
			if (collider_type != COLLIDER_NONE)
				p->collider = App->collision->AddCollider(p->anim.GetCurrentFrame(), collider_type, this);
			active[i] = p;
			break;
		}
	}
}

void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			/*if (c1->type == COLLIDER_PLAYER_SHOT && c2->type == COLLIDER_WALL)
			{
				App->audio->PlayFx(4, 0);
				
			}

			if (c1->type == COLLIDER_ENEMY_SHOT && c2->type == COLLIDER_PLAYER)
			{

				
			}*/
			break;
		}
	}
}


// -------------------------------------------------------------
// -------------------------------------------------------------

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

