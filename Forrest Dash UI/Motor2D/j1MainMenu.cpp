#include "j1MainMenu.h"
#include "j1Scene.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1EntityManager.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "p2Log.h"
#include "j1Player.h"
#include "j1Textures.h"


j1MainMenu::j1MainMenu()
{
	name.create("main_menu");

	
}

j1MainMenu::~j1MainMenu()
{
}

bool j1MainMenu::Awake()
{
	return true;
}

bool j1MainMenu::Start()
{
	graphics = App->tex->Load("textures/atlas.png");
	r = { 0, 0, 427, 218 };
	//r = { 444, 169, 201, 65 };
	App->render->camera.x = -3060;

	return true;
}

bool j1MainMenu::PreUpdate()
{
	
	return true;
}

bool j1MainMenu::Update(float dt)
{
	App->render->Blit(graphics, 3345, 30, &r);
	//App->render->Blit(graphics, 3480, 390, &r);
	return true;
}

bool j1MainMenu::PostUpdate(float dt)
{

	return true;
}


bool j1MainMenu::CleanUp()
{
	
	return true;
}

bool j1MainMenu::Load(pugi::xml_node& data)
{
	
	return true;
}

bool j1MainMenu::Save(pugi::xml_node& data) const
{
	
	return true;
}



