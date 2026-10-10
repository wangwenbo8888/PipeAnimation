
#include "HUDText.h"

HUDText::HUDText()
{
	m_iCurrentRow = -1;
	createPanel();

}

HUDText::~HUDText()
{

}

void HUDText::createPanel()
{
	//beging
	//画一个矩形模拟HUD面板
	osg::ref_ptr<osg::Geometry> panelGeom = new osg::Geometry;
	m_panelGeode = new osg::Geode;
	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	vertexArray->push_back(osg::Vec3(0,600,-1));
	vertexArray->push_back(osg::Vec3(1366,600,-1));
	vertexArray->push_back(osg::Vec3(1366,768,-1));
	vertexArray->push_back(osg::Vec3(0,768,-1));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

	osg::ref_ptr<osg::DrawElementsUInt> panelUnit = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON,0);
	panelUnit->push_back(0);
	panelUnit->push_back(1);
	panelUnit->push_back(2);
	panelUnit->push_back(3);

	osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
	normalArray->push_back(osg::Vec3(0.3f,0.3f,0.4f));

	osg::ref_ptr<osg::Vec2Array> textrueIndex = new osg::Vec2Array;
	textrueIndex->push_back(osg::Vec2(0.0f,0.0f));
	textrueIndex->push_back(osg::Vec2(1.0f,0.0f));
	textrueIndex->push_back(osg::Vec2(1.0f,1.0f));
	textrueIndex->push_back(osg::Vec2(0.0f,1.0f));

	panelGeom->setVertexArray(vertexArray.get());
	panelGeom->setNormalArray(normalArray.get());
	panelGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	panelGeom->setColorArray(colorArray.get());
	panelGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	panelGeom->setTexCoordArray(0,textrueIndex.get());
	panelGeom->addPrimitiveSet(panelUnit.get());

	m_panelGeode->addDrawable(panelGeom.get());

	//begin
	//载入纹理
	osg::ref_ptr<osg::Texture2D> HUDTexture = new osg::Texture2D;
	HUDTexture->setDataVariance(osg::Object::DYNAMIC);
	osg::ref_ptr<osg::StateSet> HUDStateSet = new osg::StateSet;
	HUDStateSet->setTextureAttributeAndModes(0,HUDTexture.get());
	HUDStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	HUDStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	HUDStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	HUDStateSet->setRenderBinDetails(11, "RenderBin");
	m_panelGeode->setStateSet(HUDStateSet.get());

	m_HUDProjection = new osg::Projection;
	m_HUDProjection->setMatrix(osg::Matrix::ortho2D(0,1366,0,768));
	HUDViewMatrix = new osg::MatrixTransform;
	HUDViewMatrix->setMatrix(osg::Matrix::identity());
	HUDViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_HUDProjection->addChild(HUDViewMatrix.get());
	HUDViewMatrix->addChild(m_panelGeode.get());

}

void HUDText::setHUDText(const std::vector<std::string>& infors)
{
	for (int i = 0; i < infors.size();++i)
	{
		const std::string& infor = infors[i];
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		
		text->setCharacterSize(15);
		text->setText(infor);
		text->setAlignment(osgText::Text::LEFT_TOP);
		text->setAxisAlignment(osgText::Text::SCREEN);
		text->setPosition(osg::Vec3(15,750-15*i,-1.5));
		text->setColor(osg::Vec4(0, 255, 0, 255));
	//	text->setBackdropType(osgText::Text::OUTLINE);//对文字进行描边
		//text->setBackdropColor(osg::Vec4(0.0,0.0,0.0,1.0));//描边颜色
	//	text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);//添加文字边框
		m_panelGeode->addDrawable(text.get());
	}
}

osg::ref_ptr<osg::Projection> HUDText::getProj()
{
	return m_HUDProjection;
}

void HUDText::setHUDTextColor(int i)
{
	if (m_iCurrentRow==i)
	{
		// 相同行，不变
		return;
	}

	if (m_panelGeode==NULL)
	{
		return;
	}

	int number = m_panelGeode->getNumDrawables();
	if (i >= number)
	{
		return;
	}

	// 将当前行变红
	osgText::Text* text = (osgText::Text*)m_panelGeode->getDrawable(i+1);
	if (text==NULL)
	{
		return;
	}
	text->setColor(osg::Vec4(255, 0, 0, 255));  // 当前加工设置为红色
	m_iCurrentRow = i;

	//return;

	if (i==0)
	{
		return;
	}
	//// 将前一行变绿
	text = (osgText::Text*)m_panelGeode->getDrawable(i);
	if (text==NULL)
	{
		return;
	}
	text->setColor(osg::Vec4(0, 255, 0, 255));  // 非当前加工设置为绿色

}

void HUDText::ResetAllColor()
{
	int number = m_panelGeode->getNumDrawables();

	for(int i=1;i<number;i++)
	{
		osgText::Text* text = (osgText::Text*)m_panelGeode->getDrawable(i);
		text->setColor(osg::Vec4(0, 255, 0, 255));  // 非当前加工设置为绿色
	}
}

