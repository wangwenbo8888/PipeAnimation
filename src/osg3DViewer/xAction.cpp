#include "xAction.h"

xAction::xAction(QString strGroupName,QString strGCode,int iMoveType,osg::Vec3 vecRotateCenter,int iDirectly)
{
	m_strGroupName = strGroupName;
	m_strGCode = strGCode;
	m_iMoveType = iMoveType;
	m_vecRotateCenter = vecRotateCenter;
	m_iDirectly = iDirectly;
}

void xAction::SetMoveToPosition(osg::Vec3 MoveToPosition)
{
	m_vecMoveToPosition = MoveToPosition;
}

void xAction::SetAimFileValue(QString strAimFileValue)
{
	m_strAimFileValue = strAimFileValue;
}
