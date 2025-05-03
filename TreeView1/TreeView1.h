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

enum class TREE_ICON_COLOR_INDEX
{
    GREY = 0,
    RED = 1,
    YELLOW = 2,
    GREEN = 3
};

enum class TREE_ARROW_INDEX
{
    NO_USE = 0,
    NONE = 1,
    CLOSE = 2,
    OPEN = 3
};

typedef struct TreeNode
{
    std::wstring text;           // ǥ���� �ؽ�Ʈ
    int stateIndex;     // STATE �̹��� �ε��� (0=����,1=����,2=���� ȭ��ǥ,3=���� ȭ��ǥ)
    int iconColorIndex;      // NORMAL �̹��� �ε��� (�� ����)
    std::vector<TreeNode*> children; // �ڽ� ����

    // ������
    TreeNode(const std::wstring& t, int s = 0, int i = 0) 
        : text(t), stateIndex(s), iconColorIndex(i)
    {}
} ST_TreeNode;

class CTreeData
{
public:

    std::vector<std::unique_ptr<ST_TreeNode>> roots;

    // ��Ʈ ����
    ST_TreeNode* AddRoot(const std::wstring& txt, int state = 0, int icon_color = 0);
    // �ڽ� �߰�
    ST_TreeNode* AddChild(ST_TreeNode* parent, const std::wstring& txt, int state = 0, int icon_color = 0);
    // UI ������Ʈ
    void PopulateTreeView(HWND hTree);              // ���� �׸��� ������ ����� �ٽ� �׸���
    void RefreshView(HWND hTree);            // ȭ�鿡 �����ܻ��� �ݿ��Ѵ�

    void SetNodeIconColor(ST_TreeNode* node, TREE_ICON_COLOR_INDEX color);
    void SetNodeText(ST_TreeNode* node, const std::wstring& newText);
    void CycleAllIconColors();
    
private:
    std::vector<std::unique_ptr<ST_TreeNode>> allNodes;

    // ������ �׸���
    void InsertRecursive(HWND hTree, HTREEITEM hParent, ST_TreeNode* node);
};

HBITMAP createGdiPlusCircleBitmap(int circle_size, COLORREF fillColor);