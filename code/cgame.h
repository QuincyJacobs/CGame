#if !defined(CGAME_H)
/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2020 by Grouse Games. All Rights Reserved. $
   ==================================================================== */

#include "cgame_platform.h"

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

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

inline game_controller_input *GetController(game_input *Input, int unsigned ControllerIndex)
{
    Assert(ControllerIndex < ArrayCount(Input->Controllers));
    game_controller_input *Result = &Input ->Controllers[ControllerIndex];
    return(Result);
}

//
//
//

struct memory_arena
{
    memory_index Size;
    uint8 * Base;
    memory_index Used;
};

internal void InitializeArena(memory_arena *Arena, memory_index Size, uint8 *Base)
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, (Count)*sizeof(type))
void *PushSize_(memory_arena *Arena, memory_index Size)
{
    Assert((Arena->Used + Size) <= Arena->Size); 
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += Size;

    return(Result);
}

#include "cgame_intrinsics.h"
#include "cgame_tile.h"

struct world
{
    tile_map *TileMap;
};

struct loaded_bitmap
{
    int32 Width;
    int32 Height;
    uint32 *Pixels;
};

struct game_state
{
    memory_arena WorldArena;
    world *World;
    
    tile_map_position PlayerP;

    loaded_bitmap Backdrop;
    loaded_bitmap HeroHead;
    loaded_bitmap HeroCape;
    loaded_bitmap HeroTorso;
};

#define CGAME_H
#endif
