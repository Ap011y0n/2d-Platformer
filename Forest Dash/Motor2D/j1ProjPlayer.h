#pragma once
#ifndef __j1PROJPLAYER_H__
#define __j1PROJPLAYER_H__

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




class j1ProjPlayer : public j1Entity
{
public:

	j1ProjPlayer(int posx, int posy, int speedx, int speedy, float angle);
	~j1ProjPlayer();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	void OnCollision(Collider* c1, Collider* c2);


private:
	bool Movement(float dt);
	void setAnimation();


	

public:

	Animation anim;
	uint fx = 0;
	iPoint position;
	iPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;
	int angle = 0;
	bool fx_played = false;
};

#endif
