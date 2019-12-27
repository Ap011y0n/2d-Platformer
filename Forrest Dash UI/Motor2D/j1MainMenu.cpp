#include "p2Defs.h"
#include "p2Log.h"
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
#include "j1Url.h"


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
	
	
	//Menu Sections
	
	camLock = false;

	moveToPoint[(int)Section::credits].create(-5060, 0);
	moveToPoint[(int)Section::main_menu].create(-3060, 0);
	moveToPoint[(int)Section::settings].create(-1060, 0);

	camPos.create(-3060, 0);
	camSpeed = 1000.0f;

	//Buttons
	SDL_Rect rect = { 0, 0, 427, 218 };

	banner = App->gui->CreateGuiElement(Types::image, COORDS(350), 30, rect, nullptr);
	
	rect = { 444, 169, 244, 65 };

	buttonNewGame = App->gui->CreateGuiElement(Types::button, 95, 370, { 444, 169, 244, 65 }, banner, this, NULL);
	buttonNewGame->setRects({ 444, 413, 244, 66 }, { 444, 661, 244, 65 });
	text = App->gui->CreateGuiElement(Types::text, 40, 14, rect, buttonNewGame, this, "NEW GAME");
	
	buttonContinue = App->gui->CreateGuiElement(Types::button, 95, 470, { 444, 169, 244, 65 }, banner, this, NULL);
	buttonContinue->setRects({ 444, 413, 244, 66 }, { 444, 661, 244, 65 });

	text1 = App->gui->CreateGuiElement(Types::text, 50, 14, rect, buttonContinue, this, "CONTINUE");
	
	buttonExit = App->gui->CreateGuiElement(Types::button, 95, 570, { 444, 169, 244, 65 }, banner, this, NULL);
	buttonExit->setRects({ 444, 413, 244, 66 }, { 444, 661, 244, 65 });

	text2 = App->gui->CreateGuiElement(Types::text, 90, 14, rect, buttonExit, this, "EXIT");
	
	
	buttonCredits = App->gui->CreateGuiElement(Types::button, 590, 600, { 1980, 360, 98, 108 }, banner, this);
	buttonCredits->setRects({ 1980, 473, 98, 108 }, { 1980, 587, 98, 108 });

	buttonSettings = App->gui->CreateGuiElement(Types::button, -250, 600, { 2087, 700, 98, 108 }, banner, this);
	buttonSettings->setRects({ 2087, 814, 98, 108 }, { 2087, 927, 98, 108 });

	rect = { 760, 12, 886, 604 };
	creditsPanel = App->gui->CreateGuiElement(Types::image, COORDS(2160), 40, rect, nullptr);
	licenseText = App->gui->CreateGuiElement(Types::text, 325, 35, rect, creditsPanel, this, "MIT License");
	rect = { 760, 617, 840, 503 };
	creditsText = App->gui->CreateGuiElement(Types::image, COORDS(2160), 120, rect, nullptr);
	
	rect = { 760, 12, 886, 604 };
	settingsPanel = App->gui->CreateGuiElement(Types::image, COORDS(-1850), 40, rect, nullptr);
	
	buttonCreditsToMenu = App->gui->CreateGuiElement(Types::button, 1900, 580, { 1980, 19, 98, 108 }, banner, this);
	buttonCreditsToMenu->setRects({ 1980, 133, 98, 108 }, { 1980, 246, 98, 108 });

	buttonSettingsToMenu = App->gui->CreateGuiElement(Types::button, -1550, 550, { 2087, 19, 98, 108 }, banner, this);
	buttonSettingsToMenu->setRects({ 2087, 133, 98, 108 }, { 2087, 246, 98, 108 });

	buttonToRepo = App->gui->CreateGuiElement(Types::button, 2480, 580, { 1661, 360, 98, 108 }, banner, this);
	buttonToRepo->setRects({ 1661, 473, 98, 108 }, { 1661, 587, 98, 108 });

	buttonToWeb = App->gui->CreateGuiElement(Types::button, 470, 600, { 1661, 20, 98, 108 }, banner, this);
	buttonToWeb->setRects({ 1661, 133, 98, 108 }, { 1661, 246, 98, 108 });
	
	scrollBarMusic = App->gui->CreateGuiElement(Types::slider, COORDS(-1150), 250, { 50, 889, 12, 189 }, nullptr, this);
	scrollBarFx = App->gui->CreateGuiElement(Types::slider, COORDS(-1520), 250, { 50, 889, 12, 189 }, nullptr, this);
	rect = { 2099, 717, 76, 74 };
	text = App->gui->CreateGuiElement(Types::text, 300, 70, rect, settingsPanel, this, "Settings");
	text = App->gui->CreateGuiElement(Types::text, 410, 300, rect, settingsPanel, this, "Music Volume");
	text = App->gui->CreateGuiElement(Types::text, 90, 300, rect, settingsPanel, this, "FX Volume");
	logoSettings = App->gui->CreateGuiElement(Types::image, COORDS(-1390), 90, rect, nullptr);
	
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
	
	if(!camLock)
	{
		if (camVelocity.x < 0.0f)
		{
			if (camPos.x > moveToPoint[(int)current_section].x)
			{
				camPos.x += camVelocity.x * dt;
			}

			if (camPos.x < moveToPoint[(int)current_section].x)
			{
				camPos.x = moveToPoint[(int)current_section].x;
				camVelocity.x = 0.0f;
			}

		}
		else if (camVelocity.x > 0.0f)
		{
			if (camPos.x < moveToPoint[(int)current_section].x)
			{
				camPos.x += camVelocity.x * dt;
			}

			if (camPos.x > moveToPoint[(int)current_section].x)
			{
				camPos.x = moveToPoint[(int)current_section].x;
				camVelocity.x = 0.0f;
			}
		}
		else
		{
			camPos.x = moveToPoint[(int)current_section].x;
		}

		App->render->camera.x = camPos.x;
		App->render->camera.y = camPos.y;
	}
	
	

	return true;
}

