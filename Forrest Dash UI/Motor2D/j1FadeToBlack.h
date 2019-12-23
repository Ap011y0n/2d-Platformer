#ifndef __j1FADETOBLACK_H__
#define __j1FADETOBLACK_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "Animation.h"
#include "p2List.h"
#include "p2Point.h"
#include "j1Timer.h"



class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	~j1FadeToBlack();
	bool CleanUp();
	void FadeToBlack();
	void ChangeScene();
private:
	int alpha;
	float deltatime;
public: 
	bool changeScene;
	bool fade;
	j1Timer fadetimer;
	
};

#endif
