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
#include "math.h"
#include "j1ModuleCollision.h"
#include "j1Particles.h"
#include "j1Entity.h"
#include "j1Player.h"
#include "j1Wizard.h"
#include "j1Slime.h"



j1EntityManager::j1EntityManager() {
	name.create("entity");

}


j1EntityManager::~j1EntityManager() {

}

bool j1EntityManager::Awake(pugi::xml_node& config) {
	node = config;
	LOG("%d", node.child("initialPosition").attribute("x").as_int());
	return true;
}

bool j1EntityManager::Start() {
	return true;

}
bool j1EntityManager::Update(float dt) {
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->Update(dt);
		entities_list = entities_list->next;
	}

	return true;
}
bool j1EntityManager::PostUpdate(float dt) {
	return true;

}
bool j1EntityManager::CleanUp() {

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

j1Entity* j1EntityManager::CreateEntity(j1Entity::Types type, int posx, int posy, char* tag)
{
	/*static_assert(j1Entity::Types::unknown == 5, "code needs update");*/
	j1Entity* ret = nullptr;

	switch (type) {
	case j1Entity::Types::player: ret = new j1Player(posx, posy, tag); break;
	case j1Entity::Types::wizard: ret = new j1Wizard(posx, posy, tag); break;
	case j1Entity::Types::slime: ret = new j1Slime(posx, posy, tag); break;
	}
	LOG("%d", node.child("initialPosition").attribute("x").as_int());
	if (ret != nullptr){

		entities.add(ret);
		entities.end->data->Awake(node);
		entities.end->data->Start();
	}
	return ret;
}
bool j1EntityManager::Load(pugi::xml_node& data)
{
//	CleanUp();
	
		p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->Load(data);
		entities_list = entities_list->next;
	}

	return true;
}
bool j1EntityManager::Save(pugi::xml_node& data) const
{
	p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->Save(data);
		entities_list = entities_list->next;
	}
	
	return true;
}