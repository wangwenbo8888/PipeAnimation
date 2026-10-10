/******************************************************************************
osg3DViewer: a 3D file viewer
Copyright(C) 2015-2015  ronggenmiao@163.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software

*******************************************************************************/


#include "xSceneModel.h"
#include "xOsgLogger.h"
#include "xLogHandler.h"
#include "CreatePipe.h"
#include "NodeTransFormCallBack.h"
//#include "ExtentsVisitor.h"

#include <osg/PolygonOffset>
#include <osg/ShapeDrawable>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgDB/WriteFile>
#include <osg/CullFace>
#include <osg/Geode>
#include <osg/PolygonMode>
#include <osgManipulator/CommandManager>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TrackballDragger>

#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>

#include <osgUtil/Statistics>
#include <QtGui/QApplication>
// typedefs and definitions
#if   !defined(CLAMP)
#define  CLAMP(x,min,max)   ((x<min) ? min : ((x>max) ? max : x))
#endif

const int receivesShadowTraversalMask = 0x1;
const int castsShadowTraversalMask = 0x2;

// --------------------------------------------------------------------------------
xSceneModel::xSceneModel(QObject *parent) :
QObject(parent),
	m_isHighlightScene(false),
	m_isShadowScene(false),
	m_isTextrueScene(false),
	m_isLightScene(true),
	m_sceneCenter(osg::Vec3(0.0f,0.0f,0.0f)),
	m_iPipeNum(0),
	m_HudText(NULL)
{
	// set the osg log to QT message
	osg::setNotifyLevel(osg::NOTICE);
	xOsgLogger *loggerCout = new xOsgLogger(std::cout);
	connect(loggerCout, SIGNAL(sigMessage(const QString &)), xLogHandler::getInstance(), SLOT(slotReportDebug(const QString &)),Qt::QueuedConnection);
	xOsgLogger *loggerCerr = new xOsgLogger(std::cerr);
	connect(loggerCerr, SIGNAL(sigMessage(const QString &)),  xLogHandler::getInstance(), SLOT(slotReportInfo(const QString &)),Qt::QueuedConnection);

	//osg::setNotifyLevel(osg::DEBUG_INFO);
	//osg::setNotifyLevel(osg::NOTICE);
	InitStlMatrix();
	createScene();
	SetNodeStructure();
}

xSceneModel::~xSceneModel()
{
}

osg::ref_ptr<osg::Node> xSceneModel::createQuad(osg::Vec3 center,osg::Vec3 normal)//创建一个四边形节点  
{  
    // 创建一个叶节点对象  
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();  
  
    // 创建一个几何体对象  
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();  
  
    // 创建顶点数组，注意顶点的添加顺序是逆时针的  
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();  
    v->push_back(osg::Vec3(center.x(), 100.0f, -100.0f));// 添加数据  
    v->push_back(osg::Vec3(center.x(), 100.0f, 100.0f));  
    v->push_back(osg::Vec3(center.x(), -100.0f, 100.0f));  
    v->push_back(osg::Vec3(center.x(), -100.0f, -100.0f));  
  
    // 设置顶点数据  
    geom->setVertexArray(v.get());  
  
    // 创建纹理坐标  
    osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();  
    vt->push_back(osg::Vec2(0.0f, 0.0f));//添加数据  
    vt->push_back(osg::Vec2(1.0f, 0.0f));  
    vt->push_back(osg::Vec2(1.0f, 1.0f));  
    vt->push_back(osg::Vec2(0.0f, 1.0f));  
  
    // 设置纹理坐标  
    geom->setTexCoordArray(0, vt.get());  
  
    //创建颜色数组  
    osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array();  
    vc->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));//添加数据  
    vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));  
    vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));  
    vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));  
    geom->setColorArray(vc.get());//设置颜色数组     
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);// 设置颜色的绑定方式为单个顶点  
  
    // 创建法线数组  
    osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();  
    nc->push_back(osg::Vec3(normal.x(),normal.y(),normal.z()));// 添加法线  
  
    // 设置法线数组  
    geom->setNormalArray(nc.get());  
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);// 设置法线的绑定方式为全部顶点      
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));// 添加图元，绘图基元为四边形    
    geode->addDrawable(geom.get());// 添加到叶节点  
  
    return geode.get();  
}  

