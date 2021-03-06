#include "j1Entity.h"
#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "Animation.h"
#include "j1ModuleCollision.h"
#include "j1Slime.h"
#include "j1Wizard.h"
#include "j1Player.h"
#include "J1EntityManager.h"
#include "J1Console.h"



j1Entity::j1Entity(Types type) : type(type)
{
}

j1Entity::~j1Entity() {

}

//Delete this entity collider on cleanup
bool j1Entity::CleanUp() {

	if(EntityCollider!= false)
	EntityCollider->to_delete = true;
	return true;
}

bool j1Entity::Load(pugi::xml_node& data) {
	LOG("Loading %s state",name.GetString());
	App->console->write("Loading entity state");
	return true;
}

bool j1Entity::Save(pugi::xml_node& data) const {
	LOG("Saving %s state", name.GetString());
	App->console->write("Saving entity state");
	data.append_child("position");

	data.append_attribute("type") = name.GetString();
	data.child("position").append_attribute("pos_x") = position.x;
	data.child("position").append_attribute("pos_y") = position.y;
	return true;
}
//gets an specific rect dimentions, depending on the tileset
SDL_Rect TileSetEntity::GetAnimRect(int id) const
{
	int relative_id = id;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = ((rect.w) * (relative_id % num_tiles_width));
	rect.y = ((rect.h) * (relative_id / num_tiles_width));
	return rect;
}

//Automatically load and store animations from tiled
void j1Entity::LoadAnimations(const char* path) {
	pugi::xml_document	entity_file;
	pugi::xml_parse_result result = entity_file.load_file(path);
	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());
		App->console->write("Could not load map xml file");

	}
	TileSetData.firstgid = entity_file.child("map").child("tileset").attribute("firstgid").as_int();
	TileSetData.tile_width = entity_file.child("map").child("tileset").attribute("tilewidth").as_int();
	TileSetData.tile_height = entity_file.child("map").child("tileset").attribute("tileheight").as_int();
	TileSetData.tex_width = entity_file.child("map").child("tileset").child("image").attribute("width").as_int();
	TileSetData.Texname.create(entity_file.child("map").child("tileset").child("image").attribute("source").as_string());
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
	for (tile = entity_file.child("map").child("tileset").child("tile"); tile; tile = tile.next_sibling("tile")) {
		Animation* set = new Animation();
		for (frame = tile.child("animation").child("frame"); frame; frame = frame.next_sibling("frame")) {
			set->PushBack(TileSetData.GetAnimRect(frame.attribute("tileid").as_int()), (frame.attribute("duration").as_float()) / 2000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int(), 0, 0);
			LOG("Animation %d, %f, %d, %d", frame.attribute("tileid").as_int(), (frame.attribute("duration").as_float()) / 1000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int());
		}
		animations.add(*set);
	}
}

//Sets collider position depending of entity's position
void j1Entity::DrawHitbox() {
	if (EntityCollider != nullptr) {
		EntityCollider->SetPos(position.x + 10, position.y);
	}
}

/*void j1Entity::playfx(const int id, const int rep) {

}*/
