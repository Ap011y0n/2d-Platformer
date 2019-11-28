#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "Animation.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Timer.h"


#define DT_CONVERTER 60

struct SDL_Texture;
struct Collider;
struct Anim;


struct TileSetPlayer
{
	SDL_Rect GetAnimRect(int id) const;
	int tile_width;
	int tile_height;
	int firstgid;
	int num_tiles_width;
	int tex_width;
	p2SString Texname;
};

class j1Entity : public j1Module
{

public:

	enum class Types
	{
		player,
		slime,
		wizard,
		projectile_player,
		unknown
	};

	Types type;

	j1Entity(Types type);

	virtual ~j1Entity();

	bool CleanUp();
	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

	void LoadAnimations(const char* path);
	void DrawHitbox();
	void playfx(const int id, const int rep);
	//void OnCollision(Collider* c1, Collider* c2);

protected:

	int gravity;
	int speedX;
	int speedY;
	Animation* current_animation = nullptr;
	p2List<Animation> animations;
	bool Canright = true;
	bool Canleft = true;
	bool CandashR = true;
	bool CandashL = true;
	bool Canjump = true;
	bool Candown = true;
	bool shoot = false;
	bool adjust = false;
	float jumpSpeed;

public:
	iPoint position;
	Collider *EntityCollider;
	bool Godmode = false;
	bool to_delete = false;
	pugi::xml_document	entity_file;
	TileSetPlayer TileSetData;
	bool death = false;
};

#endif
