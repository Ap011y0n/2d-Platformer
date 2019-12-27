#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Window.h"
#include "j1MainMenu.h"
#include "J1Console.h"





j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	App->console->write("Loading GUI atlas");

	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	
	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{

	atlas = App->tex->Load("gui/ui.png");
	FocusIt = 0;
	return true;
}

// Update all guis
bool j1Gui::PreUpdate(float dt)
{

	buttonPressed = false;
	bool ret = true;
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) 
	{
		buttonPressed = true;
	}
	/*if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) 
	{
		App->gui->IterateFocus();
	}*/

	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		App->input->text.Cut(App->input->text.Length() - 1, App->input->text.Length()-1);
	}
		p2List_item<GuiItem*>* gui_list = guiElements.end;
		while (gui_list && ret) {
			int x, y;
			if (gui_list->data->follow)
			{
				x = -App->render->camera.x + gui_list->data->initposx;
				y = -App->render->camera.y + gui_list->data->initposy;
				gui_list->data->SetLocalPos(x, y);
			}

			
			App->input->GetMousePosition(x, y);
			if (gui_list->data->isDynamic)
			{
				if (gui_list->data->checkBoundaries(-App->render->camera.x + x, -App->render->camera.y + y)) {
					gui_list->data->SetFocus();
					
					ret = false;
				}
			}
		
		
			gui_list = gui_list->prev;
		}
	
	return true;
}

// Called after all Updates
bool j1Gui::Update(float dt)
{
	
	p2List_item<GuiItem*>* gui_list = guiElements.start;
	while (gui_list) {
		int x, y;
		if (gui_list->data->focus)
			gui_list->data->Input();

		gui_list->data->GetScreenPos(x, y);
		//	LOG("%d", gui_list->data->textureRect.h);
		
		if (gui_list->data->type == Types::text) {
		App->render->Blit(gui_list->data->texture, x, y, &gui_list->data->textureRect);
	}
		else {
			App->render->Blit(GetAtlas(), x, y, &gui_list->data->textureRect);

		}
	//	SDL_Rect	r = { 0, 0, 427, 218 };
	//	App->render->Blit(App->menu->graphics, 3480, 390, &r);

		gui_list = gui_list->next;
	}

	DeleteGuiElement();
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	App->console->write("Freeing GUI");

	p2List_item<GuiItem*>* gui_list = guiElements.end;

	while (gui_list != NULL)
	{
		RELEASE(gui_list->data);
		gui_list = gui_list->prev;
	}
	guiElements.clear();

	return true;
}

void j1Gui::DeleteGuiElement() {

	p2List_item<GuiItem*>* gui_list = guiElements.end;
	while (gui_list) {
		if (gui_list->data->to_delete == true) 
		{
			guiElements.del(gui_list);
		}
		else if (gui_list->data->parent != nullptr)
		{
			if(gui_list->data->parent->to_delete == true)
			guiElements.del(gui_list);
		}
		gui_list = gui_list->prev;
	}
}

// const getter for atlas
SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::IterateFocus() {
	if (FocusIt == 0)
	{
		guiElements.At(guiElements.count()-1)->data->focus = false;
	}
	else
	{
		guiElements.At(FocusIt - 1)->data->focus = false;
	}
	
	if (FocusIt == guiElements.count())
		FocusIt = 0;

	guiElements.At(FocusIt)->data->focus = true;
	FocusIt++;
	
}



GuiItem::GuiItem() {

}

GuiItem::~GuiItem() {

}


GuiItem* j1Gui::CreateGuiElement(Types type, int x, int y, SDL_Rect rect, GuiItem* parentnode, j1Module* callback, char* text) {

	GuiItem* ret;
	switch (type) {
	case Types::image: ret = new GuiImage(x, y, rect, callback); ret->parent = parentnode; break;
	case Types::text: ret = new GuiText(x, y, rect, text, callback); ret->parent = parentnode; break;
	case Types::button: ret = new GuiButton(x, y, rect, callback); ret->parent = parentnode; break;
	case Types::inputText: ret = new InputText(x, y, rect, callback); ret->parent = parentnode; break;
	case Types::slider: ret = new GuiSlider(x, y, rect, callback); ret->parent = parentnode; break;
	}

	
	guiElements.add(ret);

	return ret;
}

