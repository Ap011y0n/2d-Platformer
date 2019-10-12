#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#define MAX_FRAMES 25

class Animation
{
public:
	bool loop = true;
	//float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];
	int pivotx[MAX_FRAMES];
	int pivoty[MAX_FRAMES];
	int pivotx2[MAX_FRAMES];
	int pivoty2[MAX_FRAMES];
	double speed[MAX_FRAMES];
	int last_frame = 0;

private:
	float current_frame;
	int loops = 0;
	int speeddelay = 0;

public:

	void PushBack(const SDL_Rect& rect, double delay, int px, int py, int px2, int py2)
	{
		pivotx[last_frame] = px;
		pivoty[last_frame] = py;
		pivotx2[last_frame] = px2;
		pivoty2[last_frame] = py2;
		frames[last_frame++] = rect;
		speed[speeddelay++] = delay;

	}

	SDL_Rect& GetCurrentFrame()
	{
		current_frame += speed[(int)current_frame];;
		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}

		return frames[(int)current_frame];
	}
	int returnCurrentFrame() {
		return (int)current_frame;
	}
	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
	}
};

#endif