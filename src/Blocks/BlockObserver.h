
#pragma once

#include "BlockHandler.h"
#include "Mixins.h"


class cBlockObserverHandler:
	public cClearMetaOnDrop<cPitchYawRotator<cBlockHandler>>
{
	using super = cClearMetaOnDrop<cPitchYawRotator<cBlockHandler>>;

public:

	cBlockObserverHandler(BLOCKTYPE a_BlockType) : super(a_BlockType)
	{
	}

	inline static Vector3i GetObservingFaceOffset(NIBBLETYPE a_Meta)
	{
		return -GetSignalOutputOffset(a_Meta);
	}

	inline static Vector3i GetSignalOutputOffset(NIBBLETYPE a_Meta)
	{
		switch (a_Meta & 0x7)
		{
			case 0x0: return { 0, 1, 0 };
			case 0x1: return { 0, -1, 0 };
			case 0x2: return { 0, 0, 1 };
			case 0x3: return { 0, 0, -1 };
			case 0x4: return { 1, 0, 0 };
			case 0x5: return { -1, 0, 0 };
			default:
			{
				LOGWARNING("%s: Unknown metadata: %d", __FUNCTION__, a_Meta);
				ASSERT(!"Unknown metadata while determining orientation of observer!");
				return { 0, 0, 0 };
			}
		}
	}
};
