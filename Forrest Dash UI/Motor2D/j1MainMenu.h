#ifndef __j1MAINMENU_H__
#define __j1MAINMENU_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2DynArray.h"
#include "p2Point.h"

#define COORDS(a) a+3000 

enum class Section : int
{
	
	settings = 0,
	main_menu = 1,
	credits = 2,
	max = 3
};

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

private:

	bool MoveToSection(Section menu_section);
public:

	SDL_Texture* graphics = nullptr;
	GuiItem* banner;
	GuiItem* text;
	GuiItem* buttonNewGame;
	GuiItem* buttonContinue;
	GuiItem* buttonExit;
	GuiItem* buttonSettings;
	GuiItem* buttonCredits;
	GuiItem* buttonSettingsToMenu;
	GuiItem* buttonCreditsToMenu;
	GuiItem* creditsPanel;
	GuiItem* licenseText;
	GuiItem* settingsPanel;
	GuiItem* creditsText;
	GuiItem* buttonToRepo;
	GuiItem* buttonToWeb;
	bool camLock = false;

private:

	Section current_section = Section::main_menu;
	fPoint moveToPoint[(int)Section::max];
	fPoint camVelocity = { 0.0f , 0.0f };
	fPoint camPos = { 0.0f, 0.0f };
	float camSpeed = 0.0f;
	
};

#endif // !__j1MAINMENU_H__
