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
	if(map_loaded == false)
		return;

	// TODO 4: Make sure we draw all the layers and not just the first one
	p2List_item<MapLayer*>* layer_iterator = this->data.layers.start;
	/*p2List_item<Collider*>* collider = App->map->data.colliders.start;
	Collider* col = collider->data;*/
	MapLayer* layer = this->data.layers.start->data;
	while (layer_iterator != NULL) {
		layer = layer_iterator->data;
		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if(tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);
						
						if( layer->returnPropValue(paint)==0){
						App->render->Blit(tileset->texture, pos.x, pos.y, &r);
						}
					}
				}
			}
		}
		//Draw colliders
	/*	collider = App->map->data.colliders.start;
		Collider* col = collider->data;
		while (collider != NULL) {
			SDL_Rect rect2;
			rect2.x = coord.x + col->x;
			rect2.y = coord.y + col->y;
			rect2.h = col->h;
			rect2.w = col->w;
			if (l->tilegid[l->Get(x, y)] == col->id + 1) {
				App->render->DrawQuad(rect2, 0, 0, 225, 100);
			}
			collider = collider->next;

		}*/

		// --------------------------------------------------------------------------------------------------------
		layer_iterator = layer_iterator->next;
		}
	}
TileSet* j1Map::GetTilesetFromTileId(int id) const
{

	// TODO 3: Complete this method so we pick the right
	// Tileset based on a tile id
	
	p2List_item<TileSet*>* tileset_iterator = this->data.tilesets.start;
	TileSet* tileset = tileset_iterator->data;

	while (tileset_iterator != NULL)
	{
		if (id > tileset_iterator->data->firstgid) {
			tileset = tileset_iterator->data; 
			tileset_iterator = tileset_iterator->next;
			
		}
		else { 
			tileset_iterator = tileset_iterator->next;
		}

	}
	return tileset;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2);
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;

	while(item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("maps\\%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file("maps/maplevel2.0.tmx");

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}
		//if (ret == true)
		//{
		//	ret = LoadTilesetColliders(tileset);
		//}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.add(lay);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
	
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;

		
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			LOG("%d", MAX_PROPERTIES);
			for (int i = 0; i < MAX_PROPERTIES; i++) {
				LOG("Prop name: %s  value %d", l->property[i].name.GetString(), l->property[i].prop.ivalue);
			}


			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	
		LoadProperties(node, layer->property);
		
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node,Properties property[])
{
	LOG("Loading properties");
	pugi::xml_node layer;
	bool ret = true;
	
	int i = 0;
	
	for (layer = node.child("properties").child("property"); layer && ret; layer = layer.next_sibling("property"))
	{

		property[i].name = layer.attribute("name").as_string();
		if (property[i].name == "Nodraw") {
			property[i].prop.ivalue = layer.attribute("value").as_int();

			LOG("Layer name %s", property[i].name.GetString());
			LOG("Value =  %d", property[i].prop.ivalue);
			LOG("%d", i);
			i++;
		}
	
		else{
			if (property[i].name == "Navigation") {
				property[i].prop.ivalue = layer.attribute("value").as_int();
				LOG("Layer name %s", property[i].name.GetString());
				LOG("Value= %d", property[i].prop.ivalue);
				LOG("%d", i);
				i++;
			}
			}
	}

	return ret;
}

int MapLayer::returnPropValue(p2SString propName) {
	int value = 0;
	for (int i = 0; i < MAX_PROPERTIES; i++){
	
	if (propName == "Nodraw") {
		value = property->prop.ivalue;
		}
	
	if (propName == "Navigation") {
		value = property->prop.ivalue;
		}
	}
	
	return value;
}
//bool j1Map::LoadTilesetColliders(pugi::xml_node& tileset_node)
//{
//	bool ret = true;
//	pugi::xml_node collider;
//	Collider* coll = new Collider();
//
//	for (collider = tileset_node.child("tile"); collider && ret; collider = tileset_node.next_sibling("tile")) {
//		if (collider == NULL)
//		{
//			LOG("Error parsing tileset xml file: Cannot find 'tileset' tag.");
//			ret = false;
//		}
//		else {
//			coll->id = collider.attribute("id").as_int();
//			coll->type.create(collider.child("objectgroup").child("object").attribute("name").as_string());
//			coll->x = collider.child("objectgroup").child("object").attribute("x").as_int();
//			coll->y = collider.child("objectgroup").child("object").attribute("y").as_int();
//			coll->w = collider.child("objectgroup").child("object").attribute("width").as_int();
//			coll->h = collider.child("objectgroup").child("object").attribute("height").as_int();
//
//			LOG("Collider id %d", coll->id);
//			LOG("Collider type %s", coll->type);
//			LOG("Collider x %d", coll->x);
//			LOG("Collider y %d", coll->y);
//			LOG("Collider y %d", coll->w);
//			LOG("Collider y %d", coll->h);
//		}
//		data.colliders.add(coll);
//	}
//
//
//	return ret;
//}

