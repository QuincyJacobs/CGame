/* ======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Quincy Jacobs $
   $Notice: (C) Copyright 2020 by Grouse Games. All Rights Reserved. $
   ==================================================================== */

#include "cgame.h"
#include "cgame_tile.cpp"
#include "cgame_random.h"

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

internal void DrawBitmap(game_offscreen_buffer *Buffer, loaded_bitmap *Bitmap,
			 real32 RealX, real32 RealY, int32 AlignX = 0, int32 AlignY = 0)
{
    int32 MinX = RoundReal32ToInt32(RealX);
    int32 MinY = RoundReal32ToInt32(RealY);
    int32 MaxX = RoundReal32ToInt32(RealX + (real32)Bitmap->Width);
    int32 MaxY = RoundReal32ToInt32(RealY + (real32)Bitmap->Height);

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

    // TODO(Quincy): SourceRow needs to be changed based on clipping.
    uint32 *SourceRow = Bitmap->Pixels + Bitmap->Width*(Bitmap->Height - 1);
    uint8 *DestRow = ((uint8 *)Buffer->Memory +
		      MinX*Buffer->BytesPerPixel +
		      MinY*Buffer->Pitch);
    
    for(int Y = MinY; Y < MaxY; ++Y)
    {
	uint32 *Dest = (uint32 *)DestRow;
	uint32 *Source = SourceRow;
	
        for(int X = MinX; X < MaxX; ++X)
	{
	    real32 A = (real32)((*Source >> 24) & 0xFF) / 0xFF;
	    real32 SR = (real32)((*Source >> 16) & 0xFF);
	    real32 SG = (real32)((*Source >> 8) & 0xFF);
	    real32 SB = (real32)((*Source >> 0) & 0xFF);

	    real32 DR = (real32)((*Dest >> 16) & 0xFF);
	    real32 DG = (real32)((*Dest >> 8) & 0xFF);
	    real32 DB = (real32)((*Dest >> 0) & 0xFF);

	    // TODO(Quincy): Someday, we need to talk about premultiplied alpha!
	    // (this is not premultiplied alpha)
	    real32 R = (1.0f-A)*DR + A*SR;
	    real32 G = (1.0f-A)*DG + A*SG;
	    real32 B = (1.0f-A)*DB + A*SB;
			    	    
	    *Dest = (((uint32)(R + 0.5f) << 16) |
		     ((uint32)(G + 0.5f) << 8) |
		     ((uint32)(B + 0.5f) << 0));

	    ++Dest;
	    ++Source;
	}
	
	DestRow += Buffer->Pitch;
	SourceRow -= Bitmap->Width;
    }
}

#pragma pack(push, 1)
struct bitmap_header
{
    uint16 FileType;
    uint32 FileSize;
    uint16 Reserved1;
    uint16 Reserved2;
    uint32 BitmapOffset;
    uint32 Size;
    int32 Width;
    int32 Height;
    uint16 Planes;
    uint16 BitsPerPixel;
    uint32 Compression;
    uint32 SizeOfBitmap;
    int32 HorzResolution;
    int32 VertResolution;
    uint32 ColorsUsed;
    uint32 ColorsImportant;

    uint32 RedMask;
    uint32 GreenMask;
    uint32 BlueMask;
};
#pragma pack(pop)



