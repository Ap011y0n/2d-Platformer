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
#include "j1entityManager.h"
#include "j1Wizard.h"
#include "Brofiler/Brofiler.h"


j1Player::j1Player(int posx, int posy) : j1Entity(Types::player)
{
	name.create("player");
	initialPosition.x = posx;
	initialPosition.y = posy;
	LOG("Initial position %d, %d", posx, posy);
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
	crouch.loop = false;

	dead = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	dash = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	aiming = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	aiming.loop = false;

	bow = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	bow.loop = false;

	swordAttack = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	
}

j1Player::~j1Player()
{}

// Read player variables from config.xml ----------------------------------------------

bool j1Player::Awake(pugi::xml_node& config)

{

	LOG("Awake :)");

	bool ret = true;

	//initialPosition.x = config.child("initialPosition").attribute("x").as_int();
	//initialPosition.y = config.child("initialPosition").attribute("y").as_int();
	gravity = config.child("gravity").attribute("value").as_int();
	speedX = config.child("speedX").attribute("value").as_int();
	speedY = config.child("speedY").attribute("value").as_int();
	acceleration = config.child("acceleration").attribute("value").as_int();
	maxBarWidth = config.child("maxBarWidth").attribute("value").as_int();
	speedBar = config.child("speedBar").attribute("value").as_float();
	playerHeight = config.child("playerHeight").attribute("value").as_int();
	playerWidth = config.child("playerWidth").attribute("value").as_int();
	playerCentre = config.child("playerCentre").attribute("value").as_int();
	
	LOG("Pos y %d", initialPosition.y);

	dashspeed = acceleration;

	jumpSpeed = -1 * speedY;

	

	return ret;

}

// Load assets ----------------------------------------------
bool j1Player::Start()
{
	LOG("Awake :)");
	bool ret = true;
	to_delete = false;
	

	/*

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
	*/
	/*

	gravity = 11;
	speedX = 7;
	speedY = 22;
	acceleration = 20;
	speedBar = 0.2;
	maxBarWidth = 40;
	playerHeight = 50;
	playerWidth = 20;
	playerCentre = 10;
	
	LOG("Pos y %d", initialPosition.y);
	dashspeed = acceleration;
	jumpSpeed = -1 * speedY;
*/
	LOG("Loading player");
	
	position.x = initialPosition.x;
	position.y = initialPosition.y;

	SDL_Rect r;
	r.h = playerHeight;
	r.w = playerWidth;
	r.x = position.x + playerCentre;
	r.y = position.y;

	EntityCollider = App->collision->AddCollider(&r, COLLIDER_PLAYER, this);

	charging = false;

	return true;
}

// Unload assets ----------------------------------------------
/*bool j1Player::CleanUp()
{
	j1Entity::CleanUp();

	App->tex->UnLoad(graphics);
	return true;
}
*/
// Update: draw background ----------------------------------------------
bool j1Player::Update(float dt)
{
	charging = false;

	BROFILER_CATEGORY("Update_Player", Profiler::Color::SaddleBrown);

	current_animation = &idle;
	CheckCollision(dt);
	Movement(dt);
	StateMachine(dt);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->EntityManager->playerTex, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);
	DrawHitbox();
	Camera();
	MoveCondition(dt);

	current_animation_bow = &bow;
	SDL_Rect* rec = &current_animation_bow->GetCurrentFrame(dt);

	if(charging)
	App->render->Blit(App->EntityManager->playerTex, position.x + (current_animation_bow->pivotx[current_animation_bow->returnCurrentFrame()]), position.y + (current_animation_bow->pivoty[current_animation_bow->returnCurrentFrame()]), rec, 1.0f, 1.0f, flip_bow, angle);
	
	return true;
}


bool j1Player::PostUpdate(float dt)
{
	return true;

}

// Load Game State ----------------------------------------------
/*bool j1Player::Load(pugi::xml_node& data)
{
	LOG("Loading player state");

	

	return true;
}*/

// Save Game State ----------------------------------------------
/*bool j1Player::Save(pugi::xml_node& data) const
{
	LOG("Saving player state");
	pugi::xml_node adventurer = data.append_child("position");

	adventurer.append_attribute("pos_x") = position.x;
	adventurer.append_attribute("pos_y") = position.y;
	return true;
}*/

