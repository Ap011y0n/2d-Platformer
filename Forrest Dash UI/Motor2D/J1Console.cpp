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
#include "J1EntityManager.h"


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
	bool read = false;


	



	p2SString result;
	p2SString result2;
	char test[100];
	char test2[100];

	int i, j = 0;
	for (i = 0; text[i] != 0; i++) {
		//LOG("%c", text[i]);
	
		test[i] = text[i];
		test[i + 1] = '\0';
		if(!read)
		result = test;

		if (read)
		{
			if (text[i] == '0' || text[i] == '1' || text[i] == '2' || text[i] == '3' || text[i] == '4' || text[i] == '5' || text[i] == '6' || text[i] == '7' || text[i] == '8' || text[i] == '9') {
				test2[j] = text[i];
				test2[j + 1] = '\0';
				j++;
				result2 = test2;
			}
		}
	
		if (strcmp(result.GetString(), "fps") == 0) {
			read = true;
		}
	}
	LOG("%s", result.GetString());

	if(read == true){
	std::string::size_type sz;   // alias of size_t
	fpsCap = std::stoi(result2.GetString(), &sz);
	LOG("%d", fpsCap);
	}



	if (strcmp ("godmode", text) == 0) {
		ret = Commands::God_Mode;
	}

	if (strcmp("list", text) == 0) {
		ret = Commands::list;
	}

	if (strcmp("quit", text) == 0) {
		ret = Commands::quit;
	}

	if (strcmp("fps", result.GetString()) == 0) {
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
		if (App->EntityManager->GetPlayer() != NULL)
		{

			if (!App->EntityManager->GetPlayer()->Godmode)
			{
				App->EntityManager->GetPlayer()->Godmode = true;
			}
			else if (App->EntityManager->GetPlayer()->Godmode)
			{
				App->EntityManager->GetPlayer()->Godmode = false;
			}
		}
		write("- God Mode Activated");
		break;

	case Commands::quit:
		App->quitGame = true;
		write("- Quitting game");
		break;

	case Commands::list:

		write("-----------COMMAND LIST------------");
		write("- godmode --> Enables/disables player godmode");
		write("- list --> If you haven't noticed yet, displays all available commands");
		write("- fps <number> --> sets a framerate cap between 30 and 120");
		write("- map --> changes map");
		write("- Quit --> Closes the application");

		break;

	case Commands::FPS:
		LOG("Frame Cap set");
		write("- Frame Cap set");
		if (fpsCap < 30)
			fpsCap = 30;
		if (fpsCap > 120)
			fpsCap = 120;
		App->framerate = fpsCap;
		break;

	case Commands::map:
		LOG("Changing map");
		write("- Changing map");
		break;
	}
}

void j1Console::Open() 
{
	InputText = App->gui->CreateGuiElement(Types::inputText, 0, 200, { 488, 569, 344, 61 }, nullptr, this);
	InputText->follow = true;
	InputText->GetInputText()->SetSingleFocus();
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

