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
#include "J1EntityManager.h"
#include "Brofiler/Brofiler.h"

j1Wizard::j1Wizard(int posx, int posy) : j1Entity(Types::wizard)
{
	name.create("wizard");

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


// Load assets ----------------------------------------------
bool j1Wizard::Start()
{
	LOG("Start Wizard");
	App->EntityManager->wizardTex;
	to_delete = false;

	
	EntityCollider = App->collision->AddCollider(&r, COLLIDER_WIZARD, this);

	startMoving = SDL_GetTicks();
	collided = SDL_GetTicks();
	return true;
}

// Unload assets ----------------------------------------------


// Update: draw background ----------------------------------------------
bool j1Wizard::Update(float dt)
{
	BROFILER_CATEGORY("Update_Wizard", Profiler::Color::Orchid);
	Movement();
	setAnimation();

	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);

	App->render->Blit(App->EntityManager->wizardTex, position.x + (current_animation->pivotx[current_animation->returnCurrentFrame()]), position.y + (current_animation->pivoty[current_animation->returnCurrentFrame()]), r, 1.0f, 1.0f, flip);
	
	if (App->EntityManager->GetPlayer()->position.x > position.x - 300 && App->EntityManager->GetPlayer()->position.x < position.x + 300)
	Pathfinding(dt);


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

	if (SDL_GetTicks() > (startMoving + 1000))
	{
		state = WD_FORWARD;
	}

	EntityCollider->SetPos(position.x, position.y);

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
			EntityCollider->to_delete = true;

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

	if (c2->type == COLLIDER_PLAYER_SHOT) {
		c1->to_delete = true;
		to_delete = true;
	}
}

void j1Wizard::Pathfinding(float dt) {
	speedX = 0;
	speedY = 0;
	static iPoint origin;
	static bool origin_selected = false;
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->EntityManager->GetPlayer()->position;
	p = App->map->WorldToMap(p.x, p.y);

	if (origin_selected == true)
	{
		App->pathfinding->CreatePath(origin, p);
		origin_selected = false;

	}
	else
	{
		origin = App->map->WorldToMap(position.x, position.y);
		origin_selected = true;
	}

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint nextPoint = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		if (App->collision->debug)
		{
			App->render->Blit(App->scene->debug_tex, nextPoint.x, nextPoint.y);
		}

		if (App->pathfinding->CreatePath(App->map->WorldToMap(position.x +10, position.y + 10), App->map->WorldToMap(App->EntityManager->GetPlayer()->position.x, App->EntityManager->GetPlayer()->position.y)) > -1)
		{
			if (nextPoint.x < position.x)
			{
				flip = SDL_RendererFlip::SDL_FLIP_NONE;
				speedX = -40 * dt;
			}
			else if (nextPoint.x > position.x)
			{
				flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
				speedX = 40 * dt;
			}

			if (nextPoint.y < position.y)
			{
				speedY = -40 * dt;
			}
			else if (nextPoint.y > position.y)
			{
				speedY = 40 * dt;
			}
		}
		else
		{
			if (SDL_GetTicks() > (collided + 100) && App->EntityManager->GetPlayer()->position.x < position.x)
			{
				position.x -= 20;
			}
			if (SDL_GetTicks() > (collided + 100) && App->EntityManager->GetPlayer()->position.x < position.x)
			{
				position.x += 20;
			}
			if (SDL_GetTicks() > (collided + 100) && App->EntityManager->GetPlayer()->position.y < position.y)
			{
				position.y -= 20;
			}
			if (SDL_GetTicks() > (collided + 100) && App->EntityManager->GetPlayer()->position.y < position.y)
			{
				position.y += 20;
			}
			current_animation = &idle;
			speedX = 0;
			speedY = 0;
		}
		position.x += speedX;
		position.y += speedY;
	}
}

