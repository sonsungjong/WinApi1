#pragma once

template<typename T>
class Ptr
{
private:
	T* m_asset;

public:
	T* Get() const { return m_asset; }
	T** GetAddressOf() const { return &m_asset; }

public:
	// 비교 연산자 구현
	bool operator== (T* _asset) const
	{
		if (m_asset == _asset) {
			return true;
		}
		return false;
	}

	bool operator!= (T* _asset) const
	{
		if (m_asset != _asset) {
			return true;
		}
		return false;
	}

	bool operator== (const Ptr<T>& _asset) const
	{
		if (m_asset == _asset.m_asset) {
			return true;
		}
		return false;
	}

	bool operator!= (const Ptr<T>& _asset) const
	{
		if (m_asset != _asset.m_asset) {
			return true;
		}
		return false;
	}

	// = 연산자 구현 (대입연산자)
	void operator= (T* _asset)
	{
		if (nullptr != m_asset) {
			m_asset->release();
		}

		m_asset = _asset;

		if (nullptr != m_asset) {
			m_asset->addRef();
		}
	}

	void operator= (const Ptr<T>& _other)
	{
		if (nullptr != m_asset) {
			m_asset->release();
		}

		m_asset = _other.m_asset;

		if (nullptr != m_asset) {
			m_asset->addRef();
		}
	}


	// -> 연산자 구현
	T* operator->()
	{
		return m_asset;
	}

public:
	Ptr()
		: m_asset(nullptr)
	{

	}

	Ptr(T* _asset)
		: m_asset(_asset)
	{
		if (nullptr != m_asset) {
			m_asset->addRef();
		}
	}

	Ptr(const Ptr<T>& _other)
		: m_asset(_other.m_asset)
	{
		if (nullptr != m_asset) {
			m_asset->addRef();
		}
	}

	~Ptr()
	{
		if (nullptr != m_asset) {
			m_asset->release();
		}
	}
};

// 전역함수
template<typename T>
bool operator== (void* _asset, const Ptr<T>& _ptr)
{
	if (_asset == _ptr.Get()) {
		return true;
	}
	return false;
}

template<typename T>
bool operator!= (void* _asset, const Ptr<T>& _ptr)
{
	if (_asset != _ptr.Get()) {
		return true;
	}
	return false;
}
