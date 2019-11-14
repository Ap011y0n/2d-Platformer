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
#include "math.h"
#include "j1ModuleCollision.h"
#include "j1Particles.h"
#include "j1Slime.h"


j1Player::j1Player(): j1Module()
{
	name.create("player");

	graphics = NULL;
	current_animation = NULL;
	LoadAnimations("textures/adventurer_animations.tmx");

	// Load animations from an animations list ----------------------------------------------
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

	dash = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	aiming = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	
}

j1Player::~j1Player()
{}

// Read player variables from config.xml ----------------------------------------------
bool j1Player::Awake(pugi::xml_node& config)
{
	bool ret = true;
	

	moveFx = config.child("moveFx").attribute("source").as_string();
	deathFx = config.child("deathFx").attribute("source").as_string();
	jumpFx = config.child("jumpFx").attribute("source").as_string();
	winFx = config.child("winFx").attribute("source").as_string();
	dashFx = config.child("dashFx").attribute("source").as_string();

	initialPosition.x = config.child("initialPosition").attribute("x").as_int();
	initialPosition.y = config.child("initialPosition").attribute("y").as_int();
	gravity = config.child("gravity").attribute("value").as_int();
	speedX = config.child("speedX").attribute("value").as_int();
	speedY = config.child("speedY").attribute("value").as_int();
	acceleration = config.child("acceleration").attribute("value").as_int();
	maxBarWidth = config.child("maxBarWidth").attribute("value").as_int();
	speedBar = config.child("speedBar").attribute("value").as_float();
	playerHeight = config.child("playerHeight").attribute("value").as_int();
	playerWidth = config.child("playerWidth").attribute("value").as_int();
	playerCentre = config.child("playerCentre").attribute("value").as_int();
	
	dashspeed = acceleration;
	jumpSpeed = -1 * speedY;
	
	return ret;
}

// Load assets ----------------------------------------------
bool j1Player::Start()
{
	LOG("Loading player");
	App->audio->LoadFx(moveFx.GetString());
	LOG("effects list");
	LOG("Move %d", App->audio->LoadFx(moveFx.GetString()));
	App->audio->LoadFx(deathFx.GetString());
	LOG("Death %d", App->audio->LoadFx(deathFx.GetString()));
	App->audio->LoadFx(jumpFx.GetString());
	LOG("Jump %d", App->audio->LoadFx(jumpFx.GetString()));
	App->audio->LoadFx(winFx.GetString());
	LOG("Win %d", App->audio->LoadFx(winFx.GetString()));
	App->audio->LoadFx(dashFx.GetString());
	LOG("Dash %d", App->audio->LoadFx(dashFx.GetString()));
	graphics = App->tex->Load("textures/adventurer.png");

	position.x = initialPosition.x;
	position.y = initialPosition.y;

	SDL_Rect r;
	r.h = playerHeight;
	r.w = playerWidth;
	r.x = position.x + playerCentre;
	r.y = position.y;
	ColliderPlayer = App->collision->AddCollider(&r, COLLIDER_PLAYER, this);
	return true;
}

// Unload assets ----------------------------------------------
bool j1Player::CleanUp()
{

	App->tex->UnLoad(graphics);
	return true;
}

// Update: draw background ----------------------------------------------
bool j1Player::Update(float dt)
{

	current_animation = &idle;
	CheckCollision(dt);
	Movement(dt);
	StateMachine(dt);
	SDL_Rect* r = &current_animation->GetCurrentFrame();
	App->render->Blit(graphics, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);
	DrawHitbox();
	Camera();
//	MoveCondition(dt);

	return true;
}


bool j1Player::PostUpdate(float dt)
{
	return true;

}

// Load Game State ----------------------------------------------
bool j1Player::Load(pugi::xml_node& data)
{
	LOG("Loading player state");

	position.x = data.child("position").attribute("pos_x").as_int();
	position.y = data.child("position").attribute("pos_y").as_int();

	return true;
}

