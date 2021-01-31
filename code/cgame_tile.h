#if !defined(CGAME_TILE_H)
/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2021 by Grouse Games. All Rights Reserved. $
   ==================================================================== */

struct tile_map_position
{
    // NOTE(Quincy): These are fixed point tile locations.
    // The high bits are the tile chunk index, and the low
    // bits are the tile index in the chunk.
    uint32 AbsoluteTileX;
    uint32 AbsoluteTileY;
    
    // TODO(Quincy): Should these be from the center of a tile?
    // TODO(Quincy): Rename to offset X and Y
    real32 TileRelativeX;
    real32 TileRelativeY;
};

struct tile_chunk_position
{
    uint32 TileChunkX;
    uint32 TileChunkY;

    uint32 RelativeTileX;
    uint32 RelativeTileY;
};

struct tile_chunk
{
    uint32 *Tiles;
};

struct tile_map
{
    uint32 ChunkShift;
    uint32 ChunkMask;
    uint32 ChunkDimensions;
    
    real32 TileSideInMeters;
    int32 TileSideInPixels;
    real32 MetersToPixels;
    
     // TODO(Quincy): Beginner's sparseness
    uint32 TileChunkCountX;
    uint32 TileChunkCountY;
    
    tile_chunk *TileChunks;
};

#define CGAME_TILE_H
#endif