void xSceneModel::setMyClipPlane(const std::vector<osg::Vec3>& centers,
		                         const std::vector<osg::Vec3>& normals,float _z)
{
    // osg::Node* nodestl = this->getNode("NodeSTL");
	for (size_t i = 0; i < centers.size(); ++i)
	{
		osg::Plane plane;
		osg::Vec3f normal;
		// plane_normal = osg::Vec3f(0.0, 0.0, 1.0);//切割法线反方向
		normal = normals[i];//切割法线反方向
		double delta = 0.000001;
		if (fabs(normal.length()-1.0) > delta)
		{
			continue;
		}

		osg::Vec3 center = centers[i];

		plane.set(normal, osg::Vec3f(center.x(),center.y(),_z));

		osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane;
		// 为了调试，先去掉遮挡平面

        clipPlane->setClipPlane(plane);

        // clipPlane->setClipPlane(normal.x(),normal.y(),normal.z(),center.x());
		clipPlane->setClipPlaneNum(i);

		// 遮挡平面绑定根节点，防止遮挡平面乱转
		m_stlNode->getOrCreateStateSet()->setAttributeAndModes(clipPlane.get(), osg::StateAttribute::ON);
		// 解除遮挡平面对长直管的作用
		m_LongStraightPipeNode->getOrCreateStateSet()->setAttributeAndModes(clipPlane.get(), osg::StateAttribute::OFF);
        // m_rootShadowNode->addChild(createQuad(center,normal));
        // 剪切平面需要绑定一些除管子外的其他节点，不然剪切平面会跟着管子一起转，起不到遮挡的作用
        // nodestl->getOrCreateStateSet()->setAttributeAndModes(clipPlane.get(), osg::StateAttribute::ON);
		m_vClipPlanes.push_back(clipPlane);
	}
}

void xSceneModel::ResetPipeIndex()
{
	m_iPipeNum = 0;
}

void xSceneModel::InitStlMatrix()
{
	m_mtrixXiaoCheFather = new osg::MatrixTransform;
	m_mtrixJiTouWanGuan1Father = new osg::MatrixTransform;
	m_mtrixJiTouWanGuan2Father = new osg::MatrixTransform;
	m_mtrixJiTouWanGuan3Father = new osg::MatrixTransform;
	m_mtrixZhuaLiaoMoFather = new osg::MatrixTransform;
	m_mtrixZhuaMoLiaoFather = new osg::MatrixTransform;
	m_mtrixZhuJia1Father = new osg::MatrixTransform;
	m_mtrixZhuJia2Father = new osg::MatrixTransform;
	m_mtrixDaoJia1Father = new osg::MatrixTransform;
	m_mtrixDaoJia1_1Father = new osg::MatrixTransform;
	m_mtrixDaoJia2_1Father = new osg::MatrixTransform;
	m_mtrixDaoJia2Father = new osg::MatrixTransform;
	m_mtrixJiTouMoZuoFather = new osg::MatrixTransform;
	m_mtrixJiTouMoZuo1Father = new osg::MatrixTransform;
	m_mtrixWanBiFather = new osg::MatrixTransform;
	m_mtrixJiTouDiZuoFather = new osg::MatrixTransform;
	m_mtrixZhongXinMoFather = new osg::MatrixTransform;
	m_mtrixWanQuMo1_1Father = new osg::MatrixTransform;
	m_mtrixWanQuMo1Father = new osg::MatrixTransform;
	m_mtrixWanQuMo2Father = new osg::MatrixTransform;
	m_mtrixWanQuMo2PosFather = new osg::MatrixTransform;
	m_mtrixYouFuTuiFather = new osg::MatrixTransform;
	m_mtrixZuoFuTuiFather = new osg::MatrixTransform;

	m_mtrixPipeFatherLeft = new osg::PositionAttitudeTransform;
    m_mtrixPipeFatherRight = new osg::PositionAttitudeTransform;

	m_mtrixDiMianFather = new osg::MatrixTransform;
	m_mtrixJiShenFather = new osg::MatrixTransform;
	m_mtrixXiaoCheScaleFather = new osg::MatrixTransform;
	m_mtrixZhuaLiaoMoScaleFather = new osg::MatrixTransform;
	m_mtrixLunMoFather = new osg::MatrixTransform;


	m_mtrixCheTouFather = new osg::MatrixTransform;  
	m_mtrixDaoLun1Father = new osg::MatrixTransform;  
	m_mtrixDaoLun2Father = new osg::MatrixTransform;  
	m_mtrixZhuLunFather = new osg::MatrixTransform;  
	m_mtrixCheTouScaleFather = new osg::MatrixTransform;  
	m_mtrixDaoLun1ScaleFather = new osg::MatrixTransform;  
	m_mtrixDaoLun2ScaleFather = new osg::MatrixTransform;  
	m_mtrixZhuLunScaleFather = new osg::MatrixTransform;
}

