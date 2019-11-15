#ifndef __j1WIZARD_H__
#define __j1WIZARD_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
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
};

struct TileSetWizard
{
	SDL_Rect GetAnimRect(int id) const;
	int tile_width;
	int tile_height;
	int firstgid;
	int num_tiles_width;
	int tex_width;
	p2SString Texname;
};

class j1Wizard : public j1Module
{
public:

	j1Wizard();
	~j1Wizard();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
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

	TileSetWizard TileSetData;
	wizard_state state;
public:

	bool wizarDead = false;
	iPoint position;
	iPoint initialPosition;
	SDL_RendererFlip flip = SDL_FLIP_NONE;

};

#endif
