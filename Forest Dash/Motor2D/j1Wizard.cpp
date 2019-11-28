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
bool j1Wizard::Awake(pugi::xml_node& config)

{

	bool ret = true;



	return ret;

}

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
	
	pathFinding = true;

	
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
	if(pathFinding)Pathfinding(dt);
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
		current_animation = &idle;
		
		//Reset Animations
		death.Reset();

		//Reset Fx
		playedWizarDeathFx = false;
	}
	if (state == WD_DEATH)
	{
		current_animation = &death;

		if (!playedWizarDeathFx)
			App->audio->PlayFx(App->audio->wizarDeathFx);
			playedWizarDeathFx = true;
		

		if (SDL_GetTicks() > (deathTimerWizard + 2500)) {
			
			to_delete = true;

		}
	}
	if (state == WD_FORWARD)
	{
		current_animation = &forward;
		state = WD_IDLE;

		//Reset Fx
		playedWizarDeathFx = false;
	}
}

// Load animations from tiled  ----------------------------------------------


//Get an sdl rect depending on the frame id we are receiving ----------------------------------------------


void j1Wizard::OnCollision(Collider* c1, Collider* c2) {

	if (c2->type == COLLIDER_PLAYER_SHOT) {
		
		pathFinding = false;
		EntityCollider->to_delete = true;
		wizarDead = true;
		deathTimerWizard = SDL_GetTicks();
		
	}

	if (c2->type == COLLIDER_PLAYER) {

		if (position.x <= c2->rect.x)
		{
			position.x -= 50;
			//position.y -= 30;
			
		}
		else
		{
			position.x += 50;
		//	position.y -= 30;
		
		}

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
			position.x -= 1.5 * DT_CONVERTER * dt;
			flip = SDL_FLIP_NONE;
		}

		if (path->At(1)->x > origin.x) {
			position.x += 1.5* DT_CONVERTER * dt;
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

