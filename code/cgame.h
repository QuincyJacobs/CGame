#if !defined(CGAME_H)
/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2019 by Quincy Jacobs. All Rights Reserved. $
   ==================================================================== */

/*
  TODO(Quincy): Services that the platform layer provides to the game
*/

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

// FOUR THINGS - timing, controller/keyboard input, bitmap buffer to use, sound buffer to use
internal void GameUpdateAndRender(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset);

#define CGAME_H
#endif
