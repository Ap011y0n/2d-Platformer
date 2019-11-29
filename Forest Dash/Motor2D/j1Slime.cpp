#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Slime.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "Animation.h"
#include "j1ModuleCollision.h"
#include "j1Particles.h"
#include "J1EntityManager.h"
#include "j1Pathfinding.h"
#include "Brofiler/Brofiler.h"




j1Slime::j1Slime(int posx, int posy) : j1Entity(Types::slime)
{
	name.create("slime");

	current_animation = NULL;
	LoadAnimations("textures/slime_animations.tmx");
	state = SLIME_IDLE;

	// Load animations from an animations list ----------------------------------------------
	p2List_item<Animation>* animation_iterator = animations.start;

	idle = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	death = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	death.loop = false;

	forward = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	initialPosition.x = posx;
	position.x = posx;
	position.y = posy;

	r.w = 40;
	r.h = 50;
	r.x = position.x;
	r.y = position.y;

	
}

j1Slime::~j1Slime()
{

}


bool j1Slime::Awake(pugi::xml_node& config)

{

	bool ret = true;



	return ret;

}



// Load assets ----------------------------------------------
bool j1Slime::Start()
{
	LOG("Start Slime");
	App->EntityManager->slimeTex;
	to_delete = false;
	playedSlimeDeathFx = false;
	deathTimerSlime = 0;
	slimeDead = false;
	EntityCollider = App->collision->AddCollider(&r, COLLIDER_ENEMY, this);
	flip = SDL_FLIP_NONE;
	startMoving = SDL_GetTicks();
	pathFinding = true;
	return true;
}

// Unload assets ----------------------------------------------


// Update: draw background ----------------------------------------------
bool j1Slime::Update(float dt)
{	
	BROFILER_CATEGORY("Update_Slime", Profiler::Color::Green);
	CheckCollision(dt);
	Movement(dt);
	setAnimation();
	DrawHitbox();
	flip = SDL_FLIP_HORIZONTAL;
	
		if (App->EntityManager->GetPlayer()->position.x > position.x - 200 && App->EntityManager->GetPlayer()->position.x < position.x + 200 && App->EntityManager->GetPlayer()->position.y + 100 && App->EntityManager->GetPlayer()->position.y - 100)
			if (pathFinding && !App->EntityManager->GetPlayer()->is_death)Pathfinding(dt);
	
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->EntityManager->slimeTex, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);
	
	
	return true;
}


bool j1Slime::PostUpdate(float dt)
{
	return true;

}

// Load Game State ----------------------------------------------
//bool j1Slime::Load(pugi::xml_node& data)
//{
//	
//	return true;
//}
//
//// Save Game State ----------------------------------------------
//bool j1Slime::Save(pugi::xml_node& data) const
//{
//	
//	return true;
//}


void j1Slime::Movement(float dt)
{
	if (slimeDead) state = SLIME_DEATH;

	if (candown && dt < 0.05)
	{
		position.y += 2 * DT_CONVERTER * dt * GRAVITY;

	}

}

void j1Slime::setAnimation()
{
	if(state == SLIME_IDLE)
	{
		current_animation = &idle;
		death.Reset();
	}
	if (state == SLIME_DEATH)
	{
		current_animation = &death;

		if (!playedSlimeDeathFx)
			App->audio->PlayFx(App->audio->slimeDeathFx);
		playedSlimeDeathFx = true;

		if (SDL_GetTicks() > (deathTimerSlime + 2500)) {
			
			
			to_delete = true;

		}
	}
	if (state == SLIME_FORWARD)
	{
		current_animation = &forward;
	}


}


void j1Slime::OnCollision(Collider* c1, Collider* c2) {


	if (c2->type == COLLIDER_PLAYER_SHOT) {
		
		slimeDead = true;
		EntityCollider->to_delete = true;
		deathTimerSlime = SDL_GetTicks();

	}
	if (c2->type == COLLIDER_PLAYER) {

		if (position.x <= c2->rect.x)
		{
			position.x -= 50;
			
		}
		else
		{
			position.x += 50;

		}

	}

}

bool j1Slime::Pathfinding(float dt) {

	static iPoint origin;
	static bool origin_selected = false;
	int x, y;
	iPoint rightCell(position.x - 1, position.y);
	iPoint leftCell(position.x + 1, position.y);
	iPoint upCell(position.x , position.y+1);
	iPoint DownCell(position.x, position.y-1);
	App->input->GetMousePosition(x, y);
	iPoint p = App->EntityManager->GetPlayer()->position;
	p = App->map->WorldToMap(p.x + 30, p.y + 30);

	origin = App->map->WorldToMap(position.x, position.y);
	App->pathfinding->CreatePath(origin, p);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
	if (path->At(1) != NULL)
	{
		if (state != SLIME_DEATH)
		{	
			if (candown == false)
			{
				if (path->At(1)->x < origin.x && !App->pathfinding->IsWalkable(DownCell)) {
					position.x -= 2 * DT_CONVERTER * dt;
					flip = SDL_FLIP_HORIZONTAL;
				}
				if (path->At(1)->x > origin.x && !App->pathfinding->IsWalkable(rightCell)) {
					position.x += 2 * DT_CONVERTER * dt;
					flip = SDL_FLIP_NONE;
				}
			}

		}
	

	}
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint nextPoint = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		if (App->collision->debug)
		{
			App->render->Blit(App->scene->debug_tex, nextPoint.x, nextPoint.y);
		}
	}

	return true;
}

void j1Slime::CheckCollision(float dt) {


	candown = true;
	is_death = false;


	bool ret = true;
	iPoint coord;
	p2List_item<MapLayer*>* layer_iterator = App->map->data.layers.start;
	MapLayer* layer = App->map->data.layers.start->data;

	while (ret == true && layer_iterator != NULL) {
		layer = layer_iterator->data;
		// Map colliders, limit movement
		if (layer->returnPropValue("Navigation") == 1) {
			coord = App->map->WorldToMap(position.x, position.y+40 + (int)(GRAVITY * DT_CONVERTER * dt));
			if (layer->Get(coord.x, coord.y) != 0) 
				candown = false;
		}
		if (layer->returnPropValue("Navigation") == 3) {
			coord = App->map->WorldToMap(position.x + 40, position.y);
			if (layer->Get(coord.x, coord.y) != 0) {

				is_death = true;
			}
			coord = App->map->WorldToMap(position.x + 40, position.y);
			if (layer->Get(coord.x, coord.y) != 0) {

				is_death = true;
			}
			if (is_death == true) {

				state = SLIME_DEATH;
				ret = false;
			}
		}
		layer_iterator = layer_iterator->next;
	}

}