void xSceneModel::SetNodeStructure()
{
	//管子
	m_stlPipeFatherRight = new osg::Group();
    m_stlPipeFatherLeft = new osg::Group();

	QString pipefather = "PipeFatherRight";
	m_stlPipeFatherRight->setName(pipefather.toStdString());
	setNameNode(pipefather,m_stlPipeFatherRight);
	//m_stlNode->addChild(m_stlPipeFather);
    //父节点下增加旋转矩阵，C值旋转,matrix加到Node节点下
	m_stlPipeFatherRight->addChild(m_mtrixPipeFatherRight);

    pipefather = "PipeFatherLeft";
	m_stlPipeFatherLeft->setName(pipefather.toStdString());
	setNameNode(pipefather,m_stlPipeFatherLeft);
	//m_stlNode->addChild(m_stlPipeFather);
    //父节点下增加旋转矩阵，C值旋转,matrix加到Node节点下
	m_stlPipeFatherLeft->addChild(m_mtrixPipeFatherLeft);

	//機身
	m_stlJiShenFather = new osg::Group();
	QString jishenfather = "JiShenFather";
	m_stlJiShenFather->setName(jishenfather.toStdString());
	m_stlNode->addChild(m_mtrixJiShenFather);

	//小车->机身
	m_stlXiaoCheFather = new osg::Group();
	QString xiaochefather = "XiaoCheFather";
	m_stlXiaoCheFather->setName(xiaochefather.toStdString());
	m_mtrixJiShenFather->addChild(m_mtrixXiaoCheScaleFather);
	m_mtrixXiaoCheScaleFather->addChild(m_mtrixXiaoCheFather);

	//车头 ->小车
	m_mtrixXiaoCheFather->addChild(m_mtrixCheTouScaleFather);
	m_mtrixCheTouScaleFather->addChild(m_mtrixCheTouFather);

	//抓料莫 ->车头
	m_mtrixCheTouFather->addChild(m_mtrixZhuaLiaoMoScaleFather);
	m_mtrixZhuaLiaoMoScaleFather->addChild(m_mtrixZhuaLiaoMoFather);

	//机头底座 - 机身
	m_mtrixJiShenFather->addChild(m_mtrixJiTouDiZuoFather);

	//机头模座 - 机头底座
	m_mtrixJiTouDiZuoFather->addChild(m_mtrixJiTouMoZuoFather);	

	//机头弯管1 - 机头模座
	m_mtrixJiTouMoZuoFather->addChild(m_mtrixJiTouWanGuan1Father);

	//主夹1 - 机头弯管1
	m_mtrixJiTouWanGuan1Father->addChild(m_mtrixZhuJia1Father);

	//主夹2 - 主夹1
	m_mtrixZhuJia1Father->addChild(m_mtrixZhuJia2Father);

	//导轮1 - 主夹2
	m_mtrixZhuJia2Father->addChild(m_mtrixDaoLun1ScaleFather);
	m_mtrixDaoLun1ScaleFather->addChild(m_mtrixDaoLun1Father);

	//机头弯管2 - 机头模座
	m_mtrixJiTouMoZuoFather->addChild(m_mtrixJiTouWanGuan2Father);

	//导轮2 - 机头弯管2
	m_mtrixJiTouWanGuan2Father->addChild(m_mtrixDaoLun2ScaleFather);
	m_mtrixDaoLun2ScaleFather->addChild(m_mtrixDaoLun2Father);

	//主轮 - 机头弯管2
	m_mtrixJiTouWanGuan2Father->addChild(m_mtrixZhuLunScaleFather);
	m_mtrixZhuLunScaleFather->addChild(m_mtrixZhuLunFather);
}

void xSceneModel::setMoveTrans(osg::ref_ptr<osg::MatrixTransform> trans)
{
	m_moveTrans = trans;
}


void xSceneModel::setRotateTrans(osg::ref_ptr<osg::MatrixTransform> trans)
{
	m_rotateTrans = trans;
}


osg::ref_ptr<osg::MatrixTransform> xSceneModel::getMoveTrans()
{
	return m_moveTrans;
}

void xSceneModel::createAllPipe(osg::Vec3 pos,float radius,const std::vector<InputData>& input)
{
	double len = -1600.0;
	for (size_t i = 0; i < input.size();++i)
	{
		const InputData& data = input[i];
		len += data.point.x();
		//pos = osg::Vec3(-1.0*data.point.x(),0.0,-195.0); // 管子头挪到(0.0,0.0,-195.0)
		osg::ref_ptr<osg::Group> part = creatStraitPipe(radius,data.point.x(),pos,osg::Vec3(-1.0,0.0,0.0));
		m_Pipes.push_back(part);

		// 管子段对应的变换矩阵
		osg::ref_ptr<osg::MatrixTransform> matrix = new osg::MatrixTransform;
		m_PipesMatrix.push_back(matrix);
		osg::ref_ptr<osg::MatrixTransform> matrix1 = new osg::MatrixTransform;
		m_curvepipemtrix.push_back(matrix1);
		if (data.point.z()>0.0)
		{
			len += data.point.z();
			//pos -= osg::Vec3(0.0,0.0,-19.50);
		    osg::Vec3 center = pos + osg::Vec3(0.0,0.0,data.radius);
			osg::ref_ptr<osg::Group> part = creatArcPipe(data.radius,radius,0.0,data.point.z(),center,osg::Vec3(0.0,1.0,0.0));
			m_Pipes.push_back(part);

			// 管子段对应的变换矩阵
			osg::ref_ptr<osg::MatrixTransform> matrix = new osg::MatrixTransform;
			m_PipesMatrix.push_back(matrix);
			osg::ref_ptr<osg::MatrixTransform> matrix1 = new osg::MatrixTransform;
			m_curvepipemtrix.push_back(matrix1);

		}
	}
}

