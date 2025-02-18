#include "pch.h"
#include "func.h"
#include "CDbgRenderMgr.h"

void DrawDebugRect(Vec3 _WorldPos, Vec3 _WorldScale, Vec3 _WorldRotation, Vec4 _vColor, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;

	info.Position = _WorldPos;
	info.Scale = _WorldScale;
	info.Rotation = _WorldRotation;
	info.matWorld = XMMatrixIdentity();

	info.Color = _vColor;

	info.Duration = _Duration;
	info.Age = 0.f;

	CDbgRenderMgr::getInstance()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(Vec3 _matWorld, float _radius, Vec4 _vColor, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CIRCLE;
	info.Position = _matWorld;
	info.Scale = Vec3(_radius * 2, _radius * 2, 1.f);				// 기본 반지름이 0.5f 인 상태여서 *2
	info.matWorld = XMMatrixIdentity();
	info.Color = _vColor;
	info.Duration = _Duration;
	info.Age = 0.f;

	CDbgRenderMgr::getInstance()->AddDebugShapeInfo(info);
}

void DrawDebugLine()
{
}

void DrawDebugRect(Matrix _matWorld, Vec4 _vColor, float _Duration)
{
	tDebugShapeInfo info = {};

	info.Shape = DEBUG_SHAPE::RECT;
	info.matWorld = _matWorld;
	info.Color = _vColor;
	info.Duration = _Duration;
	info.Age = 0.f;

	CDbgRenderMgr::getInstance()->AddDebugShapeInfo(info);
}
