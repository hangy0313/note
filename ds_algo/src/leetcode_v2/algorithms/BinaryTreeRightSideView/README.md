## Binary Tree Right Side View

Given a binary tree, imagine yourself standing on the right side of it, return the values of the nodes you can see ordered from top to bottom.

For example:
Given the following binary tree,

```

   1            <---
 /   \
2     3         <---
 \     \
  5     4       <---
```

You should return `[1, 3, 4]`. 

## Solution

求每一層的最右的一個節點，層次遍歷樹，每一層的最後一個節點就是結果元素

使用一個桶封裝樹節點，加上層次，當遍歷層次更新時，上一個節點就是上一個層次的最後一個節點

## Code
```cpp
typedef struct bucket{
	TreeNode *node;
	int level;
	bucket(TreeNode *n, int l):node(n), level(l){}
} bucket, *Bucket;
class Solution {
public:
    vector<int> rightSideView(TreeNode *root) {
	    vector<int> result;
	    if (root == nullptr)
		    return result;
	    queue<bucket> q;
	    q.push(bucket(root, 0));
	    int level = 0;
	    while (!q.empty()) {
		    auto b = q.front();
		    q.pop();
		    auto p = b.node;
		    if (p->left)
			    q.push(bucket(p->left, level + 1));
		    if (p->right)
			    q.push(bucket(p->right, level + 1));
		    if (q.empty() || q.front().level != level) {
			    result.push_back(p->val);
			    level = q.front().level;
		    }
	    }
	    return result;
    }
};
```
