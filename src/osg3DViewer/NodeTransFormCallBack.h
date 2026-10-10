#ifndef _OSGVIEWER_NODETRANSFORMCALLBACK_H_
#define _OSGVIEWER_NODETRANSFORMCALLBACK_H_
#pragma once

#include "Novadef.h"

#include <osg/MatrixTransform>
#include <osg/AnimationPath>
#include "HUDText.h"
#include "Pipe.h"
#include "Table.h"
#include <osg/ClipPlane>

#include "log.h"

#include <osgCollisionDetection/CollisionInterface>

// #include "MainWindow.h"

class MainWindow;

//#define Pi 3.14159265358979323846

osg::ref_ptr<osg::AnimationPathCallback> getModelMoveCallBack(const std::vector<AnimationInfo>& infos);

osg::ref_ptr<osg::AnimationPathCallback> getModelMoveCallBackNew(const std::vector<AnimationInfo>& infos);

osg::ref_ptr<osg::AnimationPathCallback> getModelMoveCallBackNewNew(const std::vector<AnimationInfo>& infos);

osg::ref_ptr<osg::AnimationPathCallback> getModelRotateCallBack(const std::vector<AnimationInfo>& infos);
osg::ref_ptr<osg::AnimationPathCallback> getModelSecondRotateCallBack(const std::vector<AnimationInfo>& infos);

// 平移加旋转
osg::ref_ptr<osg::AnimationPathCallback> getModelMoveRotateCallBack(const std::vector<AnimationInfo>& move,const std::vector<AnimationInfo>& rotate);


class LongStraightPipe : public osg::NodeCallback
{
	public :

	LongStraightPipe(double time,osg::ref_ptr<osg::Group> obj,osg::PositionAttitudeTransform* trans,
		const std::vector<PIPE>& pipes,osg::ref_ptr<osg::ClipPlane> clipPlane)
	: m_showTime(time)
	, m_Pipenode(obj)
	, m_Trans(trans)
	, m_bShowed(false)
	, m_vPipes(pipes)
	, m_clipPlane(clipPlane)
	{

	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::PositionAttitudeTransform *mt = /*m_Trans*/dynamic_cast<osg::PositionAttitudeTransform*>(node);
		if (mt)
		{
			 osg::AnimationPathCallback* back = dynamic_cast<osg::AnimationPathCallback*>(mt->getUpdateCallback());
			//osg::AnimationPathCallback* back = dynamic_cast<osg::AnimationPathCallback*>(this);
			if (back)
			{
				double time = back->getAnimationTime();
				//bool isPause = back->getPause();
				if (!m_bShowed && time>m_showTime)
				{
					m_Pipenode->setNodeMask(-1);

					for (size_t i = 0; i < m_vPipes.size();++i)
					{
						const PIPE& pipe = m_vPipes[i];
						if (pipe.GetType()==Straight)
						{
							//pipe.GetPipeNode()->getOrCreateStateSet()->setAttributeAndModes(m_clipPlane, osg::StateAttribute::OFF);
						}

					}
					//m_Trans->addChild(m_Pipenode);
					m_bShowed = true;
				}
			}

		}

		traverse(node, nv);
	}

protected:
	osg::ref_ptr<osg::Geode> m_fathernode;
	osg::ref_ptr<osg::Group> m_Pipenode;
	const std::vector<PIPE>& m_vPipes;
	osg::PositionAttitudeTransform* m_Trans;
	bool m_bShowed;
	double m_showTime;
	osg::ref_ptr<osg::ClipPlane> m_clipPlane;

};

