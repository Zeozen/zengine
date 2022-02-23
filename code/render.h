#ifndef RENDER_H
#define RENDER_H
#include "aliases.h"

// void RenderMain(u32 t_r, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets, Menu* menu);
// void RenderOpts(u32 t_r, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets, Menu* menu);
// void RenderPlay(u32 t_r, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets, Menu* menu);
// void RenderLose(u32 t_r, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets, Menu* menu);

void RenderInit    (u32 t, r32 a, void* engine);
void RenderMain    (u32 t, r32 a, void* engine);
void RenderOptions (u32 t, r32 a, void* engine);
void RenderPlay    (u32 t, r32 a, void* engine);
void RenderEvent   (u32 t, r32 a, void* engine);
void RenderLose    (u32 t, r32 a, void* engine);
void RenderGoal    (u32 t, r32 a, void* engine);
void RenderEdit    (u32 t, r32 a, void* engine);
void RenderExit    (u32 t, r32 a, void* engine);


#endif // !RENDER_H