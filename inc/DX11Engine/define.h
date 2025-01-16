#pragma once

#define SINGLE(Type)		private:\
												Type();\
												Type(const Type& _origin) = delete;\
												public:\
														~Type();\
													friend class CSingleton<Type>;

#define DEVICE CDevice::getInstance()->GetDivice()
#define CONTEXT CDevice::getInstance()->GetContext()

#define KEY_CHECK(Key, State) (CKeyMgr::getInstance()->getKeyState(Key) == State)
#define KEY_TAP(Key)				KEY_CHECK(Key, KEY_STATE::TAP)
#define KEY_PRESSED(Key)		KEY_CHECK(Key, KEY_STATE::PRESSED)
#define KEY_RELEASE(Key)		KEY_CHECK(Key, KEY_STATE::RELEASE)
#define KEY_NONE(Key)				KEY_CHECK(Key, KEY_STATE::NONE)

#define DT					CTimeMgr::getInstance()->getDeltaTime()

typedef SimpleMath::Vector2 Vec2;
typedef SimpleMath::Vector3 Vec3;
typedef SimpleMath::Vector4 Vec4;
