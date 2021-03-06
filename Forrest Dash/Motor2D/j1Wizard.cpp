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
#include "j1Hud.h"
#include "j1Pathfinding.h"
#include "j1ModuleCollision.h"
#include "J1EntityManager.h"
#include "Brofiler/Brofiler.h"
#include "J1Console.h"

j1Wizard::j1Wizard(int posx, int posy) : j1Entity(Types::wizard)
{
	name.create("wizard");

	current_animation = NULL;
	LoadAnimations("textures/wizard_animations.tmx");
	
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
	initialPosition.x = posx;
	r.w = 40;
 	r.h = 70;
	r.x = position.x;
	r.y = position.y;
}

j1Wizard::~j1Wizard()
{

}



// Read variables from config.xml ----------------------------------------------
bool j1Wizard::Awake(pugi::xml_node& config)
{
	bool ret = true;

	speedX = config.child("wizard").child("speed").attribute("x").as_float();
	speedY = config.child("wizard").child("speed").attribute("y").as_float();
	rangeX = config.child("wizard").child("range").attribute("x").as_int();
	rangeY = config.child("wizard").child("range").attribute("x").as_int();

	return ret;

}

// Load assets ----------------------------------------------
bool j1Wizard::Start()
{
	LOG("Start Wizard");
	App->console->write("Start Slime");
	App->EntityManager->wizardTex;
	to_delete = false;
	wizarDead = false;
	state = WD_IDLE;
	move = position.x;
	EntityCollider = App->collision->AddCollider(&r, COLLIDER_WIZARD, this);
	playedWizarDeathFx = false;
	deathTimerWizard = 0;
	collided = SDL_GetTicks();
	flip = SDL_FLIP_NONE;
	pathFinding = true;
	patrolr = true;
	
	return true;
}


// Update: draw background ----------------------------------------------
bool j1Wizard::Update(float dt)
{
	BROFILER_CATEGORY("Update_Wizard", Profiler::Color::Orchid);
	Movement(dt);
	setAnimation(dt);
	if (!App->EntityManager->GetPlayer()->is_death) {
	if (App->EntityManager->GetPlayer()->position.x > position.x - rangeX && App->EntityManager->GetPlayer()->position.x < position.x + rangeX && App->EntityManager->GetPlayer()->position.y + rangeY && App->EntityManager->GetPlayer()->position.y - rangeY)
			if (pathFinding)Pathfinding(dt);
	}
	if(EntityCollider != NULL)
	DrawHitbox();
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->EntityManager->wizardTex, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);
	return true;
}


bool j1Wizard::PostUpdate(float dt)
{
	return true;

}


void j1Wizard::Movement(float dt)
{
	if (wizarDead) state = WD_DEATH;
	if (state != WD_PATHFINDING && state != WD_DEATH)
	{
		if (position.x > initialPosition.x + 100)
		{
			patrolr = false;
			patroll = true;
		}
		if (position.x < initialPosition.x - 100)
		{
			patrolr = true;
			patroll = false;
		}

		if (patroll == true)
		{
			position.x -= 2*dt* DT_CONVERTER;
			state = WD_FORWARD;
			flip = SDL_FLIP_NONE;
		}
		if (patrolr == true)
		{
			position.x += 2 * dt * DT_CONVERTER;
			flip = SDL_FLIP_HORIZONTAL;
			state = WD_FORWARD;
		}
	}
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
	if (state == WD_PATHFINDING)
	{
		current_animation = &forward;
		state = WD_IDLE;

	}
}

//Detects if the enemy collided against another entity
void j1Wizard::OnCollision(Collider* c1, Collider* c2) {

	//If it's a player attack, the entity will die
	if (c2->type == COLLIDER_PLAYER_SHOT) {
		App->hud->score += 20;
		App->render->StartCameraShake(500, 3);
		pathFinding = false;
		EntityCollider->to_delete = true;
		wizarDead = true;
		deathTimerWizard = SDL_GetTicks();
		
	}

	//if it's the player, the entity will receive a little knocback
	if (c2->type == COLLIDER_PLAYER) {

		if (position.x <= c2->rect.x)
		{
			position.x -= 50;
			position.y -= 30;
			
		}
		else
		{
			position.x += 50;
			position.y -= 30;
		
		}

	}
}
//Called if the player is close enough
bool j1Wizard::Pathfinding(float dt) {

	static iPoint origin;
	static bool origin_selected = false;
	//the destiny is the player position
	iPoint p = App->EntityManager->GetPlayer()->position;
	p = App->map->WorldToMap(p.x+30, p.y+30);
	//The origin is the current position of the Wizard
	origin = App->map->WorldToMap(position.x+30, position.y+30);
	App->pathfinding->CreatePath(origin, p);
	//Path created with both inputs
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	if (path->At(1) != NULL)
	{
		state = WD_PATHFINDING;
		//This makes a comparison with the players position to make the correct move
		if (path->At(1)->x < origin.x) {
			position.x -= speedX * DT_CONVERTER * dt;
			flip = SDL_FLIP_NONE;
		}

		if (path->At(1)->x > origin.x) {
			position.x += speedX * DT_CONVERTER * dt;
			flip = SDL_FLIP_HORIZONTAL;
		}

		if (path->At(1)->y < origin.y) {
			position.y -= speedY * DT_CONVERTER * dt;
		}

		if (path->At(1)->y > origin.y) {
			position.y += speedY * DT_CONVERTER * dt;
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