class PipeHide : public osg::NodeCallback
{
public:
	PipeHide(double time,osg::ref_ptr<osg::Geode> obj)
	: m_HideTime(time)
	, m_obj(obj)
	, m_bIsTrans(false)
	{
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::PositionAttitudeTransform *mt = dynamic_cast<osg::PositionAttitudeTransform*>(node);
		if (mt)
		{
			osg::AnimationPathCallback* back = dynamic_cast<osg::AnimationPathCallback*>(mt->getUpdateCallback());
			if (back)
			{
				double time = back->getAnimationTime();
				bool isPause = back->getPause();
				if (!isPause && !m_bIsTrans && time>=m_HideTime)
				{
					if (m_obj!=NULL)
					{
                        // 现在没有创建后退的管子，管子不用隐藏
						m_obj->setNodeMask(0);
					}

					m_bIsTrans = true;
				}
			}
		}

		traverse(node, nv);
	}

protected:
		osg::ref_ptr<osg::Geode> m_obj;
		double m_HideTime;
		bool m_bIsTrans;
};

struct StruMoveCenter
{
	double dStart;
	double dEnd;
	osg::Vec3 offset;
	bool skip;
};

// 回调函数，移动模型中心，解决设置好旋转中心后，模型发生了
// 平移，然后旋转中心不对的问题
class ModelCenterMove : public osg::NodeCallback
{
public:
	ModelCenterMove(osg::Vec3 center,std::vector<AnimationInfo> move,
		std::vector<AnimationInfo> rotate)
	{
		m_vCenter = center;
		m_vMove = move;
		m_vRotate = rotate;

		StruMoveCenter stru;
		stru.offset[0] = -28.47;
		stru.offset[2] = 106.25;
		stru.dStart = 28.0; 
		stru.dEnd = 40.0;
		stru.skip = false;
		m_vMoveCenter.push_back(stru);
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
		if (mt)
		{
			osg::AnimationPathCallback* back = dynamic_cast<osg::AnimationPathCallback*>(mt->getUpdateCallback());
			if (back)
			{
				double time = back->getAnimationTime();
				osg::Vec3 center = back->getPivotPoint();
				for (size_t i = 0; i < m_vMoveCenter.size();++i)
				{
					if (!m_vMoveCenter[i].skip && time>= m_vMoveCenter[i].dStart
						&& time < m_vMoveCenter[i].dEnd)
					{
						osg::Vec3 offset = m_vMoveCenter[i].offset;
						offset[0];
						offset[1];
						offset[2];
						osg::Vec3 center = /*m_vCenter-*/-offset;
						back->setPivotPoint(center);

						m_vMoveCenter[i].skip = true;
					}
				}

			}

		}

		traverse(node, nv);
	}

protected:
	osg::Vec3 m_vCenter;
	std::vector<AnimationInfo> m_vMove;
	std::vector<AnimationInfo> m_vRotate;

	std::vector<StruMoveCenter> m_vMoveCenter;
};


class PipeShow : public osg::NodeCallback
{
public:
	PipeShow(double time,PIPE* pipe,osg::ref_ptr<osg::Geode> obj,osg::ref_ptr<osg::ClipPlane> clipPlane,std::pair<int,int>* pairGroupMask)
		: m_HideTime(time)
		, m_pPipe(pipe)
		, m_obj(obj)
		, m_isHide(false)
		,m_clipPlane(clipPlane)
		, m_pairGroupMask(pairGroupMask)
	{
		//if (m_pPipe!=NULL)
		//{
		//	osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
		//	temp->addChild(m_pPipe->GetPipeNode());
		//	std::string name = m_pPipe->GetName().toStdString();
		//	osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(name, temp, m_pairGroupMask->first, m_pairGroupMask->second);

		//	LogInfo(QString("add collision pipe %1 group %2 mask %3").arg(name.c_str()).arg(m_pairGroupMask->first).arg(m_pairGroupMask->second));
		//}
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::PositionAttitudeTransform *mt = dynamic_cast<osg::PositionAttitudeTransform*>(node);
		if (mt)
		{
			osg::AnimationPathCallback* back = dynamic_cast<osg::AnimationPathCallback*>(mt->getUpdateCallback());
			if (back)
			{
				double time = back->getAnimationTime();
				bool isPause = back->getPause();
				if (!isPause && !m_isHide && time>=m_HideTime)
				{
					if (m_obj!=NULL)
					{
                        // 去掉剪切平面的作用，显示管子
						m_obj->getOrCreateStateSet()->setAttributeAndModes(m_clipPlane, osg::StateAttribute::OFF);
					}

					if (m_pPipe!=NULL)
					{
						osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
						temp->addChild(m_pPipe->GetPipeNode());

						m_pPipe->SetShowed(true);
					}

					m_isHide = true;
				}
			}
		}

		traverse(node, nv);
	}

protected:
	osg::ref_ptr<osg::Geode> m_obj;
	double m_HideTime;
	bool m_isHide;
	osg::ref_ptr<PIPE> m_pPipe;
	osg::ref_ptr<osg::ClipPlane> m_clipPlane;
	std::pair<int,int>* m_pairGroupMask;
};