osg::ref_ptr<osg::Group> xSceneModel::getPipe(int index)
{
	return m_Pipes[index];
}

void xSceneModel::setAllPipeToMatrix(osg::ref_ptr<osg::MatrixTransform> pmatrix,int isize)
{
	for(int i=0; i<isize; i++)
	{
		pmatrix->addChild(m_Pipes[i]);
	}
}

osg::ref_ptr<osg::MatrixTransform> xSceneModel::getRotateTrans()
{
	return m_rotateTrans;
}

osg::ref_ptr<osg::MatrixTransform> xSceneModel::getDaoJia2ScaleMtrix()
{
	return m_scalemtrix;
}

osg::ref_ptr<osg::MatrixTransform> xSceneModel::getZhuaLiaoMoScaleMtrix()
{
	return m_ZhuaLiaoMoscalemtrix;
}

std::map<std::string,std::string> xSceneModel::getChildToFatherMap()
{
	return m_ChildToFatherMap;
}

osg::ref_ptr<osgShadow::ShadowedScene> xSceneModel::GetShadowNode()
{
	return m_rootShadowNode;
}

std::vector<osg::Group *> xSceneModel::getAllAddNodes()
{
	return m_vAddStlNodes;
}

osg::Geode* xSceneModel::creatArcPipe(float r0,float r1,float rotateAngle,float angle,osg::Vec3 center,osg::Vec3 dir)
{
	PipeArc *pArcPipe = new PipeArc(rotateAngle,angle*PI/180.0,r0,r1,40);//r0圆半径，r1管半径
	m_arcPipe = pArcPipe->Create(center,dir);
	QString name("Curve");
	name.append(QString::number(m_iPipeNum++));
	m_arcPipe->setName(name.toStdString().c_str());

	return m_arcPipe;
}

osg::Geode* xSceneModel::creatWheelModel(osg::Vec3 center,osg::Vec3 dir)
{
	WheelModel *pWheelModel = new WheelModel(17,15,30,5,5,20,20,10);
	osg::ref_ptr<osg::Geode> WheelModel = pWheelModel->CreateRabbet(center,dir);

	QString name("WheelModel");
	pWheelModel->setName(name.toStdString().c_str());

	return WheelModel;
}

osg::Geode* xSceneModel::creatStraitPipe(float r,float length,osg::Vec3 center,osg::Vec3 dir)
{
	PipeStraight *pStraPipe = new PipeStraight(length,r,5);
	m_straPipe = pStraPipe->Create(center,dir);

	QString name("Straight");
	if (m_iPipeNum==0)
	{
		name.append("long");
	}
	name.append(QString::number(m_iPipeNum++));
	m_straPipe->setName(name.toStdString().c_str());

	return m_straPipe;
}

void xSceneModel::ColorShader(osg::ref_ptr<osg::Node> node)
{
	const char * vertexShader = {
		"void main(void ){\n"
		"   gl_FrontColor = gl_Color;\n"
		"   gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;\n"
		"}\n"
	};

	const char * fragShader = {
		"void main(void){\n"
		"	gl_FragColor = gl_Color;\n"
		"}\n"
	};

	osg::StateSet * ss = node->getOrCreateStateSet();
	osg::ref_ptr<osg::Program> program = new  osg::Program();
	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragShader));
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShader));
	ss->setAttributeAndModes(program, osg::StateAttribute::ON);
}

void xSceneModel::resetModel()
{
	setData(new osg::Node());
}


void xSceneModel::setNodeType(int itype)
{
	m_NodeType = itype;
}

void xSceneModel::setNameNode(QString name,osg::Node* node)
{
	m_NameToNodeMap[name] = node;
}

osg::Node* xSceneModel::getNode(QString name)
{
	return m_NameToNodeMap[name];
}

void xSceneModel::setNodeOpeType(int opeType)
{
	m_NodeOpeType = opeType;
}

std::vector<osg::ref_ptr<osg::Group>> xSceneModel::getAllPipes()
{
	return m_Pipes;

}

