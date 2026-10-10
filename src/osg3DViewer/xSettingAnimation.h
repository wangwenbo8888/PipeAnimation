#pragma once
#include "qdialog.h"

#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QColor>
#include <QTextEdit>
#include <QCheckBox>
#include <iostream>
#include <osg/Node>

#include "STL.h"


enum ENUM_STRUCT_TYPE {_QSTRING, _INT,_FLOAT,_BOOL,_VECofFLOAT,_VEC3}; // 定义枚举结构体变量类型


using namespace std;

struct ProductionValue
{
	float fmy;//送料长度my
	float fmyb;//送料补偿myb
	float fmb;//转角角度mb
	float fmbb;//转角补偿mbb,
	float fmc;//弯管角度mc
	float fmcb;//弯管补偿mcb
	float fmm; //弯管模层mm
	float flr;//弯曲方向lr	0向左弯，1向右弯
	float fmyt;//Y轴脱模myt
	float fmz;//X轴脱模mz
	float fbtm;//提前转角btm
	float fctm;//提前退弯ctm
	float fdz;//动作顺序dz
	float fmyth;//2次Y轴脱模myth
	float flc;//动作流程lc
	float fmmd;//动作模层mmd
	float fjm;//夹模模式jm
	float fxb;//芯棒选择xb
	float fjy;//夹送位置jy
	float ftwy;//推弯弧长twy
	float ftwbj;//推弯半径twbj
	float ftwc;//进弯角度twc
	float ftwb;//推弯转管twb
	float ftwlc;//推弯流程号twlc

	ENUM_STRUCT_TYPE GetProductionValueType(int _column)
	{
		return _FLOAT;
	}

};

struct TWLC1
{
	float ftwy;
	float ftwbj;
	float ftwc;
	float ftwb;

	ENUM_STRUCT_TYPE GetTWLC1ValueType(int _column)
	{
		return _FLOAT;
	}
};

struct TWLC2
{
	float ftwy;
	float ftwbj;
	float ftwc;
	float ftwb;

	ENUM_STRUCT_TYPE GetTWLC2ValueType(int _column)
	{
		return _FLOAT;
	}
};

struct Param
{
	std::vector<float> LmRadius;    // 轮模r
	std::vector<float> LmAcRadius;  // 轮模中间的半径
	std::vector<int>  LmHeaderNum;  // 轮模对应的机头编号
	float fPipeDiameter;
	float fPipeLength;
	int imn;//弯数
	float fYdPos;//原点pos
	float fZgPos;//置管pos
	float fDywPos;//第一弯pos
	float fWqmRadius;//弯曲模半径
	float fMjDistance;//模间距离
	float fZxmRadius;//中心模半径
	float fDmaqDistance;//导模安全预留量
	float fWftwPos;//往复推弯Pos
	std::vector<float> Zmlength;//主模
	std::vector<float>  RDmlength;//右导模
	std::vector<float> LDmlength;//左导模

	//ENUM_STRUCT_TYPE GetTWLC2ValueType(int _column)
	//{
	//	if(_column == 1)
	//	return _FLOAT;
	//}

	int LmValueNum;//轮模个数
	int ZmValueNum;
	int RDmValueNum;
	int LDmValueNum;

	Param()
	{
		imn	= -1;
	}
};





class MainWindow;
class xSettingAnimation :public QDialog
{
	Q_OBJECT
public:
	xSettingAnimation(MainWindow *);
	~xSettingAnimation(void);
	
	void InitDialog();
	void InitModelCombox(std::vector<QString> strModelName);
	int GetCurAnimationType();
	QString GetSettingModelName();
	double GetStartTime();
	double GetEndTime();
	int GetAxis();
	osg::Vec3 GetRotateCenter();
	float GetValue();
	QString GetModelName();
	void SetFileValues(std::vector<struct ProductionValue> pv);
	void SetParamValues(Param& param);
	void SetTWLC1(std::vector<struct TWLC1> twlc1);
	void SetTWLC2(std::vector<struct TWLC2> twlc2);
	void ResetValue(int index);
	void SetSelfInputEnabled();
	void ResetTwlcToValueCombox(int index);
	int GetCurNth();
	int GetCurPipeSide();