internal loaded_bitmap DEBUGLoadBMP(thread_context *Thread, debug_platform_read_entire_file *ReadEntireFile,
			   char *FileName)
{
    loaded_bitmap Result = {};

    debug_read_file_result ReadResult = ReadEntireFile(Thread, FileName);
    if(ReadResult.ContentsSize != 0)
    {
	bitmap_header *Header = (bitmap_header *)ReadResult.Contents;
	uint32 *Pixels = (uint32 *)((uint8 *)ReadResult.Contents + Header->BitmapOffset);
	Result.Pixels = Pixels;
	Result.Width =  Header->Width;
	Result.Height = Header->Height;

	Assert(Header->Compression == 3);

	// NOTE(Quincy): If you are using this generically for some reason,
	// please remember that BMP files CAN GO IN EITHER DIRECTION and
	// the height will be negative for top-down.
	// (Also, there can be compression, etc., etc... DON'T think this
	// is complete BMP loading code because it isn't!)

	// NOTE(Quincy): Byte order in memory is determined by the Header itself,
	// so we have to read out the masks and convert the pixels ourselves.
	uint32 RedMask = Header->RedMask;
	uint32 GreenMask = Header->GreenMask;
	uint32 BlueMask = Header->BlueMask;
	uint32 AlphaMask = (RedMask | GreenMask | BlueMask);
	AlphaMask = ~AlphaMask;

	bit_scan_result RedShift = FindLeastSignificantSetBit(RedMask);
	bit_scan_result GreenShift = FindLeastSignificantSetBit(GreenMask);
	bit_scan_result BlueShift = FindLeastSignificantSetBit(BlueMask);
	bit_scan_result AlphaShift = FindLeastSignificantSetBit(AlphaMask);

	Assert(RedShift.Found);
	Assert(GreenShift.Found);
	Assert(BlueShift.Found);
	Assert(AlphaShift.Found);

	uint32 *SourceDest = Pixels;
	for(int32 Y = 0; Y < Header->Height; ++Y)
	{
	    for(int32 X = 0; X < Header->Width; ++X)
	    {
		uint32 C = *SourceDest;
		*SourceDest = ((((C >> AlphaShift.Index) & 0xFF) << 24) |
			       (((C >> RedShift.Index) & 0xFF) << 16) |
			       (((C >> GreenShift.Index) & 0xFF) << 8) |
			       (((C >> BlueShift.Index) & 0xFF) << 0));
		++SourceDest;
	    }
	}
    }

    return(Result);
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons) - 1));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    real32 PlayerHeight = 1.4f;
    real32 PlayerWidth = 0.75f*PlayerHeight;
    

    game_state* GameState = (game_state*)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
	GameState->Backdrop = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_background.bmp");
	hero_bitmaps *Bitmap;

	Bitmap = &GameState->HeroBitmaps[0];
        Bitmap->Head = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_head.bmp");
	Bitmap->Cape = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_cape.bmp");
	Bitmap->Torso = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_torso.bmp");
	Bitmap->AlignX = 72;
	Bitmap->AlignY = 182;
	++Bitmap;
	
        Bitmap->Head = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_head.bmp");
	Bitmap->Cape = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_cape.bmp");
        Bitmap->Torso = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_torso.bmp");
	Bitmap->AlignX = 72;
	Bitmap->AlignY = 182;
        ++Bitmap;
	
	Bitmap->Head = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_head.bmp");
	Bitmap->Cape = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_cape.bmp");
        Bitmap->Torso = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_torso.bmp");
	Bitmap->AlignX = 72;
	Bitmap->AlignY = 182;
        ++Bitmap;
	
        Bitmap->Head = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_head.bmp");
	Bitmap->Cape = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_cape.bmp");
	Bitmap->Torso = DEBUGLoadBMP(Thread, Memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_torso.bmp");
	Bitmap->AlignX = 72;
	Bitmap->AlignY = 182;
	
	GameState->PlayerP.AbsoluteTileX = 1;    
	GameState->PlayerP.AbsoluteTileY = 3;
	GameState->PlayerP.OffsetX = 5.0f;
	GameState->PlayerP.OffsetY = 5.0f;
	InitializeArena(&GameState->WorldArena, Memory->PermanentStorageSize - sizeof(game_state),
			(uint8 *) Memory->PermanentStorage + sizeof(game_state));

	GameState->World = PushStruct(&GameState->WorldArena, world);
	world *World = GameState->World;
	World->TileMap = PushStruct(&GameState->WorldArena, tile_map);

	tile_map *TileMap = World->TileMap;
	
	// NOTE(Quincy): This is set to using 256x256 tile chunks.
	TileMap->ChunkShift = 4;
	TileMap->ChunkMask = (1 << TileMap->ChunkShift) - 1;
	TileMap->ChunkDimensions = (1 << TileMap->ChunkShift);;

	TileMap->TileChunkCountX = 128;
	TileMap->TileChunkCountY = 128;
	TileMap->TileChunkCountZ = 2;
	
	TileMap->TileChunks = PushArray(&GameState->WorldArena,
					TileMap->TileChunkCountX*
					TileMap->TileChunkCountY*
					TileMap->TileChunkCountZ,
					tile_chunk);

	TileMap->TileSideInMeters = 1.4f;
        
	uint32 RandomNumberIndex = 0;
	
	uint32 TilesPerWidth = 17;
	uint32 TilesPerHeight = 9;

	uint32 ScreenX = 0;
	uint32 ScreenY = 0;
	
	uint32 AbsoluteTileZ = 0;
	
	bool32 DoorLeft = false;
	bool32 DoorRight = false;
	bool32 DoorTop = false;
	bool32 DoorBottom = false;
	bool32 DoorUp = false;
	bool32 DoorDown = false;

	for(uint32 ScreenIndex = 0; ScreenIndex < 100; ++ScreenIndex)
	{
	    // TODO(Quincy): Random number generator!
	    Assert(RandomNumberIndex < ArrayCount(RandomNumberTable));
	    uint32 RandomChoice;
	    if(DoorUp || DoorDown)
	    {
		RandomChoice = RandomNumberTable[RandomNumberIndex++] % 2;
	    }
	    else
	    {
		RandomChoice = RandomNumberTable[RandomNumberIndex++] % 3;
	    }

	    bool32 CreatedZDoor = false;
	    if(RandomChoice == 2)
	    {
		CreatedZDoor = true;
		if(AbsoluteTileZ == 0)
		{
		    DoorUp = true;
		}
		else
		{
		    DoorDown = true;
		}
	    }
	    else if(RandomChoice == 1)
	    {
		DoorRight = true;
	    }
	    else
	    {
		DoorTop = true;
	    }
	    
	    for(uint32 TileY = 0; TileY < TilesPerHeight; ++TileY)
	    {
		for(uint32 TileX = 0; TileX < TilesPerWidth; ++TileX)
		{
		    uint32 AbsoluteTileX = ScreenX*TilesPerWidth + TileX;
		    uint32 AbsoluteTileY = ScreenY*TilesPerHeight + TileY;
		    

		    uint32 TileValue = 1;
		    if((TileX == 0) && (!DoorLeft || (TileY != (TilesPerHeight/2))))
		    {
			TileValue = 2;
		    }
		    
		    if((TileX == (TilesPerWidth - 1)) && (!DoorRight || (TileY != (TilesPerHeight/2))))
		    {
			TileValue = 2;
		    }

		    if((TileY == 0) && (!DoorBottom || (TileX != (TilesPerWidth/2))))
		    {
			TileValue = 2;
		    }
		    
		    if((TileY == (TilesPerHeight - 1)) && (!DoorTop || (TileX != (TilesPerWidth/2))))
		    {
			TileValue = 2;
		    }

		    if((TileX == 10) && (TileY == 6))
		    {
			if(DoorUp)
			{
			    TileValue = 3;
			}
			else if(DoorDown)
			{
			    TileValue = 4;
			}
		    }
			
		    SetTileValue(&GameState->WorldArena, World->TileMap,
				 AbsoluteTileX, AbsoluteTileY, AbsoluteTileZ,
				 TileValue);
		}
	    }
	    
	    DoorLeft = DoorRight;
	    DoorBottom = DoorTop;

	    if(CreatedZDoor)
	    {
		DoorDown = !DoorDown;
		DoorUp = !DoorUp;
	    }
	    else
	    {
		DoorDown = false;
		DoorUp = false;
	    }
	    
	    DoorRight = false;
	    DoorTop = false;

	    if(RandomChoice == 2)
	    {
		if(AbsoluteTileZ == 0)
		{
		    AbsoluteTileZ = 1;
		}
		else
		{
		    AbsoluteTileZ = 0;
		}
	    }
	    else if(RandomChoice == 1)
	    {
		ScreenX += 1;
	    }
	    else
	    {
		ScreenY += 1;
	    }
	}
	
	Memory->IsInitialized = true;
    }

    world *World = GameState->World;
    tile_map *TileMap = World->TileMap;

    int32 TileSideInPixels = 60;
    real32 MetersToPixels = TileSideInPixels / TileMap->TileSideInMeters;
    real32 LowerLeftX = -(real32)TileSideInPixels/2;
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
		GameState->HeroFacingDirection = 1;
		dPlayerY = 1.0f;
	    }
	    if(Controller->MoveDown.EndedDown)
	    {
		GameState->HeroFacingDirection = 3;
		dPlayerY = -1.0f;
	    }
	    if(Controller->MoveLeft.EndedDown)
	    {
		GameState->HeroFacingDirection = 2;
		dPlayerX = -1.0f;
	    }
	    if(Controller->MoveRight.EndedDown)
	    {
		GameState->HeroFacingDirection = 0;
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
	    tile_map_position NewPlayerP = GameState->PlayerP;
	    NewPlayerP.OffsetX += Input->dtForFrame*dPlayerX;
	    NewPlayerP.OffsetY += Input->dtForFrame*dPlayerY;
	    NewPlayerP = RecanonicalizePosition(TileMap, NewPlayerP);
	    // TODO(Quincy): Delta function that auto-recanonicalize

	    tile_map_position PlayerLeft = NewPlayerP;
	    PlayerLeft.OffsetX -= 0.5f*PlayerWidth;
	    PlayerLeft = RecanonicalizePosition(TileMap, PlayerLeft);
	    
	    tile_map_position PlayerRight = NewPlayerP;
	    PlayerRight.OffsetX += 0.5f*PlayerWidth;
	    PlayerRight = RecanonicalizePosition(TileMap, PlayerRight);

	    if(IsTileMapPointEmpty(TileMap, NewPlayerP) &&
	       IsTileMapPointEmpty(TileMap, PlayerLeft) &&
	       IsTileMapPointEmpty(TileMap, PlayerRight))
	    {
		if(!AreOnSameTile(&GameState->PlayerP, &NewPlayerP))
		{
		    uint32 NewTileValue = GetTileValue(TileMap, NewPlayerP);
		    
		    if(NewTileValue == 3)
		    {
			++NewPlayerP.AbsoluteTileZ;
		    }
		    else if(NewTileValue == 4)
		    {
			--NewPlayerP.AbsoluteTileZ;
		    }
		}
		
		GameState->PlayerP = NewPlayerP;
	    }
	}
    }

    DrawBitmap(Buffer, &GameState->Backdrop, 0.0f, 0.0f);

    real32 ScreenCenterX = 0.5f*(real32)Buffer->Width;
    real32 ScreenCenterY = 0.5f*(real32)Buffer->Height;
    
    for(int32 RelRow = -10; RelRow < 10; ++RelRow)
    {
	for(int32 RelColumn = -20; RelColumn < 20; ++RelColumn)
	{
	    uint32 Column = RelColumn + GameState->PlayerP.AbsoluteTileX;
	    uint32 Row = RelRow + GameState->PlayerP.AbsoluteTileY;
	    
	    uint32 TileID = GetTileValue(TileMap, Column, Row, GameState->PlayerP.AbsoluteTileZ);

	    if(TileID > 1)
	    {
		real32 Gray = 0.5f;
		if(TileID == 2)
		{
		    Gray = 1.0f;
		}
		
		if(TileID > 2)
		{
		    Gray = 0.25f;
		}

		if((Column == GameState->PlayerP.AbsoluteTileX) && (Row == GameState->PlayerP.AbsoluteTileY))
		{
		    Gray = 0.0f;
		}
	    
		real32 CenterX = ScreenCenterX - MetersToPixels*GameState->PlayerP.OffsetX + ((real32)RelColumn)*TileSideInPixels;
		real32 CenterY = ScreenCenterY + MetersToPixels*GameState->PlayerP.OffsetY - ((real32)RelRow)*TileSideInPixels;
	    
		real32 MinX = CenterX - 0.5f*TileSideInPixels;
		real32 MinY = CenterY - 0.5f*TileSideInPixels;
		real32 MaxX = CenterX + 0.5f*TileSideInPixels;
		real32 MaxY = CenterY + 0.5f*TileSideInPixels;
		DrawRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
	    }
	}
    }

    real32 PlayerR = 1.0f;
    real32 PlayerG = 1.0f;
    real32 PlayerB = 0.0f;
   
    real32 PlayerLeftPos = ScreenCenterX - (0.5f*MetersToPixels*PlayerWidth);
    real32 PlayerTopPos = ScreenCenterY - MetersToPixels*PlayerHeight;
    
    DrawRectangle(Buffer,
		  PlayerLeftPos, PlayerTopPos,
		  PlayerLeftPos + MetersToPixels*PlayerWidth,
		  PlayerTopPos + MetersToPixels*PlayerHeight,
		  PlayerR, PlayerG, PlayerB);

    hero_bitmaps *HeroBitmaps = &GameState->HeroBitmaps[GameState->HeroFacingDirection];
    
    DrawBitmap(Buffer, &HeroBitmaps->Torso, PlayerLeftPos, PlayerTopPos, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
    DrawBitmap(Buffer, &HeroBitmaps->Cape, PlayerLeftPos, PlayerTopPos, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
    DrawBitmap(Buffer, &HeroBitmaps->Head, PlayerLeftPos, PlayerTopPos, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
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
