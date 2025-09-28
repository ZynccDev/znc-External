#pragma once
#include <Windows.h>

namespace offsets {
	// Instance
    inline uintptr_t This = 0x8;
    inline uintptr_t name = 0x88;
    inline uintptr_t parent = 0x58;
    inline uintptr_t children = 0x68;
    inline uintptr_t children_end = 0x8;
    inline uintptr_t Gravity = 0x998;
    inline uintptr_t GravityInfo = 0x3d8;

    inline uintptr_t cframe = 0x130;

    // Player
    inline uintptr_t localplayer = 0x128;
    inline uintptr_t humanoidstate = 0x870;
    inline uintptr_t walkspeed = 0x1DC;
	inline uintptr_t walkspeedcheck = 0x3B8; // no suspicious client kicks if we use this soo W
    inline uintptr_t jumppower = 0x1B8;
    inline uintptr_t health = 0x19C;

	inline uintptr_t cancollide = 0x2E5;
    inline uintptr_t primitive = 0x178;
	// DataModel
    inline uintptr_t fakedatamodel = 0x7168648; // this is very important offset (you will crash without it)
    inline uintptr_t fakedm_to_dm = 0x1C0; // hardly changes
    // ClassName
    inline uintptr_t classdescriptor = 0x18;
    inline uintptr_t classdescriptortoclassname = 0x8;
}
