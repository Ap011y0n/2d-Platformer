#include "p2Defs.h"
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




j1Player::j1Player(): j1Module()
{
	name.create("player");

	graphics = NULL;
	current_animation = NULL;

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

	dead.PushBack({ 136, 899, 42, 52 }, 0.1, 0, 0, 0, 0);
	dead.PushBack({ 237, 903, 40, 44 }, 0.1, 0, 0, 0, 0);
	dead.PushBack({ 329, 907, 50, 44 }, 0.1, 0, 0, 0, 0);
	dead.PushBack({ 433, 903, 44, 48 }, 0.1, 0, 0, 0, 0);
	dead.PushBack({ 537, 905, 38, 46 }, 0.1, 0, 0, 0, 0);
	dead.PushBack({ 630, 907, 48, 44 }, 0.1, 0, 0, 0, 0);
	
}

j1Player::~j1Player()
{}

bool j1Player::Awake(pugi::xml_node& config)
{
	bool ret = true;
	

	moveFx = config.child("moveFx").attribute("source").as_string();
	deathFx = config.child("deathFx").attribute("source").as_string();

	position.x = config.child("initialPosition").attribute("x").as_int();
	position.y = config.child("initialPosition").attribute("y").as_int();

	
	return ret;
}

// Load assets
bool j1Player::Start()
{
	LOG("Loading player");
	App->audio->LoadFx(moveFx.GetString());
	LOG("%d", App->audio->LoadFx(moveFx.GetString()));
	App->audio->LoadFx(deathFx.GetString());
	LOG("%d", App->audio->LoadFx(deathFx.GetString()));
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
	MoveCondition();
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
	if (Godmode == false)
	{
		if (state != JUMP && state != DEATH)state = IDLE;
		if (Candown && position.y < -1 * App->render->camera.y + App->win->height)position.y += GRAVITY;
		if (!Candown)jumpSpeed = -1 * SPEED_Y;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || state == JUMP) {
			if ((!Candown) && state == JUMP) {
				state = IDLE;
				playChannel = true;
			}
			else {
				state = JUMP;
				if (jumpSpeed < GRAVITY) {
					jumpSpeed += 0.45;
					if (!Canjump) { position.y -= GRAVITY; }
					if (Canjump) { position.y += (jumpSpeed); }
				}
				if (jumpSpeed > 0) { jumpSpeed = 0; }
			}
		}
		if (Candown && state != JUMP && state != DEATH) {
			if(state != FALLING)stopChannel = true;
			playChannel = true;
			state = FALLING;
			

		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			if (state != JUMP && state != DEATH)state = CROUCH;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && state != DEATH) {
			if (Canright) {
				position.x += SPEED_X;
				flip = SDL_FLIP_NONE;
				if (state != JUMP && state != FALLING ) {
					state = IDLE;
					if (state == IDLE)state = FORWARD;
				}

			}
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && state != DEATH) {
			if (Canleft) {
				position.x -= SPEED_X;
				flip = SDL_FLIP_HORIZONTAL;
				if (state != JUMP && state != FALLING) {
					state = IDLE;
					if (state == IDLE)state = BACKWARD;
				}

			}
		}
	}
	else if (Godmode == true)
	{
		state = IDLE;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				position.x -= SPEED_X;
				flip = SDL_FLIP_HORIZONTAL;
				state = IDLE;
				if (state == IDLE)state = BACKWARD;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += SPEED_X;
			state = IDLE;
			if (state == IDLE)state = FORWARD;
			flip = SDL_FLIP_NONE;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y -= SPEED_X;
			state = IDLE;
			if (state == IDLE)state = JUMP;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += SPEED_X;
			state = IDLE; 
			if (state == IDLE)state = JUMP;
		}
	}
}

