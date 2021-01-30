/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2020 by Grouse Games. All Rights Reserved. $
   ==================================================================== */

#include "cgame.h"
#include "cgame_intrinsics.h"

internal void GameOutputSound(game_state* GameState, game_sound_output_buffer* SoundBuffer, int ToneHz)
{
    int16 ToneVolume = 3000;
    int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

    int16* SampleOut = SoundBuffer->Samples;
    for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
    {
#if 0
	real32 SineValue = sinf(GameState->tSine);
	int16 SampleValue = (int16)(SineValue * ToneVolume);
#else
	int16 SampleValue = 0;
#endif
	*SampleOut++ = SampleValue;
	*SampleOut++ = SampleValue;

#if 0
	GameState->tSine += 2.0f * Pi32 * 1.0f / (real32)WavePeriod;
	if(GameState->tSine > 2.0f*Pi32)
	{
	    GameState->tSine -= 2.0f*Pi32;
	}
#endif
    }
}


internal void DrawRectangle(game_offscreen_buffer* Buffer,
			    real32 RealMinX, real32 RealMinY, real32 RealMaxX, real32 RealMaxY,
			    real32 R, real32 G, real32 B)
{
    int32 MinX = RoundReal32ToInt32(RealMinX);
    int32 MinY = RoundReal32ToInt32(RealMinY);
    int32 MaxX = RoundReal32ToInt32(RealMaxX);
    int32 MaxY = RoundReal32ToInt32(RealMaxY);

    if(MinX < 0)
    {
	MinX = 0;
    }

    if(MinY < 0)
    {
	MinY = 0;
    }

    if(MaxX > Buffer->Width)
    {
	MaxX = Buffer->Width;
    }

    if(MaxY > Buffer->Height)
    {
	MaxY = Buffer->Height;
    }

    // TODO(Quincy): Floating point color tomorrow!
    // BIT PATTERN: 0x AA RR GG BB
    uint32 Color = ((RoundReal32ToUInt32(R * 255.0f) << 16) |
		    (RoundReal32ToUInt32(G * 255.0f) << 8) |
		    (RoundReal32ToUInt32(B * 255.0f) << 0));

    uint8 *Row = ((uint8 *)Buffer->Memory +
		    MinX*Buffer->BytesPerPixel +
		    MinY*Buffer->Pitch);
    
    for(int Y = MinY;
	Y < MaxY;
	++Y)
    {
	uint32 *Pixel = (uint32 *)Row;
	for(int X = MinX;
	    X < MaxX;
	    ++X)
	{
	    *Pixel++ = Color;
	}
	Row += Buffer->Pitch;
    }
}

inline tile_chunk *GetTileChunk(world *World, int32 TileChunkX, int32 TileChunkY)
{
    tile_chunk *TileChunk = 0;
    if((TileChunkX >= 0) && (TileChunkX < World->TileChunkCountX) &&
       (TileChunkY >= 0) && (TileChunkY < World->TileChunkCountY))
    {
        TileChunk = &World->TileChunks[(TileChunkY * World->TileChunkCountX) + TileChunkX];	
    }

    return(TileChunk);
}

inline uint32 GetTileValueUnchecked(world *World, tile_chunk *TileChunk, uint32 TileX, uint32 TileY)
{
    Assert(TileChunk);
    Assert(TileX < World->ChunkDimensions);
    Assert(TileY < World->ChunkDimensions);

    uint32 TileChunkValue = TileChunk->Tiles[(TileY * World->ChunkDimensions) + TileX];
    return(TileChunkValue);
}

inline uint32 GetTileValue(world *World, tile_chunk *TileChunk, uint32 TestTileX, uint32 TestTileY)
{
    uint32 TileChunkValue = 0;
    
    if(TileChunk)
    {
        TileChunkValue = GetTileValueUnchecked(World, TileChunk, TestTileX, TestTileY);
    }

    return(TileChunkValue);
}

inline void RecanonicalizeCoord(world *World, uint32 *Tile, real32 *TileRelative)
{
    // TODO(Quincy): Need to do something that doesn't use the divide/multiply method
    // for recanicalizing because this can end up rounding back on to the tile
    // you just came from.

    // NOTE(Quincy): World is assumed to be toroidal topology, if you step off one
    // end you come back on the other.
    
    int32 Offset = RoundReal32ToInt32(*TileRelative / World->TileSideInMeters);
    *Tile += Offset;
    *TileRelative -= Offset*World->TileSideInMeters;

    // TODO(Quincy): Fix floating point math so this can be < ?
    Assert(*TileRelative >= -0.5f*World->TileSideInMeters);
    Assert(*TileRelative <= 0.5f*World->TileSideInMeters);
}

