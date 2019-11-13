#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "Animation.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2DynArray.h"



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
	DEATH,
	DASH_L,
	DASH_R
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
	void Movement(float dt);
	void CheckCollision(float dt);
	void StateMachine(float dt);
	void DrawHitbox();
	void Camera();
	void MoveCondition(float dt);
	void LoadAnimations(const char* path);
	void playfx(const int id, const int rep);
	void OnCollision(Collider* c1, Collider* c2);


private:


	player_state state;
	player_state prev_state = IDLE;
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
	Animation dash;
	
	bool Canright = true;
	bool Canleft = true;
	bool Canjump = true;
	bool Candown = true;
	bool shoot = false;
	int playerHeight;
	int playerWidth;
	int playerCentre;
	float jumpSpeed;
	p2SString		moveFx;
	p2SString		deathFx;
	p2SString		jumpFx;
	p2SString		winFx;
	p2SString		dashFx;
	pugi::xml_document	player_file;
	TileSetPlayer TileSetData;
	int gravity;
	int speedX;
	int speedY;
	int dashspeed;
	float acceleration;
	float speedBar;

public:	
	bool death = false;
	bool Godmode = false;
	iPoint position;
	iPoint initialPosition;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	float BarWidth = 40;
	float magnitude = 0.0f;
	int opacity = 0;
	int maxBarWidth;
	Collider *ColliderPlayer;

};

#endif