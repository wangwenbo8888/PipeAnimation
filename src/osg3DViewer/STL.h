#ifndef __STL_H__
#define __STL_H__

#include <osg/Shape>
#include <osg/Group>
#include <osg/Geode>

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <QtCore/QObject>
#include <QColor>
#include "Novadef.h"
#include <osg/Node>
#include <osg/Material>


using namespace std;

class STL
{
public:

	// 纯节点对象
	STL(std::string name);

	STL(osg::ref_ptr<osg::Node> node,  std::string name);
	STL(osg::Node * node, osg::ref_ptr<osg::Material> material,std::string name);
	void replaceCurNode(osg::Node * curNode);
	void replaceCurNodeBySpecial(STL* stl);

	void AddMoveAnimation(AnimationInfo& info);
	void AddRotateAnimation(AnimationInfo& info);
	void AddSecondRotateAnimation(AnimationInfo& info);
	void RemoveCurNode();

	void SetMatrix(STL* stl);
    void SaveNormalMatrix();
	void ResumeNormalMatrix();

	std::vector<AnimationInfo> GetVMoveAnimation();
	std::vector<AnimationInfo> GetVRotateAnimation();
	std::vector<AnimationInfo> GetSecondVRotateAnimation();

	void ClearMoveAnimation();
	void ClearRotateAnimation();
	void ClearAnimation();
	void ResetAbsoluteAngle();
	void ResetAbsolutePos(osg::Vec3 pos);

	void SetPosMatrix(osg::ref_ptr<osg::MatrixTransform> Matrix ){m_PosMatrix = Matrix; }
	
	/*osg::ref_ptr<osg::MatrixTransform> m_MoveMatrix;
	osg::ref_ptr<osg::MatrixTransform> m_RotateMatrix;
	osg::ref_ptr<osg::MatrixTransform> m_SecondRotateMatrix;
	osg::ref_ptr<osg::MatrixTransform> m_PosMatrix;*/
	

	osg::ref_ptr<osg::MatrixTransform> /*osg::ref_ptr<osg::PositionAttitudeTransform>*/ GetModelMatrix(){return m_ModelMatrix;}
	osg::ref_ptr<osg::MatrixTransform> GetModelRotateMatrix(){return m_RotateMatrix;}

	osg::ref_ptr<osg::MatrixTransform> GetModelMidRotateMatrix(){return m_MidRotateMatrix;}

	osg::ref_ptr<osg::MatrixTransform> GetModelTestRotateMatrix(){return m_TestRotateMatrix;}

	osg::ref_ptr<osg::MatrixTransform> GetModelLeftRotateMatrix(){return m_LeftRotateMatrix;}

	osg::ref_ptr<osg::MatrixTransform> GetModelSecondRotateMatrix(){return m_SecondRotateMatrix;}
	osg::ref_ptr<osg::MatrixTransform> GetModelMoveMatrix(){return m_MoveMatrix;}

	std::vector<osg::ref_ptr<osg::MatrixTransform>>& GetTransMatrix(){return m_vTransMatrix;}

	osg::ref_ptr<osg::MatrixTransform> GetPosMatrix(){return m_PosMatrix;}
	osg::ref_ptr<osg::MatrixTransform> GetScaleMatrix(){return m_ScaleMatrix;}
	void SetSelected();
	void  ResumeColor();

	void SetColor(osg::Vec4f color);
	osg::ref_ptr<osg::Material> GetMaterial(){return m_pMaterial;}
	void SetMaterial(osg::ref_ptr<osg::Material> mat);

	osg::ref_ptr<osg::Material> GetOriginalMaterial(){return m_pOriginalMaterial;}
	void SetOriginalMaterial(osg::ref_ptr<osg::Material> mat);

	void SetLMHOLE_R(float r);
	float GetLMHOLE_R(){return m_hole_r;}

