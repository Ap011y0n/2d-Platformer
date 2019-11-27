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
#include "j1Particles.h"
#include "j1Entity.h"
#include "j1Player.h"
#include "j1Wizard.h"
#include "j1Slime.h"
#include "Brofiler/Brofiler.h"


j1EntityManager::j1EntityManager() {
	name.create("entity");
	
}


j1EntityManager::~j1EntityManager() {

}

bool j1EntityManager::Awake(pugi::xml_node& config) {
	node = config;

	LOG("Awake NODE %d", node.child("initialPosition").attribute("x").as_int());

	return true;
}

bool j1EntityManager::Start() {
	playerTex = App->tex->Load("textures/adventurertex.png");
	slimeTex = App->tex->Load("textures/slimetex.png");
	wizardTex = App->tex->Load("textures/wizardtex.png");
	return true;

}
bool j1EntityManager::Update(float dt) 
{
	BROFILER_CATEGORY("Update_EntityManager", Profiler::Color::Yellow);

	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->Update(dt);
		entities_list = entities_list->next;
	}
	DeleteEntity();

	return true;
}

bool j1EntityManager::PostUpdate(float dt) {
	return true;

}

bool j1EntityManager::CleanUp() {
	
	App->tex->UnLoad(App->EntityManager->playerTex);
	App->tex->UnLoad(App->EntityManager->slimeTex);
	App->tex->UnLoad(App->EntityManager->wizardTex);
	
	return true;

}

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

j1Entity* j1EntityManager::CreateEntity(j1Entity::Types type, int posx, int posy)
{
	//static_assert(j1Entity::Types::unknown == 5, "code needs update");
	j1Entity* ret = nullptr;

	LOG("Awake NODE %d", node.child("initialPosition").attribute("x").as_int());
	switch (type) {
	case j1Entity::Types::player: ret = new j1Player(posx, posy); break;
	case j1Entity::Types::wizard: ret = new j1Wizard(posx, posy); break;
	case j1Entity::Types::slime: ret = new j1Slime(posx, posy); break;
	}
	
	if (ret != nullptr){

		entities.add(ret);
		entities.end->data->Awake(node);
		entities.end->data->Start();
	}
	return ret;
}
bool j1EntityManager::Load(pugi::xml_node& data)
{
	EntityCleanUp();
	pugi::xml_node entity;

	for (entity = data.child("Entity"); entity; entity = entity.next_sibling("Entity"))
	{
		j1Entity::Types id;
		p2SString type(entity.attribute("type").as_string());
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
		CreateEntity(id, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
	}

		p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->Load(entity);
		entities_list = entities_list->next;
	}

	return true;
}
bool j1EntityManager::Save(pugi::xml_node& data) const
{
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		pugi::xml_node entity = data.append_child("Entity");
		entities_list->data->Save(entity);
		entities_list = entities_list->next;
	}
	
	return true;
}

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

void j1EntityManager::DeleteEntity() {
	
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		if (entities_list->data->to_delete == true)
			entities.del(entities_list);

		entities_list = entities_list->next;
	}
}