// Receive inputs and set movement and states ----------------------------------------------
void j1Player::Movement(float dt) {
	
	if (Godmode == false)
	{
		//Set gravity to player
		if (Candown && position.y < -1 * App->render->camera.y + App->win->height && dt < 0.05){
			position.y += (int)(gravity * DT_CONVERTER * dt);
			
		}
		//Reset jump force if on floor
		if (!Candown)
			jumpSpeed = -1 * speedY;

		//Set idle state if state is not dead nor jumping or dashing
		if (state != JUMP && state != DEATH && state != DASH_L && state != DASH_R && !adjust) {
			state = IDLE;
		}

		//Particles
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && state != DEATH && state != JUMP && state != FALLING)
		{
			charging = true;
			state = AIMING;

			aimbar.h = 3;
			aimbar.w = aimbarw;
			aimbar.x = position.x;
			aimbar.y = position.y + playerHeight - 50;
			App->render->DrawQuad(aimbar, 0, 0, 255, 255);

			int x, y;
			App->input->GetMousePosition(x, y);



			iPoint destiny = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
			iPoint origin = App->map->WorldToMap(position.x + 15, position.y + 15);
			iPoint vec(destiny.x - origin.x, destiny.y - origin.y);

			angle = -(-90 + atan2(vec.x, vec.y) * 180 / 3.14159265);
			//	LOG("%f", angle);
			yvec = (vec.y / sqrt(pow(vec.x, 2) + pow(vec.y, 2)));
			xvec = (vec.x / sqrt(pow(vec.x, 2) + pow(vec.y, 2)));
			//LOG("Raw %d, %d", vec.x, vec.y);
		//	LOG("Normal %f, %f", xvec, yvec);

			if (xvec < 0)
				flip = SDL_FLIP_HORIZONTAL;
			if (xvec > 0)
				flip = SDL_FLIP_NONE;


			xvec = xvec * 30;
			yvec = yvec * 30;

			//	LOG("Depurated %f, %f", xvec, yvec);
			//	LOG("Depurated %d, %d", (int)xvec, (int)yvec);

		}
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && state != DEATH)
		{
			LOG("Open fire");
			if (aimbar.w >= 50)
			{
				aimbarw = 0;

				//Blit arrow
				if (flip == SDL_FLIP_NONE) {
					//App->particles->AddParticle(App->particles->arrow, position.x + 25, position.y + 25, COLLIDER_PLAYER_SHOT, 0.5, (int)xvec, (int)yvec, angle);
					App->EntityManager->CreateEntity(j1Entity::Types::projectile_player, position.x + 25, position.y + 25, (int)xvec, (int)yvec, angle);
				}
				else if (flip == SDL_FLIP_HORIZONTAL)
					//App->particles->AddParticle(App->particles->arrow, position.x-10, position.y + 25, COLLIDER_PLAYER_SHOT, 0.5, (int)xvec, (int)yvec, angle);
					App->EntityManager->CreateEntity(j1Entity::Types::projectile_player, position.x - 10, position.y + 25, (int)xvec, (int)yvec, angle);
				aiming.Reset();
			}

		}

		//Sword Attack
		if (App->input->GetMouseButtonDown(3) == KEY_REPEAT)
		{
			state = ATTACK;
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
					if (Canjump) { position.y += (int)((jumpSpeed)* DT_CONVERTER * dt); }
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
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && state != DEATH && state != DASH_L && state != DASH_R && state != AIMING) {
			if (state == BACKWARD) {
				state = IDLE;
				position.x += (speedX * DT_CONVERTER * dt);
			}
			else {
				if (Canright) {
					position.x += (int)(speedX * DT_CONVERTER * dt);
					flip = SDL_FLIP_NONE;
					if (state != JUMP && state != FALLING) {
						state = IDLE;
						if (state == IDLE)state = FORWARD;
					}

				}
			}
		}

		// Move to left direction if won't collide with anything or if it's not moving to right
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && state != DEATH && state != DASH_L && state != DASH_R && state != AIMING) {
			if (state == FORWARD) {
				state = IDLE;
				position.x -= (int)(speedX * DT_CONVERTER * dt);
			}
			else {
				if (Canleft) {
					position.x -= (int)(speedX * DT_CONVERTER * dt);
					flip = SDL_FLIP_HORIZONTAL;
					if (state != JUMP && state != FALLING) {
						state = IDLE;
						if (state == IDLE)state = BACKWARD;
					}

				}
			}
		}
		// Dash: add acceleration when presing left or rigth arrows
		if ((App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN || state == DASH_L) && dashspeed >= 0 && state != DEATH && state != AIMING) {
			state = DASH_L;

			if (dashspeed > 0) { dashspeed -= (DT_CONVERTER * dt); }
			else { state = IDLE; }
			if (Canleft && CandashL)position.x -= (int)(dashspeed * DT_CONVERTER * dt);
			position.y -= (int)(gravity * (DT_CONVERTER * dt));
		}
		if ((App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN || state == DASH_R) && dashspeed >= 0 && state != DEATH && state != AIMING) {
			state = DASH_R;

			if (dashspeed > 0) { dashspeed -=  DT_CONVERTER * dt; }
			else { state = IDLE; }
			if (Canright && CandashR)position.x += (int)(dashspeed* DT_CONVERTER * dt);
			position.y -= (int)(gravity * DT_CONVERTER * dt);
		}
	}

	//God mode: movement is simpler, there is no gravity and you can't collide with anything
	else if (Godmode == true)
	{
		state = IDLE;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			position.x -= (int)(speedX * (DT_CONVERTER * dt));
			flip = SDL_FLIP_HORIZONTAL;
			state = IDLE;
			if (state == IDLE)state = BACKWARD;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += (int)(speedX * DT_CONVERTER * dt);
			state = IDLE;
			if (state == IDLE)state = FORWARD;
			flip = SDL_FLIP_NONE;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y -= (int)(speedX * DT_CONVERTER * dt);
			state = IDLE;
			if (state == IDLE)state = JUMP;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += (int)(speedX * DT_CONVERTER * dt);
			state = IDLE;
			if (state == IDLE)state = JUMP;
		}
	}
	
	
}