std::vector<osg::ref_ptr<osg::MatrixTransform>> xSceneModel::getAllPipesMatrix()
{
	return m_PipesMatrix;
}

std::vector<osg::ref_ptr<osg::MatrixTransform>> xSceneModel::getAllCurvePipesMatrix()
{
	return m_curvepipemtrix;
}

osg::ref_ptr<osg::Group> xSceneModel::getPipeFatherRight()
{
	return m_stlPipeFatherRight;
}

osg::ref_ptr<osg::Group> xSceneModel::getPipeFatherLeft()
{
	return m_stlPipeFatherLeft;
}

osg::ref_ptr<osg::Group> xSceneModel::getLongStraightPipeFatherNode()
{
	return m_LongStraightPipeNode;
}

std::vector<osg::Group *> xSceneModel::getVecGNodes()
{
	return m_vGNodes;
}

void xSceneModel::serFatherNodeName(QString str)
{
	m_strFatherNodeName = str;
}


void xSceneModel::setAddNodeName(QString str)
{
	m_strAddNodeName = str;
}

bool xSceneModel::saveSceneData(const std::string & file)
{
	return osgDB::writeNodeFile(*m_currentData, file);
}

void xSceneModel::setHighlightScene(bool val)
{
	if (m_isHighlightScene == val)
		return;

	m_isHighlightScene = val;
	if (m_isHighlightScene)
	{
		//osg::Group *parent = m_rootNodes->getParent(0);
		m_currentHightlight = new osgFX::Scribe();
		m_currentHightlight->setWireframeLineWidth(2.0);
		m_currentHightlight->setWireframeColor(osg::Vec4(1.0,1.0,1.0,1.0));//背景

		m_currentHightlight->addChild(m_rootShadowNode);
		m_sceneNode->replaceChild(m_rootShadowNode, m_currentHightlight);
	}
	else
	{
		//osgFX::Scribe* parentAsScribe = m_currentHightlight;
		//osg::Node::ParentList parentList = parentAsScribe->getParents();
		//for(osg::Node::ParentList::iterator itr = parentList.begin();
		//	itr!=parentList.end();
		//	++itr)
		//	(*itr)->replaceChild(parentAsScribe,parentAsScribe->getChild(0));
		m_sceneNode->replaceChild(m_currentHightlight, m_rootShadowNode);
	}
}

osg::Node *xSceneModel::getScene()
{
	return m_switchRoot;
}


osg::MatrixTransform * xSceneModel::getMaxtrixTransB(void)
{
	return m_transformSpinScene;
}


osg::MatrixTransform * xSceneModel::getMaxtrixTransC(void)
{
	return m_transformRotateC;
}


osg::MatrixTransform * xSceneModel::getMaxtrixTransMove(void)
{
	return m_transformMoveY;
}


osg::PositionAttitudeTransform * xSceneModel::getPosTransB(void)
{
	return m_PosTransB;
}


osg::PositionAttitudeTransform * xSceneModel::getPosTransC(void)
{
	return m_PosTransC;
}


osg::PositionAttitudeTransform * xSceneModel::getPosTransMove(void)
{
	return m_PosTransY;
}


osg::PositionAttitudeTransform * xSceneModel::getLongStraightPipeTrans(void)
{
	return m_LongStraightPipeTrans;
}

osg::Node* xSceneModel::getArcPipe()
{
	return m_arcPipe;
}

osg::Node* xSceneModel::getStraPipe()
{
	return m_straPipe;
}


osg::Group* xSceneModel::getGroupPipe()
{
	return m_groupPipe;
}

osg::Node * xSceneModel::getCurrentNode(void)
{
	return m_currentData;
}

osg::Node *xSceneModel::getObject()
{
	return m_currentData;
}