void j1Gui::sendInput(GuiItem* Item)
{
	Item->CallBack->GuiInput(Item);
}

//Ui Classes
bool GuiItem::checkBoundaries(int x, int y) 
{
	if (type == Types::button)
		textureRect = idleRect; //idle Button
	int posx, posy;
	GetScreenPos(posx, posy);

	if (x > posx && x < (posx + LocalRect.w))
		if (y > posy && y < (posy + LocalRect.h)) {
			if (type == Types::button)
			textureRect = illuminatedRect; //Illuminated Button
			return true;
		}
	return false;
}

void GuiItem::SetFocus() {
	if (type != Types::text) 
	{
		if (App->gui->buttonPressed == true)
		{
			p2List_item<GuiItem*>* gui_list = App->gui->guiElements.end;
			while (gui_list) {
				gui_list->data->focus = false;
				App->input->DisableTextInput();
				gui_list = gui_list->prev;
			}
			focus = true;
		}
		else {
			focus = false;
		}
	}
	else {
		if (App->gui->buttonPressed == true)
		{ 
		p2List_item<GuiItem*>* gui_list = App->gui->guiElements.end;
		while (gui_list) {
			gui_list->data->focus = false;
			App->input->DisableTextInput();
			gui_list = gui_list->prev;
		}
		App->input->EnableTextInput("");
		focus = true;
		}
	}
}

void GuiItem::Input() {

	if (type == Types::button) {
		if (focus == true)
		{
			textureRect = pushedRect; //Pushed Button
			App->gui->sendInput(this);
		}

	}
	if(parent!= nullptr){
	if (parent->type == Types::slider) {
		if (focus == true)
		{
			App->gui->sendInput(this);
			parent->slide();
		}

	}

	if (parent->type == Types::inputText) {

		if (focus == true)
		{
			SDL_DestroyTexture(texture);
			texture = App->font->Print(App->input->text.GetString());
			App->font->CalcSize(App->input->text.GetString(), textureRect.w, textureRect.h);
			
			int x, y;
			GetScreenPos(x, y);
			App->render->DrawQuad({ x + textureRect.w, y, 1, textureRect.h }, 255, 255, 255);
			if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN){
				SetText(App->input->text.GetString());
				App->gui->sendInput(this);
				focus = false;
				App->input->DisableTextInput();
			}

		}
	}
	}
}

void GuiItem::GetScreenRect(SDL_Rect& rect) {
	GuiItem* it = this->parent;
	int addx = 0, addy = 0;
	while (it != NULL) {
		addx += it->LocalRect.x;
		addy += it->LocalRect.y;
		it = it->parent;
	}
	rect.x = addx + LocalRect.x;
	rect.y = addy + LocalRect.y;
	rect.w = LocalRect.w;
	rect.h = LocalRect.h;

}

SDL_Rect* GuiItem::GetLocalRect() {
	return 	&LocalRect;
}

void GuiItem::GetScreenPos(int& x, int& y) {
	GuiItem* it = this->parent;
	int addx = 0, addy = 0;
	while (it != NULL) {
		addx += it->LocalX;
		addy += it->LocalY;
		it = it->parent;
	}

	x = addx + LocalX;
	y = addy + LocalY;
}

void GuiItem::GetLocalPos(int& x, int& y) {
	x = LocalX;
	y = LocalY;
}

void GuiItem::SetLocalPos(int& x, int& y) {
	LocalX = x;
	LocalY = y;
}


//-------------------------------------------------------------
GuiImage::GuiImage(int x, int y, SDL_Rect texrect, j1Module* callback) : GuiItem() {
	type = Types::image;
	LocalX = initposx = x;
	LocalY = initposy = y;
	
	textureRect = texrect;
	LocalRect = textureRect;
	isDynamic = false;
	texture = App->gui->GetAtlas();
	focus = false;
	follow = false;
	CallBack = callback;
	to_delete = false;
}

GuiImage::~GuiImage() {

}
//-------------------------------------------------------------
GuiText::GuiText(int x, int y, SDL_Rect texrect,  char* inputtext, j1Module* callback) : GuiItem() {
	type = Types::text;
	text = inputtext;
	LocalX = initposx = x;
	LocalY = initposy = y;
	LocalRect = texrect;
	isDynamic = false;
	focus = false;
	follow = false;
	textureRect.x = 0;
	textureRect.y = 0;
	color = SDL_Color{ 255,255,255 };
	CallBack = callback;
	texture = App->font->Print(text, color);
	App->font->CalcSize(text, textureRect.w, textureRect.h);
	to_delete = false;

}