//Depending of the state set on Movement(), play fx, change facing direction and other interaction ----------------------------------------------
void j1Player::StateMachine(float dt)
{
	if (state == IDLE) {
		
		aimbarw = 0;
		animationStart = 0;
		//Reset Animations
		up.Reset();
		crouch.Reset();
		dash.Reset();
		aiming.Reset();
		bow.Reset();
		swordAttack.Reset();

		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;
		playeDashFx = false;

		//Reset collider
		collider = true;
		
	}
	if (state == DASH_L) {

		if (dashspeed < 0) {
			dashspeed = 0;
		}
		
		flip = SDL_FLIP_HORIZONTAL;
		current_animation = &dash;

		//Play dashFx
		if (!playeDashFx) {
			App->audio->PlayFx(App->audio->dashFx);
			playeDashFx = true;
		}

		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;

		//Reset collider
		collider = true;
		
	}
	if (state == DASH_R) {
		if (dashspeed < 0) {
			dashspeed = 0;
		}
		
		flip = SDL_FLIP_NONE;
		current_animation = &dash;

		//Play dashFx
		if (!playeDashFx) {
			App->audio->PlayFx(App->audio->dashFx);
			playeDashFx = true;
		}

		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;

		//Reset collider
		collider = true;
		
	}
	if(state == FORWARD)
	{
		animationStart = 0;
		dashspeed = acceleration;
		current_animation = &forward;
		if (BarWidth < maxBarWidth)	BarWidth += 2;

		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;
		playeDashFx = false;

		//Reset collider
		collider = true;
		
	}
	if(state == BACKWARD)
	{
		
		animationStart = 0;
		dashspeed = acceleration;
		current_animation = &forward;
		if(BarWidth < maxBarWidth)	BarWidth += 2;

		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;
		playeDashFx = false;

		//Reset collider
		collider = true;
		
	}
	if (state == CROUCH)
	{
		animationStart = 0;
		current_animation = &crouch;

		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;
		playeDashFx = false;

		//Reset collider
		collider = true;
		
	}
	if(state == JUMP)
	{
		animationStart = 0;
		current_animation = &up;

		//Play jumpFx
		if (!playedJumpFx) {
			App->audio->PlayFx(App->audio->jumpFx);
			playedJumpFx = true;
		}

		//Reset FX
		playeDeadFx = false;
		playeDashFx = false;

		//Reset collider
		collider = true;
		
	}
	if (state == FALLING)
	{
		animationStart = 0;
		current_animation = &up;

		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;
		playeDashFx = false;

		//Reset collider
		collider = true;
		
	}
	if (state == DEATH)
	{
		animationStart = 0;
		BarWidth = 0;
		current_animation = &dead;
		if(position.y < -1 * App->render->camera.y + App->win->height)position.y += (jumpSpeed += 0.45* (DT_CONVERTER * dt));
		
		//Play death FX
		if (!playeDeadFx)
			App->audio->PlayFx(App->audio->deathFx);
		playeDeadFx = true;

		//Reset FX
		playedJumpFx = false;
		playeDashFx = false;

		//Reset collider
		collider = true;
		

		if (SDL_GetTicks() > (DeathTimer + 2500)) {
			state = IDLE;
			BarWidth = maxBarWidth;
			position.x = initialPosition.x;
			position.y = initialPosition.y;
			
		}
	}
	if (state == AIMING)
	{
		animationStart = 0;
		if(aimbarw<50)
		aimbarw += ( 1 * DT_CONVERTER *dt);
		AimTimer = SDL_GetTicks();
		current_animation = &aiming;
		
		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;
		playeDashFx = false;

		//Reset collider
		collider = true;
		
	}
	if (state == ATTACK)
	{
		
		current_animation = &swordAttack;
		SDL_Rect r = {position.x, position.y, 35, 20 };
		
		if (collider == true)
		{
			colliderAttack = App->collision->AddCollider(&r, COLLIDER_PLAYER_SHOT, this);
			collider = false;
			
		}
		//Reset FX
		playedJumpFx = false;
		playeDeadFx = false;
		playeDashFx = false;
		
	}

	if(animationStart == 0)
	{
		if (colliderAttack != nullptr)
		{
			if (flip == SDL_FLIP_NONE)	colliderAttack->SetPos(position.x + 30, position.y+15);
			if (flip == SDL_FLIP_HORIZONTAL) colliderAttack->SetPos(position.x - 30, position.y+15);
			if (current_animation->AnimationEnd() == true && animationStart == 0) colliderAttack->to_delete = true;
		}
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
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + (int)((jumpSpeed + gravity) * DT_CONVERTER * dt));
					if (layer->Get(coord.x, coord.y) != 0) Canjump = false;
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight + (int)(gravity* DT_CONVERTER * dt));
					if (layer->Get(coord.x, coord.y) != 0) Candown = false;
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight + (int)(3 * DT_CONVERTER * dt));
					if (layer->Get(coord.x, coord.y) != 0) adjust = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (int)(speedX * DT_CONVERTER * dt), position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;						
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (int)(speedX * DT_CONVERTER * dt), position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;						 
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (int)(speedX * DT_CONVERTER * dt), position.y);
					if (layer->Get(coord.x, coord.y) != 0) Canright = false;
					coord = App->map->WorldToMap(position.x + playerCentre - (int)(speedX * DT_CONVERTER * dt), position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					coord = App->map->WorldToMap(position.x + playerCentre - (int)(speedX * DT_CONVERTER * dt), position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					coord = App->map->WorldToMap(position.x + playerCentre - (int)(speedX * DT_CONVERTER * dt), position.y);
					if (layer->Get(coord.x, coord.y) != 0) Canleft = false;
					
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (int)(dashspeed * DT_CONVERTER * dt), position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) CandashR = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (int)(dashspeed * DT_CONVERTER * dt), position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) CandashR = false;
					coord = App->map->WorldToMap(position.x + playerWidth + playerCentre + (int)(dashspeed * DT_CONVERTER * dt), position.y);
					if (layer->Get(coord.x, coord.y) != 0) CandashR = false;
				
					coord = App->map->WorldToMap(position.x + playerCentre - (int)(dashspeed * DT_CONVERTER * dt), position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) CandashL = false;
					coord = App->map->WorldToMap(position.x + playerCentre - (int)(dashspeed * DT_CONVERTER * dt), position.y + playerHeight);
					if (layer->Get(coord.x, coord.y) != 0) CandashL = false;
					coord = App->map->WorldToMap(position.x + playerCentre - (int)(dashspeed * DT_CONVERTER * dt), position.y);
					if (layer->Get(coord.x, coord.y) != 0) CandashL = false;
					
				}
		// Scene change colliders, when colliding change scene
				if (layer->returnPropValue("Navigation") == 2 ) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
						
						//Play winFx
						if (!playedWinFx)
							App->audio->PlayFx(App->audio->winFx);
							playedWinFx = true;
						
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
				if(!App->scene->checkpoint){
				if (layer->returnPropValue("Navigation") == 5) {
					coord = App->map->WorldToMap(position.x + playerCentre, position.y + playerHeight / 2);
					if (layer->Get(coord.x, coord.y) != 0) {
						App->SaveGame();
						App->scene->checkpoint = true;
					}
				}
				}
			
		layer_iterator = layer_iterator->next;
	}
	
}

// Blits a rect to make an idea of player position
			
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


//Get an sdl rect depending on the frame id we are receiving ----------------------------------------------

void j1Player::OnCollision(Collider* c1, Collider* c2) {
	
	if (c1 == EntityCollider && c2->type == COLLIDER_ENEMY) {
		LOG("Damage");
		BarWidth -= 15;
		position.y -= 30;
		DeathTimer = SDL_GetTicks();

		if (position.x <= c2->rect.x)
		{
			position.x -= 30;

		}
		else
		{
			position.x += 30;
		}
		
		
	}
	if (c1 == EntityCollider && c2->type == COLLIDER_WIZARD) {
		BarWidth -= 20;
		position.y -= 30;
		DeathTimer = SDL_GetTicks();

		if (position.x <= c2->rect.x)
		{
			position.x -= 30;
		}
		else
		{
			position.x += 30;
		}


	}

}

