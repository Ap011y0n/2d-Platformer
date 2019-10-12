#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	bool j1Audio::Save(pugi::xml_node& config);
	bool j1Audio::Load(pugi::xml_node& config);

private:

	_Mix_Music*			music = NULL;
	p2List<Mix_Chunk*>	fx;
	p2SString			music_folder;
	float volume;
};

#endif // __j1AUDIO_H__