#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"


#include<stdio.h>

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

j1Player::j1Player()
{
	graphics = NULL;
	current_animation = NULL;

	position.x = 120;
	position.y = 427;

	// idle animation (just the ship)
	idle.PushBack({ 28, 14, 38, 58 });
	idle.PushBack({ 132, 12, 34, 60 });
	idle.PushBack({ 230, 12, 38, 60 });
	idle.PushBack({ 326, 14, 40, 58 });
	idle.speed = 0.08f;


}

j1Player::~j1Player()
{}

// Load assets
bool j1Player::Start()
{
	
	LOG("Loading player");

	graphics = App->tex->Load("textures/adventurer.png");

	return true;
}

// Unload assets
bool j1Player::CleanUp()
{
	App->tex->UnLoad(graphics);
	return true;
}

// Update: draw background
bool j1Player::Update(float dt)
{
	current_animation = &idle;

	App->render->Blit(graphics, position.x, position.y, &(current_animation->GetCurrentFrame()), 1.0f);
	return true;
}

bool j1Player::PostUpdate(float dt)
{
	return true;
	
}