void xSceneModel::createScene()
{
	// init
	m_switchRoot = new osg::Switch(); // switch
	m_switchRoot->setName("rootSwitch");

	m_PosTransB = new osg::PositionAttitudeTransform;
	m_PosTransB->setName("PosTransB");

	m_matrixStlNode = new osg::MatrixTransform();
	m_matrixStlNode->setName("m_matrixStlNode");
	m_switchRoot->addChild(m_matrixStlNode);

	m_sceneNode = new osg::Group(); // scene
	m_sceneNode->setName("NodeScene");
	m_matrixStlNode->addChild(m_sceneNode);
	m_NameToNodeMap["NodeScene"]=m_sceneNode;

	// shadow node
	m_rootShadowNode = new osgShadow::ShadowedScene();
	m_rootShadowNode->setName("m_rootShadowNode");
	m_rootShadowNode->setReceivesShadowTraversalMask(receivesShadowTraversalMask);
	m_rootShadowNode->setCastsShadowTraversalMask(castsShadowTraversalMask);
	m_rootShadowNode->setDataVariance(osg::Object::DYNAMIC);
	m_sceneNode->addChild(m_rootShadowNode.get());

    //m_clipPlaneNode = new osg::Group();
    //m_sceneNode->addChild(m_clipPlaneNode);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    osg::ref_ptr<osg::ShapeDrawable> shap = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), 0.1));
 
	//第四个参数0.25表示不透明度，0表示完全透明，1表示完全不透明
	shap->setColor(osg::Vec4(0.0,0.0,0.0,0.1));
	geode->addDrawable(shap);

	//m_clipPlaneNode->addChild(geode);

 //   osg::ref_ptr<osg::Geode> geode = new osg::Geode();
 //   osg::ref_ptr<osg::ShapeDrawable> shap = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), 0.01));
	////第四个参数0.25表示不透明度，0表示完全透明，1表示完全不透明
	//shap->setColor(osg::Vec4(0.0,0.0,0.0,0.0));
	//geode->addDrawable(shap);
 //   m_clipPlaneNode->addChild(geode);

	m_LongStraightPipeNode = new osg::Group();
	m_stlNode = new osg::Group();
	m_LongStraightPipeTrans = new osg::PositionAttitudeTransform;
	QString nodestl = "NodeSTL";
	m_stlNode->setName(nodestl.toStdString());
	m_rootShadowNode->addChild(m_stlNode);
	// m_rootShadowNode->addChild(m_LongStraightPipeNode);

	m_stlNode->addChild(m_LongStraightPipeNode);

	m_LongStraightPipeNode->addChild(m_LongStraightPipeTrans);

	m_vGNodes.push_back(m_stlNode->asGroup());
	setNameNode(nodestl,m_stlNode);

	m_stlNode->addChild(geode);

	//osg::ref_ptr<osg::Group> wheelmodel = creatWheelModel(osg::Vec3(0,0,0),osg::Vec3(0,1,0));
	//m_stlNode->addChild(wheelmodel);

	m_HudText = new HUDText();
	m_rootShadowNode->addChild(m_HudText->getProj());

	//osg::Plane plane;
	//osg::Vec3f plane_normal;
	//plane_normal = osg::Vec3f(0.0, 0.0, 1.0);//切割法线反方向


	//plane.set(plane_normal, osg::Vec3f(0.0,0.0,0.0));
	//m_clipPlane = new osg::ClipPlane;
	//m_clipPlane->setClipPlane(plane);
	//m_clipPlane->setClipPlaneNum(0);

	//m_rootShadowNode->addChild(createQuade(0.0237));
	//管子遮挡盒
	float center_z = -60;
	float wid = 300;
	osg::ref_ptr<osg::Vec3Array> tempArray = new osg::Vec3Array();
	tempArray->push_back(osg::Vec3(110,585,1090));
	tempArray->push_back(osg::Vec3(110,585,890));
	tempArray->push_back(osg::Vec3(110,610,1090));
	tempArray->push_back(osg::Vec3(110,610,890));
	tempArray->push_back(osg::Vec3(-400,585,1090));
	tempArray->push_back(osg::Vec3(-400,585,890));
	tempArray->push_back(osg::Vec3(-400,610,1090));
	tempArray->push_back(osg::Vec3(-400,610,890));

	/*m_mtrixPipeFather->addChild(CreateOccluder(tempArray->at(0),tempArray->at(1),tempArray->at(2),tempArray->at(3)));
	m_mtrixPipeFather->addChild(CreateOccluder(tempArray->at(4),tempArray->at(5),tempArray->at(6),tempArray->at(7)));
	m_mtrixPipeFather->addChild(CreateOccluder(tempArray->at(0),tempArray->at(1),tempArray->at(5),tempArray->at(4)));
	m_mtrixPipeFather->addChild(CreateOccluder(tempArray->at(1),tempArray->at(2),tempArray->at(6),tempArray->at(5)));
	m_mtrixPipeFather->addChild(CreateOccluder(tempArray->at(2),tempArray->at(6),tempArray->at(7),tempArray->at(3)));
	m_mtrixPipeFather->addChild(CreateOccluder(tempArray->at(3),tempArray->at(7),tempArray->at(4),tempArray->at(0)));*/

	//创建一个裁剪平面给机头生成的管子
	//m_clipNode = new osg::ClipNode();
	////根据前面指定的包围盒创建6个裁剪平面
	//osg::BoundingBox bb;
	//osg::Vec3 min = osg::Vec3(0.0, -10000.0, -10000.0);
	//osg::Vec3 max = osg::Vec3(10000.0, 10000.0,10000.0);
	//bb.set(min,max);
	//m_clipNode->createClipBox(bb);
	////禁用拣选（拣选即显示一些东西不显示一些东西）
	//m_clipNode->setCullingActive(false);
	//m_stlPipeFather->addChild(m_clipNode);

	////创建一个裁剪平面给机头生成的管子
	//m_clipNode1 = new osg::ClipNode();
	////根据前面指定的包围盒创建6个裁剪平面
	//osg::BoundingBox bb1;
	//osg::Vec3 min1 = osg::Vec3(-10000, 30.0,-10000.0);
	//osg::Vec3 max1 = osg::Vec3(0.0, 10000.0,10000.0);
	//bb1.set(min1,max1);
	//m_clipNode1->createClipBox(bb1);
	////禁用拣选（拣选即显示一些东西不显示一些东西）
	//m_clipNode1->setCullingActive(false);
	//m_mtrixPipeFather->addChild(m_clipNode1);

	//创建一个裁剪平面给原始直管
	//m_straiclipmtrx = new osg::MatrixTransform;
	//m_straclipNode = new osg::ClipNode();
	////根据前面指定的包围盒创建6个裁剪平面
	//osg::BoundingBox bb2;
	//osg::Vec3 min2 = osg::Vec3(-10000.0, -10000.0, -10000.0);
	//osg::Vec3 max2 = osg::Vec3(0.0,10000.0,10000.0);
	//bb2.set(min2,max2);
	//m_straclipNode->createClipBox(bb2);
	////禁用拣选（拣选即显示一些东西不显示一些东西）
	//m_straclipNode->setCullingActive(false);
	//m_stlNode->addChild(m_straiclipmtrx);
	//m_straiclipmtrx->addChild(m_straclipNode);

	m_transformMoveY= new osg::MatrixTransform;
	m_transformRotateC = new osg::MatrixTransform;
	m_PosTransY = new osg::PositionAttitudeTransform;
	m_PosTransC = new osg::PositionAttitudeTransform;
	m_groupPipe = new osg::Group;

} 

