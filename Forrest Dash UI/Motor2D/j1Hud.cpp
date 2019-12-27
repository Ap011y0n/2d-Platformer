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

	LOG("LIFES %d ", lifesCounter);

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
	
	return true;
}





