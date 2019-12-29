#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1ProjPlayer.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "Animation.h"
#include "j1ModuleCollision.h"
#include "J1EntityManager.h"
#include "Brofiler/Brofiler.h"
#include "J1Console.h"




j1ProjPlayer::j1ProjPlayer(int posx, int posy, int speedx, int speedy, float anglepar) : j1Entity(Types::projectile_player)
{
	name.create("Projectile Player");

	current_animation = NULL;
	born = SDL_GetTicks();
	position.x = posx;
	position.y = posy;
	speed.x = speedx;
	speed.y = speedy;
	angle = anglepar;

}

j1ProjPlayer::~j1ProjPlayer()
{

}


bool j1ProjPlayer::Awake(pugi::xml_node& config)

{
	bool ret = true;
	return ret;

}



// Prepare animations ----------------------------------------------
bool j1ProjPlayer::Start()
{
	LOG("Start Projectile");
	App->console->write("Start Projectile");

	anim.PushBack({ 4, 26, 62, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 67, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 132, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 198, 26, 58, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 260, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 323, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 386, 26, 62, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 449, 26, 63, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 512, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 576, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 640, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 704, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 772, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 835, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 961, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1028, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1092, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1155, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1155, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1219, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1283, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1348, 26, 60, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1477, 26, 59, 12 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1540, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1603, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1667, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1733, 26, 59, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1796, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1860, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	life = 2000;

	anim.loop = true;
	to_delete = false;

	SDL_Rect r;
	r.w = 10;
	r.h = 10;
	r.x = position.x;
	r.y = position.y;
	
	EntityCollider = App->collision->AddCollider(&r, COLLIDER_PLAYER_SHOT, this);

	
	return true;
}



// Update:----------------------------------------------
bool j1ProjPlayer::Update(float dt)
{
	BROFILER_CATEGORY("Update_Projectile", Profiler::Color::CornflowerBlue);
	Movement(dt);
	setAnimation();
	CheckCollision(dt);
	DrawHitbox();
	App->render->Blit(App->EntityManager->icespiketex, position.x, position.y, &(anim.GetCurrentFrame(dt)), 1, 1, SDL_FLIP_NONE, angle);

	return true;
}


bool j1ProjPlayer::PostUpdate(float dt)
{
	return true;
}

//Calculates arrow movement
bool j1ProjPlayer::Movement(float dt)
{
	bool ret = false;

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life) {
			to_delete = true;

			return ret;
		}
	}
	else
		if (anim.Finished())
			return ret;

	position.x += speed.x * DT_CONVERTER* dt;
	position.y += speed.y * DT_CONVERTER* dt;
	ret = true;
	return ret;

}

//Sets an animation and plays some audio efects
void j1ProjPlayer::setAnimation()
{
	current_animation = &anim;

	//Play arrow FX
	if (!playedArrowFx)
		App->audio->PlayFx(App->audio->arrowFx);
	playedArrowFx = true;
	
}


 

//Destroy the particle when colliding with an enemy
void j1ProjPlayer::OnCollision(Collider* c1, Collider* c2) {

	
	if (c1->type == COLLIDER_PLAYER_SHOT && c2->type != COLLIDER_COIN) {
		c1->to_delete = true;
		to_delete = true;
	}
	
}

//Checks if we collided with a map collider, if so, destroy the particle ----------------------------------------------
void j1ProjPlayer::CheckCollision(float dt) {
	p2List_item<MapLayer*>* layer_iterator = App->map->data.layers.start;
	MapLayer* layer = App->map->data.layers.start->data;
	bool ret = true;

	while (ret == true && layer_iterator != NULL) {
		layer = layer_iterator->data;
		if (layer->returnPropValue("Navigation") == 1) {
			iPoint coord = App->map->WorldToMap(position.x, position.y);
			if (layer->Get(coord.x, coord.y) != 0) {
				EntityCollider->to_delete = true;
				to_delete = true;
				ret = false;
			}
		}
		layer_iterator = layer_iterator->next;
	}
}