#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "Animation.h"

#define COORDS(a) a+3000 

struct SDL_Texture;
class j1Entity;
class GuiItem;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

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
	void Nextmap();
	void Debug();
	bool CreateEntities();
	void startlevel1();
	void startlevel2();
	
private:
	bool changeEntities = false;
	void PauseMenu();
	void DestroyMenu();
	void GuiInput(GuiItem*);
public:
	p2SString current_level;
	p2List<p2SString> levels;
	SDL_Texture* debug_tex = nullptr;
	SDL_Texture* flag_tex = nullptr;
	Animation* current_animation;
	Animation flag;
	bool checkpoint = false;


	bool pausemenu = false;
	GuiItem* text;
	GuiItem* text2;
	GuiItem* resumeButton;
	GuiItem* mainmenuButton;
	GuiItem* volume;
	GuiItem* panel;
	GuiItem* logo;
};

#endif // __j1SCENE_H__