class PipeInfoCallback : public osg::NodeCallback
{
public:
	PipeInfoCallback(double s,double e,int i,int iTw,bool Istw,MainWindow* window,bool autoplan);
	//	: m_TimeStart(s)
	//	, m_TimeEnd(e)
	//	, m_iCurrent(i)
	//	,m_bIsTw(Istw)
	//{
	//}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	//{
	//	TextTransform *mt = dynamic_cast<TextTransform*>(node);
	//	if (mt)
	//	{
	//		osg::AnimationPathCallback* back = dynamic_cast<osg::AnimationPathCallback*>(mt->getUpdateCallback());
	//		if (back)
	//		{
	//			double time = back->getAnimationTime();
	//			bool isPause = back->getPause();
	//			HUDText* hud = mt->GetHUDText();
	//			Table* tab = mt->GetTextTable();
	//			Table* twtable = mt->GetTWTable();
	//			if (tab!=NULL && !isPause && time>=m_TimeStart && time<m_TimeEnd && !m_bIsTw)
	//			{
	//				tab->SetRowItemsColor(m_iCurrent);
	//				tab->AddiText(m_iCurrent);
	//			}	

	//			if (twtable!=NULL && !isPause && time>=m_TimeStart && time<m_TimeEnd && m_bIsTw)
	//			{
	//				twtable->SetRowItemsColor(m_iCurrent);
	//			}

	//			if (m_iCurrent==(tab->rowCount()-2) && time > m_TimeEnd)
	//			{
	//				// 动画结束
	//				LogInfo("current row "+QString::number(m_iCurrent));
	//				LogInfo("tab row is "+QString::number(tab->rowCount()));
	//				LogInfo("animation end !");
	//			}
	//		}
	//	}

	//	traverse(node, nv);
	//}

	//osg::ref_ptr<osg::PositionAttitudeTransform> getTextCallBack(){return m_back;}

protected:

	double m_TimeStart;
	double m_TimeEnd;
	int m_iCurrent;
	int m_iCurrentTw;
	std::vector<int> itext;
	bool m_bIsTw;
	MainWindow* m_window;
	bool m_bFinished;  // 是否加工已经结束
    bool m_bAutoPlan;  // 是否加工自动优化
	//osg::ref_ptr<osg::PositionAttitudeTransform> m_back;
};


osg::ref_ptr<osg::AnimationPathCallback> getCheTouCallback(const std::vector<AnimationInfo>& infos);



