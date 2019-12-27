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
	InputText = App->gui->CreateGuiElement(Types::inputText, 30, 200, { 488, 569, 344, 61 }, nullptr, this);
	InputText->follow = true;
	//ConsoleText = App->gui->CreateGuiElement(Types::text, -10, -40, { 0, 0, 0, 0 }, InputText, this,"Console \n text");
	//temp = ConsoleText->GetText();
	
	for (int i = 0; i < MAXTEXT; i++) {
		ConsoleText[i] = App->gui->CreateGuiElement(Types::text, 20, -i*40, { 0, 0, 0, 0 }, InputText, this, "- ");
		
	}
	

	/*graphics = App->font->Print("text \n text");
	textureRect = { 0,0,0,0 };
	App->font->CalcSize("text \n text", textureRect.w, textureRect.h);*/
	return true;
}

bool j1Console::PreUpdate(float dt)
{

	return true;
}

bool j1Console::Update(float dt)
{
//	App->render->Blit(graphics, COORDS(100), 400, NULL);
	/*for (int i = 0; i < MAXTEXT; i++) {
		ConsoleText[i]->SetText(App->temp[i].GetString());
		ConsoleText[i]->texture = App->font->Print(ConsoleText[i]->GetText());
		App->font->CalcSize(ConsoleText[i]->GetText(), ConsoleText[i]->textureRect.w, ConsoleText[i]->textureRect.h);
	}*/
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

	for (int i = 0; i < MAXTEXT; i++) {
		ConsoleText[i]->SetText(App->temp[i].GetString());
		ConsoleText[i]->texture = App->font->Print(ConsoleText[i]->GetText());
		App->font->CalcSize(ConsoleText[i]->GetText(), ConsoleText[i]->textureRect.w, ConsoleText[i]->textureRect.h);
	}

	item->SetText("");
	item->texture = App->font->Print(item->GetText());
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
		LOG("none");
		break;
	case Commands::God_Mode:
		LOG("God Mode");
		break;
	case Commands::quit:
		LOG("quit");
		break;
	case Commands::FPS:
		LOG("FPS");
		break;
	case Commands::map:
		LOG("map");
		break;
	}
}