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
#include "J1Gui.h"
#include "j1Pathfinding.h"
#include "j1MainMenu.h"
#include "j1FadeToBlack.h"
#include "Brofiler/Brofiler.h"
#include "J1Console.h"
#include "J1Hud.h"

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
	App->console->write("Loading Scene");
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
	App->console->write("Start scene");

	current_level = "menu.tmx";
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

	if(current_level == "maplevel1.tmx" || current_level == "maplevel2.tmx")
	{
		CreateEntities();
	}
	
	debug_tex = App->tex->Load("textures/bullside.png");
	flag_tex = App->tex->Load("textures/flag.png");

	if (App->GetPause())
		App->Pause();

	//SDL_Rect rect = { 2, 111, 226, 69 };
	//text = App->gui->CreateGuiElement(Types::text, 25 * (1/ App->win->GetScale()), 5 * (1 / App->win->GetScale()),nullptr,0, this, "COINS");
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Scene", Profiler::Color::Snow);

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
	BROFILER_CATEGORY("Update_Scene", Profiler::Color::Tomato);
	//PAUSE
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		if (current_level == "maplevel1.tmx" || current_level == "maplevel2.tmx")
		{
				if (App->Pause())
				{
					PauseMenu();
				}
				else
				{
					DestroyMenu();
				}
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		//panel->to_delete = true;
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
	
	if(current_level == "maplevel1.tmx")App->render->Blit(flag_tex, 3240 + current_animation->pivotx[current_animation->returnCurrentFrame()], 520 - r.h + current_animation->pivoty[current_animation->returnCurrentFrame()], &r);
	if (current_level == "maplevel2.tmx")App->render->Blit(flag_tex, 3142 + current_animation->pivotx[current_animation->returnCurrentFrame()], 1767 - r.h + current_animation->pivoty[current_animation->returnCurrentFrame()], &r);
	

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
	/*if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;*/

	

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	App->map->CleanUp();
	App->tex->UnLoad(debug_tex);
	App->tex->UnLoad(flag_tex);
	
	LOG("Freeing scene");
	App->console->write("Freeing scene");

	return true;
}
bool j1Scene::Load(pugi::xml_node& data)
{
	LOG("Loading Scene state");
	App->console->write("Loading Scene state");

//	App->player->BarWidth = App->player->maxBarWidth;
	checkpoint = false;
	App->map->CleanUp();
	current_level= data.child("scenename").attribute("name").as_string();
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
	App->console->write("Saving Scene state");

	pugi::xml_node scene = data.append_child("scenename");
	scene.append_attribute("name") = current_level.GetString();

	return true;
}
void j1Scene::Nextmap() {
	
	if (App->menu->ShowHud) {
		App->hud->ShowHud();
		App->menu->ShowHud = false;
	}

	changeEntities = true;
	App->menu->CleanUp();
	App->map->CleanUp();
	checkpoint = false;
//	App->player->BarWidth = App->player->maxBarWidth;
	p2List_item<p2SString>* iterator;
	if (current_level == "menu.tmx")
	{
		current_level = "maplevel1.tmx";
	}
	else {

	
	for (iterator = levels.start; iterator->data != current_level.GetString(); iterator = iterator->next) {
		LOG("%s  %s", iterator->data.GetString(), current_level.GetString());
	}
	if (iterator->next != NULL) { iterator = iterator->next; }
	else { iterator = levels.start; }
	current_level = iterator->data;
	}
	App->map->Load(current_level.GetString());
	App->audio->PlayMusic(App->map->data.music.GetString());

	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);
	App->SaveGame();
	App->hud->coins = 0;
	RELEASE_ARRAY(data);
	
}

//Function that groups all debug functionality ------------------------------------------------
void j1Scene::Debug() {
	// Start from first map

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		startlevel1();
		//App->player->position.x = App->player->initialPosition.x;
		//App->player->position.y = App->player->initialPosition.y;
		//App->player->BarWidth = App->player->maxBarWidth;
		//App->player->flip = SDL_FLIP_NONE;
	
	}
	// Start from second map
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		

		startlevel2();
		/*App->player->position.x = App->player->initialPosition.x;
		App->player->position.y = App->player->initialPosition.y;
		App->player->BarWidth = App->player->maxBarWidth;
		App->player->flip = SDL_FLIP_NONE;*/
	}

	// Start from actual map
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && current_level != "menu.tmx")
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
		if (App->CheckSaveGame()) {
			App->menu->CleanUp();
			App->LoadGame();
		}
		
	}

	// Save current state
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && current_level != "menu.tmx"){
		App->SaveGame();
	}
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {
		if (App->gui->debug) {
			App->gui->debug = false;

		}
		else {
			App->gui->debug = true;
		}
	}
	// View colliders
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		if (App->map->blitColliders) App->map->blitColliders = false;
		else if (!App->map->blitColliders) App->map->blitColliders = true;
	}
	
	// Activate God Mode
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (App->EntityManager->GetPlayer() != NULL) 
		{

		if (!App->EntityManager->GetPlayer()->Godmode)
			{
				App->EntityManager->GetPlayer()->Godmode = true;
			}
		else if (App->EntityManager->GetPlayer()->Godmode)
			{
				App->EntityManager->GetPlayer()->Godmode = false;
			}
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
						case 2079:
								App->EntityManager->CreateEntity(j1Entity::Types::coin, App->map->MapToWorld(x, y).x, App->map->MapToWorld(x, y).y);
								break;
						case 2607:
							App->EntityManager->CreateEntity(j1Entity::Types::coin, App->map->MapToWorld(x, y).x, App->map->MapToWorld(x, y).y);
							break;
						}
					}
				}
			}
		}
		

		layer_iterator = layer_iterator->next;
	}

	App->EntityManager->CreateEntity(j1Entity::Types::player, 100, 500);
	
	return ret;
}

