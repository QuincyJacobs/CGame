#if !defined(CGAME_PLATFORM_H)
/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2021 by Grouse Games. All Rights Reserved. $
   ==================================================================== */

#ifdef __cplusplus
extern "C" {
#endif
    
/*
  NOTE(Quincy):

  CGAME_INTERNAL:
  0 - Build for public release
  1 - Build for developer only

  CGAME_SLOW:
  0 - No slow code allowed!
  1 - Slow code welcome.
*/

// TODO(Quincy): Implement sine myself
#include <stdint.h>


typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;


typedef struct thread_context
{
    int Placeholder;
} thread_context;

/*
  NOTE(Quincy): Services that the platform layer provides to the game
*/

#if CGAME_INTERNAL
/* IMPORTANT(Quincy):

   These are NOT for doing anything in the shipping game - they are
   blocking and the write doesn't protect againt lost data
*/
typedef struct debug_read_file_result
{
    uint32 ContentsSize;
    void* Contents;
}debug_read_file_result;

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context *Thread, void *Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context *Thread, char * Filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context *Thread, char *Filename, uint32 MemorySize, void *Memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#endif

/*
  NOTE(Quincy): Services that the game provides to the platform layer.
*/

typedef struct game_offscreen_buffer
{
    // NOTE: Pixels are always 32-bits wide, Memory order BB GG RR XX
    void* Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
} game_offscreeb_buffer;

typedef struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    int16* Samples;
} game_sound_output_buffer;

typedef struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
} game_button_state;

typedef struct game_controller_input
{
    bool32 IsConnected;
    bool32 IsAnalog;

    real32 StickAverageX;
    real32 StickAverageY;

    union
    {
	game_button_state Buttons[13];
	struct
	{
	    game_button_state MoveUp;
	    game_button_state MoveDown;
	    game_button_state MoveLeft;
	    game_button_state MoveRight;

	    game_button_state ActionUp;
	    game_button_state ActionDown;
	    game_button_state ActionLeft;
	    game_button_state ActionRight;

	    game_button_state LeftShoulder;
	    game_button_state RightShoulder;

	    game_button_state Back;
	    game_button_state Start;

	    // NOTE(Quincy): All buttons must be added above the dummy terminator button

	    game_button_state Terminator;
	};
    };
} game_controller_input;

typedef struct game_input
{
    game_button_state MouseButtons[5];
    int32 MouseX, MouseY, MouseZ;

    real32 dtForFrame;
    
    game_controller_input Controllers[5];
} game_input;


typedef struct game_memory
{
    bool32 IsInitialized;

    uint64 PermanentStorageSize;
    void* PermanentStorage; // NOTE(Quincy): REQUIRED to be cleared to zero at startup

    uint64 TransientStorageSize;
    void* TransientStorage; // NOTE(Quincy): REQUIRED to be cleared to zero at startup

    debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
    debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
} game_memory;

#define GAME_UPDATE_AND_RENDER(name) void name(thread_context *Thread, game_memory* Memory, game_input* Input, game_offscreen_buffer* Buffer)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

// NOTE(Quincy): At the moment, this had to be a very fast function, it cannot be
// more than a millisecond or so.
// TODO(Quincy): Reduce the pressure on this function's performance by measuring it
// or asking for it.
#define GAME_GET_SOUND_SAMPLES(name) void name(thread_context *Thread, game_memory* Memory, game_sound_output_buffer* SoundBuffer)
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);

#ifdef __cplusplus
}
#endif

#define CGAME_PLATFORM_H
#endif