osg::ref_ptr<osg::AnimationPathCallback> getCarPathCallback(const std::vector<AnimationInfo>& infos);
//class CarCallback : public osg::NodeCallback
//{
//public:
//	CarCallback(const std::vector<AnimationInfo>& info)
//	: m_info(info),
//	  _currentPos(0.0)
//	  //_start(start)
//	  {
//	  }
//
//	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	  {
//		  osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		  if (mt)
//		  {
//				osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//				pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//				for (int i = 0; i < m_info.size();++i)
//				{
//					const AnimationInfo& info = m_info[i];
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//					_currentPos += info.translate.x();
//					pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//				}
//
//				//创建更新动画回调
//				mt->setUpdateCallback(new osg::AnimationPathCallback(pAnimationPath.get()));
//		  }
//
//		  //traverse(node, nv);
//	  }
//
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	//osg::Vec3d  _axis;
//	//double      _pos;
//	//double      _start;
//	//double      _speed;
//	double      _currentPos;
//};
osg::ref_ptr<osg::AnimationPathCallback> getJiTouWanGuan2PathCallback(const std::vector<AnimationInfo>& infos);

osg::ref_ptr<osg::AnimationPathCallback> getJiTouWanGuan1PathCallback(const std::vector<AnimationInfo>& infos);
//class JiTouWanGuan1Callback : public osg::NodeCallback
//{
//public:
//	JiTouWanGuan1Callback(const std::vector<AnimationInfo>& info)
//	: m_info(info),
//	  _currentPos(0.0)
//	  {
//		  _lastPos = 0;
//	  }
//
//	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	  {
//		  osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		  if (mt)
//		  {
//			  osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			  pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			  for (int i = 0; i < m_info.size();++i)
//			  {
//				  const AnimationInfo& info = m_info[i];
//
//				//  _currentPos = 0.0;
//				  if(i == 0)
//				  {
//					  pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						  osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					  _lastPos = 0;
//				  }
//				  else
//				  {
//					  pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						  osg::Quat(0.0, osg::X_AXIS, _lastPos, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				  }
//
//				  _currentPos += info.rotateY;
//				  _lastPos = _currentPos;
//				  pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//					  osg::Quat(0.0, osg::X_AXIS, _currentPos, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//
//			  }
//				//创建更新动画回调
//
//			  m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			  mt->setUpdateCallback(m_AnimationPathCallBack);
//		  }
//	  }
//
//	  osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};


//class JiTouWanGuan1BackCallback : public osg::NodeCallback
//{
//public:
//	JiTouWanGuan1BackCallback(const std::vector<AnimationInfo>& info)
//	: m_info(info),
//	  _currentPos(0.0)
//	  {
//
//	  }
//
//	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	  {
//		  osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		  if (mt)
//		  {
//			  osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			  pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			  for (int i=0; i<m_info.size(); ++i)
//			  {
//				  const AnimationInfo& info = m_info[i];
//				  _currentPos = info.rotateX*(PI/180);
//				  pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//					  osg::Quat(0.0, osg::X_AXIS, _currentPos, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//
//				  _currentPos = 0.0;
//				  pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//					  osg::Quat(0.0, osg::X_AXIS, _currentPos, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//
//			  }
//
//
//				//创建更新动画回调
//			  m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			  mt->setUpdateCallback(m_AnimationPathCallBack);
//		  }
//
//		 //traverse(node, nv);
//	  }
//	  osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};

osg::ref_ptr<osg::AnimationPathCallback> getZhuaLiaoMoPathCallback(const std::vector<AnimationInfo>& infos);
//class ZhuaLiaoMoCallback : public osg::NodeCallback
//{
//public:
//	ZhuaLiaoMoCallback(const std::vector<AnimationInfo>& info)
//	: m_info(info),
//	  _currentPos(0.0)
//	  //_start(start)
//	  {
//
//	  }
//
//	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	  {
//		  osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		  if (mt)
//		  {
//			  osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			  pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			  for (int i = 0; i < m_info.size();++i)
//			  {
//				  const AnimationInfo& info = m_info[i];
//				  pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//					  osg::Quat(_currentPos, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//				  _currentPos += info.rotateX/*MYDEG_TO_RAD*/;
//				  pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//					  osg::Quat(_currentPos, osg::X_AXIS, 0.0, osg::Y_AXIS,0.0 , osg::Z_AXIS)));
//
//			  }
//
//
//				//创建更新动画回调
//
//			  m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			  mt->setUpdateCallback(m_AnimationPathCallBack);
//
//		  }
//
//	  }
//
//	  osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};

