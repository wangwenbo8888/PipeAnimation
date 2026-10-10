
#include <iostream>
#include <QString>

#include <vector>

using namespace std;

typedef struct STreeNode* pSTreeNode;
typedef QString TreeNodeName;

struct STreeNode
{
	TreeNodeName Name; // 节点名
	pSTreeNode pFirstChild;
	pSTreeNode pNextBrother;

	STreeNode(TreeNodeName name )
	{
		Name = name;
		pFirstChild = NULL;
		pNextBrother = NULL;
	}
}; 

class CTree
{
public:
	CTree();
	CTree(TreeNodeName Value);
	~CTree();

public:
	void Insert(TreeNodeName parentValue, TreeNodeName Value);	//	parentValue：该节点的父结点；Value：该节点的值
	void InsertBrother( pSTreeNode pParentNode, TreeNodeName Value );

	pSTreeNode Search( pSTreeNode pNode, TreeNodeName Value );

	void Preorder(pSTreeNode pNode);		//	前序遍历
	void Inorder(pSTreeNode pNode);		//	中序遍历

    // 只找后代，不找兄弟
    void GetChild(pSTreeNode pNode,vector<QString>& vNodes,QString self);
    // 输出节点名
    void InorderOutName(pSTreeNode pNode,vector<QString>& vNodes,QString self);

	void FindNode(pSTreeNode pNode,QString name,vector<QString>& vNodes);

    void GetSubNode(pSTreeNode pNode,QString name,vector<QString>& vNodes);

	void postorder( pSTreeNode pNode );		//	后续遍历

	void PrintNode( pSTreeNode pNode );
	void FreeMemory( pSTreeNode pNode );	//	释放内存

	public:
		pSTreeNode pRoot;
};
