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