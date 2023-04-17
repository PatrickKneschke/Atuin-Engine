
#pragma once


#include "IAllocator.h"

#include <algorithm>


namespace Atuin {


class FreeTreeAllocator : public IAllocator {


    struct TreeNode {

        Size size;
        Size maxSize;
        TreeNode *parent;
        TreeNode *left;
        TreeNode *right;

        TreeNode() : size {0}, maxSize {0}, parent {nullptr}, left {nullptr}, right {nullptr} {}
        TreeNode(const Size size_, TreeNode *parent_ = nullptr, TreeNode *left_ = nullptr, TreeNode *right_ = nullptr) : 
            size {size_}, parent {parent_}, left {left_}, right {right_} 
        {
            maxSize = size;
            if (left != nullptr)
            {
                maxSize = std::max(maxSize, left->maxSize);
            }
            if (right != nullptr)
            {
                maxSize = std::max(maxSize, right->maxSize);
            }
        }
    };

    struct AllocHeader {

        Size size;
        Size adjustment;
    };


public:

    FreeTreeAllocator() = delete;
    FreeTreeAllocator(Size totalMemory, IAllocator *parent);
    ~FreeTreeAllocator();


    void* Allocate(Size size, U8 alignment) override;
    void  Free(void *ptr) override;
    void  Clear() override;


private:

    TreeNode* FindNode(Size size, TreeNode *root);
    std::pair<TreeNode*, TreeNode*> FindNeighbors(TreeNode *node);
    void InsertNode(TreeNode *newNode);
    void RemoveNode(TreeNode *target);
    void ReplaceNode(TreeNode *target, TreeNode *newNode);
    void ShiftNodeUp(TreeNode *target, TreeNode *node);
    void UpdateMaxSize(TreeNode *node);


    TreeNode *pRoot;
};

    
} // Atuin