GuiText::~GuiText() {

}
//-------------------------------------------------------------
GuiButton::GuiButton(int x, int y, SDL_Rect idle_rect, j1Module* callback) : GuiItem() {
	type = Types::button;
	LocalX = initposx = x;
	LocalY = initposy = y;
	textureRect = idle_rect;
	idleRect = idle_rect;
	LocalRect = textureRect;
	isDynamic = true;
	follow = false;
	texture = App->gui->GetAtlas();
	focus = false;
	CallBack = callback;
	to_delete = false;

}

GuiButton::~GuiButton() {

}


void GuiButton::setRects(SDL_Rect iluminated, SDL_Rect pushed) {

	illuminatedRect = iluminated;
	pushedRect = pushed;
}
//--------------------------------------------------------------
InputText::InputText(int x, int y, SDL_Rect texrect, j1Module* callback) : GuiItem()
	{
	type = Types::inputText;
	LocalX = initposx = x;
	LocalY = initposy = y;
	textureRect = { 0, 0, 0, 0 };
	LocalRect = textureRect;
	isDynamic = true;
	follow = false;
	texture = App->gui->GetAtlas();
	focus = false;
	CallBack = callback;

	to_delete = false;

	image = App->gui->CreateGuiElement(Types::image, 0, 0, texrect, this);
	text = App->gui->CreateGuiElement(Types::text, 20, 15, texrect, this, callback, "Insert Text");
	text->isDynamic = true;

}

InputText::~InputText() {
	
}

//--------------------------------------------------------------
GuiSlider::GuiSlider(int x, int y, SDL_Rect texrect, j1Module* callback) : GuiItem()
{
	type = Types::slider;
	LocalX = initposx = x;
	LocalY = initposy = y;
	textureRect = { 0, 0, 0, 0 };
	LocalRect = textureRect;
	isDynamic = false;
	follow = false;
	texture = App->gui->GetAtlas();
	focus = false;
	CallBack = callback;
	Image = App->gui->CreateGuiElement(Types::image, 0, 0, texrect, this);
	ScrollThumb = App->gui->CreateGuiElement(Types::image, -3, 0, { 843, 322, 15, 26 }, this, callback);
	
	ScrollThumb->isDynamic = true;
	to_delete = false;

}

GuiSlider::~GuiSlider() {

}

void GuiSlider::slide() {
	int x, y, LocalX, LocalY, ScreenX, ScreenY, parentx, parenty, difference, height;
	App->input->GetMousePosition(x, y);
	x -= 5;
	y -= 5;
	height = Image->GetLocalRect()->h - ScrollThumb->GetLocalRect()->h + 1;
	GetScreenPos(parentx, parenty);
	ScrollThumb->GetScreenPos(ScreenX, ScreenY);
	ScrollThumb->GetLocalPos(LocalX, LocalY);
	difference = LocalY + y - ScreenY;
	
	if (y > ScreenY) {
		if(ScreenY <= parenty+ Image->GetLocalRect()->h - ScrollThumb->GetLocalRect()->h)
		{
			ScrollThumb->SetLocalPos(LocalX, difference);
		}
		else {
			
			ScrollThumb->SetLocalPos(LocalX, height);
		}
	}
	else if (y < ScreenY) {
		difference = LocalY - y - ScreenY;
		if (ScreenY >= parenty) 
		{
			ScrollThumb->SetLocalPos(LocalX, difference);
		}
		else {
			int pos = -1;
			ScrollThumb->SetLocalPos(LocalX, pos);
		}
	}

}
float GuiSlider::returnSliderPos()
{
	float ratio;
	int x, y;
	float a, b;
	a = Image->GetLocalRect()->h;
	b = ScrollThumb->GetLocalRect()->h;
	a = a - b;
	ScrollThumb->GetLocalPos(x, y);
	ratio =  (y / a);

	return ratio;
}

const char* GuiText::GetText() const{
	
	return text;
}

void GuiText::SetText(const char* newtext)
{
	text = newtext;
}