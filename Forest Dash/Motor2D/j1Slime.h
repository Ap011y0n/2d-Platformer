#ifndef __j1SLIME_H__
#define __j1SLIME_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "Animation.h"
#include "j1Entity.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2DynArray.h"

struct SDL_Texture;
struct Collider;
struct Anim;

enum slime_state
{
	SLIME_IDLE = 0,
	SLIME_DEATH,
	SLIME_FORWARD,
	SLIME_FALLING
};


class j1Slime : public j1Entity
{
public:

	j1Slime(int posx, int posy);
	~j1Slime();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
//	bool CleanUp();
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;
//	void LoadAnimations(const char* path);
	void OnCollision(Collider* c1, Collider* c2);
	bool Pathfinding(float dt);
	void CheckCollision(float dt);
	void DrawHitbox();

private:
	void Movement(float dt);
	void setAnimation();
	

	Animation* current_animation = nullptr;
	Animation idle;
	Animation death;
	Animation forward;
	bool pathFinding = false;
	bool playedSlimeDeathFx = false;
	
	
	slime_state state;

public:
	bool candown = false;
	bool slimeDead = false;
	int deathTimerSlime = 0;
	int startMoving = 0;
	iPoint initialPosition;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Rect r;
};

#endif
