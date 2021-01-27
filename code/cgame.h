#if !defined(CGAME_H)
/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2020 by Quincy Jacobs. All Rights Reserved. $
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

struct canonical_position
{
    int32 TileMapX;
    int32 TileMapY;

    int32 TileX;
    int32 TileY;

    // NOTE(Quincy): This is rile-relative X and Y
    real32 TileRelativeX;
    real32 TileRelativeY;
};

// TODO(Quincy): Is this ever necessary?
struct raw_position
{
    int32 TileMapX;
    int32 TileMapY;

    // NOTE(Quincy): Tile-map relative X and Y
    real32 X;
    real32 Y;
};

struct tile_map
{
    uint32 *Tiles;
};

struct world
{
    int32 CountX;
    int32 CountY;
    real32 UpperLeftX;
    real32 UpperLeftY;
    real32 TileWidth;
    real32 TileHeight;
    
    // TODO(Quincy): Beginner's sparseness
    int32 TileMapCountX;
    int32 TileMapCountY;
    
    tile_map *TileMaps;
};

struct game_state
{
    // TODO(Quincy): Player state should be canonical position now?
    int32 PlayerTileMapX;
    int32 PlayerTileMapY;
    
    real32 PlayerX;
    real32 PlayerY;
};



#define CGAME_H
#endif
