#pragma once

#include "palleon/Effect.h"
#include "palleon/VertexBuffer.h"

class CUmbralEffect
{
public:
	enum VERTEX_ITEM_ID
	{
		VERTEX_ITEM_ID_UV2 = Palleon::VERTEX_ITEM_ID_USER_START,
		VERTEX_ITEM_ID_UV3,
		VERTEX_ITEM_ID_TANGENT,
		VERTEX_ITEM_ID_PLACEHOLDER
	};
};
