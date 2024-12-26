#pragma once
class CEntity
{
private:
	static ULONGLONG g_NextID;

private:
	std::wstring m_name;
	ULONGLONG m_id;

public:
	const std::wstring& getName() { return m_name; }
	void setName(const std::wstring& _name) { m_name = _name; }
	ULONGLONG getID() { return m_id; }

public:
	CEntity();
	CEntity(const CEntity& _origin);			// 복사생성자 정의(아이디 복제 방지)
	~CEntity();
};