	void SetSTLColor(osg::Vec4 color);
	osg::Vec4 GetSTLColor();
	void SetModelCenter(osg::Vec3 center);
	osg::Vec3 GetCurModelCenter(){return m_3dModelCenter;}

	void SetModelOriginalCenter(osg::Vec3 center);
	osg::Vec3 GetModelOriginalCenter(){return m_3dOriginalCenter;}

	void SetModelCopyFatherCenter(osg::Vec3 center){m_CopyFatherCenter = center;}
	osg::Vec3 GetModelCopyFatherCenter(){return m_CopyFatherCenter;}

	void SetCurLayer(int layer);
	int GetCurLayer(){return m_curLayer;}

	QString GetSTLName(){return m_stlName;}
	osg::Node* GetSTLNode();
	void SetRangeMax(double max);
	void SetRangeMin(double min);
	double GetRangeMax();
	double GetRangeMin();


	void SetIsAtLoadModel(bool var);
	bool GetIsAtLoadModel(){return m_isAtLoadModel;}

	void SetJMLen(float len);
	float GetJMLen(){return m_JMLen;}
	void SetJMWide(float wide);
	float GetJMWide(){return m_JMWide;}

	void SetJMOriginalLen(float len);
	float GetJMOriginalLen() {return m_OriginalJMLen;}
	void SetJMCurLen(float len);
	float GetJMCurLen() {return m_CurJMLen;}

	void SetLMr(float r);
	float GetLMr() {return m_LMr;}

	void SetLMShow(int t);
	int GetIsShow(){return m_iShow;}

	void SetLMupLen(float up);
	float GetLMupLen() {return m_LMupLen;}
	void SetLMdownLen(float down);
	float GetLMdownLen() {return m_LMdownLen;}

	void SetLMtuLen(float tu);
	float GetLMtuLen() {return m_LMtuLen;}

	void SetType(QString type);
	QString GetType() {return m_strType;}

	void SetSpecialType(int specialType);
	int GetSpecialType(){return m_iSpecialType;}

	void SetJMAoSide(int side);
	int GetJMAoSide() {return m_iJMAoSide;}

	void SetJMAid(int aid);
	int GetJMAid() {return m_iJMAid;}

	void SetLMTuSide(int side);
	int GetLMTuSide() {return m_LMTuSide;}
	void SetLMNthJT(int ith);
	int GetLMNthJT() {return m_LMNthJT;}

	void SetLastUpUnit(int unit);
	int GetLastUpUnit(){return m_iLastUpUnit;}
	void SetCurUpUnit(int unit);
	int GetCurUpUnit(){return m_iCurUpUnit;}

	void SetIsNew(bool val){m_isNewWheel = val;}
	bool GetIsNew(){return m_isNewWheel;}

	void SetAsSpecialReplaceModel(int val){m_iAsSpecialReplaceModel = val;}
	int GetAsSpecialReplaceModel(){return m_iAsSpecialReplaceModel;}


	void SetCanChanged(int val){m_canChanged = val;}
	int GetCanChanged(){return m_canChanged;}

	void SetShift(double shift){m_dShift = shift;}
	double GetShift(){return m_dShift;}

	void SetSTLAbsolutePos(osg::Vec3 pos);
	void SetSTLAbsolutePosX(float pos_x);
	void SetSTLAbsolutePosY(float pos_y);
	void SetSTLAbsolutePosZ(float pos_z);

	float GetSTLAbsolutePosX(){return m_STLAbsolutePos.x();}
	float GetSTLAbsolutePosY(){return m_STLAbsolutePos.y();}
	float GetSTLAbsolutePosZ(){return m_STLAbsolutePos.z();}

	osg::Vec3 GetSTLAbsolutePos(){return m_STLAbsolutePos;}

	osg::Vec3& GetCurAbsolutePos(){return m_STLAbsolutePos;}

