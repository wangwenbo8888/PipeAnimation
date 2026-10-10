#include "STL.h"

#include <osg/Node>
#include <osg/Geode>//Geode是专门管理可绘制几何体的总闸
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/Material>
#include <osgUtil/Optimizer>


// 保存普通模型对应的变换矩阵
void STL::SaveNormalMatrix()
{
	m_ModelMatrix_bak = new osg::MatrixTransform;
	m_MoveMatrix_bak = new osg::MatrixTransform;
	m_ScaleMatrix_bak = new osg::MatrixTransform;
	m_RotateMatrix_bak = new osg::MatrixTransform;
	m_MidRotateMatrix_bak = new osg::MatrixTransform;
	m_LeftRotateMatrix_bak = new osg::MatrixTransform;
	m_SecondRotateMatrix_bak = new osg::MatrixTransform;
	m_PosMatrix_bak = new osg::MatrixTransform; 

	osg::Matrix mat = m_ModelMatrix->getMatrix();
	m_ModelMatrix_bak->setMatrix(mat);

	mat = m_MoveMatrix->getMatrix();
	m_MoveMatrix_bak->setMatrix(mat);

	mat = m_ScaleMatrix->getMatrix();
	m_ScaleMatrix_bak->setMatrix(mat);

	mat = m_RotateMatrix->getMatrix();
	m_RotateMatrix_bak->setMatrix(mat);

	mat = m_MidRotateMatrix->getMatrix();
	m_MidRotateMatrix_bak->setMatrix(mat);

	mat = m_LeftRotateMatrix->getMatrix();
	m_LeftRotateMatrix_bak->setMatrix(mat);

	mat = m_SecondRotateMatrix->getMatrix();
	m_SecondRotateMatrix_bak->setMatrix(mat);

	mat = m_PosMatrix->getMatrix();
	m_PosMatrix_bak->setMatrix(mat);
}

// 恢复非特殊模型对应的变换矩阵
void STL::ResumeNormalMatrix()
{
	m_ModelMatrix->setMatrix(m_ModelMatrix_bak->getMatrix());
	m_MoveMatrix->setMatrix(m_MoveMatrix_bak->getMatrix());
	m_ScaleMatrix->setMatrix(m_ScaleMatrix_bak->getMatrix());
	m_RotateMatrix->setMatrix(m_RotateMatrix_bak->getMatrix());

	m_MidRotateMatrix->setMatrix(m_MidRotateMatrix_bak->getMatrix());
	m_LeftRotateMatrix->setMatrix(m_LeftRotateMatrix_bak->getMatrix());

	m_SecondRotateMatrix->setMatrix(m_SecondRotateMatrix_bak->getMatrix());
	m_PosMatrix->setMatrix(m_PosMatrix_bak->getMatrix()); 
}

// 设置STL对象变换矩阵为特殊模型矩阵
void STL::SetMatrix(STL* stl)
{
	m_ModelMatrix->setMatrix(stl->m_ModelMatrix->getMatrix());
	m_MoveMatrix->setMatrix(stl->m_MoveMatrix->getMatrix());
	m_ScaleMatrix->setMatrix(stl->m_ScaleMatrix->getMatrix());
	m_RotateMatrix->setMatrix(stl->m_RotateMatrix->getMatrix());

	m_MidRotateMatrix->setMatrix(stl->m_MidRotateMatrix->getMatrix());
	m_LeftRotateMatrix->setMatrix(stl->m_LeftRotateMatrix->getMatrix());

	m_SecondRotateMatrix->setMatrix(stl->m_SecondRotateMatrix->getMatrix());
	m_PosMatrix->setMatrix(stl->m_PosMatrix->getMatrix()); 
}

