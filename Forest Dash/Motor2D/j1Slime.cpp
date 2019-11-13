#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Slime.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "Animation.h"
#include "j1ModuleCollision.h"



j1Slime::j1Slime() : j1Module()
{
	name.create("slime");

	graphics = NULL;
	current_animation = NULL;
	LoadAnimations("textures/slime_animations.tmx");

	// Load animations from an animations list ----------------------------------------------
	p2List_item<Animation>* animation_iterator = animations.start;

	idle = animation_iterator->data;
	animation_iterator = animation_iterator->next;
}

j1Slime::~j1Slime()
{

}

// Read player variables from config.xml ----------------------------------------------
bool j1Slime::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;
}

// Load assets ----------------------------------------------
bool j1Slime::Start()
{

	graphics = App->tex->Load("textures/slime.png");

	position.x = 690;
	position.y = 525;
	SDL_Rect r;
	r.w = 60;
	r.h = 60;
	r.x = 690;
	r.y = 525;
	App->collision->AddCollider(r, COLLIDER_ENEMY, this);
	return true;
}

// Unload assets ----------------------------------------------
bool j1Slime::CleanUp()
{

	App->tex->UnLoad(graphics);

	return true;
}

// Update: draw background ----------------------------------------------
bool j1Slime::Update(float dt)
{
	current_animation = &idle;
	SDL_Rect* r = &current_animation->GetCurrentFrame();

	App->render->Blit(graphics, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);

	return true;
}


bool j1Slime::PostUpdate(float dt)
{
	return true;

}

// Load Game State ----------------------------------------------
bool j1Slime::Load(pugi::xml_node& data)
{
	
	return true;
}

// Save Game State ----------------------------------------------
bool j1Slime::Save(pugi::xml_node& data) const
{
	
	return true;
}

// Load animations from tiled  ----------------------------------------------
void j1Slime::LoadAnimations(const char* path)
{
	pugi::xml_parse_result result = slime_file.load_file(path);
	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());

	}

	TileSetData.firstgid = slime_file.child("map").child("tileset").attribute("firstgid").as_int();
	TileSetData.tile_width = slime_file.child("map").child("tileset").attribute("tilewidth").as_int();
	TileSetData.tile_height = slime_file.child("map").child("tileset").attribute("tileheight").as_int();
	TileSetData.tex_width = slime_file.child("map").child("tileset").child("image").attribute("width").as_int();
	TileSetData.Texname.create(slime_file.child("map").child("tileset").child("image").attribute("source").as_string());
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

	for (tile = slime_file.child("map").child("tileset").child("tile"); tile; tile = tile.next_sibling("tile")) 
	{
		Animation* set = new Animation();
		for (frame = tile.child("animation").child("frame"); frame; frame = frame.next_sibling("frame"))
		{
			set->PushBack(TileSetData.GetAnimRect(frame.attribute("tileid").as_int()), (frame.attribute("duration").as_float()) / 2000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int(), 0, 0);
			LOG("Animation %d, %d, %d, %d", frame.attribute("tileid").as_int(), (frame.attribute("duration").as_float()) / 1000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int());
		}
		animations.add(*set);

	}

}

//Get an sdl rect depending on the frame id we are receiving ----------------------------------------------
SDL_Rect TileSetSlime::GetAnimRect(int id) const
{
	int relative_id = id;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = ((rect.w) * (relative_id % num_tiles_width));
	rect.y = ((rect.h) * (relative_id / num_tiles_width));
	return rect;
}

void j1Slime::OnCollision(Collider* c1, Collider* c2) {

}