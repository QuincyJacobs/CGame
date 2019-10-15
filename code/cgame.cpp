/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2019 by Quincy Jacobs. All Rights Reserved. $
   ==================================================================== */

#include "cgame.h"

internal void GameOutputSound(game_sound_output_buffer *SoundBuffer, int SampleCountToOutput)
{
    DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
    int16 *SampleOut = (int16 *)Region1;
    for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex)
    {
	real32 SineValue = sinf(SoundOutput->tSine);
	int16 SampleValue = (int16)(SineValue * SoundOutput->ToneVolume);
	*SampleOut++ = SampleValue;
	*SampleOut++ = SampleValue;

	SoundOutput->tSine += 2.0f*Pi32*1.0f/(real32)SoundOutput->WavePeriod;
	++SoundOutput->RunningSampleIndex;
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

internal void GameUpdateAndRender(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
{
    // TODO(Quincy): Allow sample offsets here for more robust platform options
    GameOutputSound(SoundBuffer, SampleCountToOutput);
    RenderWeirdGradient(Buffer, BlueOffset, GreenOffset);
}