void j1Scene::PauseMenu()
{
	if (App->GetPause() == false) { App->Pause(); }
	LOG("menu de pausa");
	SDL_Rect rect = { 0, 764, 643, 356 };
	panel = App->gui->CreateGuiElement(Types::image, (App->win->width - 643) / 2, (App->win->height - 310) / 2, rect, nullptr, this);
	panel->follow = true;
	panel->delayBlit = true;
	resumeButton = App->gui->CreateGuiElement(Types::button, 20, 100, { 444, 169, 244, 65 }, panel, this, NULL);
	resumeButton->setRects({ 444, 413, 244, 66 }, { 444, 661, 244, 65 });
	resumeButton->delayBlit = true;
	text = App->gui->CreateGuiElement(Types::text, 30, 14, rect, resumeButton, this, "RESUME");
	text->delayBlit = true;

	mainmenuButton = App->gui->CreateGuiElement(Types::button, 20, 180, { 444, 169, 244, 65 }, panel, this, NULL);
	mainmenuButton->setRects({ 444, 413, 244, 66 }, { 444, 661, 244, 65 });
	mainmenuButton->delayBlit = true;
	text2 = App->gui->CreateGuiElement(Types::text, 30, 14, rect, mainmenuButton, this, "MAIN MENU");
	scrollMusic = App->gui->CreateGuiElement(Types::slider, 420, 130, { 28, 257, 12, 189 }, panel, this);
	scrollMusic->delayBlit = true;
	/*GuiItem* image;
	GuiItem* scroll;
	scrollMusic->returnChilds(image, scroll);
	image->delayBlit = true;
	scroll->delayBlit = true;*/

	text2->delayBlit = true;
	scrollFx = App->gui->CreateGuiElement(Types::slider, 550, 130, { 28, 257, 12, 189 }, panel, this);
	scrollFx->delayBlit = true;
	/*scrollFx->returnChilds(image, scroll);
	image->delayBlit = true;
	scroll->delayBlit = true;*/

	text = App->gui->CreateGuiElement(Types::text, 300, 280, rect, panel, this, "Music");
	text->delayBlit = true;
	text = App->gui->CreateGuiElement(Types::text, 490, 280, rect, panel, this, "Fx");
	text->delayBlit = true;
	soundLogo = App->gui->CreateGuiElement(Types::image, 450, 30, { 1884, 379, 72, 71 }, panel, this);
	soundLogo->delayBlit = true;

}

void j1Scene::DestroyMenu()
{
	if (App->GetPause() == true) { App->Pause(); }
	panel->to_delete = true;
	resumeButton->to_delete = true;
	mainmenuButton->to_delete = true;
	text->to_delete = true;
	text2->to_delete = true;
	scrollMusic->to_delete = true;
	scrollFx->to_delete = true;
	soundLogo->to_delete = true;
}

void j1Scene::GuiInput(GuiItem* item)
{
	float f;
	if (item->parent == scrollMusic) {
		f = item->parent->returnSliderPos();
		if (f > 1)
		{
			f = 1;
		}
		if (f < 0)
		{
			f = 0;
		}
		App->audio->musicvolume(f);
	}
	float fxf;
	if (item->parent == scrollMusic) {
		fxf = item->parent->returnSliderPos();
		if (fxf > 1)
		{
			fxf = 1;
		}
		if (fxf < 0)
		{
			fxf = 0;
		}
		App->audio->fxvolume(fxf);
	}

	if (item == resumeButton)
	{
		DestroyMenu();

	}
	if (item == mainmenuButton)
	{
		App->map->CleanUp();
		App->menu->Start();
		checkpoint = false;
		current_level = "menu.tmx";
		App->map->Load(current_level.GetString());
		App->audio->PlayMusic(App->map->data.music.GetString());
		App->EntityManager->EntityCleanUp();
		App->hud->CleanUp();
		DestroyMenu();
	}


}

void j1Scene::startlevel1()
{
	App->map->CleanUp();
	App->menu->CleanUp();

	checkpoint = false;
	current_level = "maplevel1.tmx";
	App->map->Load(current_level.GetString());
	App->audio->PlayMusic(App->map->data.music.GetString());
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);
	RELEASE_ARRAY(data);
	App->hud->coins = 0;
	App->EntityManager->EntityCleanUp();
	CreateEntities();

}

void j1Scene::startlevel2()
{
	App->map->CleanUp();
	App->menu->CleanUp();

	checkpoint = false;
	current_level = "maplevel2.tmx";
	App->map->Load(current_level.GetString());
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);
	RELEASE_ARRAY(data);
	App->hud->coins = 0;
	App->EntityManager->EntityCleanUp();
	CreateEntities();
}