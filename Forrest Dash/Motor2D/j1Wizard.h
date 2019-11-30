#ifndef __j1WIZARD_H__
#define __j1WIZARD_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "j1Entity.h"
#include "Animation.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2DynArray.h"

struct SDL_Texture;
struct Collider;
struct Anim;

enum wizard_state
{
	WD_IDLE = 0,
	WD_DEATH,
	WD_FORWARD,
	WD_PATHFINDING
};


class j1Wizard : public j1Entity
{
public:

	j1Wizard(int posx, int posy);
	~j1Wizard();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	void OnCollision(Collider* c1, Collider* c2);
	bool Pathfinding(float dt);
	void setAnimation(float);
	void Movement();

private:


	Animation idle;
	Animation death;
	Animation forward;

	int move;
	int  i = 0, j = 0;
	bool patrolr=false;
	bool patroll=false;
	bool playedWizarDeathFx = false;
	int rangeX;
	int rangeY;

	wizard_state state;
	
	const p2DynArray<iPoint>* path;

public:

	bool wizarDead = false;
	bool pathFinding = false;
	int deathTimerWizard = 0;
	int collided;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Rect r;
};

#endif
