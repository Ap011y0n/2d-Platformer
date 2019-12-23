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
#include "j1FadeToBlack.h"
#include "j1Gui.h"


j1MainMenu::j1MainMenu()
{
	name.create("main_menu");


	SDL_Rect r = { 0, 0, 427, 218 };

	//Button_1
	r = { 444, 169, 244, 65 }; //Normal
	r = { 444, 413, 244, 66 }; //Illuminated  
	r = { 444, 661, 244, 65 }; //Pressed

	//Big Panel
	r = { 760, 12, 990, 846 };

	//Scroll Bar
	r = { 392, 858, 322, 16 }; //Scroll Bar
	r = { 520, 798, 31, 57 }; //Normal Scroll Bar Button
	r = { 551, 798, 30, 57 }; //Illuminated Scroll Bar Button
	r = { 581, 802, 31, 53 }; //Pressed Scroll Bar Button

	//Left Arrow
	r = { 1980, 19, 98, 108 }; //Normal
	r = { 1980, 133, 98, 108 }; //Illuminated
	r = { 1980, 246, 98, 108 }; //Pressed

	//Right Arrow
	r = { 2087, 19, 98, 108 }; //Normal
	r = { 2087, 133, 98, 108 }; //Illuminated
	r = { 2087, 246, 98, 108 }; //Pressed

	//Settings Button
	r = { 2087, 700, 98, 108 }; //Normal
	r = { 2087, 814, 98, 108 }; //Illuminated
	r = { 2087, 927, 98, 108 }; //Pressed

	//Restart Button
	r = { 1980, 700, 98, 108 }; //Normal
	r = { 1980, 814, 98, 108 }; //Illuminated
	r = { 1980, 927, 98, 108 }; //Pressed

	//Sound Button
	r = { 1873, 360, 98, 108 }; //Normal
	r = { 1873, 473, 98, 108 }; //Illuminated
	r = { 1873, 587, 98, 108 }; //Pressed

	//Silenced Button
	r = { 1767, 360, 98, 108 }; //Normal
	r = { 1767, 473, 98, 108 }; //Illuminated
	r = { 1767, 587, 98, 108 }; //Pressed

	//Credits Button
	r = { 1980, 360, 98, 108 }; //Normal
	r = { 1980, 473, 98, 108 }; //Illuminated
	r = { 1980, 587, 98, 108 }; //Pressed

	//LOGO
	r = { 0, 0, 427, 218 };
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
	graphics = App->tex->Load("gui/ui.png");
	
	
	App->render->camera.x = -3060;

	SDL_Rect rect{ 485, 829, 328, 103 };

	banner = App->gui->CreateGuiElement(Types::image, COORDS(345), 30, rect, nullptr);
	rect = { 2, 111, 226, 69 };

	//Buttons
	buttonNewGame = App->gui->CreateGuiElement(Types::button, 100, 350, rect, banner, this);
	text = App->gui->CreateGuiElement(Types::text, 50, 20, rect, buttonNewGame, this, "NEW GAME");

	/*
	buttonNewGame = App->gui->CreateGuiElement(Types::button, 50, 0, rect, banner, this);
	text = App->gui->CreateGuiElement(Types::text, 50, 20, rect, buttonNewGame, this, "NEW GAME");
	buttonNewGame = App->gui->CreateGuiElement(Types::button, 200, 100, rect, banner, this);
	buttonNewGame = App->gui->CreateGuiElement(Types::button, 200, 200, rect, banner, this);

	App->gui->CreateGuiElement(Types::inputText, COORDS(200), 400, { 488, 569, 344, 61 }, nullptr, this);
	//App->gui->CreateGuiElement(Types::inputText, COORDS(400), 600, { 488, 569, 344, 61 }, nullptr, this);
	App->gui->CreateGuiElement(Types::slider, COORDS(100), 300, { 973, 786, 11, 157 }, nullptr, this);
	*/

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

	if(item == buttonNewGame)
	{
		App->fade->FadeToBlack();

		if(App->fade->fadetimer.Read() > 1000)
		{
			App->scene->Nextmap();
		}

	}
}

