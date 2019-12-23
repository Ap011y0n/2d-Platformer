#include "j1MainMenu.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "J1Console.h"



j1Console::j1Console()
{
	name.create("console");


}

j1Console::~j1Console()
{
}

bool j1Console::Awake()
{
	return true;
}

bool j1Console::Start()
{
	

	return true;
}

bool j1Console::PreUpdate()
{

	return true;
}

bool j1Console::Update(float dt)
{
	
	return true;
}

bool j1Console::PostUpdate(float dt)
{

	return true;
}


bool j1Console::CleanUp()
{

	return true;
}

bool j1Console::Load(pugi::xml_node& data)
{

	return true;
}

bool j1Console::Save(pugi::xml_node& data) const
{

	return true;
}

void j1Console::GuiInput(GuiItem* item)
{
	
}
