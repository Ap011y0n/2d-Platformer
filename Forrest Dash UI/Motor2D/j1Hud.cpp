#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1EntityManager.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Timer.h"
#include "j1Gui.h"
#include "j1Hud.h"
#include "Brofiler/Brofiler.h"

j1Hud::j1Hud() :j1Module()
{
	name.create("hud");
}

bool j1Hud::Awake(pugi::xml_node& node)
{
	
	return true;
}

bool j1Hud::Start()
{
	
	lifesCounter = 3;

	return true;
}

bool j1Hud::PreUpdate()
{
	return true;
}

bool j1Hud::Update(float dt)
{

	
	//TIMER
	if(activateTimer)
	{
		

		if (timer_game.Read() >= 1000)
		{
			timer_game.Start();
			timer++;

		}
		if (timer == 60)
		{
			timer = 0;
			minutes += 1;
		}

		sprintf_s(timer_text, 10, "%d", timer);
		sprintf_s(minutes_text, 10, "%d", minutes);

		if (timer < 10)
		{
			timer_text[2] = timer_text[1];
			timer_text[1] = timer_text[0];
			timer_text[0] = '0';
		}
		if (minutes < 10)
		{
			minutes_text[2] = minutes_text[1];
			minutes_text[1] = minutes_text[0];
			minutes_text[0] = '0';
		}

		timer_item = App->gui->CreateGuiElement(Types::text, 200, 50, { 157, 258, 36, 34 }, NULL, this, timer_text);
		timer_item->follow = true;
		timer_item->to_delete = true;

		minutes_item = App->gui->CreateGuiElement(Types::text, 160, 50, { 157, 258, 36, 34 }, NULL, this, minutes_text);
		minutes_item->follow = true;
		minutes_item->to_delete = true;
	}


	return true;
}

bool j1Hud::PostUpdate()
{
	
	return true;
}

bool j1Hud::CleanUp()
{
	
	return true;
}

bool j1Hud::Load(pugi::xml_node& node)
{

	return true;
}

bool j1Hud::Save(pugi::xml_node& node) const
{
	

	return true;
}

bool j1Hud::SetLifes(int lifes)
{
	if (lifes <= 0)
	{
		lifes = 0;
		
	}
	else if (lifes > MAX_LIFES)
	{
		lifes = MAX_LIFES;
	}
	else
	{
		lifesCounter = lifes;
	}

	switch (lifes)
	{
	case 2:
		liveFull3->to_delete = true;
		break;
	case 1:
		liveFull2->to_delete = true;
		break;
	case 0:
		liveFull->to_delete = true;
		break;
	default:
		break;

	}
	
	

	return true;
}

int j1Hud::GetLifes() const
{

	return lifesCounter;
}

bool j1Hud::ShowHud()
{
	timer_game.Start();
	liveEmpty = App->gui->CreateGuiElement(Types::image, 20, 50, { 157, 258, 36, 34 }, nullptr);
	liveEmpty2 = App->gui->CreateGuiElement(Types::image, 60, 50, { 157, 258, 36, 34 }, nullptr);
	liveEmpty3 = App->gui->CreateGuiElement(Types::image, 100, 50, { 157, 258, 36, 34 }, nullptr);
	liveFull = App->gui->CreateGuiElement(Types::image, 20, 50, { 119, 258, 36, 34 }, nullptr);
	liveFull2 = App->gui->CreateGuiElement(Types::image, 60, 50, { 119, 258, 36, 34 }, nullptr);
	liveFull3 = App->gui->CreateGuiElement(Types::image, 100, 50, { 119, 258, 36, 34 }, nullptr);
	liveEmpty->follow = true;
	liveEmpty2->follow = true;
	liveEmpty3->follow = true;
	liveFull->follow = true;
	liveFull2->follow = true;
	liveFull3->follow = true;
	activateTimer = true;
	
	return true;
}





