#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Slime.h"
#include "J1EntityManager.h"
#include "J1Entity.h"
#include "j1Pathfinding.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");

	flag.PushBack({ 28, 40, 30, 62 }, 0.2, 0, 0, 0, 0);
	flag.PushBack({ 98, 40, 25, 62 }, 0.2, 0, 0, 0, 0);
	flag.PushBack({ 169, 38, 28, 64 }, 0.2, 0, 0, 0, 0);
	flag.PushBack({ 28, 128, 32, 62 }, 0.2, 0, 0, 0, 0);
	flag.PushBack({ 98, 130, 27, 60 }, 0.2, 0, 0, 0, 0);
	flag.PushBack({ 169, 130, 26, 60 }, 0.2, 0, 0, 0, 0);

}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	pugi::xml_node map;
	for (map = config.child("map"); map; map = map.next_sibling("map")) {
		p2SString lvlname;
		lvlname.create(map.attribute("name").as_string());
		levels.add(lvlname.GetString());
	}
	
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	LOG("Start scene");
	current_level = levels.start->data;
	changeEntities = false;
	checkpoint = false;
	//App->map->Load(current_level.GetString());

	if (App->map->Load(current_level.GetString()) == true)
	{
		App->audio->PlayMusic(App->map->data.music.GetString());
		LOG("%s", current_level.GetString());
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	CreateEntities();
		debug_tex = App->tex->Load("textures/bullside.png");
	
	flag_tex = App->tex->Load("textures/flag.png");
	
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate(float dt)
{
	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN) {
		App->audio->musicvolume(0.05, 1);
		App->audio->fxvolume(0.05, 1);
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN) {
		App->audio->musicvolume(0.05, 0);
		App->audio->fxvolume(0.05, 0);
	}
	
	Debug();
	App->map->Draw();

	int x=0, y=0;
	App->input->GetMousePosition(x, y);

	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	//p2SString title("Forest Dash");
	//Uncomment if want to display map info in game title
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
				App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),map_coordinates.x, map_coordinates.y);
	
	iPoint p = App->render->ScreenToWorld(x, y);
	

	App->render->Blit(debug_tex, p.x-15, p.y-15);

	//App->win->SetTitle(title.GetString());
	current_animation = &flag;
	SDL_Rect r = current_animation->GetCurrentFrame(dt);
	
	App->render->Blit(flag_tex, 3240 + current_animation->pivotx[current_animation->returnCurrentFrame()], 520 - r.h + current_animation->pivoty[current_animation->returnCurrentFrame()], &r);

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	bool ret = true;
	
	if (changeEntities) {
		App->EntityManager->EntityCleanUp();
		CreateEntities();
		changeEntities = false;
	}
	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	App->map->CleanUp();
	App->tex->UnLoad(debug_tex);
	App->tex->UnLoad(flag_tex);
	
	LOG("Freeing scene");

	return true;
}
bool j1Scene::Load(pugi::xml_node& data)
{
	LOG("Loading Scene state");
//	App->player->BarWidth = App->player->maxBarWidth;
	checkpoint = false;
	App->map->CleanUp();
	current_level.create(data.child("scenename").attribute("name").as_string());
	App->map->Load(current_level.GetString());
	
	int w, h;
	uchar* datamap = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &datamap))
		App->pathfinding->SetMap(w, h, datamap);
	RELEASE_ARRAY(datamap);
	App->audio->PlayMusic(App->map->data.music.GetString());

	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const
{
	LOG("Saving Scene state");
	pugi::xml_node scene = data.append_child("scenename");
	scene.append_attribute("name") = current_level.GetString();

	return true;
}
void j1Scene::Nextmap() {
	


	changeEntities = true;
	App->map->CleanUp();
	checkpoint = false;
//	App->player->BarWidth = App->player->maxBarWidth;
	p2List_item<p2SString>* iterator;
	for (iterator = levels.start; iterator->data != current_level.GetString(); iterator = iterator->next) {
		LOG("%s  %s", iterator->data.GetString(), current_level.GetString());
	}
	if (iterator->next != NULL) { iterator = iterator->next; }
	else { iterator = levels.start; }
	current_level = iterator->data;
	
	App->map->Load(current_level.GetString());
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);
	RELEASE_ARRAY(data);
	
}

