#pragma once

// �迭 �ȿ� ����ִ� �� �����Ҵ� �޸𸮵��� �迭 �ּҸ� ���޹޾� ����
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

template<typename T>
void SafeDelVector(std::vector<T*>& _vec)
{
	for (size_t i = 0U; i < _vec.size(); ++i) {
		if (nullptr != _vec[i]) {
			delete _vec[i];
		}
	}

	_vec.clear();
}