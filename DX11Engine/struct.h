#pragma once

#pragma pack(1)

// ����(Vertex) : 3���� ������ ��ǥ�� ��Ÿ���� ����
struct Vtx
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;
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