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

#ifndef _OSGVIEWER_XSCENEMODEL_H_
#define _OSGVIEWER_XSCENEMODEL_H_

#ifdef min
#undef min
#endif


#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osgFX/Scribe>

#include <QtCore/QObject>
#include <osgShadow/ShadowedScene>

#include <osg/ref_ptr>
#include <osg/ClipNode>
#include <osg/ClipPlane>

#include <iostream>
#include "Novadef.h"
#include "HUDText.h"
#include "STL.h"


// 创建遮挡平面
osg::ref_ptr<osg::OccluderNode> CreateOccluder(const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3, const osg::Vec3& v4);

class xSceneModel : public QObject
{
	Q_OBJECT

public:
	xSceneModel(QObject *parent = NULL);
	virtual ~xSceneModel();

	void setMyClipPlane(const std::vector<osg::Vec3>& centers,
		                const std::vector<osg::Vec3>& normals,float _z);

	void setZ(float z){m_z = z;}
	HUDText* GetHudtext(){return m_HudText.get();}
	void ResetPipeIndex();
	void InitStlMatrix();
	void SetNodeStructure();
	void setMoveTrans(osg::ref_ptr<osg::MatrixTransform> trans);
	void setRotateTrans(osg::ref_ptr<osg::MatrixTransform> trans);
	osg::ref_ptr<osg::MatrixTransform> getMoveTrans();
	osg::ref_ptr<osg::MatrixTransform> getRotateTrans();

	osg::ref_ptr<osg::MatrixTransform> getDaoJia2_1ScaleMtrix(){return m_DaoJia2_1scalemtrix;}
	osg::ref_ptr<osg::MatrixTransform> getDaoJia2ScaleMtrix();
	osg::ref_ptr<osg::MatrixTransform> getZhuaLiaoMoScaleMtrix();
	osg::ref_ptr<osg::Group> getPipe(int index);
	void  setAllPipeToMatrix( osg::ref_ptr<osg::MatrixTransform> pmatrix,int isize);
	std::map<std::string,std::string> getChildToFatherMap();
	osg::ref_ptr<osgShadow::ShadowedScene> GetShadowNode();
	std::vector<osg::Group *> getAllAddNodes();
	osg::Geode* creatArcPipe(float r0,float r1,float rotateAngle,float angle,osg::Vec3 center,osg::Vec3 dir);
	osg::Geode* creatStraitPipe(float r,float length,osg::Vec3 center,osg::Vec3 dir);

	osg::Geode* creatWheelModel(osg::Vec3 center,osg::Vec3 dir);
	
	void ColorShader(osg::ref_ptr<osg::Node> node);
	void setData(osg::Node *data, bool resetHome = true);

	void resetModel();
	void setNodeType(int);
	void setNodeOpeType(int opeType);
	std::vector<osg::ref_ptr<osg::Group>> getAllPipes();
	std::vector<osg::ref_ptr<osg::MatrixTransform>> getAllPipesMatrix();
	std::vector<osg::ref_ptr<osg::MatrixTransform>> getAllCurvePipesMatrix();

	osg::ref_ptr<osg::Group> getPipeFatherRight();
    osg::ref_ptr<osg::Group> getPipeFatherLeft();

	osg::ref_ptr<osg::Group> getLongStraightPipeFatherNode();

	osg::ref_ptr<osg::Group> GetNodeScene(){return m_sceneNode;}

	std::vector<osg::Group *> getVecGNodes();
	void serFatherNodeName(QString str);
	void setAddNodeName(QString str);
	// debug
	bool saveSceneData(const std::string &);

	void setShadowEnabled(bool val);
	void setHighlightScene(bool val);

	void setNameNode(QString name,osg::Node* node);
	osg::Node* getNode(QString name);

	void setLightingEnabled(bool bLightingOn) {m_isLightScene=bLightingOn;}
	bool getLightingEnabled() const {return m_isLightScene;}

	void createAllPipe(osg::Vec3 pos,float radius,const std::vector<InputData>& input); // 创建所有各节管子

