#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Window.h"

j1FadeToBlack::j1FadeToBlack() : j1Module()
{
	
	name.create("Fade");
}

// Destructor
j1FadeToBlack::~j1FadeToBlack()
{
}
bool j1FadeToBlack::CleanUp()
{
	return true;
}

bool j1FadeToBlack::Awake(pugi::xml_node & config)
{
	return true;
}

bool j1FadeToBlack::Start()
{
	fade = false;
	return true;
}

bool j1FadeToBlack::Update(float dt)
{
	if (fade) {
		FadeToBlack();
	}
	return true;
}
bool j1FadeToBlack::PostUpdate(float dt)
{
	return true;
}

void j1FadeToBlack::FadeToBlack() {
	if(!fade)fadetimer.Start();
	fade = true;
	SDL_Rect screen;
	screen.x = -1 * App->render->camera.x;
	screen.y = -1 * App->render->camera.y;
	screen.w = App->win->width * App->win->GetScale();
	screen.h = App->win->height * App->win->GetScale();
	//App->render->DrawQuad(screen, 0, 0, 0, 255);
	LOG("%f", fadetimer.Read());
	if (fadetimer.Read() > 1000) {
		App->want_to_load = true;
		fade = false;
	}

}