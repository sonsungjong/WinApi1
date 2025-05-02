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
    std::wstring     text;           // 표시할 텍스트
    int              stateIndex;     // STATE 이미지 인덱스 (0=없음,1=닫힘 화살표,2=열림 화살표,3=없음…)
    int              iconIndex;      // NORMAL 이미지 인덱스 (원 색상)
    std::vector<TreeNode*> children; // 자식 노드들

    // 생성자
    TreeNode(const std::wstring& t, int s = 0, int i = 0) 
        : text(t), stateIndex(s), iconIndex(i) 
    {}
} ST_TreeNode;

class CTreeData
{
public:
    std::vector<std::unique_ptr<ST_TreeNode>> roots;

    // 루트 정의
    ST_TreeNode* AddRoot(const std::wstring& txt, int state = 0, int icon = 0);
    // 자식 추가
    ST_TreeNode* AddChild(ST_TreeNode* parent, const std::wstring& txt, int state = 0, int icon = 0);
    // UI 업데이트
    void PopulateTreeView(HWND hTree);
    
private:
    std::vector<std::unique_ptr<ST_TreeNode>> allNodes;

    // 개별로 그리기
    void InsertRecursive(HWND hTree, HTREEITEM hParent, ST_TreeNode* node);
};

HBITMAP createCircleBitmap(int circle_size, COLORREF fillColor, COLORREF maskColor);
HBITMAP CreateGdiPlusCircleBitmap(int circle_size, COLORREF fillColor);
void UpdateAllNodesByCycling(HWND hTree);