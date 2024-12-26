#pragma once

#pragma pack(1)

// 정점(Vertex) : 3차원 공간의 좌표를 나타내는 단위
struct Vtx
{
	Vec3 vPos;
	Vec4 vColor;
};

// 상수버퍼 관련 구조체
struct tTransform
{
	Vec4 Position;			// x,y,z만 담을거긴한데 GPU가 16바이트 기반이라 Vec4로 선언
};