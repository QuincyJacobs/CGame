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
    uint32 AbsoluteTileZ;

    // NOTE(Quincy): These are the offsets from the tile center
    real32 OffsetX;
    real32 OffsetY;
};

struct tile_chunk_position
{
    uint32 TileChunkX;
    uint32 TileChunkY;
    uint32 TileChunkZ;

    uint32 RelativeTileX;
    uint32 RelativeTileY;
};

struct tile_chunk
{
    // TODO(Quincy): Real structure for a tile!
    uint32 *Tiles;
};

struct tile_map
{
    uint32 ChunkShift;
    uint32 ChunkMask;
    uint32 ChunkDimensions;
    
    real32 TileSideInMeters;
    
     // TODO(Quincy): REAL sparseness so anywhere in the world can be
    // represented without the giant pointer array.
    uint32 TileChunkCountX;
    uint32 TileChunkCountY;
    uint32 TileChunkCountZ;
    
    tile_chunk *TileChunks;
};

#define CGAME_TILE_H
#endif
