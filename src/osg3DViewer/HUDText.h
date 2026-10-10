
#ifndef _HUD_TEXT_H_
#define _HUD_TEXT_H_

#include <osg/Group>
#include <osg/Node>

#include <osg/PositionAttitudeTransform>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osgText/Text>
#include <osgText//Font>

class HUDText:public osg::Node
{

public:

	HUDText();
	~HUDText();

	// 初始化时设置所有加工信息
	void setHUDText(const std::vector<std::string>& infors);

	void setHUDTextColor(int i);
	void ResetAllColor();

	osg::ref_ptr<osg::Projection> getProj();
	osg::ref_ptr<osg::MatrixTransform> getTextMatrix(){return HUDViewMatrix;}
private:

	void createPanel();
	int m_iCurrentRow;

	osg::ref_ptr<osg::Geode> m_panelGeode;
	osg::ref_ptr<osg::Projection> m_HUDProjection;
	osg::ref_ptr<osg::MatrixTransform> HUDViewMatrix;
	//float m_z;
};

#endif // _HUD_TEXT_H_