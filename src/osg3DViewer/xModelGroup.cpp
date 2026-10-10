#include "xModelGroup.h"

xModelGroup::xModelGroup(QString strGroupName,std::vector<STL*> vGroupSTLs)
{
	m_strGroupName = strGroupName;
	m_vSTLs = vGroupSTLs;
}

void xModelGroup::AddActionToGroup(xAction *action)
{
	m_GActions.push_back(action);
}

std::vector<STL*> xModelGroup::GetCurGroupSTLs()
{
	return m_vSTLs;
}

xModelGroup::~xModelGroup()
{

}
