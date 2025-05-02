#pragma once

#include "resource.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <gdiplus.h>

typedef struct TreeNode
{
    std::wstring     text;           // ǥ���� �ؽ�Ʈ
    int              stateIndex;     // STATE �̹��� �ε��� (0=����,1=���� ȭ��ǥ,2=���� ȭ��ǥ,3=������)
    int              iconIndex;      // NORMAL �̹��� �ε��� (�� ����)
    std::vector<TreeNode*> children; // �ڽ� ����

    // ������
    TreeNode(const std::wstring& t, int s = 0, int i = 0) 
        : text(t), stateIndex(s), iconIndex(i) 
    {}
} ST_TreeNode;

class CTreeData
{
public:
    std::vector<std::unique_ptr<ST_TreeNode>> roots;

    // ��Ʈ ����
    ST_TreeNode* AddRoot(const std::wstring& txt, int state = 0, int icon = 0);
    // �ڽ� �߰�
    ST_TreeNode* AddChild(ST_TreeNode* parent, const std::wstring& txt, int state = 0, int icon = 0);
    // UI ������Ʈ
    void PopulateTreeView(HWND hTree);
    
private:
    std::vector<std::unique_ptr<ST_TreeNode>> allNodes;

    // ������ �׸���
    void InsertRecursive(HWND hTree, HTREEITEM hParent, ST_TreeNode* node);
};

HBITMAP createCircleBitmap(int circle_size, COLORREF fillColor, COLORREF maskColor);
HBITMAP CreateGdiPlusCircleBitmap(int circle_size, COLORREF fillColor);
void UpdateAllNodesByCycling(HWND hTree);