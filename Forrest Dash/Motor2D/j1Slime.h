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
	void OnCollision(Collider* c1, Collider* c2);
	bool Pathfinding(float dt);
	void CheckCollision(float dt);
	void DrawHitbox();
	void Movement(float dt);
	void setAnimation();

private:
	
	Animation idle;
	Animation death;
	Animation forward;
	bool pathFinding = false;
	bool playedSlimeDeathFx = false;
	int rangeX;
	int rangeY;
	
	slime_state state;

public:
	bool candown = false;
	bool slimeDead = false;
	int deathTimerSlime = 0;
	int startMoving = 0;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Rect r;
};

#endif
