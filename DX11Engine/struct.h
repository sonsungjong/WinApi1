#pragma once

//#pragma pack(1)

// 정점(Vertex) : 3차원 공간의 좌표를 나타내는 단위
struct Vtx
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;
};

// 디버그 렌더 요청 정보
struct tDebugShapeInfo
{
	DEBUG_SHAPE Shape;				// 도형 종류
	Vec3 Position;							// 위치값
	Vec3 Scale;								// 크기
	Vec3 Rotation;							// 회전
	Matrix matWorld;						// 매트릭스로 정리
	Vec4 Color;								// 색상
	float Duration;							// 유지 시간
	float Age;									// 현재 시간 (도형이 화면에 존재한 시간)
};

// 상수버퍼 관련 구조체
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};

extern tTransform g_Trans;				// 이러한 자료형과 변수가 어딘가에 있을 것이다라는 선언 (extern) -> 모든 파일이 접근 가능한 유일한 전역변수

// 재질이 전달할 수 있는 상수값들
struct tMtrlConst
{
	// 상수버퍼이기 때문에 고정된 배열로 만들 것 (4096 바이트 이하, 16 배수)
	int iArr[4];
	float fArr[4];
	Vec2 v2Arr[4];
	Vec4 v4Arr[4];
	Matrix matArr[4];
};