// Save Game State ----------------------------------------------
bool j1Player::Save(pugi::xml_node& data) const
{
	LOG("Saving player state");
	pugi::xml_node adventurer = data.append_child("position");

	adventurer.append_attribute("pos_x") = position.x;
	adventurer.append_attribute("pos_y") = position.y;
	return true;
}

// Receive inputs and set movement and states ----------------------------------------------
void j1Player::Movement(float dt){

	if (Godmode == false)
	{
		//Set gravity to player
		if (Candown && position.y < -1 * App->render->camera.y + App->win->height && dt < 0.5)
			position.y += (int)(gravity * DT_CONVERTER * dt);
		//Reset jump force if on floor
		if (!Candown)
			jumpSpeed = -1 * speedY;

		//Set idle state if state is not dead nor jumping or dashing
		if (state != JUMP && state != DEATH && state != DASH_L && state != DASH_R && !adjust) {
			state = IDLE;
		}

		//Jump code
		if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || state == JUMP) && state != DEATH) {
		// Check if can move down
			if ((!Candown) && state == JUMP) {
				state = IDLE;	
			}
			else {
				state = JUMP;
				if (jumpSpeed < (gravity * DT_CONVERTER * dt)) {
					jumpSpeed += (0.45* DT_CONVERTER * dt);
					if (!Canjump) { position.y -= (int)(gravity * DT_CONVERTER * dt); }
					if (Canjump) { position.y += ((jumpSpeed)* DT_CONVERTER * dt);}
				}
		// Cap falling speed to avoid conflicts with collisions
				if (jumpSpeed > 0) { jumpSpeed = 0; }
			}
		}
		//If can fall down and is actually falling down change state to falling
		if (Candown && state != JUMP && state != DEATH && state != DASH_L && state != DASH_R) {
			state = FALLING;

		}
		if (!Candown && adjust && state != JUMP && state != DEATH && state != DASH_L && state != DASH_R) {
			position.y += (int)(3 * DT_CONVERTER * dt);
			state = ADJUST;

		}
		//Crouch wen in floor
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && state != DEATH && state != DASH_L && state != DASH_R) {
			if (state != JUMP && state != DEATH)state = CROUCH;
		}

		// Move to left direction if won't collide with anything or if it's not moving to left
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && state != DEATH && state != DASH_L && state != DASH_R) {
			if (state == BACKWARD) {
				state = IDLE;
				position.x += (speedX * DT_CONVERTER * dt);
			}
			else {
				if (Canright) {
					position.x += (speedX * DT_CONVERTER * dt);
					flip = SDL_FLIP_NONE;
					if (state != JUMP && state != FALLING) {
						state = IDLE;
						if (state == IDLE)state = FORWARD;
					}

				}
			}
		}

	// Move to left direction if won't collide with anything or if it's not moving to right
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && state != DEATH && state != DASH_L && state != DASH_R) {
			if (state == FORWARD) {
				state = IDLE;
				position.x -= (speedX * DT_CONVERTER * dt);
			}
			else{
			if (Canleft) {
				position.x -= (speedX * DT_CONVERTER * dt);
				flip = SDL_FLIP_HORIZONTAL;
				if (state != JUMP && state != FALLING) {
					state = IDLE;
					if (state == IDLE)state = BACKWARD;
				}

			}
			}
		}
	// Dash: add acceleration when presing left or rigth arrows
		if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || state == DASH_L) && dashspeed >= 0 && state != DEATH && CandashL) {
			state = DASH_L;
			if (dashspeed > 0)dashspeed -= (1 * DT_CONVERTER * dt);
			else { state = IDLE; }
			if (Canleft)position.x -= dashspeed * (DT_CONVERTER * dt);
			position.y -= gravity * ((DT_CONVERTER * dt));
		}
		if ((App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || state == DASH_R) && dashspeed >= 0 && state != DEATH && CandashR) {
			state = DASH_R;
			if (dashspeed > 0)dashspeed -= 1 * (DT_CONVERTER * dt);
			else { state = IDLE; }
			if(Canright)position.x += dashspeed *(DT_CONVERTER * dt);

			position.y -= (gravity * (DT_CONVERTER * dt));
		}
	}

	//God mode: movement is simpler, there is no gravity and you can't collide with anything
	else if (Godmode == true)
	{
		state = IDLE;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				position.x -= (speedX * (DT_CONVERTER * dt));
				flip = SDL_FLIP_HORIZONTAL;
				state = IDLE;
				if (state == IDLE)state = BACKWARD;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += (speedX * DT_CONVERTER * dt);
			state = IDLE;
			if (state == IDLE)state = FORWARD;
			flip = SDL_FLIP_NONE;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y -= (speedX * DT_CONVERTER * dt);
			state = IDLE;
			if (state == IDLE)state = JUMP;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += (speedX * DT_CONVERTER * dt);
			state = IDLE; 
			if (state == IDLE)state = JUMP;
		}
	}

	//Particles
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		state = AIMING;
		SDL_Rect aimbar;
		aimbar.h = 3;
		aimbar.w = aimbarw;
		aimbar.x = position.x;
		aimbar.y = position.y + playerHeight - 50;
		App->render->DrawQuad(aimbar, 0, 0, 255, 90);
		if (aimbar.w >= 50)
		{
			aimbarw = 0;
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint destiny = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
			iPoint origin = App->map->WorldToMap(position.x + 15, position.y + 15);
			iPoint vec(destiny.x - origin.x, destiny.y - origin.y);
			float angle = -(-90 + atan2(vec.x, vec.y) * 180 / 3.14159265);
			LOG("%f", angle);
			float yvec = (vec.y / sqrt(pow(vec.x, 2) + pow(vec.y, 2)));
			float xvec = (vec.x / sqrt(pow(vec.x, 2) + pow(vec.y, 2)));
			LOG("Raw %d, %d", vec.x, vec.y);
			LOG("Normal %f, %f", xvec, yvec);

			if (abs(xvec) > abs(yvec)) {
				if (xvec < 0) {
					float convert = -20 / xvec;
					xvec = xvec * convert;
					yvec = yvec * convert;
				}
				else {
					if (xvec > 0) {
						float convert = 20 / xvec;
						xvec = xvec * convert;
						yvec = yvec * convert;
					}
					else {
						yvec = yvec * 20;
					}
				}
			}
			else {
				if (yvec < 0) {
					float convert = -20 / yvec;
					xvec = xvec * convert;
					yvec = yvec * convert;
				}
				else {
					if (yvec > 0) {
						float convert = 20 / yvec;
						xvec = xvec * convert;
						yvec = yvec * convert;
					}
					else {
						xvec = xvec * 20;
					}
				}
			}

			LOG("Depurated %f, %f", xvec, yvec);
			LOG("Depurated %d, %d", (int)xvec, (int)yvec);
			App->particles->AddParticle(App->particles->arrow, position.x + 15, position.y + 15, COLLIDER_PLAYER_SHOT, 0.5, (int)xvec, (int)yvec, angle);
		}
	}

		
}

