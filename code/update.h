#ifndef UPDATE_H
#define UPDATE_H
#include "aliases.h"

// Gamestate UpdateMain(u32 t, r32 dt, u32 t0, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets, Menu* menu);
// Gamestate UpdateOpts(u32 t, r32 dt, u32 t0, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets, Menu* menu);
// Gamestate UpdatePlay(u32 t, r32 dt, u32 t0, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets, Menu* menu);

i32 UpdateInit    (u32 t, r32 dt, void* engine);
i32 UpdateMain    (u32 t, r32 dt, void* engine);
i32 UpdateOptions (u32 t, r32 dt, void* engine);
i32 UpdatePlay    (u32 t, r32 dt, void* engine);
i32 UpdateEvent   (u32 t, r32 dt, void* engine);
i32 UpdateLose    (u32 t, r32 dt, void* engine);
i32 UpdateGoal    (u32 t, r32 dt, void* engine);
i32 UpdateEdit    (u32 t, r32 dt, void* engine);
i32 UpdateExit    (u32 t, r32 dt, void* engine);

#endif // !UPDATE_H