osg::Group * xSceneModel::createSceneLight()
{
	osg::Group *grp = new osg::Group;

	// create and attach the light source
	osg::LightSource *lightSource = new osg::LightSource;
	lightSource->setLocalStateSetModes(osg::StateAttribute::ON);

	lightSource->getLight()->setAmbient(osg::Vec4(0.2f,0.2f,0.2f,1.0f));
	lightSource->getLight()->setDiffuse(osg::Vec4(0.5f,1.0f,0.0f,1.0f));
	lightSource->getLight()->setPosition(osg::Vec4(1000.f,1000.0f,1000.0f,1.0f));
	grp->addChild(lightSource);

	return grp;
}

void xSceneModel::setData(osg::Node *data, bool resetHome)//增加节点
{
	osg::Matrixd matrix;
	osg::Vec3d center;

	// reset selection
	emit sigLoadBegin(resetHome);

	m_sceneCenter = osg::Vec3(0.0f,0.0f,0.0f);
	m_currentData = data;
	if (!data)
		return;
	//m_sceneNode->addChild(m_currentData);

	//m_sceneNode->addChild(m_currentData);//所有节点添加到场景节点
	//把模型节点添加到对应的矩阵父节点下
	//if(m_NodeType == 1)//父节点
	//{
	//	osg::ref_ptr<osg::Node> tmpGNode = m_currentData.get();
	//	m_vGNodes.push_back(tmpGNode->asGroup());
	//}
	//else if(m_NodeType == 2)//子节点
	//{	

	//}
	//m_vAddStlNodes.push_back(data->asGroup());
	//
	m_sceneCenter = m_sceneNode->getBound().center();

	if (m_isHighlightScene)
	{
		setHighlightScene(false);
		setHighlightScene(true);
	}

	setShadowEnabled(m_isShadowScene);
	emit sigLoadFinished();
}

void xSceneModel::setShadowEnabled(bool val)
{
	if (val)
	{
		int alg = 3;
		const osg::BoundingSphere& bs = m_rootShadowNode->getBound();
		// test bidon pour modifier l'algo d'ombrage en fonction de la taille de l'objet
		if (alg == 0)
		{
			osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,5);
			pssm->setTextureResolution(2048);
			m_rootShadowNode->setShadowTechnique(pssm.get());

			pssm->init();
		}
		else if (alg == 2)
		{
			osg::ref_ptr<osgShadow::SoftShadowMap> pssm = new osgShadow::SoftShadowMap();

			pssm->setTextureSize(osg::Vec2s(2048,2048));

			//pssm->setTextureResolution(2048);
			m_rootShadowNode->setShadowTechnique(pssm.get());

			pssm->init();
		}
		else if (alg == 3)
		{
			osg::ref_ptr<osgShadow::MinimalShadowMap> pssm = new osgShadow::LightSpacePerspectiveShadowMapDB();
			pssm->setTextureSize(osg::Vec2s(2048,2048));

			//pssm->setTextureResolution(2048);
			m_rootShadowNode->setShadowTechnique(pssm.get());

			pssm->init();
		}
	}
	else
	{
		m_rootShadowNode->setShadowTechnique(NULL);
	}
}

