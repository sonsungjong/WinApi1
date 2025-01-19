#pragma once

#pragma pack(1)

// 정점(Vertex) : 3차원 공간의 좌표를 나타내는 단위
struct Vtx
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;
};

// 상수버퍼 관련 구조체
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};

extern tTransform g_Trans;				// 이러한 자료형과 변수가 어딘가에 있을 것이다라는 선언 (extern) -> 모든 파일이 접근 가능한 유일한 전역변수