//Depending of the state set on Movement(), play fx, change facing direction and other interaction ----------------------------------------------
void j1Player::StateMachine(float dt)
{
	if (state == IDLE) {
		App->audio->StopFx();
		aimbarw = 0;
	}
	if (state == DASH_L) {
		if (!CandashL) {
			state = IDLE;
		}
		playfx(10, 10);
		flip = SDL_FLIP_HORIZONTAL;
		current_animation = &dash;
	}
	if (state == DASH_R) {
		if (!CandashR) {
			state = IDLE;
		}
		playfx(10, 10);
		flip = SDL_FLIP_NONE;
		current_animation = &dash;
	}
	if(state == FORWARD)
	{
		playfx(2, 20);
		dashspeed = acceleration;
		current_animation = &forward;
		if (BarWidth < maxBarWidth)	BarWidth += 2;
	}
	if(state == BACKWARD)
	{
		playfx(2, 20);
		
		dashspeed = acceleration;
		current_animation = &forward;
		if(BarWidth < maxBarWidth)	BarWidth += 2;
	}
	if (state == CROUCH)
	{

		current_animation = &crouch;
	}
	if(state == JUMP)
	{
		playfx(6, 0);
		
		current_animation = &up;
	}
	if (state == FALLING)
	{

		current_animation = &up;
	}
	if (state == DEATH)
	{
		BarWidth = 0;
		current_animation = &dead;
		if(position.y < -1 * App->render->camera.y + App->win->height)position.y += (jumpSpeed += 0.45* (DT_CONVERTER * dt));
		playfx(4, 0);
		if (SDL_GetTicks() > (DeathTimer + 2500)) {
			state = IDLE;
			BarWidth = maxBarWidth;
			position.x = initialPosition.x;
			position.y = initialPosition.y;
			
		}
	}
	if (state == AIMING)
	{
		aimbarw ++;
		AimTimer = SDL_GetTicks();
		current_animation = &aiming;

	}
}