//class PipeCallback : public osg::NodeCallback
//{
//public:
//	PipeCallback(std::vector<AnimationInfo> info)
//	 : m_info(info),
//	  //m_clipNode(clipNode),
//	  _currentPos(0.0),
//	  _currentAngleX(0.0),
//	  _currentAngleY(0.0),
//      _wheelRadius(0.0)
//	  {
//	  }
//
//	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	  {
//		  osg::PositionAttitudeTransform *mt = dynamic_cast<osg::PositionAttitudeTransform*>(node);
//		  osg::Vec3 oldCenter;
//		  if (mt)
//		  {
//			  osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			  pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			  for (int i = 0; i < m_info.size();++i)
//			  {
//				  const AnimationInfo& info = m_info[i];
//
//				  if (info.translate.x()>0.0)
//				  {
//					    // 沿x轴前进
//					    pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, _wheelRadius), 
//                            osg::Quat(osg::DegreesToRadians(_currentAngleY), osg::X_AXIS, osg::DegreesToRadians(_currentAngleX), osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//						_currentPos += info.translate.x();
//						pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, _wheelRadius), 
//							osg::Quat(osg::DegreesToRadians(_currentAngleY), osg::X_AXIS, osg::DegreesToRadians(_currentAngleX), osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//				  }
//				  else if (info.rotateY != 0.0)
//				  {
//						// 沿Y轴旋转
//						osg::Vec3 center;
//						center = info.center3d/*osg::Vec3(0.0,54.984997,-29.57)*/;
//						pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center, 
//							osg::Quat( osg::DegreesToRadians(_currentAngleX), osg::Vec3f(0.0,info.fdir,0.0))));
//
//						_currentAngleX += info.rotateY;
//						pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
//							osg::Quat( osg::DegreesToRadians(_currentAngleX), osg::Vec3f(0.0,info.fdir,0.0))));
//
//						osg::AnimationPathCallback* nc = new osg::AnimationPathCallback(pAnimationPath);
//						nc->setPivotPoint(center);
//						mt->setUpdateCallback(nc);
//
//						//traverse(node, nv);
//
//						return;
//
//
//				  }
//				  else
//				  {
//					  // 沿Z轴旋转
//					  osg::Vec3 center;
//					  center = info.center3d/*osg::Vec3(0.0,-2.44,-64.4232)*/;
//					  pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center, 
//						  osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(1.0,0.0,0.0))));
//
//					  _currentAngleY += info.rotateX;
//					  pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
//						  osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(1.0,0.0,0.0))));
//
//
//					  osg::AnimationPathCallback* nc = new osg::AnimationPathCallback(pAnimationPath);
//
//					  nc->setPivotPoint(center);
//					  mt->setUpdateCallback(nc);
//
//					 // traverse(node, nv);
//
//					  return;
//
//				  }
//
//			  }	
//
//			  mt->setUpdateCallback(new osg::AnimationPathCallback(pAnimationPath.get()));
//			  
//		  }
//
//		  //traverse(node, nv);
//	  }
//
//protected:
//
//	//const std::vector<osg::ref_ptr<osg::Group>> m_pipes;
//	std::vector<AnimationInfo> m_info;
//	//osg::ref_ptr<osg::ClipNode> m_clipNode;
//	double      _currentPos;
//	double      _currentAngleX;  // 对应YBC文件B值
//	double      _currentAngleY;  // 对应YBC文件C值
//    double      _wheelRadius;    // 轮模半径
//};


osg::ref_ptr<osg::AnimationPathCallback> getPipePathCallBack(const std::vector<AnimationInfo>& infos);
osg::ref_ptr<osg::AnimationPathCallback> getWanQuMo2CallBack(const std::vector<AnimationInfo>& infos);

