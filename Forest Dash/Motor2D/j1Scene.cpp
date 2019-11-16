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

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
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
	App->map->Load(current_level.GetString());
	App->audio->PlayMusic(App->map->data.music.GetString());
	LOG("%s", current_level.GetString());

	App->EntityManager->CreateEntity(j1Entity::Types::player);
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

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
	App->render->fade = false;
	Debug();
	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	//p2SString title("Forest Dash");
	//Uncomment if want to display map info in game title
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	//App->win->SetTitle(title.GetString());

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	
	App->map->CleanUp();
	LOG("Freeing scene");

	return true;
}
bool j1Scene::Load(pugi::xml_node& data)
{
	LOG("Loading Scene state");
//	App->player->BarWidth = App->player->maxBarWidth;
	App->map->CleanUp();
	current_level.create(data.child("scenename").attribute("name").as_string());
	App->map->Load(current_level.GetString());
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

	App->map->CleanUp();
//	App->player->BarWidth = App->player->maxBarWidth;
	p2List_item<p2SString>* iterator;
	for (iterator = levels.start; iterator->data != current_level.GetString(); iterator = iterator->next) {
		LOG("%s  %s", iterator->data.GetString(), current_level.GetString());
	}
	if (iterator->next != NULL) { iterator = iterator->next; }
	else { iterator = levels.start; }
	current_level = iterator->data;
	
	App->map->Load(current_level.GetString());
	
}

//Function that groups all debug functionality ------------------------------------------------
void j1Scene::Debug() {
	// Start from first map
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	/*{
		App->map->CleanUp();
		current_level.create("maplevel1.tmx");
		App->map->Load(current_level.GetString());
		App->player->position.x = App->player->initialPosition.x;
		App->player->position.y = App->player->initialPosition.y;
		App->player->BarWidth = App->player->maxBarWidth;
		App->player->flip = SDL_FLIP_NONE;
	
	}*/
	// Start from second map
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	//{
	//	App->map->CleanUp();
	//	current_level.create("maplevel2.tmx");
	//	App->map->Load(current_level.GetString());
	//	App->player->position.x = App->player->initialPosition.x;
	//	App->player->position.y = App->player->initialPosition.y;
	//	App->player->BarWidth = App->player->maxBarWidth;
	//	App->player->flip = SDL_FLIP_NONE;
	//}

	// Start from actual map
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	//{
	//	App->player->position.x = App->player->initialPosition.x;
	//	App->player->position.y = App->player->initialPosition.y;
	//	App->player->BarWidth = App->player->maxBarWidth;
	//	App->player->flip = SDL_FLIP_NONE;
	//}
	// Load last save
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->render->fade = true; App->LoadGame();
	}

	// Save current state
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->SaveGame();

	// View colliders
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		if (App->map->blitColliders) App->map->blitColliders = false;
		else if (!App->map->blitColliders) App->map->blitColliders = true;
	}
	
	// Activate God Mode
	//if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
	//	if (App->player->Godmode == false)
	//	{
	//		App->player->Godmode = true;
	//	}
	//	else if (App->player->Godmode == true)
	//	{
	//		App->player->Godmode = false;
	//	}
	//}

}
