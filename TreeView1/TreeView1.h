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
    std::wstring text;           // 표시할 텍스트
    int stateIndex;     // STATE 이미지 인덱스 (0=없음,1=없음,2=닫힘 화살표,3=열림 화살표)
    int iconColorIndex;      // NORMAL 이미지 인덱스 (원 색상)
    std::vector<TreeNode*> children; // 자식 노드들

    // 생성자
    TreeNode(const std::wstring& t, int s = 0, int i = 0) 
        : text(t), stateIndex(s), iconColorIndex(i)
    {}
} ST_TreeNode;

class CTreeData
{
public:

    std::vector<std::unique_ptr<ST_TreeNode>> roots;

    // 루트 정의
    ST_TreeNode* AddRoot(const std::wstring& txt, int state = 0, int icon_color = 0);
    // 자식 추가
    ST_TreeNode* AddChild(ST_TreeNode* parent, const std::wstring& txt, int state = 0, int icon_color = 0);
    // UI 업데이트
    void PopulateTreeView(HWND hTree);              // 기존 항목을 모조리 지우고 다시 그린다
    void RefreshView(HWND hTree);            // 화면에 아이콘색깔만 반영한다

    void SetNodeIconColor(ST_TreeNode* node, TREE_ICON_COLOR_INDEX color);
    void SetNodeText(ST_TreeNode* node, const std::wstring& newText);
    void CycleAllIconColors();
    
private:
    std::vector<std::unique_ptr<ST_TreeNode>> allNodes;

    // 개별로 그리기
    void InsertRecursive(HWND hTree, HTREEITEM hParent, ST_TreeNode* node);
};

HBITMAP createGdiPlusCircleBitmap(int circle_size, COLORREF fillColor);