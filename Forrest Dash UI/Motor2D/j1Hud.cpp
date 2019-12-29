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
#include "j1Map.h"
#include "j1MainMenu.h"
#include "j1Audio.h"
#include "j1Gui.h"
#include "j1Hud.h"
#include "Brofiler/Brofiler.h"
#include "J1Console.h"
#include "j1Fonts.h"

j1Hud::j1Hud() :j1Module()
{
	name.create("hud");
}

bool j1Hud::Awake(pugi::xml_node& node)
{
	timer = 0;
	minutes = 0;
	score = 0;
	score2 = 500;
	coins = 0;
	return true;
}

bool j1Hud::Start()
{
	
	lifesCounter = 3;

	extraLife = true;
	liveFull = nullptr;
	liveFull2 = nullptr;
	liveFull3 = nullptr;
	liveEmpty = nullptr;
	liveEmpty2 = nullptr;
	liveEmpty3 = nullptr;
	timer_item = nullptr;
	minutes_item = nullptr;
	coins_image = nullptr;
	coins_collected = nullptr;
	score_title = nullptr;
	score_item = nullptr;


	drawLife = false;
	return true;
}

bool j1Hud::PreUpdate()
{
	return true;
}

bool j1Hud::Update(float dt)
{
	BROFILER_CATEGORY("Hud Update", Profiler::Color::LightGray);


		if (score >= score2)
		{
			score2 += 500;
			drawLife = true;
			SetLifes(App->hud->GetLifes() + 1);
		}
	
	

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
	
		if (timer_item != nullptr) {

		
	timer_item->SetText(timer_text);
	SDL_DestroyTexture(timer_item->texture);
	p2List_item<SDL_Texture*>* texlist = App->tex->textures.At(App->tex->textures.find(timer_item->texture));
	App->tex->textures.del(texlist);

	timer_item->texture = App->font->Print(timer_item->GetText(), { 255, 255, 255, 255 });
	App->font->CalcSize(timer_item->GetText(), timer_item->textureRect.w, timer_item->textureRect.h);
	}
		if (minutes_item != nullptr) {

		
	minutes_item->SetText(minutes_text);
	SDL_DestroyTexture(minutes_item->texture);
	p2List_item<SDL_Texture*>* texlist = App->tex->textures.At(App->tex->textures.find(minutes_item->texture));
	App->tex->textures.del(texlist);

	minutes_item->texture = App->font->Print(minutes_item->GetText(), { 255, 255, 255, 255 });
	App->font->CalcSize(minutes_item->GetText(), minutes_item->textureRect.w, minutes_item->textureRect.h);
		}
	}

	////SCORE
	if (score_item != nullptr)
	{
		sprintf_s(score_text, 10, "%d", score);
		score_item->SetText(score_text);
		SDL_DestroyTexture(score_item->texture);
		p2List_item<SDL_Texture*>* texlist = App->tex->textures.At(App->tex->textures.find(score_item->texture));
		App->tex->textures.del(texlist);

		score_item->texture = App->font->Print(score_item->GetText(), { 255, 255, 255, 255 });
		App->font->CalcSize(score_item->GetText(), score_item->textureRect.w, score_item->textureRect.h);
	}

	// COINS

	if (coins_collected != nullptr)
	{
		sprintf_s(coins_text, 10, "%d", coins);
		coins_collected->SetText(coins_text);
		SDL_DestroyTexture(coins_collected->texture);
		p2List_item<SDL_Texture*>* texlist = App->tex->textures.At(App->tex->textures.find(coins_collected->texture));
		App->tex->textures.del(texlist);
		coins_collected->texture = App->font->Print(coins_collected->GetText(), { 255, 255, 255, 255 });
		App->font->CalcSize(coins_collected->GetText(), coins_collected->textureRect.w, coins_collected->textureRect.h);
	}

	return true;
}

bool j1Hud::PostUpdate()
{
	
	return true;
}

bool j1Hud::CleanUp()
{
	score = 0; 
	coins = 0;
	score2 = 500;
	timer = 0;
	minutes = 0;

	activateTimer = false;
	if (timer_item != nullptr)
		timer_item->to_delete = true;
	if (minutes_item != nullptr)
		minutes_item->to_delete = true;
	if(score_title != nullptr)
		score_title->to_delete = true;
	if (score_item != nullptr)
		score_item->to_delete = true;
	if(liveFull != nullptr)
		liveFull->to_delete = true;
	if (liveFull2 != nullptr)
		liveFull2->to_delete = true;
	if (liveFull3 != nullptr)
		liveFull3->to_delete = true;
	if (liveEmpty != nullptr)
		liveEmpty->to_delete = true;
	if (liveEmpty2 != nullptr)
		liveEmpty2->to_delete = true;
	if (liveEmpty3 != nullptr)
		liveEmpty3->to_delete = true;
	if (coins_collected != nullptr)
		coins_collected->to_delete = true;
	if (coins_image != nullptr)
		coins_image->to_delete = true;
	return true;
}

