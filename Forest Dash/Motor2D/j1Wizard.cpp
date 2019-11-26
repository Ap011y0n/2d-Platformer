#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Wizard.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "Animation.h"
#include "j1Pathfinding.h"
#include "j1ModuleCollision.h"
#include "J1EntityManager.h"
#include "Brofiler/Brofiler.h"

j1Wizard::j1Wizard(int posx, int posy) : j1Entity(Types::wizard)
{
	name.create("wizard");

	current_animation = NULL;
	LoadAnimations("textures/wizard_animations.tmx");
	state = WD_IDLE;
	// Load animations from an animations list ----------------------------------------------
	p2List_item<Animation>* animation_iterator = animations.start;

	idle = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	death = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	death.loop = false;

	forward = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	forward.loop = true;

	position.x = posx;
	position.y = posy;

	r.w = 40;
	r.h = 70;
	r.x = position.x;
	r.y = position.y;
}

j1Wizard::~j1Wizard()
{

}

// Read player variables from config.xml ----------------------------------------------


// Load assets ----------------------------------------------
bool j1Wizard::Start()
{
	LOG("Start Wizard");
	App->EntityManager->wizardTex;
	to_delete = false;

	move = position.x;
	patrol = true;
	EntityCollider = App->collision->AddCollider(&r, COLLIDER_WIZARD, this);

	
	collided = SDL_GetTicks();
	return true;
}

// Unload assets ----------------------------------------------


// Update: draw background ----------------------------------------------
bool j1Wizard::Update(float dt)
{
	BROFILER_CATEGORY("Update_Wizard", Profiler::Color::Orchid);
	Movement();
	setAnimation(dt);

	
	if (App->EntityManager->GetPlayer()->position.x > position.x - 400 && App->EntityManager->GetPlayer()->position.x < position.x + 400 && App->EntityManager->GetPlayer()->position.y + 100 && App->EntityManager->GetPlayer()->position.y - 100)
	Pathfinding(dt);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	DrawHitbox();
	App->render->Blit(App->EntityManager->wizardTex, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);
	return true;
}


bool j1Wizard::PostUpdate(float dt)
{
	return true;

}

//// Load Game State ----------------------------------------------
//bool j1Wizard::Load(pugi::xml_node& data)
//{
//
//	return true;
//}
//
//// Save Game State ----------------------------------------------
//bool j1Wizard::Save(pugi::xml_node& data) const
//{
//
//	return true;
//}

void j1Wizard::Movement()
{
	if (wizarDead) state = WD_DEATH;

}

void j1Wizard::setAnimation(float dt)
{
	if (state == WD_IDLE)
	{
	/*	if(position.x < move){
			patrol = true;
		}
		if (position.x > move+200) {
			patrol = false;
		}
		if(patrol){
			position.x += 2 * (int)(DT_CONVERTER*dt);
			flip = SDL_FLIP_HORIZONTAL;
		}
		if (!patrol) {
			flip = SDL_FLIP_NONE;
			position.x -= 2 * (int)(DT_CONVERTER*dt);
		}*/
		current_animation = &idle;
		death.Reset();
	}
	if (state == WD_DEATH)
	{
		
		current_animation = &death;

		if (SDL_GetTicks() > (deathTimerWizard + 2500)) {

			CleanUp();
			EntityCollider->to_delete = true;

		}
	}
	if (state == WD_FORWARD)
	{
		current_animation = &forward;
		state = WD_IDLE;
	}
}

// Load animations from tiled  ----------------------------------------------

//Get an sdl rect depending on the frame id we are receiving ----------------------------------------------

void j1Wizard::OnCollision(Collider* c1, Collider* c2) {

	if (c2->type == COLLIDER_PLAYER_SHOT) {
		c1->to_delete = true;
		to_delete = true;
	}
}

bool j1Wizard::Pathfinding(float dt) {
	speedX = 0;
	speedY = 0;
	static iPoint origin;
	static bool origin_selected = false;
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->EntityManager->GetPlayer()->position;
	p = App->map->WorldToMap(p.x+30, p.y+30);

	origin = App->map->WorldToMap(position.x+30, position.y+30);
	App->pathfinding->CreatePath(origin, p);

	state = WD_FORWARD;
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
	if (path->At(1) != NULL)
	{

		if (path->At(1)->x < origin.x) {
			position.x -= 2 * DT_CONVERTER * dt;
			flip = SDL_FLIP_NONE;
		}

		if (path->At(1)->x > origin.x) {
			position.x += 2* DT_CONVERTER * dt;
			flip = SDL_FLIP_HORIZONTAL;
		}

		if (path->At(1)->y < origin.y) {
			position.y -= 2 * DT_CONVERTER * dt;
		}

		if (path->At(1)->y > origin.y) {
			position.y += 2 * DT_CONVERTER * dt;
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
	
		/*
			const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint nextPoint = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		if (App->collision->debug)
		{
			App->render->Blit(App->scene->debug_tex, nextPoint.x, nextPoint.y);
		}

		if (App->pathfinding->CreatePath(App->map->WorldToMap(position.x +10, position.y + 10), App->map->WorldToMap(App->EntityManager->GetPlayer()->position.x, App->EntityManager->GetPlayer()->position.y)) > -1)
		{
			state = WD_FORWARD;
			if (nextPoint.x < position.x)
			{
				flip = SDL_RendererFlip::SDL_FLIP_NONE;
				speedX = -40 * dt;
			}
			else if (nextPoint.x > position.x)
			{
				flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
				speedX = 40 * dt;
			}

			if (nextPoint.y < position.y)
			{
				speedY = -40 * dt;
			}
			else if (nextPoint.y > position.y)
			{
				speedY = 40 * dt;
			}
		}
		else
		{

			if (SDL_GetTicks() > (collided + 500) && App->EntityManager->GetPlayer()->position.x < position.x)
			{
				position.x -= 20;
			}
			if (SDL_GetTicks() > (collided + 500) && App->EntityManager->GetPlayer()->position.x < position.x)
			{
				position.x += 20;
			}
			if (SDL_GetTicks() > (collided + 500) && App->EntityManager->GetPlayer()->position.y < position.y)
			{
				position.y -= 20;
			}
			if (SDL_GetTicks() > (collided + 500) && App->EntityManager->GetPlayer()->position.y < position.y)
			{
				position.y += 20;
			}
			
			speedX = 0;
			speedY = 0;
		}

		position.x += speedX;
		position.y += speedY;
	}
	*/
	return true;
}

