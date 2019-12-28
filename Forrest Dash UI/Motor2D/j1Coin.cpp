#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Coin.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "Animation.h"
#include "j1Hud.h"
#include "j1ModuleCollision.h"
#include "J1EntityManager.h"
#include "j1Pathfinding.h"
#include "Brofiler/Brofiler.h"
#include "J1Console.h"

j1Coin::j1Coin(int posx, int posy) : j1Entity(Types::coin)
{
	name.create("coin");

	current_animation = NULL;
	LoadAnimations("textures/coin_animations.tmx");

	state = COIN_IDLE;
	// Load animations from an animations list ----------------------------------------------
	p2List_item<Animation>* animation_iterator = animations.start;

	idle = animation_iterator->data;
	animation_iterator = animation_iterator->next;


	initialPosition.x = posx;
	position.x = posx;
	position.y = posy;

	r.w = 40;
	r.h = 50;
	r.x = position.x;
	r.y = position.y;

}

j1Coin::~j1Coin()
{

}

bool j1Coin::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;

}
bool j1Coin::Start()
{
	LOG("Start Coins");
	App->console->write("Start Coins");

	App->EntityManager->cointex;
	to_delete = false;
	coinpickedfx = false;
	pickedup = false;
	EntityCollider = App->collision->AddCollider(&r, COLLIDER_COIN, this);
	flip = SDL_FLIP_NONE;
	return true;
}
bool j1Coin::Update(float dt)
{
	BROFILER_CATEGORY("Update_coin", Profiler::Color::LightYellow);
	setAnimation();

	if (EntityCollider != NULL)
		DrawHitbox();

	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->EntityManager->cointex, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);


	return true;
}


bool j1Coin::PostUpdate(float dt)
{
	return true;

}

void j1Coin::setAnimation()
{	
	if (state == COIN_IDLE)
	{
		current_animation = &idle;
		coinpickedfx = false;
	}

	if (state == COIN_PICKED)
	{
		current_animation = &idle;
		if (!coinpickedfx)
			App->audio->PlayFx(App->audio->coinpickupFx);
		coinpickedfx = true;
		state = COIN_IDLE;
	}

}


void j1Coin::OnCollision(Collider* c1, Collider* c2) {

	if (c2->type == COLLIDER_PLAYER) {
		App->hud->score += 10;
		EntityCollider->to_delete = true;
		to_delete = true;
		state = COIN_PICKED;
	}

}
