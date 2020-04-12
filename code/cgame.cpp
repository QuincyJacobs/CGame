/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2020 by Quincy Jacobs. All Rights Reserved. $
   ==================================================================== */

#include "cgame.h"

internal void GameOutputSound(game_state* GameState, game_sound_output_buffer* SoundBuffer, int ToneHz)
{
    int16 ToneVolume = 3000;
    int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

    int16* SampleOut = SoundBuffer->Samples;
    for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
    {
	real32 SineValue = sinf(GameState->tSine);
	int16 SampleValue = (int16)(SineValue * ToneVolume);
	*SampleOut++ = SampleValue;
	*SampleOut++ = SampleValue;

	GameState->tSine += 2.0f * Pi32 * 1.0f / (real32)WavePeriod;
	if(GameState->tSine > 2.0f*Pi32)
	{
	    GameState->tSine -= 2.0f*Pi32;
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

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons) - 1));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    game_state* GameState = (game_state*)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
	char* Filename = __FILE__;

	debug_read_file_result File = Memory->DEBUGPlatformReadEntireFile(Filename);
	if (File.Contents)
	{
	    Memory->DEBUGPlatformWriteEntireFile("test.out", File.ContentsSize, File.Contents);
	    Memory->DEBUGPlatformFreeFileMemory(File.Contents);
	}

	GameState->ToneHz = 512;
	GameState->tSine = 0.0f;

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

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state* GameState = (game_state*)Memory->PermanentStorage;
    GameOutputSound(GameState, SoundBuffer, GameState->ToneHz);
}

#if CGAME_WIN32
#include <windows.h>

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif
