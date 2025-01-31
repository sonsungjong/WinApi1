#pragma once

//#pragma pack(1)

// ����(Vertex) : 3���� ������ ��ǥ�� ��Ÿ���� ����
struct Vtx
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;
};

// ����� ���� ��û ����
struct tDebugShapeInfo
{
	DEBUG_SHAPE Shape;				// ���� ����
	Vec3 Position;							// ��ġ��
	Vec3 Scale;								// ũ��
	Vec3 Rotation;							// ȸ��
	Matrix matWorld;						// ��Ʈ������ ����
	Vec4 Color;								// ����
	float Duration;							// ���� �ð�
	float Age;									// ���� �ð� (������ ȭ�鿡 ������ �ð�)
};

// ������� ���� ����ü
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};

extern tTransform g_Trans;				// �̷��� �ڷ����� ������ ��򰡿� ���� ���̴ٶ�� ���� (extern) -> ��� ������ ���� ������ ������ ��������

// ������ ������ �� �ִ� �������
struct tMtrlConst
{
	// ��������̱� ������ ������ �迭�� ���� �� (4096 ����Ʈ ����, 16 ���)
	int iArr[4];
	float fArr[4];
	Vec2 v2Arr[4];
	Vec4 v4Arr[4];
	Matrix matArr[4];
};