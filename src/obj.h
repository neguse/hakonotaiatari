
#pragma once

enum VERTEX_TYPE {
	VERTEX_TYPE_NONE,
	VERTEX_TYPE_XY,
	VERTEX_TYPE_XYZ,
	VERTEX_TYPE_MAX,
};

static const u32 VERTEX_SIZE[VERTEX_TYPE_MAX] = {
	0,									// VERTEX_TYPE_NONE
	sizeof(float) * 2,	// VERTEX_TYPE_XY
	sizeof(float) * 3,	// VERTEX_TYPE_XYZ
};

enum INDEX_TYPE {
	INDEX_TYPE_NONE,
	INDEX_TYPE_LINES,
	INDEX_TYPE_LINE_STRIP,
};

struct OBJ_FILE {

public:
	u32 size;
	u32 vertex_type;
	u32 vertex_count;
	u32 vertex_begin;
	u32 index_type;
	u32 index_count;
	u32 index_begin;

public:

	const float* vertexAddr() const
	{
		return reinterpret_cast<const float*>(reinterpret_cast<const glm::u8*>(this) + vertex_begin);
	}

	const u32* indexAddr() const
	{
		return reinterpret_cast<const u32*>(reinterpret_cast<const glm::u8*>(this) + index_begin);
	}

}; // struct OBJ_FILE