bool j1MainMenu::PostUpdate(float dt)
{

	return true;
}


bool j1MainMenu::CleanUp()
{
	if (banner != nullptr)
	banner->to_delete = true;
	if (text != nullptr)
	text->to_delete = true;
	if (text1 != nullptr)
	text1->to_delete = true;
	if (text2 != nullptr)
	text2->to_delete = true;
	if (buttonNewGame != nullptr)
	buttonNewGame->to_delete = true;
	if (buttonContinue != nullptr)
	buttonContinue->to_delete = true;
	if (buttonExit != nullptr)
	buttonExit->to_delete = true;
	if (buttonSettings != nullptr)
	buttonSettings->to_delete = true;
	if (buttonCredits != nullptr)
	buttonCredits->to_delete = true;
	if (buttonSettingsToMenu != nullptr)
	buttonSettingsToMenu->to_delete = true;
	if (buttonCreditsToMenu != nullptr)
	buttonCreditsToMenu->to_delete = true;
	if (creditsPanel != nullptr)
	creditsPanel->to_delete = true;
	if (licenseText != nullptr)
	licenseText->to_delete = true;
	if (settingsPanel != nullptr)
	settingsPanel->to_delete = true;
	if (creditsText != nullptr)
	creditsText->to_delete = true;
	if (buttonToRepo != nullptr)
	buttonToRepo->to_delete = true;
	if (buttonToWeb != nullptr)
	buttonToWeb->to_delete = true;
	if (scrollBarMusic != nullptr)
	scrollBarMusic->to_delete = true;
	if (scrollBarFx != nullptr)
	scrollBarFx->to_delete = true;
	if (logoSettings != nullptr)
	logoSettings->to_delete = true;
	

	return true;
}

//bool j1MainMenu::Load(pugi::xml_node& data)
//{
//	
//	return true;
//}
//
//bool j1MainMenu::Save(pugi::xml_node& data) const
//{
//	
//	return true;
//}

void j1MainMenu::GuiInput(GuiItem* item)
{
	float f;
	if (item->parent->type == Types::slider) {
		f = item->parent->returnSliderPos();
		LOG("%f", f);
	}

	if(item == buttonNewGame)
	{
		camLock = true;
		App->fade->ChangeScene();

	}
	else if (item == buttonContinue)
	{
		camLock = true;
		CleanUp();
		App->LoadGame();

	}
	
	else if (item == buttonSettings)
	{
		MoveToSection(Section::settings);
	}
	else if (item == buttonCredits)
	{
		MoveToSection(Section::credits);
	}
	else if (item == buttonSettingsToMenu)
	{
		MoveToSection(Section::main_menu);
	}
	else if (item == buttonCreditsToMenu)
	{
		MoveToSection(Section::main_menu);
	}
	else if (item == buttonExit)
	{
		App->quitGame = true;
	}
	else if (item == buttonToRepo)
	{
		openUrl("https://github.com/Ap011y0n/2d-Platformer");
	}
}

bool j1MainMenu::MoveToSection(Section section)
{
	current_section = section;

	if (App->render->camera.x > moveToPoint[(int)section].x)
	{
		camVelocity.x = -camSpeed;
	}
	else if (App->render->camera.x < moveToPoint[(int)section].x)
	{
		camVelocity.x = camSpeed;
	}
	else
	{
		camVelocity.x = 0.0f;
	}

	return false;
}

