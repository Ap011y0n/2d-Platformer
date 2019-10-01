#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if (map_loaded == false)
		return;
	
	// TODO 6: Iterate all tilesets and draw all their 
	// images in 0,0 (you should have only one tileset for now)
	for (j = 0; j < maxTilesets; j++) {
		rect.x = 0;
		rect.y = 0;
		rect.h = map_file.child("map").child("tileset").child("image").attribute("height").as_uint();
		rect.w = map_file.child("map").child("tileset").child("image").attribute("width").as_uint();
	}
	App->render->Blit(imageLoad, 0, 0, &rect);
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// TODO 2: Make sure you clean up any memory allocated
	// from tilesets / map
	delete [] tiles;
	delete[] layerData;
	//delete MapData;
		
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	if (ret == true)
	{
		LoadMap(map_file.child("map"));
		imageLoad = (App->tex->Load("maps/tmw_desert_spacing.png"));

		// TODO 3: Create and call a private function to load and fill
		// all your map data
	}

	// TODO 4: Create and call a private function to load a tileset
	// remember to support more any number of tilesets!
	for (pugi::xml_node tileset = map_file.child("map").child("tileset"); tileset; tileset = tileset.next_sibling("tileset")) {
		maxTilesets =  map_file.child("map").child("tileset").attribute("firstgid").as_uint();
	}
	for (pugi::xml_node layer = map_file.child("map").child("layer"); layer; layer = layer.next_sibling("layer")) {
		maxLayers++;
	}
	LOG("Reporting sir, the max number of layers is %d", maxLayers);
	tiles = new tileset[maxTilesets];
	layerData = new layer[maxLayers];
	
	for (j = 0; j < maxTilesets; j++) {
		LoadTileset(tiles[j]);
	}
	for (i = 0; i < maxLayers; i++) {
		LoadLayer(layerData[i]);
		LOG("%d", i);
	}

	if (ret == true)
	{
		// TODO 5: LOG all the data loaded
		// iterate all tilesets and LOG everything
		LOG("Orientation%d", MapData.orientation);
		LOG("Renderorder %d", MapData.renderorder);
		LOG("Width %d", MapData.width);
		LOG("Height %d", MapData.height);
		LOG("Tilewidth %d", MapData.tilewidth);
		LOG("TileHeigth %d", MapData.tileheight);
		LOG("Nextobject %d", MapData.nextobjectid);

		for (j = 0; j < maxTilesets; j++) {
			LOG("TILE Nº %d", j);
			LOG("Firstgid %d", tiles[j].firstgid);
			LOG("Name %s", tiles[j].name);
			LOG("Tilewidth %d", tiles[j].tilewidth);
			LOG("Tileheight %d", tiles[j].tileheight);
			LOG("Spacing %d", tiles[j].spacing);
			LOG("Margin %d", tiles[j].margin);
		}
		for (i = 0; i < maxTilesets; i++) {
			LOG("LAYER Nº %d", i);
			LOG("Name %s", layerData[i].name);
			LOG("Layerwidth %d", layerData[i].width);
			LOG("Layerheight %d", layerData[i].height);
		}
	}
	
	
	map_loaded = ret;

	return ret;
}

void j1Map::LoadMap(pugi::xml_node& node) {

	MapData.orientation = MapData.OR_ORTHOGONAL;
	MapData.renderorder = MapData.REN_RIGHTDOWN;
	MapData.width = node.attribute("width").as_uint();
	MapData.height = node.attribute("height").as_uint();
	MapData.tilewidth = node.attribute("tilewidth").as_uint();
	MapData.tileheight = node.attribute("tileheight").as_uint();
	MapData.nextobjectid = node.attribute("nextobjectid").as_uint();
	
	
}

void j1Map::LoadTileset(tileset& tile) {
		for (pugi::xml_node tileset = map_file.child("map").child("tileset"); tileset; tileset = tileset.next_sibling("tileset")) {

	if (tileset.attribute("firstgid").as_uint()-1 == j) {
			tile.firstgid = tileset.attribute("firstgid").as_uint();
			tile.name = tileset.attribute("name").as_string();
			tile.tilewidth = tileset.attribute("tilewidth").as_uint();
			tile.tileheight = tileset.attribute("tileheight").as_uint();
			tile.spacing = tileset.attribute("spacing").as_uint();
			tile.margin = tileset.attribute("margin").as_uint();

		}
	}

}

void j1Map::LoadLayer(layer& layerdata) {
	int x = 0;
	for (pugi::xml_node layer = map_file.child("map").child("layer"); layer; layer = layer.next_sibling("layer")) {

		if (x == i) {
		
			layerdata.name = layer.attribute("name").as_string();
			layerdata.width = layer.attribute("width").as_uint();
			layerdata.height = layer.attribute("height").as_uint();
			
		}
		x++;
	}

}

