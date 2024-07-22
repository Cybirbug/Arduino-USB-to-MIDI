#pragma once
//keyboard stuff
#define REPORT_BUFFER_SIZE 128//best way to log for keyboard report data
#define KEYBOARD_NOTE_ON 0X90

//joystick
#define JOY_NOTE_ON 0x90
#define JOY_OFFSET 102//top channel for keyboards tested so far is 101

//mouse
#define MOUSE_RESOLUTION 512//higher number means less sensitive mouse
#define MOUSE_DIVISOR 4//should always be mouseresolution/128
#define MOUSE_NOTE_ON 0x91

//dacepad
#define DANCEPAD_NOTE_ON 0x92