	void setTextureEnabled(bool bTextureOn) {m_isTextrueScene = bTextureOn;}
	bool getTextureEnabled() const {return m_isTextrueScene;}

	void setBackfaceEnabled(bool bBackface) {m_isBackface = bBackface;}
	bool getBackfaceEnabled() const {return m_isBackface;}


	//各个模型的父节点
	osg::ref_ptr<osg::MatrixTransform> getXiaoCheScaleMatrix() {return m_mtrixXiaoCheScaleFather;}
	osg::ref_ptr<osg::MatrixTransform> getStraightPipeClipMatrix() {return m_straiclipmtrx;}

	osg::ref_ptr<osg::MatrixTransform> getXiaoCheFatherMatrix() {return m_mtrixXiaoCheFather;}
	osg::ref_ptr<osg::MatrixTransform> getJiTouWanGuan1FatherMatrix() {return m_mtrixJiTouWanGuan1Father;}
	osg::ref_ptr<osg::MatrixTransform> getJiTouWanGuan2FatherMatrix() {return m_mtrixJiTouWanGuan2Father;}
	osg::ref_ptr<osg::MatrixTransform> getJiTouWanGuan3FatherMatrix() {return m_mtrixJiTouWanGuan3Father;}
	osg::ref_ptr<osg::MatrixTransform> getZhuLiaoMoFatherMatrix() {return m_mtrixZhuaLiaoMoFather;}

	osg::ref_ptr<osg::MatrixTransform> getJiTouDiZuoFatherMatrix() {return m_mtrixJiTouDiZuoFather;}
	osg::ref_ptr<osg::MatrixTransform> getJiTouMoZuoFatherMatrix() {return m_mtrixJiTouMoZuoFather;}
	osg::ref_ptr<osg::MatrixTransform> getJiTouMoZuo1FatherMatrix() {return m_mtrixJiTouMoZuo1Father;}
	osg::ref_ptr<osg::MatrixTransform> getZhongXinMoFatherMatrix() {return m_mtrixZhongXinMoFather;}
	osg::ref_ptr<osg::MatrixTransform> getZhuJia1FatherMatrix() {return m_mtrixZhuJia1Father;}
	osg::ref_ptr<osg::MatrixTransform> getAllZhuJia2Matrix() {return m_mtrixAllZhuJia2;}

	osg::ref_ptr<osg::MatrixTransform> getZhuJia2FatherMatrix() {return m_mtrixZhuJia2Father;}
	osg::ref_ptr<osg::MatrixTransform> getZhuJia2ScaleMatrix() {return m_ZhuJia2scalemtrix;}

	osg::ref_ptr<osg::MatrixTransform> getDaoJia1FatherMatrix() {return m_mtrixDaoJia1Father;}
	osg::ref_ptr<osg::MatrixTransform> getDaoJia1_1FatherMatrix() {return m_mtrixDaoJia1_1Father;}
	osg::ref_ptr<osg::MatrixTransform> getDaoJia2_1FatherMatrix() {return m_mtrixDaoJia2_1Father;}
	osg::ref_ptr<osg::MatrixTransform> getDaoJia2FatherMatrix() {return m_mtrixDaoJia2Father;}
	osg::ref_ptr<osg::MatrixTransform> getWanQuMo1FatherMatrix() {return m_mtrixWanQuMo1Father;}
	osg::ref_ptr<osg::MatrixTransform> getWanQuMo1_1FatherMatrix() {return m_mtrixWanQuMo1_1Father;}
	osg::ref_ptr<osg::MatrixTransform> getWanQuMo2FatherMatrix() {return m_mtrixWanQuMo2Father;}
	osg::ref_ptr<osg::MatrixTransform> getWanQuMo2PosFatherMatrix() {return m_mtrixWanQuMo2PosFather;}

	osg::ref_ptr<osg::MatrixTransform> getYouFuTuiFatherMatrix() {return m_mtrixYouFuTuiFather;}
	osg::ref_ptr<osg::MatrixTransform> getZuoFuTuiFatherMatrix() {return m_mtrixZuoFuTuiFather;}
	osg::ref_ptr<osg::MatrixTransform> getJiShenFatherMatrix() {return m_mtrixJiShenFather;}
	osg::ref_ptr<osg::MatrixTransform> getDiMianFatherMatrix() {return m_mtrixDiMianFather;}
	osg::ref_ptr<osg::MatrixTransform> getLunMoFatherMatrix() {return m_mtrixLunMoFather;}

