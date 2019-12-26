#ifndef __j1COIN_H__
#define __j1COIN_H__

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
enum coin_state
{
	COIN_IDLE = 0,
	COIN_PICKED,
};
class j1Coin : public j1Entity {
public:

	j1Coin(int posx, int posy);
	~j1Coin();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	void OnCollision(Collider* c1, Collider* c2);
	void setAnimation();

private:

	Animation idle;
	bool coinpickedfx = false;
	int rangeX;
	int rangeY;

	coin_state state;

public:
	bool pickedup = false;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Rect r;


};
#endif