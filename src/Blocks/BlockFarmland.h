
// BlockFarmland.h

// Declares the cBlcokFarmlandHandler representing the block handler for farmland





#pragma once

#include "BlockHandler.h"
#include "../BlockArea.h"





class cBlockFarmlandHandler :
	public cBlockHandler
{
	using super = cBlockHandler;

public:

	cBlockFarmlandHandler(BLOCKTYPE a_BlockType):
		super(a_BlockType)
	{
	}





	virtual cItems ConvertToPickups(NIBBLETYPE a_BlockMeta, cBlockEntity * a_BlockEntity, const cEntity * a_Digger, const cItem * a_Tool) override
	{
		return cItem(E_BLOCK_DIRT, 1, 0);
	}





	virtual void OnUpdate(cChunkInterface & cChunkInterface, cWorldInterface & a_WorldInterface, cBlockPluginInterface & a_PluginInterface, cChunk & a_Chunk, int a_RelX, int a_RelY, int a_RelZ) override
	{
		NIBBLETYPE BlockMeta = a_Chunk.GetMeta(a_RelX, a_RelY, a_RelZ);

		if (IsWaterInNear(a_Chunk, a_RelX, a_RelY, a_RelZ))
		{
			// Water was found, set block meta to 7
			a_Chunk.FastSetBlock(a_RelX, a_RelY, a_RelZ, m_BlockType, 7);
			return;
		}

		// Water wasn't found, de-hydrate block:
		if (BlockMeta > 0)
		{
			a_Chunk.FastSetBlock(a_RelX, a_RelY, a_RelZ, E_BLOCK_FARMLAND, --BlockMeta);
			return;
		}

		// Farmland too dry. If nothing is growing on top, turn back to dirt:
		BLOCKTYPE UpperBlock = (a_RelY >= cChunkDef::Height - 1) ? static_cast<BLOCKTYPE>(E_BLOCK_AIR) : a_Chunk.GetBlock(a_RelX, a_RelY + 1, a_RelZ);
		switch (UpperBlock)
		{
			case E_BLOCK_BEETROOTS:
			case E_BLOCK_CROPS:
			case E_BLOCK_POTATOES:
			case E_BLOCK_CARROTS:
			case E_BLOCK_MELON_STEM:
			case E_BLOCK_PUMPKIN_STEM:
			{
				// Produce on top, don't revert
				break;
			}
			default:
			{
				a_Chunk.SetBlock({a_RelX, a_RelY, a_RelZ}, E_BLOCK_DIRT, 0);
				break;
			}
		}
	}





	virtual void OnNeighborChanged(cChunkInterface & a_ChunkInterface, Vector3i a_BlockPos, eBlockFace a_WhichNeighbor) override
	{
		// Don't care about any neighbor but the one above us (fix recursion loop in #2213):
		if (a_WhichNeighbor != BLOCK_FACE_YP)
		{
			return;
		}

		// Don't care about anything if we're at the top of the world:
		if (a_BlockPos.y >= cChunkDef::Height)
		{
			return;
		}

		// Check whether we should revert to dirt:
		auto upperBlock = a_ChunkInterface.GetBlock(a_BlockPos.addedY(1));
		if (cBlockInfo::FullyOccupiesVoxel(upperBlock))
		{
			a_ChunkInterface.SetBlock(a_BlockPos, E_BLOCK_DIRT, 0);
		}
	}





	bool IsWaterInNear(cChunk & a_Chunk, int a_RelX, int a_RelY, int a_RelZ)
	{
		if (a_Chunk.GetWorld()->IsWeatherWetAt(a_RelX, a_RelZ))
		{
			// Rain hydrates farmland, too, except in Desert biomes.
			return true;
		}

		// Search for water in a close proximity:
		// Ref.: https://minecraft.gamepedia.com/Farmland#Hydration
		// TODO: Rewrite this to use the chunk and its neighbors directly
		cBlockArea Area;
		int BlockX = a_RelX + a_Chunk.GetPosX() * cChunkDef::Width;
		int BlockZ = a_RelZ + a_Chunk.GetPosZ() * cChunkDef::Width;
		if (!Area.Read(*a_Chunk.GetWorld(), BlockX - 4, BlockX + 4, a_RelY, a_RelY + 1, BlockZ - 4, BlockZ + 4))
		{
			// Too close to the world edge, cannot check surroundings
			return false;
		}

		size_t NumBlocks = Area.GetBlockCount();
		BLOCKTYPE * BlockTypes = Area.GetBlockTypes();
		for (size_t i = 0; i < NumBlocks; i++)
		{
			if (IsBlockWater(BlockTypes[i]))
			{
				return true;
			}
		}  // for i - BlockTypes[]

		return false;
	}

	virtual bool CanSustainPlant(BLOCKTYPE a_Plant) override
	{
		return (
			(a_Plant == E_BLOCK_BEETROOTS) ||
			(a_Plant == E_BLOCK_CROPS) ||
			(a_Plant == E_BLOCK_CARROTS) ||
			(a_Plant == E_BLOCK_POTATOES) ||
			(a_Plant == E_BLOCK_MELON_STEM) ||
			(a_Plant == E_BLOCK_PUMPKIN_STEM)
		);
	}
} ;
