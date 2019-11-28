#ifndef __j1PlAYER_H__
#define __j1PlAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "j1Entity.h"
#include "Animation.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2DynArray.h"

#define DT_CONVERTER 60

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
	DASH_R,
	AIMING,
	ATTACK,
	ADJUST
};



class j1Player : public j1Entity
{
public:
	j1Player(int posx, int posy);
	~j1Player();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
//	bool CleanUp();
//	bool Load(pugi::xml_node&);
//	bool Save(pugi::xml_node&) const;
	void Movement(float dt);
	void CheckCollision(float dt);
	void StateMachine(float dt);
//	void DrawHitbox();
	void Camera();
	void MoveCondition(float dt);
//	void LoadAnimations(const char* path);
	void OnCollision(Collider* c1, Collider* c2);
	


private:

	player_state state;
	player_state prev_state = IDLE;

	bool charging = false;
	bool playedJumpFx = false;
	bool playeDeadFx = false;
	bool playeDashFx = false;
	bool playedWinFx = false;
	bool collider = true;
	

	Animation idle;
	Animation forward;
	Animation backward;
	Animation crouch;
	Animation up;
	Animation down;
	Animation dead;
	Animation dash;
	Animation aiming;
	Animation bow;
	Animation swordAttack;
	SDL_Rect aimbar;
	
	int DeathTimer = 0;
	int AimTimer = 0;
	int playerHeight;
	int playerWidth;
	int playerCentre;
	int animationStart = 0;
	int numCollider = 0;

	p2SString		moveFx;
	p2SString		deathFx;
	p2SString		jumpFx;
	p2SString		winFx;
	p2SString		dashFx;

	float dashspeed;
	float acceleration;
	float speedBar;
	float angle;

	Animation* current_animation_bow = nullptr;

public:	

	
	Collider* colliderAttack;

	bool death = false;
	iPoint initialPosition;
	
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_RendererFlip flip_bow = SDL_FLIP_NONE;
	
	float BarWidth = 40;
	float aimbarw = 0;
	float magnitude = 0.0f;
	
	int opacity = 0;
	int maxBarWidth;

	float yvec = 0;
	float xvec = 0;



};

#endif
