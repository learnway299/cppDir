//
// Created by learyuan on 2025/6/22.
//

#ifndef TREENODETRAVERSAL_H
#define TREENODETRAVERSAL_H
#include <vector>
#include "../util/Util.h"


namespace TreeNodeTraversal {
    void solution();

    /**
     * preorder inorder postorder traversal treeNode
     * @param head
     * @return
     */
    std::vector<int> preorderTraversal(TreeNode* head);
    std::vector<int> inorderTraversal(TreeNode* head);
    std::vector<int> postorderTraversal(TreeNode* head);
};



#endif //TREENODETRAVERSAL_H
