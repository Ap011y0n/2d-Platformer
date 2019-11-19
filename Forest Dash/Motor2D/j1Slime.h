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
};

struct TileSetSlime
{
	SDL_Rect GetAnimRect(int id) const;
	int tile_width;
	int tile_height;
	int firstgid;
	int num_tiles_width;
	int tex_width;
	p2SString Texname;
};

class j1Slime : public j1Entity
{
public:

	j1Slime(int posx, int posy, char* tag);
	~j1Slime();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;
	void LoadAnimations(const char* path);
	void OnCollision(Collider* c1, Collider* c2);


private:

	void setAnimation();

	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;
	p2List<Animation> animations;
	Animation idle;
	Animation death;

	pugi::xml_document	slime_file;

	TileSetSlime TileSetData;
	slime_state state;

public:

	Collider* colliderSlime;
	bool slimeDead = false;
	int deathTimerSlime = 0;
	iPoint initialPosition;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Rect r;
};

#endif
