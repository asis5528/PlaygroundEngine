#pragma once
namespace PGEResourceTypes
{
	struct Texture {
		__int64 id;
		int width;
		int height;
		int depth;
	};
	struct Texture3D :Texture {

	};
}