void j1Player::setAnimation()
{
	if (state == IDLE) {
		App->audio->StopFx();
		playChannel = true;
		stopChannel = true;
	}
	if(state == FORWARD)
	{
		//if (stopChannel) { App->audio->StopFx(); stopChannel = false; }
		if (playChannel) { App->audio->PlayFx(2, 10); playChannel = false; }
		
		current_animation = &forward;
		if (BarWidth < 40)	BarWidth += 2;
	}
	if(state == BACKWARD)
	{
		//if (stopChannel) { App->audio->StopFx(); stopChannel = false; }
		if (playChannel) { App->audio->PlayFx(2, 10); playChannel = false; }
		current_animation = &forward;
		if(BarWidth < 40)	BarWidth += 2;
	}
	if (state == CROUCH)
	{
		if (stopChannel) { App->audio->StopFx(); stopChannel = false; }
		current_animation = &crouch;
	}
	if(state == JUMP)
	{
		if (stopChannel) { App->audio->StopFx(); stopChannel = false; }
		current_animation = &up;
	}
	if (state == FALLING)
	{
		if (stopChannel) { App->audio->StopFx(); stopChannel = false; }
		current_animation = &up;
	}
	if (state == DEATH) 
	{
		if (stopChannel) { App->audio->StopFx(); stopChannel = false; }
		current_animation = &dead;
		if(position.y < -1 * App->render->camera.y + App->win->height)position.y += (jumpSpeed += 0.45);
		if (playChannel) { App->audio->PlayFx(4, 0); playChannel = false; }
		if (SDL_GetTicks() > (DeathTimer + 2000)) {
			state = IDLE;
			BarWidth = 40;
			position.x = 120;
			position.y = 400;
		}
	}
}

void j1Player::CheckCollision() {

	Canright = true;
	Canleft = true;
	Canjump = true;
	Candown = true;
	death = false;

	bool ret = true;
	iPoint coord;
	p2List_item<MapLayer*>* layer_iterator = App->map->data.layers.start;
	MapLayer* layer = App->map->data.layers.start->data;

	while (ret == true && layer_iterator != NULL) {
		layer = layer_iterator->data;
				if (layer->returnPropValue("Navigation") == 1 && state != DEATH) {
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
				if (layer->returnPropValue("Navigation") == 2 ) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
						App->scene->Nextmap();
						position.x = 120;
						position.y = 350;
						ret = false;
					}
				}
				if (layer->returnPropValue("Navigation") == 3 && Godmode == false && state != DEATH) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
						playChannel = true;
						death = true;	
					}
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) {
						playChannel = true;
						death = true;
					}
					if (death == true){
						
					state = DEATH;
					jumpSpeed = -1 * SPEED_Y;
					DeathTimer = SDL_GetTicks();
					ret = false;
					}
				}
				if (layer->returnPropValue("Navigation") == 4 && Godmode == false && state != DEATH) {
	
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight + GRAVITY);
					if (layer->Get(coord.x, coord.y) != 0) {
						if (jumpSpeed < 0) { Candown = false; }
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
	if (state != DEATH){
	App->render->camera.x = -position.x + App->win->width/2;
	App->render->camera.y = -position.y + App->win->height/1.5;
	if (App->render->camera.x > 0) {
		App->render->camera.x = 0;
	}
	}
	/*if (App->render->camera.x < -4347) {
		App->render->camera.x = -4347;
	}*/
	
}

void j1Player::MoveCondition() {
	SDL_Rect redbar;
	SDL_Rect TimerBar;

	if (BarWidth > 0)BarWidth -= 0.3;
	else {
		if(state != DEATH && Godmode == false){
		jumpSpeed = -1 * SPEED_Y;
		DeathTimer = SDL_GetTicks();
		/*state = DEATH;*/
		}
	}

	redbar.h = 3;
	redbar.w = 40;
	TimerBar.h = 3;
	TimerBar.w = BarWidth;
	
	redbar.x = position.x;
	redbar.y = position.y + playerHeight + 20;

	TimerBar.x = position.x;
	TimerBar.y = position.y + playerHeight + 20;
	
	App->render->DrawQuad(redbar, 255, 0, 0);
	App->render->DrawQuad(TimerBar, 255, 255, 0);
}
