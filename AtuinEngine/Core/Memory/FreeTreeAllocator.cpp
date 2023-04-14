
#include "FreeTreeAllocator.h"


namespace Atuin {


FreeTreeAllocator::FreeTreeAllocator(Size totalMemory, IAllocator *parent) :
    IAllocator(totalMemory, parent)
{
    pRoot = new (pBase) TreeNode(mTotalMemory);
}


FreeTreeAllocator::~FreeTreeAllocator() {

}


void* FreeTreeAllocator::Allocate(Size size, U8 alignment) {

    // Pad size so that total allocated space can fit a TreeNode when freed
    Size paddedSize = std::max(size, sizeof(TreeNode) - sizeof(AllocHeader));

    // Find best memory region to allocate from
    Size requiredSize = paddedSize + sizeof(AllocHeader) + alignment - 1;    
    TreeNode *allocNode = FindNode(requiredSize, pRoot);
    if (allocNode == nullptr)
    {
        throw std::overflow_error("Free tree allocator does not have a large enough memory region available.");
    }

    // Get aligned address for allocation
    Size adjustment = GetAlignmentAdjustment(reinterpret_cast<UPtr>(allocNode) + sizeof(AllocHeader), alignment);
    UPtr alignedAddress = reinterpret_cast<UPtr>(allocNode) + adjustment + sizeof(AllocHeader);

    // Create new node for remaining memory region if it is large enough to fit a TreeNode
    // If not use the whole free region for allocation
    Size newSize = reinterpret_cast<UPtr>(allocNode) + allocNode->size - alignedAddress - paddedSize;
    Size allocSize = paddedSize;
    if (newSize >= sizeof(TreeNode))
    {
        TreeNode *newNode = new (reinterpret_cast<void*>(alignedAddress + paddedSize)) TreeNode(newSize);
        ReplaceNode(allocNode, newNode);
    }
    else
    {
        allocSize += newSize;
        RemoveNode(allocNode);
    }

    // Place allocation header in front of allocated memory section
    AllocHeader *header = reinterpret_cast<AllocHeader*>(alignedAddress - sizeof(AllocHeader));
    header->size = allocSize;
    header->adjustment = adjustment;

    mUsedMemory += header->adjustment + sizeof(AllocHeader) + header->size;
    mMaxUsedMemory = std::max(mMaxUsedMemory, mUsedMemory);

    return reinterpret_cast<void*>(alignedAddress);
}


void FreeTreeAllocator::Free(void *ptr) {

    assert(ptr != nullptr);

    // start address and size of freed memory section
    UPtr freeAddress = reinterpret_cast<UPtr>(ptr);
    AllocHeader *header = reinterpret_cast<AllocHeader*>( freeAddress - sizeof(AllocHeader) );
    freeAddress -= header->adjustment + sizeof(AllocHeader);
    Size freeSize = header->adjustment + sizeof(AllocHeader) + header->size;

    mUsedMemory -= freeSize;
    
    TreeNode *newNode = new (reinterpret_cast<void*>(freeAddress)) TreeNode(freeSize);

    // try merge adjacent nodes
    auto [leftNode, rightNode] = FindNeighbors(newNode);
    if (rightNode != nullptr && reinterpret_cast<UPtr>(newNode) + newNode->size == reinterpret_cast<UPtr>(rightNode))
    {
        newNode->size += rightNode->size;
        RemoveNode(rightNode); 
    }
    if (leftNode != nullptr && reinterpret_cast<UPtr>(leftNode) + leftNode->size == reinterpret_cast<UPtr>(newNode))
    {
        leftNode->size += newNode->size;
        UpdateMaxSize(leftNode);
    }
    else
    {
        InsertNode(newNode);
        UpdateMaxSize(newNode);
    }
}


void FreeTreeAllocator::Clear() {

    pRoot = new (pBase) TreeNode(mTotalMemory);
    mUsedMemory = 0;
}


FreeTreeAllocator::TreeNode* FreeTreeAllocator::FindNode(Size size, TreeNode *root) {

    if (root == nullptr || root->maxSize < size)
    {
        return nullptr;
    }

    if (root->size >= size)
    {
        return root;
    }

    if (root->left != nullptr && root->left->maxSize >= size)
    {
        return FindNode(size, root->left);
    }

    return FindNode(size, root->right);
}


std::pair<FreeTreeAllocator::TreeNode*, FreeTreeAllocator::TreeNode*> FreeTreeAllocator::FindNeighbors(TreeNode* node) {

    TreeNode *curr = pRoot, *left = nullptr, *right = nullptr;
    while (curr != nullptr && curr != node)
    {
        if (reinterpret_cast<UPtr>(node) < reinterpret_cast<UPtr>(curr))
        {
            right = curr;
            curr = curr->left;
        }
        else
        {
            left = curr;
            curr = curr->right;
        }
    }

    return {left, right}; 
}


void FreeTreeAllocator::InsertNode(TreeNode *newNode) {

    if(pRoot == nullptr)
    {
        pRoot =  newNode;
        return;
    }

    TreeNode *curr = pRoot, *prev = nullptr;
    while (curr != nullptr)
    {
        prev = curr;
        curr->maxSize = std::max(curr->maxSize, newNode->size);

        if (reinterpret_cast<UPtr>(newNode) < reinterpret_cast<UPtr>(curr))
        {
            curr = curr->left;
        }
        else
        {
            curr = curr->right;
        }
    }

    newNode->parent = prev;
    if (reinterpret_cast<UPtr>(newNode) < reinterpret_cast<UPtr>(prev))
    {
        prev->left = newNode;
    }
    else
    {
        prev->right = newNode;
    }
}


void FreeTreeAllocator::RemoveNode(TreeNode *target) {

    // Node from which to start the maxSize update
    TreeNode *sizeUpdateNode = target->parent;
    
    if (target->left == nullptr)
    {
        ShiftNodeUp(target, target->right);
    }
    else if (target->right == nullptr)
    {
        ShiftNodeUp(target, target->left);
    }
    else
    {
        TreeNode *nextNode = target->right;
        while (nextNode->left != nullptr)
        {
            nextNode = nextNode->left;
        }

        if (nextNode->parent != target)
        {
            sizeUpdateNode = nextNode->parent;
            ShiftNodeUp(nextNode, nextNode->right);
            nextNode->right = target->right;
            nextNode->right->parent = nextNode;
        }
        else
        {
            sizeUpdateNode = nextNode;
        }
        ShiftNodeUp(target, nextNode);
        nextNode->left = target->left;
        nextNode->left->parent = nextNode;
    }

    UpdateMaxSize(sizeUpdateNode);
}


void FreeTreeAllocator::ReplaceNode(TreeNode *target, TreeNode *newNode) {

    if (target == pRoot)
    {
        pRoot = newNode;
    }
    else
    {
        newNode->parent = target->parent;
        target == target->parent->left ? target->parent->left = newNode : target->parent->right = newNode;
    }

    if (target->left != nullptr)
    {
        newNode->left = target->left;
        newNode->left->parent = newNode;
    }

    if (target->right != nullptr)
    {
        newNode->right = target->right;
        newNode->right->parent = newNode;
    }

    UpdateMaxSize(newNode->parent);  
}


void FreeTreeAllocator::ShiftNodeUp(TreeNode *target, TreeNode *node) {

    if (node != nullptr)
    {
        node->parent = target->parent;
    }

    if(target == pRoot)
    {
        pRoot = node;
    }    
    else if (target == target->parent->left)
    {
        target->parent->left = node;
    }
    else
    {
        target->parent->right = node;
    } 
}


void FreeTreeAllocator::UpdateMaxSize(TreeNode *node) {

    TreeNode *curr = node;
    while (curr != nullptr)
    {
        curr->maxSize = curr->size;
        if (curr->left != nullptr)
        {
            curr->maxSize = std::max(curr->maxSize, curr->left->maxSize);
        }

        if (curr->right != nullptr)
        {
            curr->maxSize = std::max(curr->maxSize, curr->right->maxSize);
        }

        curr = curr->parent;
    }
}

    
} // Atuin