STL::STL(osg::Node * node, osg::ref_ptr<osg::Material> material,std::string name)
{
	m_AbsoluteAngle = 0.0;
	m_Node = node;
	m_pMaterial = material;
	m_pOriginalMaterial = material;
	m_iShow = 1;
	m_isAtLoadModel = false;
	m_stlName = QString::fromLocal8Bit(name.c_str());
	m_curLayer = 1;

	m_dShift = 0.0;

	m_ModelMatrix = new osg::MatrixTransform;
	m_ScaleMatrix = new osg::MatrixTransform;
	m_MoveMatrix = new osg::MatrixTransform;
	m_RotateMatrix = new osg::MatrixTransform;
	m_MidRotateMatrix = new osg::MatrixTransform;
	m_LeftRotateMatrix = new osg::MatrixTransform;

	m_TestRotateMatrix = new osg::MatrixTransform;

	m_SecondRotateMatrix = new osg::MatrixTransform;
	m_PosMatrix = new osg::MatrixTransform;
	m_PosMatrix->setName(name.c_str());

	m_ModelMatrix_bak = NULL;
	m_MoveMatrix_bak = NULL;
	m_ScaleMatrix_bak = NULL;
	m_RotateMatrix_bak = NULL;
	m_MidRotateMatrix_bak = NULL;
	m_LeftRotateMatrix_bak = NULL;

	m_SecondRotateMatrix_bak = NULL;
	m_PosMatrix_bak = NULL;

	m_bSavedNormalMatrix = false;
	m_bCurModelSpecial = false;

	m_ModelMatrix->addChild(m_MoveMatrix);
	//m_ScaleMatrix->addChild(m_MoveMatrix);
	m_MoveMatrix->addChild(m_SecondRotateMatrix);
	m_SecondRotateMatrix->addChild(m_RotateMatrix);
	m_RotateMatrix->addChild(m_MidRotateMatrix);
	m_MidRotateMatrix->addChild(m_LeftRotateMatrix);

	m_LeftRotateMatrix->addChild(m_TestRotateMatrix);

	m_TestRotateMatrix->addChild(m_PosMatrix);

	m_PosMatrix->addChild(m_Node);
}

STL::STL(osg::ref_ptr<osg::Node> node, std::string name)
{
	m_Node = node;

	m_dShift = 0.0;

	m_stlName = QString::fromLocal8Bit(name.c_str());

	m_ModelMatrix = new osg::MatrixTransform;
	m_MoveMatrix = new osg::MatrixTransform;
	m_RotateMatrix = new osg::MatrixTransform;

	m_MidRotateMatrix = new osg::MatrixTransform;
	m_LeftRotateMatrix = new osg::MatrixTransform;

	m_SecondRotateMatrix = new osg::MatrixTransform;
	m_PosMatrix = new osg::MatrixTransform; 

	m_ModelMatrix->addChild(m_MoveMatrix);
	m_MoveMatrix->addChild(m_SecondRotateMatrix);
	m_SecondRotateMatrix->addChild(m_RotateMatrix);

	m_RotateMatrix->addChild(m_MidRotateMatrix);
	m_MidRotateMatrix->addChild(m_LeftRotateMatrix);
	m_LeftRotateMatrix->addChild(m_PosMatrix);

	m_PosMatrix->addChild(m_Node);
}

STL::STL(std::string name)
{
	//m_Node = node;

	m_dShift = 0.0;

	m_stlName = QString::fromLocal8Bit(name.c_str());

	m_ModelMatrix = new osg::MatrixTransform;
	m_MoveMatrix = new osg::MatrixTransform;
	m_RotateMatrix = new osg::MatrixTransform;

	m_MidRotateMatrix = new osg::MatrixTransform;
	m_LeftRotateMatrix = new osg::MatrixTransform;

	m_SecondRotateMatrix = new osg::MatrixTransform;
	m_PosMatrix = new osg::MatrixTransform; 

	m_ModelMatrix->addChild(m_MoveMatrix);
	m_MoveMatrix->addChild(m_SecondRotateMatrix);
	m_SecondRotateMatrix->addChild(m_RotateMatrix);

	m_RotateMatrix->addChild(m_MidRotateMatrix);
	m_MidRotateMatrix->addChild(m_LeftRotateMatrix);
	m_LeftRotateMatrix->addChild(m_PosMatrix);

	//m_PosMatrix->addChild(m_Node);
}

