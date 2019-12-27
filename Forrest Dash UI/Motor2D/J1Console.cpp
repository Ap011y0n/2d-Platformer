#include "j1MainMenu.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "J1Console.h"
#include "j1Gui.h"
#include "j1Fonts.h"


j1Console::j1Console()
{
	name.create("console");


}

j1Console::~j1Console()
{
}

bool j1Console::Awake()
{
	LOG("Start console");
	write("Start console");

	return true;
}

bool j1Console::Start()
{
	opened = false;
	ConsText = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf",16);
	InText = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf", 36);
	return true;
}

bool j1Console::PreUpdate(float dt)
{

	return true;
}

bool j1Console::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN) 
	{
		if (opened == false)
		{
			opened = true;
			Open();
		}
		else
		{
			opened = false;
			Close();
		}
	}
	if (opened == true)
	{
		App->render->DrawQuad({ -App->render->camera.x, -App->render->camera.y, (int)App->win->width, (int)App->win->height/3-50 }, 0, 0, 0, 170);
		for (int i = 0; i < MAXTEXT; i++) {

			ConsoleText[i]->SetText(App->temp[i].GetString());

			SDL_DestroyTexture(ConsoleText[i]->texture);
			p2List_item<SDL_Texture*>* texlist = App->tex->textures.At(App->tex->textures.find(ConsoleText[i]->texture));
			App->tex->textures.del(texlist);

			ConsoleText[i]->texture = App->font->Print(ConsoleText[i]->GetText(), { 255, 255, 255, 255 }, ConsText);
			App->font->CalcSize(ConsoleText[i]->GetText(), ConsoleText[i]->textureRect.w, ConsoleText[i]->textureRect.h, ConsText);
			
			//&& temp2 == -30
			}

		if (App->input->ScrollUp == true)
		{
			int temp, temp2;
			ConsoleText[0]->GetLocalPos(temp, temp2);
			temp2 -= 10;
			ConsoleText[0]->SetLocalPos(temp, temp2);
		}

		if (App->input->ScrollDown == true)
		{
			int temp, temp2;
			ConsoleText[0]->GetLocalPos(temp, temp2);
			temp2 += 10;
			ConsoleText[0]->SetLocalPos(temp, temp2);
		}
		
	}

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

	write(item->GetText());
	ExecuteCommand(ReturnCommand(item->GetText()));

	item->SetText("");

	SDL_DestroyTexture(item->texture);

	item->texture = App->font->Print(item->GetText(), { 255,255,255,255 }, InText);
	App->font->CalcSize(item->GetText(), item->textureRect.w, item->textureRect.h);
}

void j1Console::write(const char* newtext) {

	for (int i = MAXTEXT - 1; i > 0; i--) {

		App->temp[i] = App->temp[i - 1];

	}
	App->temp[0] = newtext;
	
}

Commands j1Console::ReturnCommand(const char* text) {
	Commands ret = Commands::none;
	if (strcmp ("godmode", text) == 0) {
		ret = Commands::God_Mode;
	}
	
	if (strcmp("quit", text) == 0) {
		ret = Commands::quit;
	}

	if (strcmp("fps", text) == 0) {
		ret = Commands::FPS;
	}	

	if (strcmp("map", text) == 0) {
		ret = Commands::map;
	}
	return ret;
}

void j1Console::ExecuteCommand(Commands command) {
	switch (command) {
	case Commands::none:
		LOG("Error, command not found");
		write("Error, command not found");
		break;
	case Commands::God_Mode:
		LOG("God Mode Activated");
		write("God Mode Activated");

		break;
	case Commands::quit:
		LOG("Quitting game");
		write("Quitting game");

		break;
	case Commands::FPS:
		LOG("Frame Cap set");
		write("Frame Cap set");

		break;
	case Commands::map:
		LOG("Changing map");
		write("Changing map");
		break;
	}
}

void j1Console::Open() 
{
	InputText = App->gui->CreateGuiElement(Types::inputText, 0, 200, { 488, 569, 344, 61 }, nullptr, this);
	InputText->follow = true;
	
	ConsoleText[0] = App->gui->CreateGuiElement(Types::text, 30, -30, { 0, 0, 0, 0 }, InputText, this, "- ");
	for (int i = 1; i < MAXTEXT; i++) {
		ConsoleText[i] = App->gui->CreateGuiElement(Types::text, 0, -i * 20, { 0, 0, 0, 0 }, ConsoleText[0], this, "- ");
	}
}

void j1Console::Close() 
{
	InputText->to_delete = true;

	for (int i = 0; i < MAXTEXT; i++) {
		ConsoleText[i]->to_delete = true;
		SDL_DestroyTexture(ConsoleText[i]->texture);
		p2List_item<SDL_Texture*>* texlist = App->tex->textures.At(App->tex->textures.find(ConsoleText[i]->texture));
		App->tex->textures.del(texlist);
	}

}