	osg::ref_ptr<osg::MatrixTransform> getDaoLun1FatherMatrix() {return m_mtrixDaoLun1Father;}
	osg::ref_ptr<osg::MatrixTransform> getDaoLun2FatherMatrix() {return m_mtrixDaoLun2Father;}
	osg::ref_ptr<osg::MatrixTransform> getCheTouFatherMatrix() {return m_mtrixCheTouFather;}
	osg::ref_ptr<osg::MatrixTransform> getZhuLunFatherMatrix() {return m_mtrixZhuLunFather;}

    osg::ref_ptr<osg::PositionAttitudeTransform> getPipeFatherMatrixLeft() {return m_mtrixPipeFatherLeft;}
	osg::ref_ptr<osg::PositionAttitudeTransform> getPipeFatherMatrixRight() {return m_mtrixPipeFatherRight;}

	osg::ref_ptr<osg::ClipNode> getClipNode() {return m_clipNode;}
	osg::ref_ptr<osg::ClipNode> getClipNode1() {return m_clipNode1;}

	osg::ref_ptr<osg::ClipNode> getStraightClipNode() {return m_straclipNode;}

	osg::ref_ptr<osg::MatrixTransform> getStlNodeMatrix() {return m_matrixStlNode;}


	// get the root node of the scene
	osg::Node *getScene(void);
	osg::MatrixTransform *getMaxtrixTransB(void);
	osg::MatrixTransform *getMaxtrixTransC(void);
	osg::MatrixTransform *getMaxtrixTransMove(void);
	osg::PositionAttitudeTransform *getPosTransB(void);
	osg::PositionAttitudeTransform *getPosTransC(void);
	osg::PositionAttitudeTransform *getPosTransMove(void);
	osg::PositionAttitudeTransform *getLongStraightPipeTrans(void);
	osg::Node* getArcPipe();
	osg::Node* getStraPipe();
	osg::Group* getGroupPipe();
	osg::ref_ptr<osg::ClipPlane> getClipPlane(int i)
    {
        return m_vClipPlanes[i];
    }

    int getClipPlaneSize()
    {
        return m_vClipPlanes.size();
    }

	osg::ref_ptr<osg::ClipPlane> getLongClipPlane(){return m_LongclipPlane;}

	osg::Node *getCurrentNode(void);
	// get the terrain node if exist
	osg::Node *getObject(void);

	const osg::Vec3 & getSceneCenter() {return m_sceneCenter;}

signals:
	void sigLoadBegin(bool);
	void sigLoadFinished();

private:
	// functions
	void createScene();

    //创建一个四边形节点
    osg::ref_ptr<osg::Node> createQuad(osg::Vec3 center,osg::Vec3 normal);

	osg::Group *createSceneLight();

	double getNextVal (double curr);

	void reoderUnderlays();

	osg::ref_ptr<osg::Node> createQuade(float z);


private:
	// Type node (visible, ir3, ...)
	osg::PositionAttitudeTransform* m_LongStraightPipeTrans;
	std::map<int, osg::Node *> m_mapTypeNodes;
	osg::ref_ptr<osg::Geode> m_straPipe;
	osg::ref_ptr<osg::Geode> m_arcPipe;
	osg::Group* m_groupPipe;
	// Palette node (default, 0, 1, 2, ...)
	std::map<int, osg::Node *> m_mapPaletteNodes;
	std::map<int, bool> m_mapHidePaletteNodes;

	bool m_hideInstrumentNodes;
	bool m_hideObjectNodes;
	std::map<int, bool> m_mapHideTypeNodes;

	osg::ref_ptr<osg::Switch> m_switchRoot;

	osg::ref_ptr<osg::Group> m_sceneNode;