osg::ref_ptr<osg::AnimationPathCallback> getJitouWanGuan3CallBack(const std::vector<AnimationInfo>& infos);

class PipeCallbackNew : public osg::Node
{
public:
	PipeCallbackNew(std::vector<AnimationInfo> info)
		: m_info(info),
		//m_clipNode(clipNode),
		_currentPos(0.0),
		_currentAngleX(0.0),
		_currentAngleY(0.0),
		_wheelRadius(0.0)
	{
	}

	osg::ref_ptr<osg::AnimationPathCallback> getPipePathCallBack()
	{

			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

			for (size_t i = 0; i < m_info.size();++i)
			{
				const AnimationInfo& info = m_info[i];

				if (info.translate.x()>0.0)
				{
					// 沿x轴前进
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, _wheelRadius), 
						osg::Quat(osg::DegreesToRadians(_currentAngleY), osg::X_AXIS, osg::DegreesToRadians(_currentAngleX), osg::Y_AXIS, 0.0, osg::Z_AXIS)));

					_currentPos += info.translate.x();
					pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, _wheelRadius), 
						osg::Quat(osg::DegreesToRadians(_currentAngleY), osg::X_AXIS, osg::DegreesToRadians(_currentAngleX), osg::Y_AXIS, 0.0, osg::Z_AXIS)));
				}
				else if (info.rotateY != 0.0)
				{
					// 沿Y轴旋转
					osg::Vec3 center;
					center = info.center3d/*osg::Vec3(0.0,54.984997,-29.57)*/;
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center, 
						osg::Quat( osg::DegreesToRadians(_currentAngleX), osg::Vec3f(0.0,info.fdir,0.0))));

					_currentAngleX += info.rotateY;
					pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
						osg::Quat( osg::DegreesToRadians(_currentAngleX), osg::Vec3f(0.0,info.fdir,0.0))));

					osg::ref_ptr<osg::AnimationPathCallback> nc = new osg::AnimationPathCallback(pAnimationPath);
					nc->setPivotPoint(center);

					//traverse(node, nv);

					return nc;
				}
				else
				{
					// 沿Z轴旋转
					osg::Vec3 center;
					center = info.center3d/*osg::Vec3(0.0,-2.44,-64.4232)*/;
					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(center, 
						osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(1.0,0.0,0.0))));

					_currentAngleY += info.rotateX;
					pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(center, 
						osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(1.0,0.0,0.0))));


					osg::ref_ptr<osg::AnimationPathCallback> nc = new osg::AnimationPathCallback(pAnimationPath);

					nc->setPivotPoint(center);
					//mt->setUpdateCallback(nc);

				   // traverse(node, nv);

					return nc;

				}

			}	

			//mt->setUpdateCallback(new osg::AnimationPathCallback(pAnimationPath.get()));
			return new osg::AnimationPathCallback(pAnimationPath);

	}

protected:

	//const std::vector<osg::ref_ptr<osg::Group>> m_pipes;
	std::vector<AnimationInfo> m_info;
	//osg::ref_ptr<osg::ClipNode> m_clipNode;
	double      _currentPos;
	double      _currentAngleX;  // 对应YBC文件B值
	double      _currentAngleY;  // 对应YBC文件C值
	double      _wheelRadius;    // 轮模半径
};

osg::ref_ptr<osg::AnimationPathCallback> getBackPipePathCallBack(const std::vector<AnimationInfo>& infos);
osg::ref_ptr<osg::AnimationPathCallback> getBackPipePathCallBack(const std::vector<AnimationInfo>& infos);


osg::ref_ptr<osg::AnimationPathCallback> getLongStaightPipePathCallBack(const std::vector<AnimationInfo>& infos);


osg::ref_ptr<osg::AnimationPathCallback> getZhuJia2PathCallBack(const std::vector<AnimationInfo>& infos);
//class ZhuJia2Callback : public osg::NodeCallback
//{
//public:
//	ZhuJia2Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};

