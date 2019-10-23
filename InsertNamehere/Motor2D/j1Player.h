#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "Animation.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2DynArray.h"


#define SPEED_X 7
#define SPEED_Y 22

struct SDL_Texture;
struct Collider;
struct Anim;

enum player_state
{
	IDLE = 0,
	IDLE_FLIP,
	FORWARD,
	BACKWARD,
	CROUCH,
	JUMP,
	FALLING,
	DEATH
};
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


class j1Player : public j1Module
{
public:
	j1Player();
	~j1Player();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	void Movement();
	void CheckCollision();
	void setAnimation();
	void DrawHitbox();
	void Camera();
	void MoveCondition();
	void LoadAnimations(const char* path);



public:

	player_state state;
	int DeathTimer = 0;
	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;
	p2List<Animation> animations;
	Animation idle;
	Animation forward;
	Animation backward;
	Animation crouch;
	Animation up;
	Animation down;
	Animation dead;
	iPoint position;
	bool Canright = true;
	bool Canleft = true;
	bool Canjump = true;
	bool Candown = true;
	bool Godmode = false;
	bool death = false;
	bool playChannel = true;
	bool stopChannel = true;
	int playerHeight = 50;
	int playerWidth = 20;
	int playerCentre = 10;
	float jumpSpeed = -1*SPEED_Y;
	p2SString		moveFx;
	p2SString		deathFx;
	p2SString		jumpFx;
	p2SString		winFx;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	float BarWidth = 40;
	pugi::xml_document	player_file;
	TileSetPlayer TileSetData;
	int gravity;
};

#endif
