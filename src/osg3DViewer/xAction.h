#pragma once
#include <QMessageBox>
#include <QString>
#include <osg/Node>

class xAction
{
public:
	xAction(QString strGroupName,QString strGCode,int iMoveType,osg::Vec3 vecRotateCenter,int iDirectly);
	~xAction();

	void SetMoveToPosition(osg::Vec3 MoveToPosition);
	void SetAimFileValue(QString strAimFileValue);

private:
	QString m_strGroupName;
	QString m_strGCode;
	int m_iMoveType;
	osg::Vec3 m_vecRotateCenter;
	osg::Vec3 m_vecMoveToPosition;
	int m_iDirectly;
	QString m_strAimFileValue;


};