	bool GetIsCreatPipe();
	int GetPipeStyle();
	osg::Vec3 GetPipeDir();
	osg::Vec3 GetPipeCenter();
	float GetPipeLength();
	float GetCurvePipeR();
	float GetCurvePipeAngle();

public slots:
	void SlotOk();
	void SlotCancel();
	void SetInputLineEditEnable(int state);
	void SlotResetValueCombox(int index);
	void SlotSetRotateCenter(int index);
	void SlotSetTwlcToValue(int state);
	void SlotEnablePipeValue(int state);
	void SlotSetPipeStyle(int index);
signals:
	void sigSettingAnimationOK();


private:

	std::vector<struct ProductionValue> m_ProductionFileValues;
	std::vector<struct TWLC1> m_vTwlc1;	 
	std::vector<struct TWLC2> m_vTwlc2;
	Param m_paramvalues;
	float m_fResultValue;
	osg::Vec3 m_RotateCenter;
	QString m_curModelName;
	int m_AnimationType;//平移1，旋转2，静止3
	int m_Axis;//1X,2Y,3Z
	float m_fPipeLen;
	osg::Vec3 m_vec3PipeCenter;
	float m_fCurvePipeR;
	float m_fCurvePipeAngle;
	osg::Vec3 m_vec3PipeDir;
	int m_iPipeStyle;//0无，1直管，2弯管
	int m_iPipeSide;//1左，2右




	MainWindow *m_pMainWin;
	QHBoxLayout *m_pHInputLayout;
	QGridLayout *m_pBtnGLayout;
	QPushButton *m_pOk;
	QPushButton *m_pCancel;
	QPushButton *m_AsLine;
	
	QLabel *m_ModelNameLabel;//模型名
	QLabel *m_STimeLabel;//开始时间
	QLabel *m_ETimeLabel;//结束时间
	QLabel *m_AxisLabel;//X;0 Y1 Z2
	QLabel *m_ValueLabel;
	QLabel *m_RotateCenterLabel;
	QLabel *m_AnimationMethodLabel;//
	QLabel *m_SelfInputLable;
	QLabel *m_NthValueLabel;
	QLabel *m_TwlcLabel;
	QLabel *m_IsCreatPipeLabel;
	QLabel *m_PipeLenLabel;
	QLabel *m_PipeDirLabel;
	QLabel *m_PipeCenterLabel;
	QLabel *m_CurvePipeRLabel;
	QLabel *m_CurvePipeAngleLabel;
	QLabel *m_PipeStyleLabel;
	QLabel *m_PipeSideLabel;

	QLineEdit *m_STimeLEdit;
	QLineEdit *m_ETimeLEdit;
	QLineEdit *m_RotateCenterLEdit;
	QLineEdit *m_RotateCenter_XLEdit;
	QLineEdit *m_RotateCenter_YLEdit;
	QLineEdit *m_RotateCenter_ZLEdit;
	QLineEdit *m_SelfInputLEdit;
	QLineEdit *m_PipeLenLEdit;
	QLineEdit *m_PipeCenter_XLEdit;
	QLineEdit *m_PipeCenter_YLEdit;
	QLineEdit *m_PipeCenter_ZLEdit;
	QLineEdit *m_CurvePipeRLEdit;
	QLineEdit *m_CurvePipeAngleLEdit;

	QComboBox *m_NthValueComBox;//第几弯加工值
	QComboBox *m_AnimationMethodComBox;//平移、旋转、静止
	QComboBox *m_ModelNameComBox;
	QComboBox *m_AxisComBox;//轴
	QComboBox *m_ValueComBox;//加工值
	QComboBox *m_PipeDirComBox;
	QComboBox *m_PipeStyleComBox;
	QComboBox *m_PipeSideComBox;



	QCheckBox *m_pSelfInputValueCheckBox;
	QCheckBox *m_pTwlcCheckBox;
	QCheckBox *m_pIsCreatPipeCheckBox;

};