    // 为防止剪切平面乱转，创建一个虚拟剪切平面节点，绑定剪切平面
    osg::ref_ptr<osg::Group> m_clipPlaneNode;
	osg::ref_ptr<osg::Group> m_stlNode;
	osg::ref_ptr<osg::Group> m_LongStraightPipeNode;

	osg::ref_ptr<osg::MatrixTransform> m_matrixStlNode;//导夹2

	//创建一个裁剪节点
	osg::ref_ptr<osg::ClipNode> m_clipNode;
	osg::ref_ptr<osg::ClipNode> m_clipNode1;

	osg::ref_ptr<osg::ClipNode> m_straclipNode;
	std::vector<osg::ref_ptr<osg::ClipPlane>>  m_vClipPlanes;
	osg::ref_ptr<osg::ClipPlane>  m_LongclipPlane;


	// 各模型变换矩阵父节点的父节点
	osg::ref_ptr<osg::Group> m_stlCheTouFather;  // 小车父节点

	osg::ref_ptr<osg::Group> m_stlXiaoCheFather;  // 小车父节点
	osg::ref_ptr<osg::Group> m_stlJiTouWanGuan1Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlZhuaLiaoMoFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlZhuaMoLiaoFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlJiTouWanGuan2Father;  // 小车父节点
	osg::ref_ptr<osg::Group> m_stlJiTouWanGuan3Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlZhuJia1Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlZhuJia2Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlDaoJia1Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlDaoJia1_1Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlDaoJia2Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlDaoJia2_1Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlWanQuMo1Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlWanQuMo1_1Father;  // 机头弯管1父节点

	osg::ref_ptr<osg::Group> m_stlWanQuMo2Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlJiTouDiZuoFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlJiTouMoZuoFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlJiTouMoZuo1Father;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlZhongXinMoFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlWanBiFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlYouFuTuiFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlZuoFuTuiFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlJiShenFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::Group> m_stlDiMianFather;  // 机头弯管1父节点
	osg::ref_ptr<osg::MatrixTransform> m_straiclipmtrx;

    // 长直管加工时，可能是加工左侧，也可能加工右侧
	osg::ref_ptr<osg::Group> m_stlPipeFatherRight;  // 管件模型右侧父节点
    osg::ref_ptr<osg::Group> m_stlPipeFatherLeft;  // 管件模型左侧父节点

	//osg::ref_ptr<osg::MatrixTransform> m_mtrixPipeFather;  // 管件模型父节点下变换矩阵

	osg::ref_ptr<osg::MatrixTransform> m_scalemtrix;//导夹2
	osg::ref_ptr<osg::MatrixTransform> m_DaoJia2_1scalemtrix;//导夹2_1
	osg::ref_ptr<osg::MatrixTransform> m_ZhuJia2scalemtrix;//导夹2_1

	osg::ref_ptr<osg::MatrixTransform> m_ZhuaLiaoMoscalemtrix;

	// 各模型变换矩阵父节点
	osg::ref_ptr<osg::MatrixTransform> m_mtrixXiaoCheScaleFather;
	osg::ref_ptr<osg::MatrixTransform> m_mtrixXiaoCheFather;  // 小车父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixJiTouWanGuan1Father; // 机头弯管1父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixJiTouWanGuan2Father; // 机头弯管2父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixJiTouWanGuan3Father; // 机头弯管3父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixZhuJia1Father; // 主夹1父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixAllZhuJia2; // 主夹1父节点变换矩阵

	osg::ref_ptr<osg::MatrixTransform> m_mtrixZhuJia2Father; // 主夹2父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixDaoJia1Father; // 导夹1父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixDaoJia1_1Father; // 导夹1父节点变换矩阵

	osg::ref_ptr<osg::MatrixTransform> m_mtrixDaoJia2Father; // 导夹2父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixDaoJia2_1Father; // 导夹2父节点变换矩阵

	osg::ref_ptr<osg::MatrixTransform> m_mtrixWanQuMo1Father; 
	osg::ref_ptr<osg::MatrixTransform> m_mtrixWanQuMo1_1Father; 

	osg::ref_ptr<osg::MatrixTransform> m_mtrixWanQuMo2Father; 
	osg::ref_ptr<osg::MatrixTransform> m_mtrixWanQuMo2PosFather; 

