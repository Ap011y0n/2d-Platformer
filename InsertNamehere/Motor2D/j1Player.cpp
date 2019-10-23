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
	LoadAnimations("textures/animations.tmx");
	p2List_item<Animation>* animation_iterator = animations.start;

	idle = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	forward = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	up = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	crouch = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	dead = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	
	/*idle.PushBack({ 28, 14, 38, 58 }, 0.1, 0, 0, 0, 0);
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
	dead.PushBack({ 630, 907, 48, 44 }, 0.1, 0, 0, 0, 0);*/
	
}

j1Player::~j1Player()
{}

bool j1Player::Awake(pugi::xml_node& config)
{
	bool ret = true;
	

	moveFx = config.child("moveFx").attribute("source").as_string();
	deathFx = config.child("deathFx").attribute("source").as_string();
	jumpFx = config.child("jumpFx").attribute("source").as_string();
	winFx = config.child("winFx").attribute("source").as_string();

	position.x = config.child("initialPosition").attribute("x").as_int();
	position.y = config.child("initialPosition").attribute("y").as_int();
	gravity = config.child("gravity").attribute("value").as_int();
	speedX = config.child("speedX").attribute("value").as_int();
	speedY = config.child("speedY").attribute("value").as_int();
	jumpSpeed = -1 * speedY;
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
	App->audio->LoadFx(jumpFx.GetString());
	LOG("%d", App->audio->LoadFx(jumpFx.GetString()));
	App->audio->LoadFx(winFx.GetString());
	LOG("%d", App->audio->LoadFx(winFx.GetString()));
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
		if (Candown && position.y < -1 * App->render->camera.y + App->win->height)position.y += gravity;
		if (!Candown)jumpSpeed = -1 * speedY;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || state == JUMP) {
			if ((!Candown) && state == JUMP) {
				state = IDLE;
				playChannel = true;
			}
			else {
				state = JUMP;
				if (jumpSpeed < gravity) {
					jumpSpeed += 0.45;
					if (!Canjump) { position.y -= gravity; }
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
				position.x += speedX;
				flip = SDL_FLIP_NONE;
				if (state != JUMP && state != FALLING ) {
					state = IDLE;
					if (state == IDLE)state = FORWARD;
				}

			}
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && state != DEATH) {
			if (Canleft) {
				position.x -= speedX;
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
				position.x -= speedX;
				flip = SDL_FLIP_HORIZONTAL;
				state = IDLE;
				if (state == IDLE)state = BACKWARD;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += speedX;
			state = IDLE;
			if (state == IDLE)state = FORWARD;
			flip = SDL_FLIP_NONE;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y -= speedX;
			state = IDLE;
			if (state == IDLE)state = JUMP;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += speedX;
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
		if (playChannel) { App->audio->PlayFx(2, 0); playChannel = false; }
		
		current_animation = &forward;
		if (BarWidth < 40)	BarWidth += 2;
	}
	if(state == BACKWARD)
	{
		//if (stopChannel) { App->audio->StopFx(); stopChannel = false; }
		if (playChannel) { App->audio->PlayFx(2, 0); playChannel = false; }
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
		if (playChannel) { App->audio->PlayFx(6, 0); playChannel = false; }
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
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + jumpSpeed + gravity);
					if (layer->Get(coord.x, coord.y) != 0) Canjump = false;
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight + gravity);
					if (layer->Get(coord.x, coord.y) != 0) Candown = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + speedX, position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + speedX, position.y + playerHeight/2);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + speedX, position.y);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerCentre - speedX, position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					coord = App->map->WorldToMap(position.x + playerCentre - speedX, position.y + playerHeight/2);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					coord = App->map->WorldToMap(position.x + playerCentre - speedX, position.y);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
				}
				if (layer->returnPropValue("Navigation") == 2 ) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
						App->audio->PlayFx(8, 0);
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
					jumpSpeed = -1 * speedY;
					DeathTimer = SDL_GetTicks();
					ret = false;
					}
				}
				if (layer->returnPropValue("Navigation") == 4 && Godmode == false && state != DEATH) {
	
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight + gravity);
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

	}
	//Same limits for both maps
	if (App->render->camera.x > 0) {
		App->render->camera.x = 0;
	}
	if (App->render->camera.y > 0) {
		App->render->camera.y = 0;
	}
	//specific for map1
	if (App->scene->current_level == "maplevel1.tmx")
	{
		if (App->render->camera.x < -4347) {
			App->render->camera.x = -4347;

		}
		if (App->render->camera.y < -200 && App->render->camera.x  > -2700 ) {
			App->render->camera.y = -200;
		}
		if (App->render->camera.y < -1050 && App->render->camera.x < -2700) {
			App->render->camera.y = -1050;
		}
	}
	//specific for map2
	if (App->scene->current_level == "maplevel2.tmx")
	{
		if (App->render->camera.y < -200 && App->render->camera.x > -1750) {
			App->render->camera.y = -200;
		}

		if (App->render->camera.y < -1100 && App->render->camera.x < -1750) {
			App->render->camera.y = -1100;
		}
		if (App->render->camera.x < -5350) {
			App->render->camera.x = -5350;

		}
	}
}

void j1Player::MoveCondition() {
	SDL_Rect redbar;
	SDL_Rect TimerBar;

	if (BarWidth > 0)BarWidth -= 0.3;
	else {
		if(state != DEATH && Godmode == false){
		jumpSpeed = -1 * speedY;
		DeathTimer = SDL_GetTicks();
		state = DEATH;
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
void j1Player::LoadAnimations(const char* path) {
	pugi::xml_parse_result result = player_file.load_file(path);
	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());
	
	}
	TileSetData.firstgid = player_file.child("map").child("tileset").attribute("firstgid").as_int();
	TileSetData.tile_width = player_file.child("map").child("tileset").attribute("tilewidth").as_int();
	TileSetData.tile_height = player_file.child("map").child("tileset").attribute("tileheight").as_int();
	TileSetData.tex_width = player_file.child("map").child("tileset").child("image").attribute("width").as_int();
	TileSetData.Texname.create(player_file.child("map").child("tileset").child("image").attribute("source").as_string());
	TileSetData.num_tiles_width = TileSetData.tex_width / TileSetData.tile_width;

	LOG("%d", TileSetData.firstgid);
	LOG("%d", TileSetData.tile_width);
	LOG("%d", TileSetData.tile_height);
	LOG("%d", TileSetData.tex_width);
	LOG("%s", TileSetData.Texname.GetString());
	LOG("%d", TileSetData.num_tiles_width);
	int i = 0;
	pugi::xml_node tile;
	pugi::xml_node frame;
	for (tile = player_file.child("map").child("tileset").child("tile"); tile; tile = tile.next_sibling("tile")) {
		Animation* set = new Animation();
		for (frame = tile.child("animation").child("frame"); frame; frame = frame.next_sibling("frame")) {
			set->PushBack(TileSetData.GetAnimRect(frame.attribute("tileid").as_int()),(frame.attribute("duration").as_float())/2000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int(),0,0);
			LOG("Animation %d, %d, %d, %d", frame.attribute("tileid").as_int(), (frame.attribute("duration").as_float()) / 1000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int());
		}
		animations.add(*set);

	}
	
}

SDL_Rect TileSetPlayer::GetAnimRect(int id) const
{
	int relative_id = id;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = ((rect.w ) * (relative_id % num_tiles_width));
	rect.y = ((rect.h ) * (relative_id / num_tiles_width));
	return rect;
}