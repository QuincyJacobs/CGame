/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2021 by Grouse Games. All Rights Reserved. $
   ==================================================================== */

#include "cgame_tile.h"

inline void RecanonicalizeCoord(tile_map *TileMap, uint32 *Tile, real32 *TileRelative)
{
    // TODO(Quincy): Need to do something that doesn't use the divide/multiply method
    // for recanicalizing because this can end up rounding back on to the tile
    // you just came from.

    // NOTE(Quincy): TileMap is assumed to be toroidal topology, if you step off one
    // end you come back on the other.
    
    int32 Offset = RoundReal32ToInt32(*TileRelative / TileMap->TileSideInMeters);
    *Tile += Offset;
    *TileRelative -= Offset*TileMap->TileSideInMeters;

    // TODO(Quincy): Fix floating point math so this can be < ?
    Assert(*TileRelative >= -0.5f*TileMap->TileSideInMeters);
    Assert(*TileRelative <= 0.5f*TileMap->TileSideInMeters);
}

inline tile_map_position
RecanonicalizePosition(tile_map *TileMap, tile_map_position Pos)
{
    tile_map_position Result = Pos;

    RecanonicalizeCoord(TileMap, &Result.AbsoluteTileX, &Result.TileRelativeX);
    RecanonicalizeCoord(TileMap, &Result.AbsoluteTileY, &Result.TileRelativeY);

    return(Result);
}

inline tile_chunk *GetTileChunk(tile_map *TileMap, uint32 TileChunkX, uint32 TileChunkY)
{
    tile_chunk *TileChunk = 0;
    if((TileChunkX >= 0) && (TileChunkX < TileMap->TileChunkCountX) &&
       (TileChunkY >= 0) && (TileChunkY < TileMap->TileChunkCountY))
    {
        TileChunk = &TileMap->TileChunks[(TileChunkY * TileMap->TileChunkCountX) + TileChunkX];	
    }

    return(TileChunk);
}

inline uint32 GetTileValueUnchecked(tile_map *TileMap, tile_chunk *TileChunk, uint32 TileX, uint32 TileY)
{
    Assert(TileChunk);
    Assert(TileX < TileMap->ChunkDimensions);
    Assert(TileY < TileMap->ChunkDimensions);

    uint32 TileChunkValue = TileChunk->Tiles[(TileY * TileMap->ChunkDimensions) + TileX];
    return(TileChunkValue);
}

inline void SetTileValueUnchecked(tile_map *TileMap, tile_chunk *TileChunk, uint32 TileX, uint32 TileY, uint32 TileValue)
{
    Assert(TileChunk);
    Assert(TileX < TileMap->ChunkDimensions);
    Assert(TileY < TileMap->ChunkDimensions);

    TileChunk->Tiles[(TileY * TileMap->ChunkDimensions) + TileX] = TileValue;
}

inline uint32 GetTileValue(tile_map *TileMap, tile_chunk *TileChunk, uint32 TestTileX, uint32 TestTileY)
{
    uint32 TileChunkValue = 0;
    
    if(TileChunk)
    {
        TileChunkValue = GetTileValueUnchecked(TileMap, TileChunk, TestTileX, TestTileY);
    }

    return(TileChunkValue);
}

inline void SetTileValue(tile_map *TileMap, tile_chunk *TileChunk, uint32 TestTileX, uint32 TestTileY, uint32 TileValue)
{
    uint32 TileChunkValue = 0;
    
    if(TileChunk)
    {
        SetTileValueUnchecked(TileMap, TileChunk, TestTileX, TestTileY, TileValue);
    }
}

inline tile_chunk_position GetChunkPositionFor(tile_map *TileMap, uint32 AbsoluteTileX, uint32 AbsoluteTileY)
{
    tile_chunk_position Result;
    
    Result.TileChunkX = AbsoluteTileX >> TileMap->ChunkShift;
    Result.TileChunkY = AbsoluteTileY >> TileMap->ChunkShift;
    Result.RelativeTileX = AbsoluteTileX & TileMap->ChunkMask;
    Result.RelativeTileY = AbsoluteTileY & TileMap->ChunkMask;

    return(Result);
}

internal uint32 GetTileValue(tile_map *TileMap, uint32 AbsoluteTileX, uint32 AbsoluteTileY)
{
    tile_chunk_position ChunkPosition = GetChunkPositionFor(TileMap, AbsoluteTileX, AbsoluteTileY);
    tile_chunk *TileChunk = GetTileChunk(TileMap, ChunkPosition.TileChunkX, ChunkPosition.TileChunkY);
    uint32 TileChunkValue = GetTileValue(TileMap, TileChunk, ChunkPosition.RelativeTileX, ChunkPosition.RelativeTileY);

    return(TileChunkValue);
}

internal bool32 IsTileMapPointEmpty(tile_map *TileMap, tile_map_position TileMapPosition)
{
    uint32 TileChunkValue = GetTileValue(TileMap, TileMapPosition.AbsoluteTileX, TileMapPosition.AbsoluteTileY);
    bool32 IsEmpty = (TileChunkValue == 0);
     
    return(IsEmpty);
}

internal void SetTileValue(memory_arena *Arena, tile_map *TileMap, uint32 AbsoluteTileX, uint32 AbsoluteTileY, uint32 TileValue)
{
    tile_chunk_position ChunkPosition = GetChunkPositionFor(TileMap, AbsoluteTileX, AbsoluteTileY);
    tile_chunk *TileChunk = GetTileChunk(TileMap, ChunkPosition.TileChunkX, ChunkPosition.TileChunkY);

    // TODO(Quincy): On-demandd tile chunk creation
    Assert(TileChunk);

    SetTileValue(TileMap, TileChunk, ChunkPosition.RelativeTileX, ChunkPosition.RelativeTileY, TileValue);
}
