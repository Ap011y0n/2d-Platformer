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
#include "Animation.h"


#include<stdio.h>


bool j1Player::Awake(pugi::xml_node& config)
{
	bool ret = true;
	config = config.child("player");

	moveFx = config.child("moveFx").attribute("source").as_string();
	LOG("%s", moveFx);
	name.create("player");
	return ret;
}


j1Player::j1Player()
{
	graphics = NULL;
	current_animation = NULL;

	position.x = 120;
	position.y = 350;

	// idle animation (just the ship)
	idle.PushBack({ 28, 14, 38, 58 }, 0.1, 0, 0, 0, 0);
	idle.PushBack({ 132, 12, 34, 60 }, 0.1, 0, 0, 0, 0);
	idle.PushBack({ 230, 12, 38, 60 }, 0.1, 0, 0, 0, 0);
	idle.PushBack({ 326, 14, 40, 58 }, 0.1, 0, 0, 0, 0);

	forward.PushBack({ 34, 90, 40, 56 }, 0.1, 0, 0, 0, 0);
	forward.PushBack({ 132, 92, 40, 54 }, 0.1, 0, 0, 0, 0);
	forward.PushBack({ 232, 96, 40, 50 }, 0.1, 0, 0, 0, 0);
	forward.PushBack({ 334, 90, 46, 56 }, 0.1, 0, 0, 0, 0);
	forward.PushBack({ 431, 92, 40, 54 }, 0.1, 0, 0, 0, 0);
	forward.PushBack({ 531, 96, 40, 50 }, 0.1, 0, 0, 0, 0);

	crouch.PushBack({ 629, 98, 40, 48 }, 0.3, 0, 16, 0, 0);
	crouch.PushBack({ 729, 102, 40, 44 }, 0.2, 0, 16, 0, 0);
	crouch.PushBack({ 431, 30, 38, 42 }, 0.05, 0, 16, 0, 0);
	crouch.loop = false;

	up.PushBack({ 34, 162, 38, 54 }, 0.1, 0, 0, 0, 0);
	up.PushBack({ 128, 158, 42, 46 }, 0.1, 0, 0, 0, 0);
	up.PushBack({ 236, 162, 30, 42 }, 0.1, 0, 0, 0, 0);
	up.PushBack({ 328, 168, 48, 34 }, 0.1, 0, 0, 0, 0);
	up.PushBack({ 439, 168, 36, 42 }, 0.1, 0, 0, 0, 0);
	up.PushBack({ 521, 174, 52, 34 }, 0.1, 0, 0, 0, 0);
	
}

j1Player::~j1Player()
{}

// Load assets
bool j1Player::Start()
{
	App->audio->LoadFx(moveFx.GetString());
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
	setAnimation();
	SDL_Rect* r = &current_animation->GetCurrentFrame();
	App->render->Blit(graphics, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);
	DrawHitbox();
	Camera();
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT) {
		App->render->FadeToBlack();
	}

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
	position.x = data.child("position").attribute("pos_x").as_int();
	position.y = data.child("position").attribute("pos_y").as_int();

	return true;
}

// Save Game State
bool j1Player::Save(pugi::xml_node& data) const
{
	LOG("Saving player state");
	pugi::xml_node adventurer = data.append_child("position");

	adventurer.append_attribute("pos_x") = position.x;
	adventurer.append_attribute("pos_y") = position.y;
	return true;
}


void j1Player::Movement(){
	if (state != JUMP)state = IDLE;
	if (Candown)position.y += GRAVITY;
	if (!Candown)jumpSpeed = -1 * SPEED_Y;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || state == JUMP){
		if ((!Candown) && state == JUMP) {
			state = IDLE;
		}
		else { state = JUMP; 
		if (jumpSpeed < GRAVITY) {
			jumpSpeed += 0.45;
			if (!Canjump) { position.y -= GRAVITY; }
			if (Canjump) { position.y += (jumpSpeed); }
		}
		if(jumpSpeed > 0) { jumpSpeed = 0; }
		}
	}
	if (Candown && state != JUMP) {
		state = FALLING;

	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT){
		if (state != JUMP)state = CROUCH;
		}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT){
	/*
		if (Canright) {
			if (state != JUMP && state != FALLING) {
				state = FORWARD;
				position.x += SPEED_X;
			}
			else {
				if (flip == SDL_FLIP_HORIZONTAL) {
					position.x += SPEED_X / 2;
				}
				else { position.x += SPEED_X; }
			}
		}*/
		if (Canright) {
			position.x += SPEED_X;
			flip = SDL_FLIP_NONE;
			if (state != JUMP && state != FALLING) {
				state = FORWARD;
			}

		}
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT){
		/*if (Canleft){
			if (state != JUMP && state != FALLING) {
				state = BACKWARD;
				position.x -= SPEED_X;
			}
			else {
				if(flip == SDL_FLIP_NONE){position.x -= SPEED_X/2;}
				else{ position.x -= SPEED_X; }
			}
		}*/
		if (Canleft) {
			position.x -= SPEED_X;
			flip = SDL_FLIP_HORIZONTAL;
			if (state != JUMP && state != FALLING) {
				state = BACKWARD;
			}
			
		}
	}
}

void j1Player::setAnimation()
{

	if(state == FORWARD)
	{
		App->audio->PlayFx(0);
		//LOG("%s", App->audio->PlayFx(0));
		current_animation = &forward;
		//flip = SDL_FLIP_NONE;
	}
	if(state == BACKWARD)
	{
		current_animation = &forward;
	//	flip = SDL_FLIP_HORIZONTAL;
	}
	if (state == CROUCH)
	{
		current_animation = &crouch;
	}
	if(state == JUMP)
	{
		current_animation = &up;
	}
	if (state == FALLING)
	{
		current_animation = &up;
	}
}

void j1Player::CheckCollision() {

	Canright = true;
	Canleft = true;
	Canjump = true;
	Candown = true;
	bool ret = true;
	iPoint coord;
	p2List_item<MapLayer*>* layer_iterator = App->map->data.layers.start;
	MapLayer* layer = App->map->data.layers.start->data;

	while (ret == true && layer_iterator != NULL) {
		layer = layer_iterator->data;
				if (layer->returnPropValue("Navigation") == 1) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + jumpSpeed + GRAVITY);
					if (layer->Get(coord.x, coord.y) != 0) Canjump = false;
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight + GRAVITY);
					if (layer->Get(coord.x, coord.y) != 0) Candown = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + SPEED_X, position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + SPEED_X, position.y + playerHeight/2);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + SPEED_X, position.y);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerCentre - SPEED_X, position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					coord = App->map->WorldToMap(position.x + playerCentre - SPEED_X, position.y + playerHeight/2);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					coord = App->map->WorldToMap(position.x + playerCentre - SPEED_X, position.y);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
				}
				if (layer->returnPropValue("Navigation") == 2) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
						App->scene->Nextmap();
						position.x = 120;
						position.y = 350;
						ret = false;
					}
				}
				if (layer->returnPropValue("Navigation") == 3) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
							position.x = 120;
							position.y = 400;
							ret = false;
					}
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
	if(App->map->blitColliders)	App->render->DrawQuad(hitbox, 0, 225, 0, 70);

}
void j1Player::Camera() {
	
	App->render->camera.x = -position.x + App->win->width/2;
	App->render->camera.y = -position.y + App->win->height/1.5;
	if (App->render->camera.x > 0) {
		App->render->camera.x = 0;
	}
	if (App->render->camera.x < -4347) {
		App->render->camera.x = -4347;
	}
	
}