bool j1Hud::Load(pugi::xml_node& node)
{
	
	timer = node.child("Timer").attribute("seconds").as_int();
	minutes = node.child("Timer").attribute("minutes").as_int();
	coins = node.child("Points").attribute("coins").as_int();
	score = node.child("Points").attribute("score").as_int();

	if (App->menu->LoadHp) {
		lifesCounter = node.child("Hp").attribute("current").as_int();
		App->menu->LoadHp = false;
		ShowHud();
	}
	
	return true;
}

bool j1Hud::Save(pugi::xml_node& node) const
{
	LOG("Saving HUD state");
	App->console->write("Saving HUD state");

	pugi::xml_node scene = node.append_child("Hp");
	pugi::xml_node TimeManagement = node.append_child("Timer");
	pugi::xml_node ScoreManagement = node.append_child("Points");
	scene.append_attribute("current") = lifesCounter;
	TimeManagement.append_attribute("seconds") = timer;
	TimeManagement.append_attribute("minutes") = minutes;
	ScoreManagement.append_attribute("coins") = coins;
	ScoreManagement.append_attribute("score") = score;
	return true;
}

bool j1Hud::SetLifes(int lifes)
{
	if (lifes < 0)
	{
		lifes = 0;
	

		App->map->CleanUp();
		App->menu->Start();
		App->scene->checkpoint = false;
		App->scene->current_level = "menu.tmx";
		App->map->Load(App->scene->current_level.GetString());
		App->audio->PlayMusic(App->map->data.music.GetString());
		App->EntityManager->DeleteEntities = true;

		CleanUp();
		
	}
	else if (lifes > MAX_LIFES)
	{
		lifes = MAX_LIFES;
		drawLife = false;
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
	
	if(drawLife)
	{
		switch (lifes)
		{
		case 3:
			liveFull3 = App->gui->CreateGuiElement(Types::image, 100, 50, { 119, 258, 36, 34 }, nullptr);
			liveFull3->follow = true;
			drawLife = false;
			break;
		case 2:
			liveFull2 = App->gui->CreateGuiElement(Types::image, 60, 50, { 119, 258, 36, 34 }, nullptr);
			liveFull2->follow = true;
			drawLife = false;
			break;
		case 1:
			liveFull = App->gui->CreateGuiElement(Types::image, 20, 50, { 119, 258, 36, 34 }, nullptr);
			liveFull->follow = true;
			drawLife = false;
			break;
		default:
			break;
		}

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
	if (lifesCounter > 0)
	{
		liveFull = App->gui->CreateGuiElement(Types::image, 20, 50, { 119, 258, 36, 34 }, nullptr);
		liveFull->follow = true;
	}
	if (lifesCounter > 1)
	{
		liveFull2 = App->gui->CreateGuiElement(Types::image, 60, 50, { 119, 258, 36, 34 }, nullptr);
		liveFull2->follow = true;
	}
	if (lifesCounter > 2)
	{
		liveFull3 = App->gui->CreateGuiElement(Types::image, 100, 50, { 119, 258, 36, 34 }, nullptr);
		liveFull3->follow = true;
	}
	liveEmpty->follow = true;
	liveEmpty2->follow = true;
	liveEmpty3->follow = true;

	timer_item = App->gui->CreateGuiElement(Types::text, 200, 50, { 157, 258, 36, 34 }, NULL, this, "0");
	timer_item->follow = true;

	minutes_item = App->gui->CreateGuiElement(Types::text, 160, 50, { 157, 258, 36, 34 }, NULL, this, "0");
	minutes_item->follow = true;

	activateTimer = true;

	score_title = App->gui->CreateGuiElement(Types::text, 780, 50, { 157, 258, 36, 34 }, NULL, this, "SCORE:");
	score_title->follow = true;
	score_item = App->gui->CreateGuiElement(Types::text, 900, 50, { 157, 258, 36, 34 }, NULL, this, "0");
	score_item->follow = true;
	
	coins_collected = App->gui->CreateGuiElement(Types::text, 400, 50, { 157, 258, 36, 34 }, NULL, this, "0");
	coins_collected->follow = true;
	coins_image = App->gui->CreateGuiElement(Types::image, 350, 40, { 198, 251, 46, 45 }, NULL, this);
	coins_image->follow = true;

	return true;
}





