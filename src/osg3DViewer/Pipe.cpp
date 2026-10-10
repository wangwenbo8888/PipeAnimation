#include "Pipe.h"

TextTransform::TextTransform(HUDText* hud,Table*tab,Table * twtab)
: m_hud(hud)
, m_tab(tab)
,m_twTable(twtab)
{
}

TextTransform::~TextTransform()
{
}

PIPE::PIPE()
{
}

PIPE::PIPE(osg::ref_ptr<osg::Geode> pipe,QString str,PIPESHAPE type,osg::Vec3 center,HUDText* hud,Table *tab,Table* twtab,double length)
{
	m_Pipe = pipe;
	m_strPipeName = str; 
	m_vCenter = center;
	m_dLength = length;
	m_bHide = false;

	m_bShowed = false;

	m_MoveMatrix =  new TextTransform(hud,tab,twtab);
	m_RotateMtrix  = new osg::PositionAttitudeTransform;
	m_PosMtrix  = new osg::PositionAttitudeTransform;

	m_CollisionMatrix = new MatrixTransform;

	m_enumPipeShape = type;

	m_CollisionMatrix->setName(str.toStdString());
	m_MoveMatrix->setName(str.toStdString());
	m_RotateMtrix->setName(str.toStdString());
	m_PosMtrix->setName(str.toStdString());

	//m_CollisionMatrix->addChild(m_RotateMtrix);
	m_RotateMtrix->addChild(m_MoveMatrix);
	m_MoveMatrix->addChild(m_PosMtrix);
	//m_MoveMatrix->addChild(m_RotateMtrix);
	//m_RotateMtrix->addChild(m_PosMtrix);
	m_PosMtrix->addChild(m_Pipe);
}