void STL::replaceCurNode(osg::Node * curNode)
{
	//if (!m_bSavedNormalMatrix && !m_bCurModelSpecial)
	//{
	//	SaveNormalMatrix();
	//	m_bSavedNormalMatrix = true;
	//}

	if (m_bCurModelSpecial)
	{
		// 恢复通用模型变换矩阵
		ResumeNormalMatrix();
	}

	m_PosMatrix->replaceChild(m_Node,curNode);
	//m_ModelMatrix->addChild(curNode);
	m_Node = curNode;
	
	// 通用类型模型
	m_bCurModelSpecial = false;
	//m_Node = curNode;
	//m_Node->re
}

void STL::replaceCurNodeBySpecial(STL* stl)
{
	//SetBakMatrix();

	if (!m_bSavedNormalMatrix && !m_bCurModelSpecial)
	{
		SaveNormalMatrix();
		m_bSavedNormalMatrix = true;
	}

	SetMatrix(stl);

	//m_ModelMatrix->
	//m_ModelMatrix->addChild(curNode);
	m_PosMatrix->replaceChild(m_Node,stl->GetSTLNode());
	//m_Node->setNodeMask(0);
	m_Node = stl->GetSTLNode();

	// 特殊类型模型
	m_bCurModelSpecial = true;
}

void STL::AddMoveAnimation(AnimationInfo& info)
{
	//if(m_vModelMoveAnimation.empty())
	//{

	//}
	//else
	//{
	//	AnimationInfo& last = m_vModelMoveAnimation.back();
	//	if (info.fStartTime>last.fEndTime)
	//	{
	//		AnimationInfo temp;
	//		temp.fStartTime = last.fEndTime;
	//		temp.fEndTime = info.fStartTime;
	//		m_vModelMoveAnimation.push_back(temp);
	//	}
	//	
	//}
	
	m_vModelMoveAnimation.push_back(info);

}

void STL::AddRotateAnimation(AnimationInfo& info)
{
	//if (m_vModelRotateAnimation.empty() == false)
	//{
	//	AnimationInfo& last = m_vModelRotateAnimation.back();
	//	if (info.fStartTime>last.fEndTime)
	//	{
	//		AnimationInfo temp;
	//		temp.fStartTime = last.fEndTime;
	//		temp.fEndTime = info.fStartTime;
	//		m_vModelRotateAnimation.push_back(temp);
	//	}
	//}
	m_vModelRotateAnimation.push_back(info);

}

void STL::AddSecondRotateAnimation(AnimationInfo& info)
{
	//if (m_vModelSecondRotateAnimation.empty() == false)
	//{
	//	AnimationInfo& last = m_vModelSecondRotateAnimation.back();
	//	if (info.fStartTime>last.fEndTime)
	//	{
	//		AnimationInfo temp;
	//		temp.fStartTime = last.fEndTime;
	//		temp.fEndTime = info.fStartTime;
	//		m_vModelSecondRotateAnimation.push_back(temp);
	//	}
	//}

	m_vModelSecondRotateAnimation.push_back(info);
}

void STL::RemoveCurNode()
{
	m_PosMatrix->removeChild(m_Node);
}

std::vector<AnimationInfo> STL::GetVMoveAnimation()
{
	return m_vModelMoveAnimation;
}

std::vector<AnimationInfo> STL::GetVRotateAnimation()
{
	return m_vModelRotateAnimation;
}

std::vector<AnimationInfo> STL::GetSecondVRotateAnimation()
{
	return m_vModelSecondRotateAnimation;

}

void STL::ClearMoveAnimation()
{
	m_vModelMoveAnimation.clear();
}

void STL::ClearRotateAnimation()
{
	m_vModelRotateAnimation.clear();
	m_vModelSecondRotateAnimation.clear();
}

void STL::ClearAnimation()
{
	m_vModelMoveAnimation.clear();
	m_vModelRotateAnimation.clear();
}

void STL::ResetAbsoluteAngle()
{
	m_AbsoluteAngle = 0.0;
}

void STL::ResetAbsolutePos(osg::Vec3 pos)
{
	m_STLAbsolutePos = pos;
}

void STL::SetSelected()
{
	osg::ref_ptr<osg::Material> material = GetMaterial();
	if (material!=NULL)
	{
		material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0,0.0,0.0,1.0f));
		material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0,0.0,0.0,1.0f));
	}
}

