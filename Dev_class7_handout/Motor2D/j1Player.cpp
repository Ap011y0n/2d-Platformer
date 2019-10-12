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


j1Player::j1Player()
{
	graphics = NULL;
	current_animation = NULL;

	position.x = 120;
	position.y = 200;

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
	CheckCollision();
	Movement();
	App->render->Blit(graphics, position.x, position.y, &(current_animation->GetCurrentFrame()), 1.0f);
	DrawHitbox();

	return true;
}


bool j1Player::PostUpdate(float dt)
{
	return true;

}

// Load Game State
bool j1Player::Load(pugi::xml_node& data)
{
	LOG("Loading player state");

	return true;
}

// Save Game State
bool j1Player::Save(pugi::xml_node& data) const
{
	LOG("Saving player state");

	return true;
}
void j1Player::Movement(){
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		if (Canjump)position.y -= SPEED_Y;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		if (Candown)position.y += SPEED_Y;

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		if (Canright)position.x += SPEED_X;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		if (Canleft)position.x -= SPEED_X;
}


void j1Player::CheckCollision() {

	Canright = true;
	Canleft = true;
	Canjump = true;
	Candown = true;

	iPoint coord;
	p2List_item<MapLayer*>* layer_iterator = App->map->data.layers.start;
	MapLayer* layer = App->map->data.layers.start->data;

	while (layer_iterator != NULL) {
		layer = layer_iterator->data;
		//	LOG("%d", layer->returnPropValue("Navigation"));
				if (layer->returnPropValue("Navigation") == 1) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y - SPEED_Y);
					if (layer->Get((int)coord.x, (int)coord.y) != 0) Canjump = false;
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight + SPEED_Y);
					if (layer->Get((int)coord.x, (int)coord.y) != 0) Candown = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + SPEED_X, position.y + playerHeight);
					if (layer->Get((int)coord.x, (int)coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerCentre - SPEED_X, position.y + playerHeight);
					if (layer->Get((int)coord.x, (int)coord.y) != 0) Canleft = false;

				}
		layer_iterator = layer_iterator->next;
	}
	
}

void j1Player::DrawHitbox() {
	SDL_Rect hitbox;
	hitbox.h = playerHeight;
	hitbox.w = playerWidth;
	hitbox.x = position.x + playerCentre;
	hitbox.y = position.y;
	App->render->DrawQuad(hitbox, 0, 225, 0, 70);
}