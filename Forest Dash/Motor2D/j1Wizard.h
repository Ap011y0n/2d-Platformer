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
	WD_FORWARD
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
//	bool CleanUp();
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;
	//void LoadAnimations(const char* path);
	void OnCollision(Collider* c1, Collider* c2);
	bool Pathfinding(float dt);

private:

	void setAnimation(float);
	void Movement();

	Animation* current_animation = nullptr;

	Animation idle;
	Animation death;
	Animation forward;

	int move;

	bool patrol;
	bool playedWizarDeathFx = false;
	
	wizard_state state;
	
	const p2DynArray<iPoint>* path;

public:

	bool wizarDead = false;
	bool pathFinding = false;
	int deathTimerWizard = 0;
	int collided;
	iPoint initialPosition;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Rect r;
};

#endif
