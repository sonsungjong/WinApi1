#pragma once

#pragma pack(1)

// ����(Vertex) : 3���� ������ ��ǥ�� ��Ÿ���� ����
struct Vtx
{
	Vec3 vPos;
	Vec4 vColor;
};

// ������� ���� ����ü
struct tTransform
{
	Vec4 Position;			// x,y,z�� �����ű��ѵ� GPU�� 16����Ʈ ����̶� Vec4�� ����
};