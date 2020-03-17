#if !defined(CGAME_H)
/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2020 by Quincy Jacobs. All Rights Reserved. $
   ==================================================================== */

/*
  NOTE(Quincy): 

  CGAME_INTERNAL:
  0 - Build for public release
  1 - Build for developer only
  
  CGAME_SLOW:
  0 - No slow code allowed!
  1 - Slow code welcome.
*/

#if CGAME_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert (Expression)
#endif

// TODO(Quincy): Should these always use 64-bit?
#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

// TODO(Quincy): swap min/max macro's??

inline uint32 SafeTruncateUInt64(uint64 Value)
{
    // TODO(Quincy): Defines for maximum values
    Assert(Value <= 0xFFFFFFFF);
    uint32 Result = (uint32)Value;
    return(Result);
}

/*
  NOTE(Quincy): Services that the platform layer provides to the game
*/

#if CGAME_INTERNAL
/* IMPORTANT(Quincy):

   These are NOT for doing anything in the shipping game - they are
   blocking and the write doesn't protect againt lost data
 */
struct debug_read_file_result
{
    uint32 ContentsSize;
    void *Contents;
};
internal debug_read_file_result DEBUGPlatformReadEntireFile(char *Filename);
internal void DEBUGPlatformFreeFileMemory(void *Memory);
internal bool32 DEBUGPlatformWriteEntireFile(char *Filename, uint32 MemorySize, void *Memory);
#endif

/*
  NOTE(Quincy): Services that the game provides to the platform layer.
*/

struct game_offscreen_buffer
{
    // NOTE: Pixels are always 32-bits wide, Memory order BB GG RR XX
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    int16 *Samples;
};

struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct game_controller_input
{
    bool32 IsAnalog;
    
    real32 StickAverageX;
    real32 StickAverageY;
    
    union
    {
	game_button_state Buttons[10];
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

	    game_button_state Start;
	    game_button_state Back;
	};
    };
};

struct game_input
{
    // TODO(Quincy): Insert clock value here.
    game_controller_input Controllers[5];
};

struct game_memory
{
    bool32 IsInitialized;
    
    uint64 PermanentStorageSize;
    void *PermanentStorage; // NOTE(Quincy): REQUIRED to be cleared to zero at startup
    
    uint64 TransientStorageSize;
    void *TransientStorage; // NOTE(Quincy): REQUIRED to be cleared to zero at startup
};

// FOUR THINGS - timing, controller/keyboard input, bitmap buffer to use, sound buffer to use
internal void GameUpdateAndRender(game_memory *Memory,
				  game_input *Input,
				  game_offscreen_buffer *Buffer,
				  game_sound_output_buffer *SoundBuffer);

struct game_state
{
    int ToneHz;
    int GreenOffset;
    int BlueOffset;
};

#define CGAME_H
#endif
