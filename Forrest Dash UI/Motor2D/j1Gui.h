#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"


#define CURSOR_WIDTH 2

// TODO 1: Create your structure of classes

enum class Types
{
	image,
	text,
	button,
	inputText,
	slider,
};

struct Button_Definition
{
public:
	Button_Definition() {}
	Button_Definition(SDL_Rect idle_rect, SDL_Rect illuminated_rect, SDL_Rect pushed_rect)
	{
		this->idleRect = idle_rect;
		this->illuminatedRect = illuminated_rect;
		this->pushedRect = pushed_rect;
	}

	SDL_Rect idleRect = { 0,0,0,0 };
	SDL_Rect illuminatedRect = { 0,0,0,0 };
	SDL_Rect pushedRect = { 0,0,0,0 };
};

struct _TTF_Font;
struct SDL_Color;

// Instrucciones:
//Meter un nodo GuiItem en la clase guiItem (parent)
//Nos servira para calcular todas las funciones de rect etc
//El input, sacarlo fuera de checkboundaries
//Checkboundaries sea un bool, en el momento que devuelva un true,llamar a su input y
//cortar la iteración, solo detectaremos input de un elemento ui
class GuiItem
{

public:
	GuiItem();
	virtual ~GuiItem();
	bool checkBoundaries(int, int);
	void Input();
	void SetFocus();
	void GetScreenRect(SDL_Rect&);
	SDL_Rect* GetLocalRect();
	void GetScreenPos(int&, int&);
	void GetLocalPos(int&, int&);
	void SetLocalPos(int&, int&);
	virtual void slide() {
	}
	virtual float returnSliderPos() {
		return 0;
	}
	virtual void SetText(const char*) {
	}
	
	virtual const char* GetText() const {
		return "none";
	}

protected:
	int LocalX;
	int LocalY;
	SDL_Rect LocalRect;
	

public:
	GuiItem* parent;
	Types type;
	j1Module* CallBack;
	bool focus;
	SDL_Texture* texture;
	SDL_Rect textureRect;
	SDL_Rect idleRect;
	SDL_Rect illuminatedRect;
	SDL_Rect pushedRect;
	bool isDynamic;
	bool follow;
};

class GuiImage: public GuiItem
{
public:
	GuiImage(int, int, SDL_Rect, j1Module* callback = nullptr);
	virtual ~GuiImage();

};

class GuiText : public GuiItem
{
public:
	GuiText(int, int, SDL_Rect, char*, j1Module* callback = nullptr);
	virtual ~GuiText();

private:
	SDL_Color color;
	const char *text;

public:
	const char* GetText() const;
	void SetText(const char*);
};

class GuiButton : public GuiItem
{
public:
	GuiButton(int, int, SDL_Rect, SDL_Rect, SDL_Rect, j1Module* callback = nullptr);
	virtual ~GuiButton();

};

class InputText : public GuiItem
{
public:
	InputText(int x, int y, SDL_Rect texrect, j1Module* callback = nullptr);
	virtual ~InputText();
private:
	GuiItem* text;
	GuiItem* image;

};

class GuiSlider : public GuiItem
{
public:
	GuiSlider(int x, int y, SDL_Rect texrect, j1Module* callback = nullptr);
	virtual ~GuiSlider();
	void slide();
	float returnSliderPos();
private:
	GuiItem* Image;
	GuiItem* ScrollThumb;

public:
	int dragarea;
};

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt);

	// Called after all Updates
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions
	void IterateFocus();

	void sendInput(GuiItem* item);

	SDL_Texture* GetAtlas() const;
	
	GuiItem* CreateGuiElement(Types type, int x, int y, SDL_Rect, SDL_Rect, SDL_Rect, GuiItem* parentnode = NULL, j1Module* callback = nullptr, char* text = "");
public:
	bool buttonPressed;
	int FocusIt;
	p2List<GuiItem*> guiElements;

private:
	p2SString atlas_file_name;
	SDL_Texture* atlas;


};

#endif // __j1GUI_H__