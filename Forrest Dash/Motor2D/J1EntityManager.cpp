#include "J1EntityManager.h"
#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "Animation.h"
#include "j1ModuleCollision.h"
#include "j1Entity.h"
#include "j1Player.h"
#include "j1Wizard.h"
#include "j1Slime.h"
#include "j1ProjPlayer.h"

#include "Brofiler/Brofiler.h"


j1EntityManager::j1EntityManager() {
	name.create("entity");
	
}


j1EntityManager::~j1EntityManager() {

}

//Store config node, in case we want to send it to entities
bool j1EntityManager::Awake(pugi::xml_node& config) {
	node = config;

	return true;
}

//Load and store all the textures we want to use for our entities
bool j1EntityManager::Start() {
	playerTex = App->tex->Load("textures/adventurertex.png");
	slimeTex = App->tex->Load("textures/slimetex.png");
	wizardTex = App->tex->Load("textures/wizardtex.png");
	icespiketex = App->tex->Load("textures/icepick.png");
	return true;

}

//In this section we call al the preupdates, updates and postupdates for our entities --------------------------------------------------
bool j1EntityManager::PreUpdate(float dt) {
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->PreUpdate(dt);
		entities_list = entities_list->next;
	}
	return true;

}

bool j1EntityManager::Update(float dt) 
{
	BROFILER_CATEGORY("Update_EntityManager", Profiler::Color::Yellow);
	if(dt < 0.05) {

		p2List_item<j1Entity*>* entities_list = entities.start;
		while (entities_list) {
			entities_list->data->Update(dt);
			entities_list = entities_list->next;
		}
	}
	//Cheks if there is some entity ready to be deleted
		DeleteEntity();
	
	return true;
}

bool j1EntityManager::PostUpdate(float dt) {
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->PostUpdate(dt);
		entities_list = entities_list->next;
	}
	//In case we want to reset entities, we call to a function in this module
	if (ResetEntities)
		EntitiesReset();
	return true;

}

//  ------------------------------------------------------------------------------------------------------------------------------------------------------
//Unload all textures
bool j1EntityManager::CleanUp() {
	
	App->tex->UnLoad(playerTex);
	App->tex->UnLoad(slimeTex);
	App->tex->UnLoad(wizardTex);
	App->tex->UnLoad(icespiketex);
	
	return true;

}

//Frees entities list
bool j1EntityManager::EntityCleanUp() {

	p2List_item<j1Entity*>* entities_list = entities.start;

	while (entities_list != NULL)
	{
		entities_list->data->CleanUp();
		RELEASE(entities_list->data);
		entities_list = entities_list->next;
	}
	entities.clear();
	return true;

}

//Called when we want to create an entity, we should only receive a type and position for common entities, but projectiles also receive a speed and angle
j1Entity* j1EntityManager::CreateEntity(j1Entity::Types type, int posx, int posy, int speedx, int speedy, float angle)
{
	//static_assert(j1Entity::Types::unknown == (j1Entity::Types)(4), "code needs update");
	j1Entity* ret = nullptr;

	switch (type) {
	case j1Entity::Types::player: ret = new j1Player(posx, posy); break;
	case j1Entity::Types::wizard: ret = new j1Wizard(posx, posy); break;
	case j1Entity::Types::slime: ret = new j1Slime(posx, posy); break;
	case j1Entity::Types::projectile_player: ret = new j1ProjPlayer(posx, posy, speedx, speedy, angle); break;
	}
	
	if (ret != nullptr){
	//Once an entity is created, it is added to a list in stored in the manager, and it's awake and start functions are called
		entities.add(ret);
		entities.end->data->Awake(node);
		entities.end->data->Start();
	}
	return ret;
}


bool j1EntityManager::Load(pugi::xml_node& data)
{
	//In order to load all entities, we delete them with a function
	EntityCleanUp();
	pugi::xml_node entity;

	//Then we iterate the xml, to find every entity that's been saved
	for (entity = data.child("Entity"); entity; entity = entity.next_sibling("Entity"))
	{
		j1Entity::Types id;
		p2SString type(entity.attribute("type").as_string());
		if (type != "Projectile Player")
		{
					
		if (type == "player")
		{
			id = j1Entity::Types::player;
		}
		if (type == "slime")
		{
			id = j1Entity::Types::slime;
		}
		if (type == "wizard")
		{
			id = j1Entity::Types::wizard;
		}
		
		//Once located an entity, we create it
		CreateEntity(id, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
		}
	}

		p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->Load(entity);
		entities_list = entities_list->next;
	}

	return true;
}

//Iterate all entities, create a section for each in savegame.xml nad then call entity save method
bool j1EntityManager::Save(pugi::xml_node& data) const
{
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		if (entities_list->data->name.GetString())
		{
		pugi::xml_node entity = data.append_child("Entity");
		entities_list->data->Save(entity);
		}
		entities_list = entities_list->next;
	
	}
	return true;
}

// Since player isn't an accessible module from App anymore, we created a function to acces it from the manager
j1Entity* j1EntityManager::GetPlayer() {
	
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		if (entities_list->data->name == "player") {
			return entities_list->data;
		}
		entities_list = entities_list->next;
	}
	return NULL;
}

//Check all entities, if their to_delete variable is true, then proced to delete it
void j1EntityManager::DeleteEntity() {
	
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		if (entities_list->data->to_delete == true)
			entities.del(entities_list);
		entities_list = entities_list->next;
	}
}

//If we want to restart the game, or if we die, we call the load game, to return to a previous game state
void j1EntityManager::EntitiesReset() {

		App->LoadGame();
	ResetEntities = false;
	
}