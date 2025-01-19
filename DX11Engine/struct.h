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
};

//struct tTransform
//{
//	Vec4 Position;			// x,y,z�� �����ű��ѵ� GPU�� 16����Ʈ ����̶� Vec4�� ����
//	Vec4 Scale;
//};