//Function that groups all debug functionality ------------------------------------------------
void j1Scene::Debug() {
	// Start from first map

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{

		App->map->CleanUp();
		checkpoint = false;
		current_level.create("maplevel1.tmx");
		App->map->Load(current_level.GetString());
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);
		RELEASE_ARRAY(data);

		App->EntityManager->EntityCleanUp();
		CreateEntities();

		//App->player->position.x = App->player->initialPosition.x;
		//App->player->position.y = App->player->initialPosition.y;
		//App->player->BarWidth = App->player->maxBarWidth;
		//App->player->flip = SDL_FLIP_NONE;
	
	}
	// Start from second map
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		

		App->map->CleanUp();
		checkpoint = false;
		current_level.create("maplevel2.tmx");
		App->map->Load(current_level.GetString());
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);
		RELEASE_ARRAY(data);

		App->EntityManager->EntityCleanUp();
		CreateEntities();
		/*App->player->position.x = App->player->initialPosition.x;
		App->player->position.y = App->player->initialPosition.y;
		App->player->BarWidth = App->player->maxBarWidth;
		App->player->flip = SDL_FLIP_NONE;*/
	}

	// Start from actual map
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->EntityManager->EntityCleanUp();
		CreateEntities();
		/*App->player->position.x = App->player->initialPosition.x;
		App->player->position.y = App->player->initialPosition.y;
		App->player->BarWidth = App->player->maxBarWidth;
		App->player->flip = SDL_FLIP_NONE;*/
	}
	// Load last save
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->LoadGame();
	}

	// Save current state
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN){
		App->SaveGame();
	}
	// View colliders
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		if (App->map->blitColliders) App->map->blitColliders = false;
		else if (!App->map->blitColliders) App->map->blitColliders = true;
	}
	
	// Activate God Mode
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		
		if (!App->EntityManager->GetPlayer()->Godmode)
		{
			App->EntityManager->GetPlayer()->Godmode = true;
		}
		else if (App->EntityManager->GetPlayer()->Godmode)
		{
			App->EntityManager->GetPlayer()->Godmode = false;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		if (App->cap == false) {
			App->cap = true;
			App->framecap.create("ON");
		}
		else {
			App->cap = false;
			App->framecap.create("OFF");
		}
	}

}

bool j1Scene::CreateEntities() {

	bool ret = true;
	p2List_item<MapLayer*>* layer_iterator = App->map->data.layers.start;
	MapLayer* layer = App->map->data.layers.start->data;

	

	while (ret == true && layer_iterator != NULL) {
		layer = layer_iterator->data;
		LOG("%s, %d", layer->name.GetString(), layer->returnPropValue("Navigation"));
		if (layer->returnPropValue("Navigation") == 4) {

			for (int y = 0; y < App->map->data.height; ++y)
			{
				for (int x = 0; x < App->map->data.width; ++x)
				{
					int tile_id = layer->Get(x, y);

					if (tile_id != 0)
					{
						switch (tile_id) {
						case 0:
							break;
						case 2077:
							App->EntityManager->CreateEntity(j1Entity::Types::wizard, App->map->MapToWorld(x, y).x, App->map->MapToWorld(x, y).y);
							break;
						case 2076:
							App->EntityManager->CreateEntity(j1Entity::Types::slime, App->map->MapToWorld(x, y).x, App->map->MapToWorld(x, y).y);
							break;
						case 2605:
							App->EntityManager->CreateEntity(j1Entity::Types::wizard, App->map->MapToWorld(x, y).x, App->map->MapToWorld(x, y).y);
							break;
						case 2604:
							App->EntityManager->CreateEntity(j1Entity::Types::slime, App->map->MapToWorld(x, y).x, App->map->MapToWorld(x, y).y);
							break;
						}
					}
				}
			}
		}
		

		layer_iterator = layer_iterator->next;
	}

	App->EntityManager->CreateEntity(j1Entity::Types::player, 100, 500);
	
	/*
	App->EntityManager->CreateEntity(j1Entity::Types::slime, 150, 500);
	App->EntityManager->CreateEntity(j1Entity::Types::wizard, 690, 300);
	App->EntityManager->CreateEntity(j1Entity::Types::wizard, 4200, 450);
	App->EntityManager->CreateEntity(j1Entity::Types::slime, 690, 540);*/
	return ret;
}

