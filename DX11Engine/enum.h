#pragma once

enum class COMPONENT_TYPE
{
	TRANSFORM,
	CAMERA,
	COLLIDER2D,
	COLLIDER3D,
	ANIMATOR2D,
	ANIMATOR3D,
	LIGHT2D,
	LIGHT3D,

	MESHRENDER,
	DECAL,
	PARTICLESYSTEM,
	TILEMAP,
	LANDSCAPE,

	END,

	SCRIPT,					// 기본 컴포넌트가 아님
};

enum class ASSET_TYPE
{
	PREFAB,
	MESH,
	MESH_DATA,
	MATERIAL,
	TEXTURE,
	SOUND,
	GRAPHICS_SHADER,
	COMPUTE_SHADER,

	END
};

enum class CB_TYPE
{
	TRANSFORM,

	MATERIAL,

	ANIMATION,

	GLOBAL,

	END
};

// Rasterizer State
enum class RS_TYPE
{
	CULL_BACK,						// 아무것도 설정하지 않을 때 기본값
	CULL_FRONT,
	CULL_NONE,
	WIRE_FRAME,

	END
};

// Direction Type
enum class DIR_TYPE
{
	RIGHT,
	UP,
	FRONT,

	END
};

extern Vec3 XAxis;				// 전역변수를 미리 extern으로 선언만 해놓는다.
extern Vec3 YAxis;				// 전역변수를 미리 extern으로 선언만 해놓는다.
extern Vec3 ZAxis;				// 전역변수를 미리 extern으로 선언만 해놓는다. (실제 구현 부분은 다른 곳에 꼭 있어야함)

enum class SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
	MAT_2,
	MAT_3,

	END
};

enum class TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEX_CUBE_0,
	TEX_CUBE_1,
	TEX_CUBE_2,
	TEX_CUBE_4,

	TEX_ARR_0,
	TEX_ARR_1,
	TEX_ARR_2,
	TEX_ARR_3,

	END
};

enum class DEBUG_SHAPE
{
	RECT,
	CIRCLE,
	LINE,

	CUBE,
	SPHERE,

	END
};

// 투영 종류
enum class PROJ_TYPE
{
	ORTHOGRAPHIC,							// 직교 투영 (거리가 달라도 크기가 동일함... 2D)
	PERSPECTIVE,								// 원근 투영 (거리에 따라 크기가 달라짐)
};