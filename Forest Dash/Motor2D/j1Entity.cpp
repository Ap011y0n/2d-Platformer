#include "j1Entity.h"
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
#include "j1Slime.h"
#include "j1Wizard.h"
#include "j1Player.h"


j1Entity::j1Entity(Types type) : type(type)
{
}

j1Entity::~j1Entity() {

}


bool j1Entity::CleanUp() {
	return true;
}

bool j1Entity::Load(pugi::xml_node& data) {
	LOG("Loading %s state",name.GetString());

	position.x = data.child(name.GetString()).child("position").attribute("pos_x").as_int();
	position.y = data.child(name.GetString()).child("position").attribute("pos_y").as_int();
	return true;
}

bool j1Entity::Save(pugi::xml_node& data) const {
	LOG("Saving %s state", name.GetString());

	pugi::xml_node entity = data.append_child(name.GetString());

	entity.append_child("position");

	entity.child("position").append_attribute("pos_x") = position.x;
	entity.child("position").append_attribute("pos_y") = position.y;
	return true;
}

void j1Entity::LoadAnimations(const char* path) {

}

void j1Entity::playfx(const int id, const int rep) {

}

/*
void j1Entity::OnCollision(Collider* c1, Collider* c2) {

}*/