#ifndef __j1ENTITYMANAGER_H__
#define __j1ENTITYMANAGER_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"

#define DT_CONVERTER 60

struct SDL_Texture;


class j1EntityManager : public j1Module
{
public:
	j1EntityManager();
	~j1EntityManager();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool EntityCleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&)const;

	j1Entity* CreateEntity(j1Entity::Types type, int posx = 0, int posy = 0);
public:
	p2List<j1Entity*> entities;
	pugi::xml_node node;
	SDL_Texture* playerTex = nullptr;
	SDL_Texture* slimeTex = nullptr;
	SDL_Texture* wizardTex = nullptr;
};

#endif