//Predict if in next frame, the player will collide with something ----------------------------------------------
void j1Player::CheckCollision(float dt) {

	Canright = true;
	Canleft = true;
	Canjump = true;
	Candown = true;
	death = false;
	adjust = true;
	CandashL = true;
	CandashR = true;

	bool ret = true;
	iPoint coord;
	p2List_item<MapLayer*>* layer_iterator = App->map->data.layers.start;
	MapLayer* layer = App->map->data.layers.start->data;

	while (ret == true && layer_iterator != NULL) {
		layer = layer_iterator->data;
	// Map colliders, limit movement
				if (layer->returnPropValue("Navigation") == 1 && state != DEATH) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + (jumpSpeed + gravity) * DT_CONVERTER * dt);
					if (layer->Get(coord.x, coord.y) != 0) Canjump = false;
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + (playerHeight + gravity* DT_CONVERTER * dt));
					if (layer->Get(coord.x, coord.y) != 0) Candown = false;
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + (int)(playerHeight + 3 * DT_CONVERTER * dt));
					if (layer->Get(coord.x, coord.y) != 0) adjust = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (speedX * DT_CONVERTER * dt), position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;						
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (speedX * DT_CONVERTER * dt), position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;						 
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (speedX * DT_CONVERTER * dt), position.y);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerCentre - (speedX * DT_CONVERTER * dt), position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					coord = App->map->WorldToMap(position.x + playerCentre - (speedX * DT_CONVERTER * dt), position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					coord = App->map->WorldToMap(position.x + playerCentre - (speedX * DT_CONVERTER * dt), position.y);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (dashspeed * DT_CONVERTER * dt), position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) CandashR = false;
				
					coord = App->map->WorldToMap(position.x + playerCentre - (dashspeed * DT_CONVERTER * dt), position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) CandashL = false;
					
				}
		// Scene change colliders, when colliding change scene
				if (layer->returnPropValue("Navigation") == 2 ) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
						App->audio->StopFx();
						App->audio->PlayFx(8, 0);
						App->scene->Nextmap();
						position.x = initialPosition.x;
						position.y = initialPosition.y;
						ret = false;
					}
				}
		// Damage colliders, when colliding the player dies
				if (layer->returnPropValue("Navigation") == 3 && Godmode == false && state != DEATH) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
						
						death = true;	
					}
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) {
						
						death = true;
					}
					if (death == true){
						
					state = DEATH;
					jumpSpeed = -speedY* (DT_CONVERTER * dt);
					DeathTimer = SDL_GetTicks();
					ret = false;
					}
				}
			
		layer_iterator = layer_iterator->next;
	}
	
}

