
#include "NormalTree.h"

#include "log.h"

CTree::CTree()
{
	pRoot = NULL;
}

CTree::CTree( TreeNodeName Value )
{
	pRoot = new STreeNode( Value );
	if ( pRoot == NULL )
		return;
}

CTree::~CTree()
{
	if (pRoot == NULL )
		return;

	FreeMemory( pRoot );
}

void CTree::FreeMemory( pSTreeNode pNode )
{
	if ( pNode == NULL )
		return;

	if ( pNode->pFirstChild != NULL )
		FreeMemory( pNode->pFirstChild );

	if ( pNode->pNextBrother != NULL )
		FreeMemory( pNode->pNextBrother );
	
	delete pNode;
	pNode = NULL;
}

void CTree::Insert( TreeNodeName parentValue, TreeNodeName Value )
{
	if ( pRoot == NULL )
		return;

	pSTreeNode pFindNode = Search(pRoot, parentValue);
	if ( pFindNode == NULL )
		return;

	if ( pFindNode->pFirstChild == NULL )
	{
		pFindNode->pFirstChild = new STreeNode(Value);
		return;
	}
	else
	{
		InsertBrother(pFindNode->pFirstChild, Value);
		return;
	}
}

void CTree::InsertBrother(pSTreeNode pBrotherNode, TreeNodeName Value)
{
	if (pBrotherNode->pNextBrother != NULL)
	{
		InsertBrother(pBrotherNode->pNextBrother, Value);
	}
	else
	{
		pBrotherNode->pNextBrother = new STreeNode(Value);
		return;
	}
}

pSTreeNode CTree::Search( pSTreeNode pNode, TreeNodeName Value )
{
	if ( pNode == NULL )
		return NULL;

	if ( pNode->Name == Value )
		return pNode;

	if ( pNode->pFirstChild == NULL && pNode->pNextBrother == NULL )
    {
		return NULL;
    }
	else
	{
		if ( pNode->pFirstChild != NULL )
		{
			pSTreeNode pNodeTemp = Search( pNode->pFirstChild, Value );
			if ( pNodeTemp != NULL )
				return pNodeTemp;
			else
			{
				return Search( pNode->pNextBrother, Value );
			}
		}
		else
        {
			return Search( pNode->pNextBrother, Value );
        }
	}
}

void CTree::Preorder( pSTreeNode pNode )
{
	if (pNode == NULL)
		return;

	LogInfo(" " + pNode->Name);
	
	Preorder(pNode->pFirstChild);
	Preorder(pNode->pNextBrother);
}

void CTree::FindNode(pSTreeNode pNode,QString name,vector<QString>& vNodes)
{
	if (pNode == NULL )
		return;

	if (pNode->Name==name)
	{
        GetChild(pNode,vNodes,name);

        return;
	}

    FindNode(pNode->pFirstChild,name,vNodes);

    FindNode(pNode->pNextBrother,name,vNodes);
}

void CTree::GetSubNode(pSTreeNode pNode,QString name,vector<QString>& vNodes)
{
    
    FindNode(pNode,name,vNodes);

    /*return vNodes;*/
    //vector<QString> vNodes;
    //if (root==NULL)
    //{
    //    vector<QString> empty;
    //    return empty;
    //}
    //else
    //{
    //    InorderOutName(root,vNodes);
    //    return vNodes;
    //}
}

void CTree::GetChild(pSTreeNode pNode,vector<QString>& vNodes,QString self)
{
    if (pNode == NULL )
        return;

    InorderOutName(pNode->pFirstChild,vNodes,self);

    if (pNode->Name!=self)
    {
        // 自己不放进去
        vNodes.push_back(pNode->Name);
    }
    //LogInfo(" " + pNode->Name);

    //InorderOutName(pNode->pNextBrother,vNodes,self);
}

void CTree::InorderOutName(pSTreeNode pNode,vector<QString>& vNodes,QString self)
{
    //vNodes.clear();

    if (pNode == NULL )
        return;

    InorderOutName(pNode->pFirstChild,vNodes,self);

    if (pNode->Name!=self)
    {
        // 自己不放进去
        vNodes.push_back(pNode->Name);
    }
    //LogInfo(" " + pNode->Name);

    InorderOutName(pNode->pNextBrother,vNodes,self);
}

void CTree::Inorder(pSTreeNode pNode)
{
	if (pNode == NULL )
		return;

	Inorder(pNode->pFirstChild );

	//LogInfo(" " + pNode->Name);

	Inorder(pNode->pNextBrother );
}

void CTree::postorder(pSTreeNode pNode)
{
	if (pNode == NULL )
		return;

	postorder(pNode->pFirstChild);
	postorder(pNode->pNextBrother);

	LogInfo(" " + pNode->Name);
}