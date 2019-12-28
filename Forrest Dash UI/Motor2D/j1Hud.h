#ifndef __j1HUD_H__
#define __j1HUD_H__

#include "j1Module.h"
#include "Animation.h"

#define MAX_LIFES 3

struct SDL_Texture;


class j1Hud : public j1Module
{
public:
	j1Hud();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	bool ShowHud();



public:
	
	GuiItem* liveFull;
	GuiItem* liveFull2;
	GuiItem* liveFull3;
	GuiItem* liveEmpty;
	GuiItem* liveEmpty2;
	GuiItem* liveEmpty3;
	GuiItem* timer_item;
	GuiItem* minutes_item;
	GuiItem* coins_image;
	GuiItem* coins_collected;
	GuiItem* score_title;
	GuiItem* score_item;
	j1Timer	timer_game;
	int lifesCounter = 3;
	bool activateTimer = false;
	bool extraLife = false;
	bool drawLife = false;
	uint timer = 0;
	uint minutes = 0;
	uint score = 0;
	uint coins = 0;

	char coins_text[10];
	char score_text[10];
	char timer_text[10];
	char minutes_text[10];
private:


public:

	bool SetLifes(int lifes);
	int GetLifes() const;
};



#endif // !__Hud_H__