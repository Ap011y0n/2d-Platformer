#pragma once
#ifndef __j1CONSOLE_H__
#define __j1CONSOLE_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2DynArray.h"

#define COORDS(a) a+3000 
#define MAXTEXT 100

class GuiItem;

class j1Console : public j1Module
{
public:

	j1Console();
	~j1Console();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt);

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	void GuiInput(GuiItem*);

public:

private:
	GuiItem* InputText;
	GuiItem* ConsoleText[MAXTEXT];
	p2SString temp[MAXTEXT];

	SDL_Texture* graphics = nullptr;
	SDL_Rect textureRect;


};

#endif // !__j1MAINMENU_H__