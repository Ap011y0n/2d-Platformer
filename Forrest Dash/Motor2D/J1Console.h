#pragma once
#ifndef __j1CONSOLE_H__
#define __j1CONSOLE_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2DynArray.h"

#define COORDS(a) a+3000 

struct _TTF_Font;

enum class Commands
{
	none,
	God_Mode,
	quit,
	FPS,
	map,
	list
};

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
	
	void write(const char*); 

	Commands ReturnCommand(const char*);

	void ExecuteCommand(Commands);

	void Open();

	void Close();

private:
	GuiItem* ConsoleText[MAXTEXT];
	GuiItem* InputText;
	bool opened;
	_TTF_Font* ConsText;
	_TTF_Font* InText;
	int fpsCap;
	int loadMap;



};

#endif // !__j1MAINMENU_H__
