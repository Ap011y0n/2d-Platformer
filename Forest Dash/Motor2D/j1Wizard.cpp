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
#include "j1Pathfinding.h"
#include "j1ModuleCollision.h"

#include "Brofiler/Brofiler.h"

j1Wizard::j1Wizard(int posx, int posy, char* tag) : j1Entity(Types::wizard)
{
	name.create(tag);

	graphics = NULL;
	current_animation = NULL;
	LoadAnimations("textures/wizard_animations.tmx");
	state = WD_IDLE;
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

	r.w = 40;
	r.h = 70;
	r.x = position.x;
	r.y = position.y;
}

j1Wizard::~j1Wizard()
{

}

// Read player variables from config.xml ----------------------------------------------
bool j1Wizard::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;
}

// Load assets ----------------------------------------------
bool j1Wizard::Start()
{
	LOG("Start Wizard");
	graphics = App->tex->Load("textures/wizardtexture.png");


	
	colliderWizard = App->collision->AddCollider(&r, COLLIDER_WIZARD, this);

	startMoving = SDL_GetTicks();
	return true;
}

// Unload assets ----------------------------------------------
bool j1Wizard::CleanUp()
{

	App->tex->UnLoad(graphics);

	return true;
}

// Update: draw background ----------------------------------------------
bool j1Wizard::Update(float dt)
{
	BROFILER_CATEGORY("Wizard", Profiler::Color::Orchid);
	Movement();
	setAnimation();

	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(graphics, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);
	static iPoint origin;
	static bool origin_selected = false;
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		
		}
		else
		{
			origin = App->map->WorldToMap(position.x ,position.y);
			origin_selected = true;
		}
	return true;
}


bool j1Wizard::PostUpdate(float dt)
{
	return true;

}

//// Load Game State ----------------------------------------------
//bool j1Wizard::Load(pugi::xml_node& data)
//{
//
//	return true;
//}
//
//// Save Game State ----------------------------------------------
//bool j1Wizard::Save(pugi::xml_node& data) const
//{
//
//	return true;
//}

void j1Wizard::Movement()
{
	if (wizarDead) state = WD_DEATH;

	if (SDL_GetTicks() > (startMoving + 2500))
	{
		state = WD_FORWARD;
	}

	colliderWizard->SetPos(position.x, position.y);

}

void j1Wizard::setAnimation()
{
	if (state == WD_IDLE)
	{
		current_animation = &idle;
		death.Reset();
	}
	if (state == WD_DEATH)
	{
		
		current_animation = &death;

		if (SDL_GetTicks() > (deathTimerWizard + 2500)) {

			CleanUp();
			colliderWizard->to_delete = true;

		}
	}
	if (state == WD_FORWARD)
	{
		current_animation = &forward;
	}
}

// Load animations from tiled  ----------------------------------------------
void j1Wizard::LoadAnimations(const char* path)
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
SDL_Rect TileSetWizard::GetAnimRect(int id) const
{
	int relative_id = id;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = ((rect.w) * (relative_id % num_tiles_width));
	rect.y = ((rect.h) * (relative_id / num_tiles_width));
	return rect;
}

void j1Wizard::OnCollision(Collider* c1, Collider* c2) {

}