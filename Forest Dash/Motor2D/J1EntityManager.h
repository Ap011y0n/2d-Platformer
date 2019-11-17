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

	j1Entity* CreateEntity(j1Entity::Types type);
public:
	p2List<j1Entity*> entities;
	pugi::xml_node node;
};

#endif
