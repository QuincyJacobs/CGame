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

internal void GameOutputSound(game_sound_output_buffer *SoundBuffer, int ToneHz)
{
    local_persist real32 tSine;
    int16 ToneVolume = 3000;
    int WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;

    int16 *SampleOut = SoundBuffer->Samples;
    for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
    {
	real32 SineValue = sinf(tSine);
	int16 SampleValue = (int16)(SineValue * ToneVolume);
	*SampleOut++ = SampleValue;
	*SampleOut++ = SampleValue;

    tSine += 2.0f*Pi32*1.0f/(real32)WavePeriod;
    }
}

internal void RenderWeirdGradient(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
{
    uint8 *Row = (uint8 *)Buffer->Memory;

    for (int Y = 0; Y < Buffer->Height; ++Y)
    {
	uint32 *Pixel = (uint32 *)Row;

	for (int X = 0; X < Buffer->Width; ++X)
	{
	    uint8 Blue = (X + BlueOffset);
	    uint8 Green = (Y + GreenOffset);

	    *Pixel++ = ((Green << 8) | Blue);
	}
	Row += Buffer->Pitch;
    }
}

internal void GameUpdateAndRender(game_memory *Memory,
				  game_input *Input,
				  game_offscreen_buffer *Buffer,
				  game_sound_output_buffer *SoundBuffer)
{
    
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if(!Memory->IsInitialized)
    {
	GameState->ToneHz = 256;

	// TODO(Quincy): This may be more appropriate to do in the platform layer
	Memory->IsInitialized = true;
    }

    game_controller_input *Input0 = &Input->Controllers[0];
    
    if(Input0->IsAnalog)
    {
	// NOTE(Quincy): Use analog movement tuning
	 GameState->ToneHz = 256 + (int)(128.0f*(Input0->EndX));
	 GameState->BlueOffset += (int)4.0f*(Input0->EndY);
    }
    else
    {
	// NOTE(Quincy): Use digital movement tuning
    }

    if(Input0->Down.EndedDown)
    {
	GameState->GreenOffset += 1;
    }

   
        
    // TODO(Quincy): Allow sample offsets here for more robust platform options
    GameOutputSound(SoundBuffer, GameState->ToneHz);
    RenderWeirdGradient(Buffer, GameState->BlueOffset, GameState->GreenOffset);
}
