#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1ProjPlayer.h"
#include "p2Log.h"
#include "j1Window.h"
//#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "Animation.h"
#include "j1ModuleCollision.h"
//#include "j1Particles.h"
#include "J1EntityManager.h"
#include "Brofiler/Brofiler.h"




j1ProjPlayer::j1ProjPlayer(int posx, int posy, int speedx, int speedy, float anglepar) : j1Entity(Types::projectile_player)
{
	name.create("Projectile Player");

	current_animation = NULL;
	born = SDL_GetTicks();
	position.x = posx;
	position.y = posy;
	speed.x = speedx;
	speed.y = speedy;
	angle = anglepar;

	/*LoadAnimations("textures/slime_animations.tmx");
	state = SLIME_IDLE;

	// Load animations from an animations list ----------------------------------------------
	p2List_item<Animation>* animation_iterator = animations.start;

	idle = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	death = animation_iterator->data;
	animation_iterator = animation_iterator->next;
	death.loop = false;

	forward = animation_iterator->data;
	animation_iterator = animation_iterator->next;

	initialPosition.x = posx;
	position.x = posx;
	position.y = posy;

	r.w = 40;
	r.h = 50;
	r.x = position.x;
	r.y = position.y;
	*/

}

j1ProjPlayer::~j1ProjPlayer()
{

}


bool j1ProjPlayer::Awake(pugi::xml_node& config)

{

	bool ret = true;

	


	return ret;

}



// Load assets ----------------------------------------------
bool j1ProjPlayer::Start()
{
	LOG("Start Projectile");

	anim.PushBack({ 4, 26, 62, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 67, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 132, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 198, 26, 58, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 260, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 323, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 386, 26, 62, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 449, 26, 63, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 512, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 576, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 640, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 704, 26, 64, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 772, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 835, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 961, 25, 62, 15 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1028, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1092, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1155, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1155, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1219, 26, 61, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1283, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1348, 26, 60, 14 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1477, 26, 59, 12 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1540, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1603, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1667, 26, 61, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1733, 26, 59, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1796, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	anim.PushBack({ 1860, 26, 60, 13 }, 0.1, 0, 0, 0, 0);
	life = 2000;

	anim.loop = true;
	to_delete = false;

	SDL_Rect r;
	r.w = 40;
	r.h = 50;
	r.x = position.x;
	r.y = position.y;
	
		EntityCollider = App->collision->AddCollider(&r, COLLIDER_PLAYER_SHOT, this);
	

	//EntityCollider = App->collision->AddCollider(&r, COLLIDER_ENEMY, this);

	
	return true;
}

// Unload assets ----------------------------------------------


// Update: draw background ----------------------------------------------
bool j1ProjPlayer::Update(float dt)
{
	BROFILER_CATEGORY("Update_Projectile", Profiler::Color::CornflowerBlue);
	Movement(dt);
	setAnimation();
	DrawHitbox();
	//flip = SDL_FLIP_HORIZONTAL;
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	//App->render->Blit(App->EntityManager->icespiketex, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f /*flip*/);
	App->render->Blit(App->EntityManager->icespiketex, position.x, position.y, &(anim.GetCurrentFrame(dt)), 1, 1, SDL_FLIP_NONE, angle);
	if(fx_played == false)
	{
		fx_played = true;
		App->audio->PlayFx(fx);
	}

	return true;
}


bool j1ProjPlayer::PostUpdate(float dt)
{
	
	return true;

}

// Load Game State ----------------------------------------------
//bool j1Slime::Load(pugi::xml_node& data)
//{
//	
//	return true;
//}
//
//// Save Game State ----------------------------------------------
//bool j1Slime::Save(pugi::xml_node& data) const
//{
//	
//	return true;
//}


bool j1ProjPlayer::Movement(float dt)
{
	bool ret = false;

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life) {
			EntityCollider->to_delete = true;
			to_delete = true;

			return ret;
		}
	}
	else
		if (anim.Finished())
			return ret;

	position.x += speed.x * DT_CONVERTER* dt;
	position.y += speed.y * DT_CONVERTER* dt;
	EntityCollider->SetPos(position.x, position.y);
	ret = true;
	return ret;

}

void j1ProjPlayer::setAnimation()
{
	current_animation = &anim;
	
}

// Load animations from tiled  ----------------------------------------------
//void j1Slime::LoadAnimations(const char* path)
//{
//	pugi::xml_parse_result result = slime_file.load_file(path);
//	if (result == NULL)
//	{
//		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());
//
//	}
//
//	TileSetData.firstgid = slime_file.child("map").child("tileset").attribute("firstgid").as_int();
//	TileSetData.tile_width = slime_file.child("map").child("tileset").attribute("tilewidth").as_int();
//	TileSetData.tile_height = slime_file.child("map").child("tileset").attribute("tileheight").as_int();
//	TileSetData.tex_width = slime_file.child("map").child("tileset").child("image").attribute("width").as_int();
//	TileSetData.Texname.create(slime_file.child("map").child("tileset").child("image").attribute("source").as_string());
//	TileSetData.num_tiles_width = TileSetData.tex_width / TileSetData.tile_width;
//	LOG("Tileset: %s", TileSetData.Texname.GetString());
//	LOG("firstgid %d", TileSetData.firstgid);
//	LOG("tile_width %d", TileSetData.tile_width);
//	LOG("tile_height %d", TileSetData.tile_height);
//	LOG("tex_width %d", TileSetData.tex_width);
//	LOG("num_tiles_width %d", TileSetData.num_tiles_width);
//	
//	int i = 0;
//	pugi::xml_node tile;
//	pugi::xml_node frame;
//
//	for (tile = slime_file.child("map").child("tileset").child("tile"); tile; tile = tile.next_sibling("tile")) 
//	{
//		Animation* set = new Animation();
//		for (frame = tile.child("animation").child("frame"); frame; frame = frame.next_sibling("frame"))
//		{
//			set->PushBack(TileSetData.GetAnimRect(frame.attribute("tileid").as_int()), (frame.attribute("duration").as_float()) / 2000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int(), 0, 0);
//			LOG("Animation %d, %d, %d, %d", frame.attribute("tileid").as_int(), (frame.attribute("duration").as_float()) / 1000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int());
//		}
//		animations.add(*set);
//
//	}
//
//}

//Get an sdl rect depending on the frame id we are receiving ----------------------------------------------


void j1ProjPlayer::OnCollision(Collider* c1, Collider* c2) {

	/*
	if (c2->type == COLLIDER_PLAYER_SHOT) {
		c1->to_delete = true;
		to_delete = true;
	}
	*/
}