void STL::ResumeColor()
{
	osg::ref_ptr<osg::Material> material = GetMaterial();
	if (material!=NULL)
	{
		material->setDiffuse( osg::Material::FRONT_AND_BACK, m_color);
		material->setAmbient( osg::Material::FRONT_AND_BACK, m_color);
	}
}

void STL::SetColor(osg::Vec4f color)
{
	m_color = color;

	osg::ref_ptr<osg::Material> material = GetMaterial();
	if (material!=NULL)
	{
		material->setDiffuse( osg::Material::FRONT_AND_BACK, m_color);
		material->setAmbient( osg::Material::FRONT_AND_BACK, m_color);
	}
}

void STL::SetMaterial(osg::ref_ptr<osg::Material> mat)
{
	m_pMaterial = mat;
}

void STL::SetOriginalMaterial(osg::ref_ptr<osg::Material> mat)
{
	m_pOriginalMaterial = mat;
}

void STL::SetLMHOLE_R(float r)
{
	m_hole_r = r;
}

void STL::SetSTLColor(osg::Vec4 color)
{
	m_color = color;
}

osg::Vec4 STL::GetSTLColor()
{
	return m_color;
}

void STL::SetModelCenter(osg::Vec3 center)
{
	m_3dModelCenter = center;
}

void STL::SetModelOriginalCenter(osg::Vec3 center)
{
	m_3dOriginalCenter = center;
}

void STL::SetCurLayer(int layer)
{
	m_curLayer = layer;
}

//void STL::SetSTLGeode(osg::ref_ptr<osg::Node> geo)
//{
//	m_Geode = geo;
//}
//
//osg::ref_ptr<osg::Node> STL::GetSTLGeode()
//{
//	return m_Geode.get();
//}

osg::Node* STL::GetSTLNode()
{
	return m_Node;
}

void STL::SetRangeMax(double max)
{
	m_dRangeMax = max;
}

void STL::SetRangeMin(double min)
{
	m_dRangeMin = min;
}

double STL::GetRangeMax()
{
	return m_dRangeMax;
}

double STL::GetRangeMin()
{
	return m_dRangeMin;
}

void STL::SetIsAtLoadModel(bool var)
{
	m_isAtLoadModel = var;
}

void STL::SetJMLen(float len)
{
	m_JMLen = len;
}

void STL::SetJMWide(float wide)
{
	m_JMWide = wide;
}

void STL::SetJMOriginalLen(float len)
{
	m_OriginalJMLen = len;
}

void STL::SetJMCurLen(float len)
{
	m_CurJMLen = len;
}

void STL::SetLMr(float r)
{
	m_LMr = r;
}

void STL::SetLMShow(int t)
{
	m_iShow = t;
}

void STL::SetLMupLen(float up)
{
	m_LMupLen = up;
}

void STL::SetLMdownLen(float down)
{
	m_LMdownLen = down;
}

void STL::SetLMtuLen(float tu)
{
	m_LMtuLen = tu;
}

void STL::SetType(QString type)
{
	m_strType = type;
}

void STL::SetSpecialType(int type)
{
	m_iSpecialType = type;
}

void STL::SetJMAoSide(int side)
{
	m_iJMAoSide = side;
}

void STL::SetJMAid(int aid)
{
	m_iJMAid = aid;
}

void STL::SetLMTuSide(int side)
{
	m_LMTuSide = side;
}

void STL::SetLMNthJT(int ith)
{
	m_LMNthJT = ith;
}

void STL::SetLastUpUnit(int unit)
{
	m_iLastUpUnit = unit;
}

void STL::SetCurUpUnit(int unit)
{
	m_iCurUpUnit = unit;
}

void STL::SetSTLAbsolutePos(osg::Vec3 pos)
{
	m_STLAbsolutePos = pos;

}

void STL::SetSTLAbsolutePosX(float pos_x)
{
	m_STLAbsolutePos.x() = pos_x;
}

void STL::SetSTLAbsolutePosY(float pos_y)
{
	m_STLAbsolutePos.y() = pos_y;
}

void STL::SetSTLAbsolutePosZ(float pos_z)
{
	m_STLAbsolutePos.z() = pos_z;
}

void STL::SetSTLAbsoluteAngle(float angle)
{
	m_AbsoluteAngle = angle;
}

