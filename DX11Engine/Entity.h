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

	//virtual CEntity* clone() = 0;						// ���� ���� �Լ� (�ڱ� �ڽ��� ������ ��ü�� ��ȯ�ϴ� ����)

public:
	CEntity();
	CEntity(const CEntity& _origin);			// ��������� ����(���̵� ���� ����)
	~CEntity();
};