	void SetSTLAbsoluteAngle(float angle);
	float GetSTLAbsoluteAngle(){return m_AbsoluteAngle;}

private:
	int m_iAsSpecialReplaceModel;
	int m_canChanged;
	float m_hole_r;
	float m_AbsoluteAngle;
	osg::Vec3 m_STLAbsolutePos;//当前的绝对位置
	osg::Vec4 m_color;
	QString m_stlName;
	QString m_strType;
	int m_iSpecialType;      // 被替换模型类型，0 可变形状模型，1 不可变形状也不可变位置模型，2 不可变形状，可变位置模型
	osg::Node *m_Node;
	float m_LMr;
	float m_LMupLen;
	float m_LMdownLen;
	float m_LMtuLen;
	int  m_LMTuSide;
	int m_iShow;
	int m_LMNthJT;
	float m_OriginalJMLen;
	float m_CurJMLen;
	float m_JMLen;
	float m_JMWide;
	int m_iJMAoSide;
	int m_iJMAid;
	int m_curLayer;

	double m_dShift;  // 因为调整管径造成的平移

	bool m_isAtLoadModel;//模型是否是在打开软件的时候就导入的

	bool m_bSavedNormalMatrix;   // 是否保存了通用模型对应的变换矩阵
	bool m_bCurModelSpecial;     // 当前STL对象对应模型是否是特殊模型

	int m_iLastUpUnit;
	int m_iCurUpUnit;

	bool m_isNewWheel;

	double m_dRangeMax;
	double m_dRangeMin;
	osg::Vec3 m_3dModelCenter;//当前的位置
	osg::Vec3 m_3dOriginalCenter;//最初的位置
	osg::Vec3 m_CopyFatherCenter;
	osg::ref_ptr<osg::Material> m_pMaterial;
	osg::ref_ptr<osg::Material> m_pOriginalMaterial;


	std::vector<AnimationInfo> m_vModelMoveAnimation;
	std::vector<AnimationInfo> m_vModelRotateAnimation;
	std::vector<AnimationInfo> m_vModelSecondRotateAnimation;

	AnimationInfo m_AnimationInfo; 

	osg::ref_ptr<osg::MatrixTransform> /*osg::ref_ptr<osg::PositionAttitudeTransform>*/ m_ModelMatrix;
	//osg::ref_ptr<osg::MatrixTransform> m_ModelMatrix;
	osg::ref_ptr<osg::MatrixTransform> m_ScaleMatrix;
	osg::ref_ptr<osg::MatrixTransform> m_MoveMatrix;
	osg::ref_ptr<osg::MatrixTransform> m_RotateMatrix;       // 右侧旋转矩阵
	osg::ref_ptr<osg::MatrixTransform> m_MidRotateMatrix;    // 中间旋转矩阵
	osg::ref_ptr<osg::MatrixTransform> m_LeftRotateMatrix;   // 左侧旋转矩阵

	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_vTransMatrix; 

	osg::ref_ptr<osg::MatrixTransform> m_TestRotateMatrix;   // 测试旋转矩阵

	osg::ref_ptr<osg::MatrixTransform> m_SecondRotateMatrix;
	osg::ref_ptr<osg::MatrixTransform> m_PosMatrix;

	osg::ref_ptr<osg::MatrixTransform> /*osg::ref_ptr<osg::PositionAttitudeTransform>*/ m_ModelMatrix_bak;
	osg::ref_ptr<osg::MatrixTransform> m_ScaleMatrix_bak;
	osg::ref_ptr<osg::MatrixTransform> m_MoveMatrix_bak;
	osg::ref_ptr<osg::MatrixTransform> m_RotateMatrix_bak;
	osg::ref_ptr<osg::MatrixTransform> m_MidRotateMatrix_bak;
	osg::ref_ptr<osg::MatrixTransform> m_LeftRotateMatrix_bak;
	osg::ref_ptr<osg::MatrixTransform> m_SecondRotateMatrix_bak;
	osg::ref_ptr<osg::MatrixTransform> m_PosMatrix_bak;

};

#endif