// Blits a rect to make an idea of player position
void j1Player::DrawHitbox() {
	SDL_Rect hitbox;
	hitbox.h = playerHeight;
	hitbox.w = playerWidth;
	hitbox.x = position.x + playerCentre;
	hitbox.y = position.y;
	if(App->map->blitColliders)	App->render->DrawQuad(hitbox, 0, 225, 0, 70);
	
	ColliderPlayer->SetPos(position.x + playerCentre, position.y);
}				
// Function to make the camera follow the player ----------------------------------------------
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

//Bar mechaninc, where you have to walk on the floor if you don't want to die ----------------------------------------------
//Sets a bar that decreases gradually and replenishes in forward and backward state
void j1Player::MoveCondition(float dt) {
	SDL_Rect redbar;
	SDL_Rect TimerBar;

	redbar.h = 3;
	redbar.w = maxBarWidth;
	TimerBar.h = 3;
	TimerBar.w = BarWidth;
	
	redbar.x = position.x;
	redbar.y = position.y + playerHeight + 20;

	TimerBar.x = position.x;
	TimerBar.y = position.y + playerHeight + 20;
	
	App->render->DrawQuad(redbar, 255, 0, 0, 90);
	App->render->DrawQuad(TimerBar, 255, 255, 50);

	SDL_Rect screen;
	screen.x = -1 * App->render->camera.x;
	screen.y = -1 * App->render->camera.y;
	screen.w = App->win->width * App->win->GetScale();
	screen.h = App->win->height * App->win->GetScale();
	
	if (BarWidth > 0)BarWidth -= speedBar * (DT_CONVERTER * dt);
	else{
		if (state != DEATH && Godmode == false) {
		jumpSpeed = -speedY* (70 * dt);
		DeathTimer = SDL_GetTicks();
		state = DEATH;
		}
	}
	if (BarWidth > 20){
		magnitude = 1.0f;
		opacity = 0.0f;
	}

//if bar width is lower than a preset value, camera starts turning red and shaking
	else {
	if (magnitude < 4)magnitude += 0.05f* (DT_CONVERTER * dt);
	if(opacity < 125)opacity += 1 * (DT_CONVERTER * dt);
	App->render->StartCameraShake(100, magnitude);
	App->render->DrawQuad(screen, 255, 0, 0, opacity);
	}
	
}

// Load animations from tiled  ----------------------------------------------
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
	LOG("Tileset: %s", TileSetData.Texname.GetString());
	LOG("firstgid %d", TileSetData.firstgid);
	LOG("tile_width %d", TileSetData.tile_width);
	LOG("tile_height %d", TileSetData.tile_height);
	LOG("tex_width %d", TileSetData.tex_width);
	LOG("num_tiles_width %d", TileSetData.num_tiles_width);
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

//Get an sdl rect depending on the frame id we are receiving ----------------------------------------------
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

//Play audio efects only once and stop efects that were already playing
void j1Player::playfx( const int id, const int rep) {
	if (prev_state != state) {
		App->audio->StopFx();
		App->audio->PlayFx(id, rep);
		prev_state = state;
	}
}

void j1Player::OnCollision(Collider* c1, Collider* c2) {
	if (c1 == ColliderPlayer && c2->type == COLLIDER_ENEMY) {
		LOG("Damage");
		if (position.x <= App->slime->position.x)
		{
			position.x -= 30;
			state = DEATH;
			jumpSpeed = -speedY * (DT_CONVERTER * App->collision->deltatime);
			DeathTimer = SDL_GetTicks();
		}
		else
		{
			position.x += 30;
			state = DEATH;
			jumpSpeed = -speedY * (DT_CONVERTER * App->collision->deltatime);
			DeathTimer = SDL_GetTicks();
		}

	}

}

