#include "j1Entity.h"
#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
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

j1Entity::j1Entity(Types type) : type(type)
{
}

j1Entity::~j1Entity() {

}


bool j1Entity::CleanUp() {
	return true;
}

bool j1Entity::Load(pugi::xml_node&) {
	return true;
}

bool j1Entity::Save(pugi::xml_node&) const {
	return true;
}

void j1Entity::LoadAnimations(const char* path) {

}

void j1Entity::playfx(const int id, const int rep) {

}

/*
void j1Entity::OnCollision(Collider* c1, Collider* c2) {

}*/