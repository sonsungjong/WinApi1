#pragma once

#define SINGLE(Type)		private:\
												Type();\
												Type(const Type& _origin) = delete;\
												public:\
														~Type();\
													friend class CSingleton<Type>;

#define DEVICE CDevice::getInstance()->GetDivice()
#define CONTEXT CDevice::getInstance()->GetContext()

typedef SimpleMath::Vector2 Vec2;
typedef SimpleMath::Vector3 Vec3;
typedef SimpleMath::Vector4 Vec4;
