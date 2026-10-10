#ifndef __PIPES_H__
#define __PIPES_H__

#include <osg/Shape>
#include <osg/Group>
#include <osg/Geode>

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <QtCore/QObject>

#include "Novadef.h"
#include "HUDText.h"
#include "Table.h"

using namespace osg;

typedef enum PIPESHAPE
{
	Curve = 0,
	Straight = 1

};

class TextTransform : public osg::PositionAttitudeTransform
{
public:

	TextTransform(HUDText* hud,Table* tab,Table* twtab);
	~TextTransform();

	HUDText* GetHUDText(){return m_hud;}
	Table* GetTextTable(){return m_tab;}
	Table* GetTWTable(){return m_twTable;}

private:
	HUDText* m_hud;
	Table *m_tab;
	Table * m_twTable;
};

class PIPE :public osg::Referenced
{
public:
	PIPE();

	PIPE(osg::ref_ptr<osg::Geode> pipe,QString str,PIPESHAPE type,osg::Vec3 center,HUDText* hud,Table* tab,Table* twtab,double length);
	//osg::Group* creatArcPipe(float r0,float r1,float angle,osg::Vec3 center,osg::Vec3 dir);
	//osg::Group* creatStraitPipe(float r,float length,osg::Vec3 center,osg::Vec3 dir);//管子半径,长my,中心模坐标，方向
	osg::ref_ptr<TextTransform> GetMoveMatrix(){return m_MoveMatrix;}
	osg::ref_ptr<osg::PositionAttitudeTransform> GetRotateMatrix(){return m_RotateMtrix;}

	osg::ref_ptr<osg::PositionAttitudeTransform/*PositionAttitudeTransform*/> GetPosMatrix(){return m_PosMtrix;}

	osg::ref_ptr<MatrixTransform> GetCollisionMatrix(){return m_CollisionMatrix;}

	osg::ref_ptr<osg::Geode> GetPipeNode() const
	{
		return m_Pipe;
	}

	osg::Vec3 GetCenter(){return m_vCenter;}

	PIPESHAPE GetType() const 
	{
		return m_enumPipeShape;
	}
	double GetPipeLength()
	{
		return m_dLength;
	}

    int GetTip()
	{
		return m_iTip;
	}

	int GetHeader()
	{
		return m_iHeader;
	}

	void SetName(QString name)
	{
		m_strPipeName = name;
	}

	QString GetName()
	{
		return m_strPipeName;
	}
	
	void SetHide(bool value)
	{
		m_bHide = value;
	}

	void SetTip(int tip)
	{
		m_iTip = tip;
	}

	void SetHeader(int header)
	{
		m_iHeader = header;
	}

	bool IsHide()
	{
		return m_bHide;
	}

	void SetShowed(bool b)
	{
		m_bShowed = b;
	}

	bool IsShowed()
	{
		return m_bShowed;
	}

private:
	osg::ref_ptr<TextTransform> m_MoveMatrix;//y,c
	osg::ref_ptr<osg::PositionAttitudeTransform> m_RotateMtrix;//b
	osg::ref_ptr<osg::PositionAttitudeTransform> m_PosMtrix;   //移动位置
	osg::ref_ptr<MatrixTransform> m_CollisionMatrix;  // 为碰撞检测而添加的矩阵 wangwenbo 20241215
	
	osg::ref_ptr<osg::Geode> m_Pipe;
	double m_dLength;   // 直管长度
    bool m_bHide;       // 是否需要隐藏
	bool m_bShowed;     // 管子动作是否已经做完，已经显示
	int m_iTip;         // 管子从长直管左端还是右端加工出
	int m_iHeader;      // 管子从编号多少的机头加工出
	osg::Vec3 m_vCenter;
	QString m_strPipeName;
	PIPESHAPE m_enumPipeShape;
};

//osg::Group* PIPES::creatArcPipe(float r0,float r1,float angle,osg::Vec3 center,osg::Vec3 dir)
//{
//
//}
//
//
//osg::Group* PIPES::creatStraitPipe(float r,float length,osg::Vec3 center,osg::Vec3 dir)
//{
//
//}

#endif