inline world_position
RecanonicalizePosition(world *World, world_position Pos)
{
    world_position Result = Pos;

    RecanonicalizeCoord(World, &Result.AbsoluteTileX, &Result.TileRelativeX);
    RecanonicalizeCoord(World, &Result.AbsoluteTileY, &Result.TileRelativeY);

    return(Result);
}

inline tile_chunk_position GetChunkPositionFor(world *World, uint32 AbsoluteTileX, uint32 AbsoluteTileY)
{
    tile_chunk_position Result;
    
    Result.TileChunkX = AbsoluteTileX >> World->ChunkShift;
    Result.TileChunkY = AbsoluteTileY >> World->ChunkShift;
    Result.RelativeTileX = AbsoluteTileX & World->ChunkMask;
    Result.RelativeTileY = AbsoluteTileY & World->ChunkMask;

    return(Result);
}

internal uint32 GetTileValue(world *World, uint32 AbsoluteTileX, uint32 AbsoluteTileY)
{
    tile_chunk_position ChunkPosition = GetChunkPositionFor(World, AbsoluteTileX, AbsoluteTileY);
    tile_chunk *TileChunk = GetTileChunk(World, ChunkPosition.TileChunkX, ChunkPosition.TileChunkY);
    uint32 TileChunkValue = GetTileValue(World, TileChunk, ChunkPosition.RelativeTileX, ChunkPosition.RelativeTileY);

    return(TileChunkValue);
}

internal bool32 IsWorldPointEmpty(world *World, world_position WorldPosition)
{
    uint32 TileChunkValue = GetTileValue(World, WorldPosition.AbsoluteTileX, WorldPosition.AbsoluteTileY);
    bool32 IsEmpty = (TileChunkValue == 0);
     
    return(IsEmpty);
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons) - 1));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    game_state* GameState = (game_state*)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
	GameState->PlayerP.AbsoluteTileX = 3;    
	GameState->PlayerP.AbsoluteTileY = 3;
	GameState->PlayerP.TileRelativeX = 5.0f;
	GameState->PlayerP.TileRelativeY = 5.0f;
	
	Memory->IsInitialized = true;
    }

