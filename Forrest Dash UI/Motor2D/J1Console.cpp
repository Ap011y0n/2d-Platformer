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
	return true;
}

bool j1Console::Start()
{
	InputText = App->gui->CreateGuiElement(Types::inputText, COORDS(100), 200, { 488, 569, 344, 61 }, nullptr, this);
	/*ConsoleText = App->gui->CreateGuiElement(Types::text, -10, -40, { 0, 0, 0, 0 }, InputText, this,"Console \n text");
	temp = ConsoleText->GetText();*/

	for (int i = 0; i < MAXTEXT; i++) {
		ConsoleText[i] = App->gui->CreateGuiElement(Types::text, 20, -20-i*40, { 0, 0, 0, 0 }, InputText, this, "- ");
		temp[i] = ConsoleText[i]->GetText();
	}


	//graphics = App->font->Print("text \n text");
	//textureRect = { 0,0,0,0 };
	//App->font->CalcSize("text \n text", textureRect.w, textureRect.h);
	return true;
}

bool j1Console::PreUpdate()
{

	return true;
}

bool j1Console::Update(float dt)
{
	//App->render->Blit(graphics, COORDS(100), 400, NULL);
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
	
	/*LOG("%s", temp.GetString());
	temp += "\n";
	temp += item->GetText();
	

	ConsoleText->SetText(temp.GetString());

	ConsoleText->texture = App->font->Print(ConsoleText->GetText());
	App->font->CalcSize(ConsoleText->GetText(), ConsoleText->textureRect.w, ConsoleText->textureRect.h);*/
	for (int i = MAXTEXT-1; i > 0; i--) {


		temp[i] = temp[i - 1];
		LOG("%d %s",i, temp[i].GetString());
	}
	temp[0] = item->GetText();
	LOG("0 %s",temp[0].GetString());

	for (int i = 0; i < MAXTEXT; i++) {
		
		ConsoleText[i]->SetText(temp[i].GetString());
		ConsoleText[i]->texture = App->font->Print(ConsoleText[i]->GetText());
		App->font->CalcSize(ConsoleText[i]->GetText(), ConsoleText[i]->textureRect.w, ConsoleText[i]->textureRect.h);
	}
	item->SetText(" ");
	item->texture = App->font->Print(item->GetText());
	App->font->CalcSize(item->GetText(), item->textureRect.w, item->textureRect.h);
}