	osg::ref_ptr<osg::MatrixTransform> m_mtrixJiTouDiZuoFather; 
	osg::ref_ptr<osg::MatrixTransform> m_mtrixJiTouMoZuoFather;
	osg::ref_ptr<osg::MatrixTransform> m_mtrixJiTouMoZuo1Father;

	osg::ref_ptr<osg::MatrixTransform> m_mtrixZhongXinMoFather;
	osg::ref_ptr<osg::MatrixTransform> m_mtrixZhuaLiaoMoFather; // 抓料模父节点变换矩阵
	osg::ref_ptr<osg::MatrixTransform> m_mtrixZhuaMoLiaoFather; // 抓料模父节点变换矩阵	

	osg::ref_ptr<osg::PositionAttitudeTransform> m_mtrixPipeFatherRight;   //管子父节点变换矩阵
    osg::ref_ptr<osg::PositionAttitudeTransform> m_mtrixPipeFatherLeft;   //管子父节点变换矩阵

	osg::ref_ptr<osg::MatrixTransform> m_mtrixWanBiFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixYouFuTuiFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixZuoFuTuiFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixJiShenFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixDiMianFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixZhuaLiaoMoScaleFather;
	osg::ref_ptr<osg::MatrixTransform> m_mtrixLunMoFather;

	osg::ref_ptr<osg::MatrixTransform> m_mtrixCheTouFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixDaoLun1Father;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixDaoLun2Father;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixZhuLunFather; 
	//scale
	osg::ref_ptr<osg::MatrixTransform> m_mtrixCheTouScaleFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixDaoLun1ScaleFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixDaoLun2ScaleFather;  
	osg::ref_ptr<osg::MatrixTransform> m_mtrixZhuLunScaleFather; 

	osg::ref_ptr<osg::MatrixTransform> m_transformSpinScene;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_PosTransB;

	osg::ref_ptr<osg::MatrixTransform> m_transformMoveY;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_PosTransY;

	osg::ref_ptr<osg::MatrixTransform> m_transformRotateC;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_PosTransC;
	//osg::MatrixTransform* m_transformSpinObject;

	osg::ref_ptr<osg::Node> m_currentData;

	osg::LightSource *m_defautLightSource;

	bool m_isHighlightScene;
	bool m_isShadowScene;
	bool m_isTextrueScene;
	bool m_isLightScene;
	bool m_isBackface;

	osg::Vec3 m_baseLightPosition;

	osg::ref_ptr<osgFX::Scribe> m_currentHightlight;

	osg::Vec3 m_sceneCenter;

	// root node of the model
	//osg::ref_ptr<osg::Group> m_scene;
	osg::ref_ptr<osgShadow::ShadowedScene> m_rootShadowNode;

	std::map<QString,osg::Node*> m_NameToNodeMap;  // 节点和名称映射表
	std::map<int,osg::ref_ptr<osg::Group>> m_NumberToPipeMap;  // 管子和编号映射表
	std::vector<osg::ref_ptr<osg::Group>> m_Pipes;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_PipesMatrix;  // 管子部件变换矩阵数组
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_curvepipemtrix; // 管子部件YBC变换中C值变换矩阵数组

	std::vector<osg::Group *> m_vAddStlNodes;//所有添加的stl节点
	std::vector<osg::Group *> m_vGNodes;//所有父节点
	std::map<osg::Group *,osg::Group *> m_FatherNodeToChildNode;

	int m_NodeType;//导入界面的节点类型设置
	int m_NodeOpeType;//节点运动类型
	QString m_strFatherNodeName;
	QString m_strAddNodeName;
	std::map<std::string,std::string> m_ChildToFatherMap;//stl节点与父节点map
	osg::ref_ptr<osg::MatrixTransform> m_moveTrans;
	osg::ref_ptr<osg::MatrixTransform> m_rotateTrans;

	int m_iPipeNum;
	osg::ref_ptr<HUDText> m_HudText;
	float m_z;

};

#endif // _OSGVIEWER_XSCENEMODEL_H_