#define TILE_CHUNK_COUNT_X 256
#define TILE_CHUNK_COUNT_Y 256
    
    uint32 TempTiles[TILE_CHUNK_COUNT_Y][TILE_CHUNK_COUNT_X] =
	{
	    {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, 1},
	    {1, 1, 0, 0,  0, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 1, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 1, 1, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1},
	    {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1, 1},
	    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 1},
	    {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, 1},
	};

    world World;
    // NOTE(Quincy): This is set to using 256x256 tile chunks.
    World.ChunkShift = 8;
    World.ChunkMask = (1 << World.ChunkShift) - 1;
    World.ChunkDimensions = 256;
    
    World.TileChunkCountX = 1;
    World.TileChunkCountY = 1;

    tile_chunk TileChunk;
    TileChunk.Tiles = (uint32 *)TempTiles;
    World.TileChunks = &TileChunk;

    // TODO(Quincy): Begin using tile side in meters
    World.TileSideInMeters = 1.4f;
    World.TileSideInPixels = 60;
    World.MetersToPixels = World.TileSideInPixels / World.TileSideInMeters;

    real32 PlayerHeight = 1.4f;
    real32 PlayerWidth = 0.75f*PlayerHeight;

    real32 LowerLeftX = -(real32)World.TileSideInPixels/2;
    real32 LowerLeftY = (real32)Buffer->Height;
    
    for (int ControllerIndex = 0;
	 ControllerIndex < ArrayCount(Input->Controllers);
	 ++ControllerIndex)
    {
	game_controller_input* Controller = GetController(Input, ControllerIndex);

	if (Controller->IsAnalog)
	{
	    // NOTE(Quincy): Use analog movement tuning
	}
	else
	{
	    // NOTE(Quincy): Use digital movement tuning
	    real32 dPlayerX = 0.0f; // pixels/second
	    real32 dPlayerY = 0.0f; // pixels/second
	    
	    if(Controller->MoveUp.EndedDown)
	    {
		dPlayerY = 1.0f;
	    }
	    if(Controller->MoveDown.EndedDown)
	    {
		dPlayerY = -1.0f;
	    }
	    if(Controller->MoveLeft.EndedDown)
	    {
		dPlayerX = -1.0f;
	    }
	    if(Controller->MoveRight.EndedDown)
	    {
		dPlayerX = 1.0f;
	    }
	    
	    real32 PlayerSpeed = 2.0f;
	    if(Controller->ActionUp.EndedDown)
	    {
		PlayerSpeed = 10.0f;
	    }
	    
	    dPlayerX *= PlayerSpeed;
	    dPlayerY *= PlayerSpeed;

	    // TODO(Quincy): Diagonal will be faster! Fix once we have vectors
	    world_position NewPlayerP = GameState->PlayerP;
	    NewPlayerP.TileRelativeX += Input->dtForFrame*dPlayerX;
	    NewPlayerP.TileRelativeY += Input->dtForFrame*dPlayerY;
	    NewPlayerP = RecanonicalizePosition(&World, NewPlayerP);
	    // TODO(Quincy): Delta function that auto-recanonicalize

	    world_position PlayerLeft = NewPlayerP;
	    PlayerLeft.TileRelativeX -= 0.5f*PlayerWidth;
	    PlayerLeft = RecanonicalizePosition(&World, PlayerLeft);
	    
	    world_position PlayerRight = NewPlayerP;
	    PlayerRight.TileRelativeX += 0.5f*PlayerWidth;
	    PlayerRight = RecanonicalizePosition(&World, PlayerRight);

	    if(IsWorldPointEmpty(&World, NewPlayerP) &&
	       IsWorldPointEmpty(&World, PlayerLeft) &&
	       IsWorldPointEmpty(&World, PlayerRight))
	    {
		GameState->PlayerP = NewPlayerP;
	    }
	}
    }
    
    //tile_map *TileMap = GetTileMap(&World, GameState->PlayerP.TileMapX, GameState->PlayerP.TileMapY);
    //Assert(TileMap);
    
    DrawRectangle(Buffer,
		  0.0f, 0.0f, (real32)Buffer->Width, (real32)Buffer->Height,
		  1.0f, 0.0f, 1.0f);

    real32 ScreenCenterX = 0.5f*(real32)Buffer->Width;
    real32 ScreenCenterY = 0.5f*(real32)Buffer->Height;
    
    for(int32 RelRow = -10; RelRow < 10; ++RelRow)
    {
	for(int32 RelColumn = -20; RelColumn < 20; ++RelColumn)
	{
	    uint32 Column = RelColumn + GameState->PlayerP.AbsoluteTileX;
	    uint32 Row = RelRow + GameState->PlayerP.AbsoluteTileY;
	    
	    
	    uint32 TileID = GetTileValue(&World, Column, Row);;
	    real32 Gray = 0.5f;
	    if(TileID == 1)
	    {
		Gray = 1.0f;
	    }

	    if((Column == GameState->PlayerP.AbsoluteTileX) && (Row == GameState->PlayerP.AbsoluteTileY))
	    {
		Gray = 0.0f;
	    }
	    
	    real32 CenterX = ScreenCenterX - World.MetersToPixels*GameState->PlayerP.TileRelativeX + ((real32)RelColumn)*World.TileSideInPixels;
	    real32 CenterY = ScreenCenterY + World.MetersToPixels*GameState->PlayerP.TileRelativeY - ((real32)RelRow)*World.TileSideInPixels;
	    
	    real32 MinX = CenterX - 0.5f*World.TileSideInPixels;
	    real32 MinY = CenterY - 0.5f*World.TileSideInPixels;
	    real32 MaxX = CenterX + 0.5f*World.TileSideInPixels;
	    real32 MaxY = CenterY + 0.5f*World.TileSideInPixels;
	    DrawRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
	}
    }

    real32 PlayerR = 1.0f;
    real32 PlayerG = 1.0f;
    real32 PlayerB = 0.0f;
   
    real32 PlayerLeftPos = ScreenCenterX - (0.5f*World.MetersToPixels*PlayerWidth);
    real32 PlayerTopPos = ScreenCenterY - World.MetersToPixels*PlayerHeight;
    
    DrawRectangle(Buffer,
		  PlayerLeftPos, PlayerTopPos, PlayerLeftPos + World.MetersToPixels*PlayerWidth, PlayerTopPos + World.MetersToPixels*PlayerHeight,
		  PlayerR, PlayerG, PlayerB);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state* GameState = (game_state*)Memory->PermanentStorage;
    GameOutputSound(GameState, SoundBuffer, 400);
}

/*
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

	    *Pixel++ = ((Green << 16) | Blue);
	}
	Row += Buffer->Pitch;
    }
}
*/
