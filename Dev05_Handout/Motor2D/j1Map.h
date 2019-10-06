#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "PugiXml\src\pugixml.hpp"

// TODO 1: Create a struct for the map layer
// ----------------------------------------------------
struct Layer {
	p2SString			name;
	uint				width = 0u;
	uint				height = 0u;
	uint				*tilegid;
	inline uint Get(int x, int y) const {
		uint value;
		value = x + y * width;
		return value;
	}
};

	// TODO 6: Short function to get the value of x,y


// ----------------------------------------------------
struct TileSet
{
	// TODO 7: Create a method that receives a tile id and returns it's Rect
	SDL_Rect getRekt(int gid);
	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
	
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	// TODO 2: Add a list/array of layers to the map!
	p2List<Layer*>	layers;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint ReturnPos(int x, int y, int rect);
	// TODO 8: Create a method that translates x,y coordinates from map positions to world positions

private:
	
	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	
	// TODO 3: Create a method that loads a single laye
	bool LoadLayer(pugi::xml_node& node, Layer* layer);
	int GetID(pugi::xml_node& node);

public:

	MapData data;
	int TilesetId = 0;
	int TilesetId2 = 0;
	pugi::xml_node MapNode;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
	
	SDL_Rect rect;
	SDL_Texture* image;
};

#endif // __j1MAP_H__