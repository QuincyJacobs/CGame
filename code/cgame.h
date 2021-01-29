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

struct canonical_position
{
    /* TODO(Quincy):
    
       Take the tile map x and y
       and the tile x and y.

       and pack them into single 32-bit values for x and y
       where there are some low bits for the tile index
       and the high bits are the tile "page"

       NOTE(Quincy): We can just use truncate
     */
#if 1
    int32 TileMapX;
    int32 TileMapY;

    int32 TileX;
    int32 TileY;
#else
    uint32 _TileX;
    uint32 _TileY;
#endif
    /* TODO(Quincy):

       Convert these to math-friendly, resolution indepent 
       representation of world units relative to a tile.
      
     */
    // NOTE(Quincy): This is rile-relative X and Y
    real32 TileRelativeX;
    real32 TileRelativeY;
};

struct tile_map
{
    uint32 *Tiles;
};

struct world
{
    real32 TileSideInMeters;
    int32 TileSideInPixels;
    
    int32 CountX;
    int32 CountY;
    
    real32 UpperLeftX;
    real32 UpperLeftY;
    
    // TODO(Quincy): Beginner's sparseness
    int32 TileMapCountX;
    int32 TileMapCountY;
    
    tile_map *TileMaps;
};

struct game_state
{
    canonical_position PlayerP;
};



#define CGAME_H
#endif
