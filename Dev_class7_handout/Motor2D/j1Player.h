#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2DynArray.h"

#define SPEED_X 4
#define SPEED_Y 4
#define GRAVITY 2

struct SDL_Texture;
struct Collider;

class j1Player : public j1Module
{
public:
	j1Player();
	~j1Player();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	void Movement();
	void CheckCollision();
	void DrawHitbox();



public:

	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;
	Animation idle;
	Animation up;
	Animation down;
	iPoint position;
	bool Canright = true;
	bool Canleft = true;
	bool Canjump = true;
	bool Candown = true;
	int playerHeight = 50;
	int playerWidth = 20;
	int playerCentre = 10;

};

#endif
