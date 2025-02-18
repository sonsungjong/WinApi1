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

	SCRIPT,					// �⺻ ������Ʈ�� �ƴ�
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
	CULL_BACK,						// �ƹ��͵� �������� ���� �� �⺻��
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

extern Vec3 XAxis;				// ���������� �̸� extern���� ���� �س��´�.
extern Vec3 YAxis;				// ���������� �̸� extern���� ���� �س��´�.
extern Vec3 ZAxis;				// ���������� �̸� extern���� ���� �س��´�. (���� ���� �κ��� �ٸ� ���� �� �־����)

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

// ���� ����
enum class PROJ_TYPE
{
	ORTHOGRAPHIC,							// ���� ���� (�Ÿ��� �޶� ũ�Ⱑ ������... 2D)
	PERSPECTIVE,								// ���� ���� (�Ÿ��� ���� ũ�Ⱑ �޶���)
};