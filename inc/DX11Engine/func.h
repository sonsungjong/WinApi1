#pragma once

// 배열 안에 들어있는 각 동적할당 메모리들을 배열 주소를 전달받아 해제
template<typename T, int _size>
void SafeDelArray(T* (&Array)[_size])
{
	for (int i = 0; i < _size; ++i) 
	{
		if (nullptr != Array[i])
		{
			delete Array[i];
			Array[i] = nullptr;
		}
	}
}