osg::ref_ptr<osg::AnimationPathCallback> getZhuJia1PathCallBack(const std::vector<AnimationInfo>& infos);
//class ZhuJia1Callback : public osg::NodeCallback
//{
//public:
//	ZhuJia1Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPosY(0.0),
//		_currentPosZ(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, _currentPosY, _currentPosZ), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//				_currentPosY += info.translate.y();
//				_currentPosZ += info.translate.z();
//
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, _currentPosY, _currentPosZ), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPosY;
//	double      _currentPosZ;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};

osg::ref_ptr<osg::AnimationPathCallback> getDaoJia2PathCallBack(const std::vector<AnimationInfo>& infos);
//class DaoJia2Callback : public osg::NodeCallback
//{
//public:
//	DaoJia2Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//				
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};

osg::ref_ptr<osg::AnimationPathCallback> getDaoJia2_1PathCallBack(const std::vector<AnimationInfo>& infos);

//class DaoJia2_1Callback : public osg::NodeCallback
//{
//public:
//	DaoJia2_1Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};

osg::ref_ptr<osg::AnimationPathCallback> getDaoJia1PathCallBack(const std::vector<AnimationInfo>& infos);
//class DaoJia1Callback : public osg::NodeCallback
//{
//public:
//	DaoJia1Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//				_currentPos += info.translate.z();
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};

osg::ref_ptr<osg::AnimationPathCallback> getDaoJia1_1PathCallBack(const std::vector<AnimationInfo>& infos);
//class DaoJia1_1Callback : public osg::NodeCallback
//{
//public:
//	DaoJia1_1Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//				_currentPos += info.translate.z();
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack; 
//};

osg::ref_ptr<osg::AnimationPathCallback> getLDaoJia0PathCallBack(const std::vector<AnimationInfo>& infos);
//class LDaoJia0Callback : public osg::NodeCallback
//{
//public:
//	LDaoJia0Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//};

osg::ref_ptr<osg::AnimationPathCallback> getLDaoJia1PathCallBack(const std::vector<AnimationInfo>& infos);
//class LDaoJia1Callback : public osg::NodeCallback
//{
//public:
//	LDaoJia1Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//};
osg::ref_ptr<osg::AnimationPathCallback> getLDaoJia2PathCallBack(const std::vector<AnimationInfo>& infos);
//class LDaoJia2Callback : public osg::NodeCallback
//{
//public:
//	LDaoJia2Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//
//protected:
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//};

osg::ref_ptr<osg::AnimationPathCallback> getLDaoJia3PathCallBack(const std::vector<AnimationInfo>& infos);
//class LDaoJia3Callback : public osg::NodeCallback
//{
//public:
//	LDaoJia3Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//};

osg::ref_ptr<osg::AnimationPathCallback> getRDaoJia0PathCallBack(const std::vector<AnimationInfo>& infos);
//class RDaoJia0Callback : public osg::NodeCallback
//{
//public:
//	RDaoJia0Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//};

osg::ref_ptr<osg::AnimationPathCallback> getRDaoJia1PathCallBack(const std::vector<AnimationInfo>& infos);
//class RDaoJia1Callback : public osg::NodeCallback
//{
//public:
//	RDaoJia1Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//};
osg::ref_ptr<osg::AnimationPathCallback> getRDaoJia2PathCallBack(const std::vector<AnimationInfo>& infos);
//class RDaoJia2Callback : public osg::NodeCallback
//{
//public:
//	RDaoJia2Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//};

osg::ref_ptr<osg::AnimationPathCallback> getRDaoJia3PathCallBack(const std::vector<AnimationInfo>& infos);
//class RDaoJia3Callback : public osg::NodeCallback
//{
//public:
//	RDaoJia3Callback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0),
//		_lastPos(0.0)
//	{
//
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//
//				if(i == 0)
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//					_lastPos = 0;
//				}
//				else
//				{
//					pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos, 0.0, 0.0), 
//						osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
//				}
//
//				_currentPos += info.translate.x();
//				_lastPos = _currentPos;
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos, 0.0, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//	double      _lastPos;
//};


