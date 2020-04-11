/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2020 by Quincy Jacobs. All Rights Reserved. $
   ==================================================================== */

#include "cgame.h"
#include <math.h>

#include <math.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef float real32;
typedef double real64;

internal void GameOutputSound(game_sound_output_buffer* SoundBuffer, int ToneHz)
{
    local_persist real32 tSine;
    int16 ToneVolume = 3000;
    int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

    int16* SampleOut = SoundBuffer->Samples;
    for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
    {
	real32 SineValue = sinf(tSine);
	int16 SampleValue = (int16)(SineValue * ToneVolume);
	*SampleOut++ = SampleValue;
	*SampleOut++ = SampleValue;

	tSine += 2.0f * Pi32 * 1.0f / (real32)WavePeriod;
	if(tSine > 2.0f*Pi32)
	{
	    tSine -= 2.0f*Pi32;
	}
    }
}

internal void RenderWeirdGradient(game_offscreen_buffer* Buffer, int BlueOffset, int GreenOffset)
{
    uint8* Row = (uint8*)Buffer->Memory;

    for (int Y = 0; Y < Buffer->Height; ++Y)
    {
	uint32* Pixel = (uint32*)Row;

	for (int X = 0; X < Buffer->Width; ++X)
	{
	    uint8 Blue = (uint8)(X + BlueOffset);
	    uint8 Green = (uint8)(Y + GreenOffset);

	    *Pixel++ = ((Green << 8) | Blue);
	}
	Row += Buffer->Pitch;
    }
}

internal void GameUpdateAndRender(game_memory* Memory,
				  game_input* Input,
				  game_offscreen_buffer* Buffer)
{
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons) - 1));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    game_state* GameState = (game_state*)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
	char* Filename = __FILE__;

	debug_read_file_result File = DEBUGPlatformReadEntireFile(Filename);
	if (File.Contents)
	{
	    DEBUGPlatformWriteEntireFile("test.out", File.ContentsSize, File.Contents);
	    DEBUGPlatformFreeFileMemory(File.Contents);
	}

	GameState->ToneHz = 512;

	// TODO(Quincy): This may be more appropriate to do in the platform layer
	Memory->IsInitialized = true;
    }

    for (int ControllerIndex = 0;
	 ControllerIndex < ArrayCount(Input->Controllers);
	 ++ControllerIndex)
    {
	game_controller_input* Controller = GetController(Input, ControllerIndex);

	if (Controller->IsAnalog)
	{
	    // NOTE(Quincy): Use analog movement tuning
	    GameState->ToneHz = 512 + (int)(128.0f * (Controller->StickAverageX));
	    GameState->BlueOffset += (int)(4.0f * (Controller->StickAverageY));
	}
	else
	{
	    if (Controller->MoveLeft.EndedDown)
	    {
		GameState->BlueOffset -= 1;
	    }
	    if (Controller->MoveRight.EndedDown)
	    {
		GameState->BlueOffset += 1;
	    }
	    // NOTE(Quincy): Use digital movement tuning
	}

	if (Controller->ActionDown.EndedDown)
	{
	    GameState->GreenOffset += 1;
	}
    }
    
    RenderWeirdGradient(Buffer, GameState->BlueOffset, GameState->GreenOffset);
}

internal void GameGetSoundSamples(game_memory* Memory,
				  game_sound_output_buffer* SoundBuffer)
{
    game_state* GameState = (game_state*)Memory->PermanentStorage;
    GameOutputSound(SoundBuffer, GameState->ToneHz);
}
