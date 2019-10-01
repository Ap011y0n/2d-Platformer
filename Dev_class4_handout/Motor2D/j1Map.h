#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

// TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
// ----------------------------------------------------
struct tileset {
	uint firstgid = 0u;
	p2SString name;
	uint tilewidth = 0u;
	uint tileheight = 0u;
	uint spacing = 0u;
	uint margin = 0u;
};

struct layer {
	p2SString name;
	uint width = 0u;
	uint height = 0u;
};
// TODO 1: Create a struct needed to hold the information to Map node

struct map {
	enum orientation {
		OR_NULL,
		OR_ORTHOGONAL,
		OR_ISOMETRIC,
		OR_HEXAGONAL
	};

	enum renderorder {
		REN_NULL,
		REN_RIGHTDOWN,
		REN_LEFTDOWN,
		REN_LEFTUP
	};
	uint orientation = 0u;
	uint renderorder = 0u;
	uint width = 0u;
	uint height = 0u;
	uint tilewidth = 0u;
	uint tileheight = 0u;
	uint nextobjectid = 0u;
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

	
private:
	//Load map xml
	void LoadMap(pugi::xml_node& tile);
	void LoadTileset(tileset&);
	void LoadLayer(layer&);


public:

	// TODO 1: Add your struct for map info as public for now
	struct map MapData;
	struct tileset* tiles;
	struct layer* layerData;
	
	int j = 0;
	int i = 0;
	SDL_Texture* imageLoad;
	SDL_Rect rect;
	uint maxTilesets;
	uint maxLayers = 0;
	
private:

	pugi::xml_document	map_file;
	
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__