osg::ref_ptr<osg::AnimationPathCallback> getZuoFuTuiPathCallBack(const std::vector<AnimationInfo>& infos);
//class ZuoFuTuiCallback : public osg::NodeCallback
//{
//public:
//	ZuoFuTuiCallback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		//_axis(axis),
//		//_pos(pos),
//		//_speed(5.0),
//		_currentPosX(0.0),
//		_currentPosY(0.0)
//		//_start(start)
//	{
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPosX, _currentPosY, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//				_currentPosX += info.translate.x();
//				_currentPosY += info.translate.x();
//
//
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPosX, _currentPosY, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			m_AnimationPathCallBack = new osg::AnimationPathCallback(pAnimationPath.get());
//			mt->setUpdateCallback(m_AnimationPathCallBack);
//		}
//
//		//traverse(node, nv);
//	}
//	osg::AnimationPathCallback *GetAnimationPathCallBack(){return m_AnimationPathCallBack;}
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	osg::AnimationPathCallback *m_AnimationPathCallBack;
//	double      _currentPosX;
//	double      _currentPosY;
//
//};
osg::ref_ptr<osg::AnimationPathCallback> getYouFuTuiPathCallBack(const std::vector<AnimationInfo>& infos);
//class YouFuTuiCallback : public osg::NodeCallback
//{
//public:
//	YouFuTuiCallback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		//_axis(axis),
//		//_pos(pos),
//		//_speed(5.0),
//		_currentPosX(0.0),
//		_currentPosY(0.0)
//		//_start(start)
//	{
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPosX, _currentPosY, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//				_currentPosX += info.translate.x();
//				_currentPosY += info.translate.x();
//
//
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPosX, _currentPosY, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			//创建更新动画回调
//			mt->setUpdateCallback(new osg::AnimationPathCallback(pAnimationPath.get()));
//		}
//
//		//traverse(node, nv);
//	}
//
//
//protected:
//
//	const std::vector<AnimationInfo>& m_info;
//	//osg::Vec3d  _axis;
//	//double      _pos;
//	//double      _start;
//	//double      _speed;
//	double      _currentPosX;
//	double      _currentPosY;
//};
osg::ref_ptr<osg::AnimationPathCallback> getJiTouMoZuoPathCallBack(const std::vector<AnimationInfo>& infos);
//class JiTouMoZuoCallback : public osg::NodeCallback
//{
//public:
//	JiTouMoZuoCallback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0)
//	{
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//				_currentPos += info.translate.z();
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, _currentPos), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			mt->setUpdateCallback(new osg::AnimationPathCallback(pAnimationPath.get()));
//		}
//	}
//
//protected:
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//};
osg::ref_ptr<osg::AnimationPathCallback> getJiTouDiZuoPathCallBack(const std::vector<AnimationInfo>& infos);
//class JiTouDiZuoCallback : public osg::NodeCallback
//{
//public:
//	JiTouDiZuoCallback(const std::vector<AnimationInfo>& info)
//		: m_info(info),
//		_currentPos(0.0)
//	{
//	}
//
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
//	{
//		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*>(node);
//		if (mt)
//		{
//			osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
//			pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
//
//			for (int i = 0; i < m_info.size();++i)
//			{
//				const AnimationInfo& info = m_info[i];
//				pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, _currentPos,0.0 ), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//
//				_currentPos += info.translate.y();
//				pAnimationPath->insert(info.fEndTime, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, _currentPos, 0.0), 
//					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Y_AXIS, 0.0, osg::Z_AXIS)));
//			}
//
//			mt->setUpdateCallback(new osg::AnimationPathCallback(pAnimationPath.get()));
//		}
//	}
//
//protected:
//	const std::vector<AnimationInfo>& m_info;
//	double      _currentPos;
//};

#endif