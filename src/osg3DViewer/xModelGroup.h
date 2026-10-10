#pragma once
#include <osg/Node>
#include <QString>
#include "STL.h"
#include "xAction.h"

class xModelGroup
{
public:
	xModelGroup(QString strGroupName,std::vector<STL*> vGroupSTLs);
	~xModelGroup();
	void AddActionToGroup(xAction *action);
	QString GetGroupName(){return m_strGroupName;}
	std::vector<STL*> GetCurGroupSTLs();

private:
	QString m_strGroupName;
	std::vector<xAction *>m_GActions;
	int m_iAxis;
	int m_iAnimationType;
	std::vector<STL*> m_vSTLs;
	osg::Vec3 m_GroupAbsolutePos;
};