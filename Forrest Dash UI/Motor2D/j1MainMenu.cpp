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
#include "j1Gui.h"


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
	graphics = App->tex->Load("gui/our ui.png");
	SDL_Rect r = { 0, 0, 427, 218 };
	//r = { 0, 0, 1024, 1024 };
	App->render->camera.x = -3060;

	SDL_Rect rect{ 485, 829, 328, 103 };
	banner = App->gui->CreateGuiElement(Types::image, COORDS(345), 30, rect, nullptr);
	rect = { 2, 111, 226, 69 };

	button = App->gui->CreateGuiElement(Types::button, 50, 0, rect, banner, this);
	text = App->gui->CreateGuiElement(Types::text, 50, 20, rect, button, this, "Hello World");
	button = App->gui->CreateGuiElement(Types::button, 200, 100, rect, banner, this);
	button = App->gui->CreateGuiElement(Types::button, 200, 200, rect, banner, this);

	App->gui->CreateGuiElement(Types::inputText, COORDS(200), 400, { 488, 569, 344, 61 }, nullptr, this);
	//App->gui->CreateGuiElement(Types::inputText, COORDS(400), 600, { 488, 569, 344, 61 }, nullptr, this);
	App->gui->CreateGuiElement(Types::slider, COORDS(100), 300, { 973, 786, 11, 157 }, nullptr, this);

	return true;
}

bool j1MainMenu::PreUpdate()
{
	
	return true;
}

bool j1MainMenu::Update(float dt)
{
	SDL_Rect r = { 0, 0, 427, 218 };
	App->render->Blit(graphics, COORDS(345), 30, &r);
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

void j1MainMenu::GuiInput(GuiItem* item)
{
	float f;
	if (item->parent->type == Types::slider) {
		f = item->parent->returnSliderPos();
		LOG("%f", f);
	}
}

