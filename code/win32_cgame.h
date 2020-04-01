#if !defined(WIN32_CGAME_H)
/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2020 by Quincy Jacobs. All Rights Reserved. $
   ==================================================================== */

struct win32_offscreen_buffer
{
    // NOTE: Pixels are always 32-bits wide, Memory order BB GG RR XX
    BITMAPINFO Info;
    void* Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

struct win32_sound_output
{
    int SamplesPerSecond;
    uint32 RunningSampleIndex;
    int BytesPerSample;
    DWORD SecondaryBufferSize;
    real32 tSine;
    int LatencySampleCount;
    // TODO(Quincy): Should running sample index be in bytes as well?
    // TODO(Quincy): Math get simpler if we add a "bytes per second" field?
};

struct win32_debug_time_marker
{
    DWORD PlayCursor;
    DWORD WriteCursor;
};

#define WIN32_CGAME_H
#endif
