#pragma once

template<typename T>
class Ptr
{
private:
	T* m_asset;

public:
	Ptr()
		: m_asset(nullptr)
	{

	}

	// = ������ ���� (���Կ�����)
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


	// -> ������ ����
	T* operator->()
	{
		return m_asset;
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