osg::ref_ptr<osg::OccluderNode> CreateOccluder(const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3, const osg::Vec3& v4)
{
	//创建遮挡节点对象
	osg::ref_ptr<OccluderNode> occluderNode = new osg::OccluderNode();
	//创建遮挡平面
	osg::ref_ptr<ConvexPlanarOccluder> cpo = new osg::ConvexPlanarOccluder();
	//关联遮挡平面
	occluderNode->setOccluder(cpo.get());
	occluderNode->setName("occluder");

	//初始化一个遮挡平面
	osg::ConvexPlanarPolygon& occluder = cpo->getOccluder();
	occluder.add(v1);
	occluder.add(v2);
	occluder.add(v3);
	occluder.add(v4);

	//为遮挡面画一个四边形
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array(occluder.getVertexList().begin(),occluder.getVertexList().end());
	//设置绘制图元的坐标点
	geom->setVertexArray(coords);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.00, 0.00, 0.00, 0.0);
	//设置所要绘制图元的颜色数组
	geom->setColorArray(colors.get());
	//设置颜色绑定类型，因为颜色数据中只有一个颜色，所以次数设置的类型为osg::Geometry::BIND_OVERALL，所有点使用一个颜色
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(geom.get());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	//关闭光照
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//使用混合，以保证alpha纹理正确
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	//设置透明渲染元
	//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//设置属性
	geom->setStateSet(stateset.get());

	//添加四边形作为遮挡节点，遮挡节点本身不具有遮挡能力
	occluderNode->addChild(geode.get());

	return occluderNode;
}

// set a plane for test
osg::ref_ptr<osg::Node> xSceneModel::createQuade(float z)
{
	osg::ref_ptr<osg::Geometry> geom=new osg::Geometry();//定义一个几何体
	osg::ref_ptr<osg::LineWidth> lineSize=new osg::LineWidth();
	lineSize->setWidth(10.0);//设置线的宽度
	geom->getOrCreateStateSet()->setAttributeAndModes(lineSize.get(),osg::StateAttribute::ON);
	//首先定义四个点
	osg::ref_ptr<osg::Vec3Array> v=new osg::Vec3Array();//定义一个几何体坐标集合
	v->push_back(osg::Vec3(100.0,100.0,z));//左下角坐标点
	v->push_back(osg::Vec3(100.0,0.0,z));//右下角坐标点
	v->push_back(osg::Vec3(0.0,0.0,z));//右上角坐标点
	v->push_back(osg::Vec3(0.0,100.0,z));//左上角坐标点
	geom->setVertexArray(v.get());//将坐标设置到几何体节点中
	//定义颜色数组
	osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array();//定义一个颜色数组颜色
	c->push_back(osg::Vec4(1.0,0.0,0.0,1.0));//数组的四个参数分别为RGBA，其中A表示透明度
	c->push_back(osg::Vec4(0.0,1.0,0.0,1.0));
	c->push_back(osg::Vec4(0.0,0.0,1.0,1.0));
	c->push_back(osg::Vec4(1.0,1.0,1.0,1.0));
	geom->setColorArray(c.get());//与几何体中进行关联
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);//设置绑定方式为逐点绑定。
	//定义法线
	osg::ref_ptr<osg::Vec3Array> n=new osg::Vec3Array();//定义了一个法线绑定到该四方体中
	n->push_back(osg::Vec3(0.0,-1.0,0.0));//法线为指向Y轴负半轴
	geom->setNormalArray(n.get());//添加法线到几何体中
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);//将法线进行绑定
	//设置顶点的关联方式，这里是Quad方式，总共有这么些方式：POINTS,LINES,LINE_STRIP,LINE_LOOP,TRIANGLES,TRIANGLE_STRIP,TRIANGLE_FAN,QUADS,QUAD_STRIP,POLYGON
	//此关联方式来决定绘制什么样的图形，比如Quad表示四方块,POINTS[点],LINES[线],LINE_STRIP[线带],LINE_LOOP[闭合线段],TRIANGLES[三角形],TRIANGLE_STRIP[三角带],TRIANGLE_FAN[三角扇],QUADS[四方块],QUAD_STRIP[四方块带],POLYGON[多边形]
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

	//定义一个几何组结点，并把几何体结点加入到几何组结点当中
	osg::ref_ptr<osg::Geode> geode=new osg::Geode();
	geode->addDrawable(geom.get());
	return geode.get();
}