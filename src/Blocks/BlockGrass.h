
#pragma once

#include "BlockHandler.h"
#include "../FastRandom.h"
#include "../Root.h"
#include "../Bindings/PluginManager.h"





class cBlockGrassHandler :
	public cBlockHandler
{
	using super = cBlockHandler;

public:

	cBlockGrassHandler(BLOCKTYPE a_BlockType):
		super(a_BlockType)
	{
	}





	virtual cItems ConvertToPickups(NIBBLETYPE a_BlockMeta, cBlockEntity * a_BlockEntity, const cEntity * a_Digger, const cItem * a_Tool) override
	{
		if (!ToolHasSilkTouch(a_Tool))
		{
			return cItem(E_BLOCK_DIRT, 1, 0);
		}
		return cItem(E_BLOCK_GRASS, 1, 0);
	}





	virtual void OnUpdate(
		cChunkInterface & a_ChunkInterface,
		cWorldInterface & a_WorldInterface,
		cBlockPluginInterface & a_PluginInterface,
		cChunk & a_Chunk,
		const Vector3i a_RelPos
	) override
	{
		if (!a_Chunk.GetWorld()->IsChunkLighted(a_Chunk.GetPosX(), a_Chunk.GetPosZ()))
		{
			a_Chunk.GetWorld()->QueueLightChunk(a_Chunk.GetPosX(), a_Chunk.GetPosZ());
			return;
		}
		if (cChunkDef::IsValidHeight(a_RelPos.y + 1))
		{
			// Grass turns back to dirt when the block above it is not transparent or water.
			// It does not turn to dirt when a snow layer is above.
			auto above = a_Chunk.GetBlock(a_RelPos.addedY(1));
			if (
				(above != E_BLOCK_SNOW) &&
				(!cBlockInfo::IsTransparent(above) || IsBlockWater(above)))
			{
				a_Chunk.FastSetBlock(a_RelPos, E_BLOCK_DIRT, E_META_DIRT_NORMAL);
				return;
			}

			// Make sure that there is enough light at the source block to spread
			auto light = std::max(a_Chunk.GetBlockLight(a_RelPos.addedY(1)), a_Chunk.GetTimeAlteredLight(a_Chunk.GetSkyLight(a_RelPos.addedY(1))));
			if (light < 9)
			{
				return;
			}
		}

		// Grass spreads to adjacent dirt blocks:
		auto & rand = GetRandomProvider();
		for (int i = 0; i < 2; i++)  // Pick two blocks to grow to
		{
			int OfsX = rand.RandInt(-1, 1);
			int OfsY = rand.RandInt(-3, 1);
			int OfsZ = rand.RandInt(-1, 1);

			BLOCKTYPE  DestBlock;
			NIBBLETYPE DestMeta;
			if (!cChunkDef::IsValidHeight(a_RelPos.y + OfsY))
			{
				// Y Coord out of range
				continue;
			}
			auto pos = a_RelPos + Vector3i(OfsX, OfsY, OfsZ);
			auto chunk = a_Chunk.GetRelNeighborChunkAdjustCoords(pos);
			if (chunk == nullptr)
			{
				// Unloaded chunk
				continue;
			}
			chunk->GetBlockTypeMeta(pos, DestBlock, DestMeta);
			if ((DestBlock != E_BLOCK_DIRT) || (DestMeta != E_META_DIRT_NORMAL))
			{
				// Not a regular dirt block
				continue;
			}
			auto abovePos = pos.addedY(1);
			BLOCKTYPE above = chunk->GetBlock(abovePos);
			NIBBLETYPE light = std::max(chunk->GetBlockLight(abovePos), chunk->GetTimeAlteredLight(chunk->GetSkyLight(abovePos)));
			if ((light > 4)  &&
				cBlockInfo::IsTransparent(above) &&
				(!IsBlockLava(above)) &&
				(!IsBlockWaterOrIce(above))
			)
			{
				auto absPos = chunk->RelativeToAbsolute(pos);
				if (!cRoot::Get()->GetPluginManager()->CallHookBlockSpread(*chunk->GetWorld(), absPos.x, absPos.y, absPos.z, ssGrassSpread))
				{
					chunk->FastSetBlock(pos, E_BLOCK_GRASS, 0);
				}
			}
		}  // for i - repeat twice
	}





	virtual ColourID GetMapBaseColourID(NIBBLETYPE a_Meta) override
	{
		UNUSED(a_Meta);
		return 1;
	}
} ;




