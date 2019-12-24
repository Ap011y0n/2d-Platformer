#ifndef __j1MAINMENU_H__
#define __j1MAINMENU_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2DynArray.h"

#define COORDS(a) a+3000 

struct SDL_Texture;
class GuiItem;

class j1MainMenu : public j1Module
{
public:

	j1MainMenu();
	~j1MainMenu();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	//bool Load(pugi::xml_node&);

	//bool Save(pugi::xml_node&) const;

	void GuiInput(GuiItem*);

public:

	SDL_Texture* graphics = nullptr;
	GuiItem* banner;
	GuiItem* text;
	GuiItem* buttonNewGame;
	
};

#endif // !__j1MAINMENU_H__
