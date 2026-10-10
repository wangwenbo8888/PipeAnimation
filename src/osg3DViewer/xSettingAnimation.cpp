#include "xSettingAnimation.h"

#include <QTextCodec>
#include <QColorDialog>
#include <QMessageBox>
#include <vector>
#include <string>
#include <osg/Group>
#include <QDoubleValidator>
#include <QStyleFactory>

#include "log.h"

xSettingAnimation::xSettingAnimation(MainWindow *mw)
{
	m_pMainWin = mw;

	InitDialog();
	connect(m_pCancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
	connect(m_pOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_pSelfInputValueCheckBox,SIGNAL(stateChanged(int)),this, SLOT(SetInputLineEditEnable(int)));
	connect(m_NthValueComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotResetValueCombox(int)));
	connect(m_AnimationMethodComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotSetRotateCenter(int)));
	connect(m_pTwlcCheckBox,SIGNAL(stateChanged(int)),this, SLOT(SlotSetTwlcToValue(int)));
	connect(m_pIsCreatPipeCheckBox,SIGNAL(stateChanged(int)),this, SLOT(SlotEnablePipeValue(int)));
	connect(m_PipeStyleComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotSetPipeStyle(int)));



}

xSettingAnimation::~xSettingAnimation(void)
{
}

void xSettingAnimation::InitDialog()
{
	this->resize(300,440);

	QString str ;
	str = str.fromLocal8Bit("确定");
	m_pOk = new QPushButton(str);
	str = str.fromLocal8Bit("取消");
	m_pCancel = new QPushButton(str);


	str = str.fromLocal8Bit("模型:");
	m_ModelNameLabel = new QLabel(str);
	m_ModelNameComBox = new QComboBox;
	str = str.fromLocal8Bit("   --请选择模型--");
	m_ModelNameComBox->addItem(str);
	str = str.fromLocal8Bit("开始时间:");
	m_STimeLabel = new QLabel(str);
	m_STimeLEdit = new QLineEdit;
	QDoubleValidator* lineDouble = new QDoubleValidator(0, 100000, 2, nullptr);
	m_STimeLEdit->setValidator(lineDouble);

	str = str.fromLocal8Bit("结束时间:");
	m_ETimeLabel = new QLabel(str);
	m_ETimeLEdit = new QLineEdit;
	m_ETimeLEdit->setValidator(lineDouble);

	str = str.fromLocal8Bit("轴向:");
	m_AxisLabel = new QLabel(str);
	m_AxisComBox = new QComboBox;
	str = str.fromLocal8Bit("   --请选择轴向--");
	m_AxisComBox->addItem(str);
	str = str.fromLocal8Bit("X轴");
	m_AxisComBox->addItem(str);
	str = str.fromLocal8Bit("Y轴");
	m_AxisComBox->addItem(str);
	str = str.fromLocal8Bit("Z轴");
	m_AxisComBox->addItem(str);

	str = str.fromLocal8Bit("第几弯:");
	m_NthValueLabel = new QLabel(str);
	m_NthValueComBox = new QComboBox;
	str = str.fromLocal8Bit("请选择第几弯:");
	m_NthValueComBox->addItem(str);

	str = str.fromLocal8Bit("数值:");
	m_ValueLabel = new QLabel(str);
	m_ValueComBox = new QComboBox;
	str = str.fromLocal8Bit("   --请选择加工值--");
	m_ValueComBox->addItem(str);

	str = str.fromLocal8Bit("旋转中心:");
	m_RotateCenterLabel = new QLabel(str);
	m_RotateCenterLEdit = new QLineEdit;
	m_RotateCenter_XLEdit = new QLineEdit;
	m_RotateCenter_YLEdit = new QLineEdit;
	m_RotateCenter_ZLEdit = new QLineEdit;
	QDoubleValidator* valueDouble = new QDoubleValidator(-100000, 1000000, 6, nullptr);

	m_RotateCenter_XLEdit->setValidator(valueDouble);
	m_RotateCenter_YLEdit->setValidator(valueDouble);
	m_RotateCenter_ZLEdit->setValidator(valueDouble);
	QHBoxLayout *hRotateLEdit = new QHBoxLayout;
	hRotateLEdit->addWidget(m_RotateCenter_XLEdit);
	hRotateLEdit->addWidget(m_RotateCenter_YLEdit);
	hRotateLEdit->addWidget(m_RotateCenter_ZLEdit);


	str = str.fromLocal8Bit("运动方式:");
	m_AnimationMethodLabel = new QLabel(str);
	m_AnimationMethodComBox = new QComboBox;
	str = str.fromLocal8Bit("   --请选择运动方式--");
	m_AnimationMethodComBox->addItem(str);
	str = str.fromLocal8Bit("平移");
	m_AnimationMethodComBox->addItem(str);
	str = str.fromLocal8Bit("旋转");
	m_AnimationMethodComBox->addItem(str);
	str = str.fromLocal8Bit("静止");
	m_AnimationMethodComBox->addItem(str);

	str = str.fromLocal8Bit("推弯流程");
	m_TwlcLabel = new QLabel(str);
	m_pTwlcCheckBox = new QCheckBox;
	m_pTwlcCheckBox->setStyle(QStyleFactory::create("Cleanlooks"));

	QHBoxLayout *htwlc = new QHBoxLayout;
	htwlc->addWidget(m_ValueComBox);
	htwlc->addWidget(m_TwlcLabel);
	htwlc->addWidget(m_pTwlcCheckBox);

	str = str.fromLocal8Bit("手动输入:");
	m_SelfInputLable = new QLabel(str);
	m_pSelfInputValueCheckBox = new QCheckBox(this);
	m_pSelfInputValueCheckBox->setStyle(QStyleFactory::create("Cleanlooks"));

	m_pHInputLayout = new QHBoxLayout;
	m_pHInputLayout->addWidget(m_pSelfInputValueCheckBox);
	m_pHInputLayout->addWidget(m_SelfInputLable);

	m_SelfInputLEdit = new QLineEdit;
	m_pSelfInputValueCheckBox->setChecked(false);
	m_SelfInputLEdit->setEnabled(true);
	m_SelfInputLEdit->setValidator(valueDouble);


	m_AsLine = new QPushButton(this);
	m_AsLine->setFixedSize(300,2);
	m_AsLine->setFocusPolicy(Qt::NoFocus);

	str = str.fromLocal8Bit("生成管子:");
	m_IsCreatPipeLabel = new QLabel(str);
	m_pIsCreatPipeCheckBox = new QCheckBox;
	m_pIsCreatPipeCheckBox->setStyle(QStyleFactory::create("Cleanlooks"));

	QHBoxLayout *IsCreatPipeHLayout = new QHBoxLayout;
	IsCreatPipeHLayout->addWidget(m_pIsCreatPipeCheckBox);
	IsCreatPipeHLayout->addWidget(m_IsCreatPipeLabel);

	m_PipeStyleComBox = new QComboBox;
	str = str.fromLocal8Bit(" --选择管子形状--");
	m_PipeStyleComBox->addItem(str);
	str = str.fromLocal8Bit("直管");
	m_PipeStyleComBox->addItem(str);
	str = str.fromLocal8Bit("弯管");
	m_PipeStyleComBox->addItem(str);

	str = str.fromLocal8Bit("管子形状:");
	m_PipeStyleLabel = new QLabel(str);
	QHBoxLayout *hPipeStyle = new QHBoxLayout;
	hPipeStyle->addWidget(m_PipeStyleLabel);
	hPipeStyle->addWidget(m_PipeStyleComBox);

	m_PipeDirComBox  = new QComboBox;
	str = str.fromLocal8Bit("-选择管子方向-");
	m_PipeDirComBox->addItem(str);
	str = str.fromLocal8Bit("X正轴");
	m_PipeDirComBox->addItem(str);
	str = str.fromLocal8Bit("X负轴");
	m_PipeDirComBox->addItem(str);
	str = str.fromLocal8Bit("Y正轴");
	m_PipeDirComBox->addItem(str);
	str = str.fromLocal8Bit("Y负轴");
	m_PipeDirComBox->addItem(str);
	str = str.fromLocal8Bit("Z正轴");
	m_PipeDirComBox->addItem(str);
	str = str.fromLocal8Bit("Z负轴");
	m_PipeDirComBox->addItem(str);

	str = str.fromLocal8Bit("管子生成方向:");
	m_PipeDirLabel = new QLabel(str);

	str = str.fromLocal8Bit("加工侧:");
	m_PipeSideLabel = new QLabel(str);

	m_PipeSideComBox =new QComboBox;
	str = str.fromLocal8Bit("-请选择加工侧-");
	m_PipeSideComBox->addItem(str);
	str = str.fromLocal8Bit("左侧加工");
	m_PipeSideComBox->addItem(str);
	str = str.fromLocal8Bit("右侧加工");
	m_PipeSideComBox->addItem(str);
	
	//hPipeStyle->addWidget(m_PipeDirLabel);
	//hPipeStyle->addWidget(m_PipeDirComBox);


	str = str.fromLocal8Bit("管子长度:");
	m_PipeLenLabel = new QLabel(str);
	m_PipeLenLEdit = new QLineEdit;
	m_PipeLenLEdit->setValidator(valueDouble);


	str = str.fromLocal8Bit("管子中心:");
	m_PipeCenterLabel = new QLabel(str);
	m_PipeCenter_XLEdit = new QLineEdit;
	m_PipeCenter_YLEdit = new QLineEdit;
	m_PipeCenter_ZLEdit = new QLineEdit;
	m_PipeCenter_XLEdit->setValidator(valueDouble);
	m_PipeCenter_YLEdit->setValidator(valueDouble);
	m_PipeCenter_ZLEdit->setValidator(valueDouble);

	QHBoxLayout *hPipeCenterLayout = new QHBoxLayout;
	hPipeCenterLayout->addWidget(m_PipeCenter_XLEdit);
	hPipeCenterLayout->addWidget(m_PipeCenter_YLEdit);
	hPipeCenterLayout->addWidget(m_PipeCenter_ZLEdit);

	str = str.fromLocal8Bit("弯管半径:");
	m_CurvePipeRLabel = new QLabel(str);
	m_CurvePipeRLEdit = new QLineEdit;
	m_CurvePipeRLEdit->setValidator(valueDouble);
	QHBoxLayout* hCurveRLayout = new QHBoxLayout;
	hCurveRLayout->addWidget(m_CurvePipeRLabel);
	hCurveRLayout->addWidget(m_CurvePipeRLEdit);


	str = str.fromLocal8Bit("弯管角度:");
	m_CurvePipeAngleLabel = new QLabel(str);
	m_CurvePipeAngleLEdit = new QLineEdit;
	m_CurvePipeAngleLEdit->setValidator(valueDouble);
	QHBoxLayout* hCurveAngleLayout = new QHBoxLayout;
	hCurveAngleLayout->addWidget(m_CurvePipeAngleLabel);
	hCurveAngleLayout->addWidget(m_CurvePipeAngleLEdit);
	hCurveRLayout->addLayout(hCurveAngleLayout);

	m_pBtnGLayout = new QGridLayout();
	m_pBtnGLayout->addWidget(m_ModelNameLabel,0,0,1,1);
	m_pBtnGLayout->addWidget(m_ModelNameComBox,0,1,1,1);
	m_pBtnGLayout->addWidget(m_STimeLabel,1,0,1,1);
	m_pBtnGLayout->addWidget(m_STimeLEdit,1,1,1,1);
	m_pBtnGLayout->addWidget(m_ETimeLabel,2,0,1,1);
	m_pBtnGLayout->addWidget(m_ETimeLEdit,2,1,1,1);
	m_pBtnGLayout->addWidget(m_AnimationMethodLabel,3,0,1,1);
	m_pBtnGLayout->addWidget(m_AnimationMethodComBox,3,1,1,1);
	m_pBtnGLayout->addWidget(m_AxisLabel,4,0,1,1);
	m_pBtnGLayout->addWidget(m_AxisComBox,4,1,1,1);
	m_pBtnGLayout->addWidget(m_NthValueLabel,5,0,1,1);
	m_pBtnGLayout->addWidget(m_NthValueComBox,5,1,1,1);
	m_pBtnGLayout->addWidget(m_ValueLabel,6,0,1,1);
	m_pBtnGLayout->addLayout(htwlc,6,1,1,1);
	m_pBtnGLayout->addLayout(m_pHInputLayout,7,0,1,1);
	m_pBtnGLayout->addWidget(m_SelfInputLEdit,7,1,1,1);
	m_pBtnGLayout->addWidget(m_RotateCenterLabel,8,0,1,1);
	m_pBtnGLayout->addLayout(hRotateLEdit,8,1,1,1);

	m_pBtnGLayout->addWidget(m_AsLine,9,0,1,2);

	m_pBtnGLayout->addLayout(IsCreatPipeHLayout,10,0,1,1);
	m_pBtnGLayout->addWidget(m_PipeStyleLabel,11,0,1,1);
	m_pBtnGLayout->addWidget(m_PipeStyleComBox,11,1,1,1);
	m_pBtnGLayout->addWidget(m_PipeSideLabel,12,0,1,1);
	m_pBtnGLayout->addWidget(m_PipeSideComBox,12,1,1,1);

	m_pBtnGLayout->addWidget(m_PipeDirLabel,13,0,1,1);
	m_pBtnGLayout->addWidget(m_PipeDirComBox,13,1,1,1);
	m_pBtnGLayout->addWidget(m_PipeLenLabel,14,0,1,1);
	m_pBtnGLayout->addWidget(m_PipeLenLEdit,14,1,1,1);
	m_pBtnGLayout->addWidget(m_PipeCenterLabel,15,0,1,1);
	m_pBtnGLayout->addLayout(hPipeCenterLayout,15,1,1,1);
	m_pBtnGLayout->addLayout(hCurveRLayout,16,0,1,2);


	m_pBtnGLayout->addWidget(m_pCancel,17,0,1,1);
	m_pBtnGLayout->addWidget(m_pOk,17,1,1,1);

	this->setLayout(m_pBtnGLayout);


	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置

}


void xSettingAnimation::InitModelCombox(std::vector<QString> strModelName)
{
	if(m_ModelNameComBox->count() > strModelName.size())
	{
		int j = m_ModelNameComBox->count() - strModelName.size();
		for(int i = m_ModelNameComBox->count()-1;i<j;i--)
		{
			m_ModelNameComBox->removeItem(i);
		}
	}
	else
	{
		int k = strModelName.size() - m_ModelNameComBox->count();
		for(int i=0;i<=k;i++)
		{
			m_ModelNameComBox->addItem("");

		}
	}

	for(size_t i=0;i<strModelName.size();i++)
	{
		m_ModelNameComBox->setItemText(i+1,strModelName[i]);
	}

	m_NthValueComBox->setCurrentIndex(0);
	m_AnimationMethodComBox->setCurrentIndex(0);
	m_ModelNameComBox->setCurrentIndex(0);
	m_AxisComBox->setCurrentIndex(0);
	m_ValueComBox->setCurrentIndex(0);
	m_PipeDirComBox->setCurrentIndex(0);
	m_PipeDirComBox->setEnabled(false);
	m_PipeStyleComBox->setCurrentIndex(0);
	m_PipeStyleComBox->setEnabled(false);
	m_PipeSideComBox->setCurrentIndex(0);
	m_PipeSideComBox->setEnabled(false);

	m_SelfInputLEdit->setEnabled(false);
	m_PipeLenLEdit->setEnabled(false);
	m_PipeCenter_XLEdit->setEnabled(false);
	m_PipeCenter_YLEdit->setEnabled(false);
	m_PipeCenter_ZLEdit->setEnabled(false);
	m_RotateCenter_XLEdit->setEnabled(false);
	m_RotateCenter_YLEdit->setEnabled(false);
	m_RotateCenter_ZLEdit->setEnabled(false);
	m_CurvePipeRLEdit->setEnabled(false);
	m_CurvePipeAngleLEdit->setEnabled(false);

	m_pTwlcCheckBox->setChecked(false);
	m_pTwlcCheckBox->setEnabled(false);
	m_pIsCreatPipeCheckBox->setChecked(false);
	m_pSelfInputValueCheckBox->setChecked(false);


	m_iPipeStyle = 0;
}


int xSettingAnimation::GetCurAnimationType()
{
	return m_AnimationType;
}

QString xSettingAnimation::GetSettingModelName()
{
	return m_curModelName;
}

double xSettingAnimation::GetStartTime()
{
	double st;
	st = m_STimeLEdit->text().toDouble();
	return st;
}

double xSettingAnimation::GetEndTime()
{
	double et;
	et = m_ETimeLEdit->text().toDouble();
	return et;
}



int xSettingAnimation::GetAxis()
{
	return m_Axis;
}

osg::Vec3 xSettingAnimation::GetRotateCenter()
{
	return m_RotateCenter;
}

float xSettingAnimation::GetValue()
{
	return m_fResultValue;
}

QString xSettingAnimation::GetModelName()
{
	return m_curModelName;
}

void xSettingAnimation::SetFileValues(std::vector<struct ProductionValue> pv)
{

	//清空value
	for(int j=1;j<m_ValueComBox->count();j++)
	{
		m_ValueComBox->removeItem(j);
	}


	m_ProductionFileValues = pv;
	
}

void xSettingAnimation::SetParamValues(Param &param)
{
	int k = m_NthValueComBox->count();
	while(k>1)
	{
		for(int j=1;j<m_NthValueComBox->count();j++)
		{
			m_NthValueComBox->removeItem(j);
		}

		k = m_NthValueComBox->count();
	}


	m_paramvalues = param;
	for(int i = 1;i<=m_paramvalues.imn;i++)
	{

		m_NthValueComBox->addItem(QString::number(i));
	}
}

void xSettingAnimation::SetTWLC1(std::vector<struct TWLC1> twlc1)
{
	m_vTwlc1 = twlc1;
}

void xSettingAnimation::SetTWLC2(std::vector<struct TWLC2> twlc2)
{
	m_vTwlc2 = twlc2;
}

void xSettingAnimation::ResetValue(int index)
{
	//添加加工值
	if(m_NthValueComBox->currentIndex() != 0)
	{
		int iNth = index - 1;//第几弯
		int k;
		k = m_ValueComBox->count();
		while (k>1)
		{
			for(int j=1;j<m_ValueComBox->count();j++)
			{
				m_ValueComBox->removeItem(j);
			}

			k = m_ValueComBox->count();
		} 

	//	k = m_ValueComBox->count();

		QString str;
		str = "my:"+ QString::number(m_ProductionFileValues[iNth].fmy);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmy);

		str = "myb:"+ QString::number(m_ProductionFileValues[iNth].fmyb);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmyb);

		str = "mb:"+ QString::number(m_ProductionFileValues[iNth].fmb);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmb);

		str = "mbb:"+ QString::number(m_ProductionFileValues[iNth].fmbb);		
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmbb);

		str = "mc:"+ QString::number(m_ProductionFileValues[iNth].fmc);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmc);

		str = "mcb:"+ QString::number(m_ProductionFileValues[iNth].fmcb);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmcb);

		str = "mm:"+ QString::number(m_ProductionFileValues[iNth].fmm);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmm);

		str = "lr:"+ QString::number(m_ProductionFileValues[iNth].flr);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].flr);

		str = "myt:"+ QString::number(m_ProductionFileValues[iNth].fmyt);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmyt);

		str = "mz:"+ QString::number(m_ProductionFileValues[iNth].fmz);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmz);

		str = "btm:"+ QString::number(m_ProductionFileValues[iNth].fbtm);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fbtm);

		str = "ctm:"+ QString::number(m_ProductionFileValues[iNth].fctm);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fctm);

		str = "dz:"+ QString::number(m_ProductionFileValues[iNth].fdz);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fdz);

		str = "myth:"+ QString::number(m_ProductionFileValues[iNth].fmyth);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmyth);

		str = "lc:"+ QString::number(m_ProductionFileValues[iNth].flc);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].flc);

		str = "mmd:"+ QString::number(m_ProductionFileValues[iNth].fmmd);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fmmd);

		str = "jm:"+ QString::number(m_ProductionFileValues[iNth].fjm);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fjm);

		str = "xb:"+ QString::number(m_ProductionFileValues[iNth].fxb);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fxb);

		str = "jy:"+ QString::number(m_ProductionFileValues[iNth].fjy);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].fjy);

		str = "twy:"+ QString::number(m_ProductionFileValues[iNth].ftwy);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].ftwy);

		str = "twbj:"+ QString::number(m_ProductionFileValues[iNth].ftwbj);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].ftwbj);

		str = "twc:"+ QString::number(m_ProductionFileValues[iNth].ftwc);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].ftwc);

		str = "twb:"+ QString::number(m_ProductionFileValues[iNth].ftwb);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].ftwb);

		str = "twlc:"+ QString::number(m_ProductionFileValues[iNth].ftwlc);
		m_ValueComBox->addItem(str,m_ProductionFileValues[iNth].ftwlc);

	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("请先选择第几弯!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
		
	}
}

void xSettingAnimation::SetSelfInputEnabled()
{
	//bool able = m_pSelfInputValueCheckBox->isChecked();
	//if(able == true)
	//{
	//	m_SelfInputLEdit->setReadOnly(false);
	//	m_pSelfInputValueCheckBox->setChecked(true);
	//}
	//else
	//{
	//	m_SelfInputLEdit->setReadOnly(true);
	//	m_pSelfInputValueCheckBox->setChecked(false);

	//}
}

void xSettingAnimation::ResetTwlcToValueCombox(int index)
{
	int iNth = index - 1;

	//获取当前的推弯流程
	/*std::vector<struct TWLC1> tmpTwlc;
	std::vector<struct TWLC1> tmpTwlc;*/

	int k;
	k = m_ValueComBox->count();
	while (k>1)
	{
		for(int j=1;j<m_ValueComBox->count();j++)
		{
			m_ValueComBox->removeItem(j);
		}

		k = m_ValueComBox->count();

	} 

	if(m_ProductionFileValues[iNth].ftwlc == 1)
	{
		//显示数据
		for(size_t i=0;i<m_vTwlc1.size();i++)
		{
			QString str = QString::number(i+1)+":twy " + QString::number(m_vTwlc1[i].ftwy);
			m_ValueComBox->addItem(str,m_vTwlc1[i].ftwy);

			str = QString::number(i+1)+":twbj " + QString::number(m_vTwlc1[i].ftwbj);
			m_ValueComBox->addItem(str,m_vTwlc1[i].ftwbj);

			str = QString::number(i+1)+":twc " + QString::number(m_vTwlc1[i].ftwc);
			m_ValueComBox->addItem(str,m_vTwlc1[i].ftwc);

			str = QString::number(i+1)+":twb " + QString::number(m_vTwlc1[i].ftwb);
			m_ValueComBox->addItem(str,m_vTwlc1[i].ftwb);
		}
	}
	else if(m_ProductionFileValues[iNth].ftwlc == 2)
	{
		//显示数据
		for(size_t i=0;i<m_vTwlc2.size();i++)
		{
			QString str = QString::number(i+1)+":twy " + QString::number(m_vTwlc2[i].ftwy);
			m_ValueComBox->addItem(str,m_vTwlc2[i].ftwy);

			str = QString::number(i+1)+":twbj " + QString::number(m_vTwlc2[i].ftwbj);
			m_ValueComBox->addItem(str,m_vTwlc2[i].ftwbj);

			str = QString::number(i+1)+":twc " + QString::number(m_vTwlc2[i].ftwc);
			m_ValueComBox->addItem(str,m_vTwlc2[i].ftwc);

			str = QString::number(i+1)+":twb " + QString::number(m_vTwlc2[i].ftwb);
			m_ValueComBox->addItem(str,m_vTwlc2[i].ftwb);
		}
	}
	else if(m_ProductionFileValues[iNth].ftwlc == 0)
	{
		QString strWarning;
		/*strWarning = strWarning.fromLocal8Bit("请正确选择第几弯!");
		QMessageBox::warning(this,"",strWarning);*/
		return;
	}

	
}

int xSettingAnimation::GetCurNth()
{
	int index;
	index = m_NthValueComBox->currentIndex();
	return index;
}

int xSettingAnimation::GetCurPipeSide()
{
	return m_iPipeSide;
}

bool xSettingAnimation::GetIsCreatPipe()
{
	bool bIs;
	bIs = m_pIsCreatPipeCheckBox->isChecked();
	return bIs;
}

int xSettingAnimation::GetPipeStyle()
{
	
	return m_iPipeStyle;
}

osg::Vec3 xSettingAnimation::GetPipeDir()
{
	return m_vec3PipeDir;
}

osg::Vec3 xSettingAnimation::GetPipeCenter()
{
	return m_vec3PipeCenter;
}

float xSettingAnimation::GetPipeLength()
{
	return m_fPipeLen;
}

float xSettingAnimation::GetCurvePipeR()
{
	return m_fCurvePipeR;
}

float xSettingAnimation::GetCurvePipeAngle()
{
	return m_fCurvePipeAngle;
}

void xSettingAnimation::SlotSetRotateCenter(int index)
{
	if(index == 2)
	{
		//m_RotateCenterLEdit->setEnabled(true);
		m_RotateCenter_XLEdit->setEnabled(true);
		m_RotateCenter_YLEdit->setEnabled(true);
		m_RotateCenter_ZLEdit->setEnabled(true);

	}
	else
	{
		//m_RotateCenterLEdit->setEnabled(false);
		m_RotateCenter_XLEdit->setEnabled(false);
		m_RotateCenter_YLEdit->setEnabled(false);
		m_RotateCenter_ZLEdit->setEnabled(false);

	}
}

void xSettingAnimation::SlotSetTwlcToValue(int state)
{
	int index = m_NthValueComBox->currentIndex();
	if(index != 0)
	{
		//清空value
		//int k; 
		//k = m_ValueComBox->count();

		//for(int j=1;j<m_ValueComBox->count();j++)
		//{
		//	m_ValueComBox->removeItem(j);
		//}

		if( state == Qt::Unchecked)//未选中
		{
			ResetValue(index);
		}
		else if( state == Qt::Checked)//选中
		{
			ResetTwlcToValueCombox(index);
		}
	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("请正确选择第几弯!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);

		//m_pTwlcCheckBox->setChecked(!m_pTwlcCheckBox->isChecked());
		return;
	}


}

void xSettingAnimation::SlotEnablePipeValue(int state)
{
	if( state == Qt::Unchecked)//未选中
	{
		m_PipeStyleComBox->setCurrentIndex(0);
		m_PipeStyleComBox->setEnabled(false);
		m_PipeDirComBox->setCurrentIndex(0);
		m_PipeDirComBox->setEnabled(false);
		m_PipeSideComBox->setCurrentIndex(0);
		m_PipeSideComBox->setEnabled(false);
		
		m_PipeLenLEdit->clear();
		//m_PipeCenter_XLEdit->clear();
		//m_PipeCenter_YLEdit->clear();
		//m_PipeCenter_ZLEdit->clear();
		m_CurvePipeRLEdit->clear();
		m_CurvePipeAngleLEdit->clear();

		m_PipeLenLEdit->setEnabled(false);
		m_PipeCenter_XLEdit->setEnabled(false);
		m_PipeCenter_YLEdit->setEnabled(false);
		m_PipeCenter_ZLEdit->setEnabled(false);
		m_CurvePipeRLEdit->setEnabled(false);
		m_CurvePipeAngleLEdit->setEnabled(false);
	}
	else if( state == Qt::Checked)//选中
	{
		m_PipeSideComBox->setEnabled(true);
		m_PipeDirComBox->setEnabled(true);
		m_PipeStyleComBox->setEnabled(true);
		m_PipeLenLEdit->setEnabled(true);
		m_PipeCenter_XLEdit->setEnabled(true);
		m_PipeCenter_YLEdit->setEnabled(true);
		m_PipeCenter_ZLEdit->setEnabled(true);
		//m_CurvePipeRLEdit->setEnabled(true);
		//m_CurvePipeAngleLEdit->setEnabled(true);
	}
}

void xSettingAnimation::SlotSetPipeStyle(int index)
{
	m_iPipeStyle = index;

	if(m_PipeStyleComBox->currentIndex() == 2)
	{
	
		m_PipeSideComBox->setEnabled(true);
		m_CurvePipeRLEdit->setEnabled(true);
		m_CurvePipeAngleLEdit->setEnabled(true);
	}
	else 
	{
		m_PipeSideComBox->setCurrentIndex(0);
		m_PipeSideComBox->setEnabled(false);

		m_CurvePipeRLEdit->setEnabled(false);
		m_CurvePipeAngleLEdit->setEnabled(false);		
	}
}

void xSettingAnimation::SlotOk()
{	
	//判断运动方式
	if(m_AnimationMethodComBox->currentIndex() == 2)//旋转时
	{
		if(m_pSelfInputValueCheckBox->isChecked() == false)//非手动输入
		{
			if(m_ModelNameComBox->currentIndex() == 0
				||m_STimeLEdit->text().isEmpty()
				||m_ETimeLEdit->text().isEmpty()
				||m_AxisComBox->currentIndex() == 0
				||m_NthValueComBox->currentIndex() == 0
				||m_ValueComBox->currentIndex() == 0 
				//||m_RotateCenterLEdit->text().isEmpty()
				||m_RotateCenter_XLEdit->text().isEmpty()
				||m_RotateCenter_YLEdit->text().isEmpty()
				||m_RotateCenter_ZLEdit->text().isEmpty()
				)//有空数据报错
			{
				QString strWarning;
				strWarning = strWarning.fromLocal8Bit("设置不全，请正确设置动画!");
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}
		}
		else if(m_pSelfInputValueCheckBox->isChecked() == true)//手动输入
		{
			if(m_ModelNameComBox->currentIndex() == 0
				||m_STimeLEdit->text().isEmpty()
				||m_ETimeLEdit->text().isEmpty()
				||m_AxisComBox->currentIndex() == 0
				||m_NthValueComBox->currentIndex() == 0
				//||m_ValueComBox->currentIndex() == 0 
				//||m_RotateCenterLEdit->text().isEmpty()
				||m_RotateCenter_XLEdit->text().isEmpty()
				||m_RotateCenter_YLEdit->text().isEmpty()
				||m_RotateCenter_ZLEdit->text().isEmpty()

				||m_SelfInputLEdit->text().isEmpty() )//有空数据报错
			{
				QString strWarning;
				strWarning = strWarning.fromLocal8Bit("设置不全，请正确设置动画!");
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}
		}
		
			
	}
	else
	{
		if(m_pSelfInputValueCheckBox->isChecked() == false)//非手动输入
		{
			if(m_ModelNameComBox->currentIndex() == 0
				||m_STimeLEdit->text().isEmpty()
				||m_ETimeLEdit->text().isEmpty()
				||m_AxisComBox->currentIndex() == 0
				||m_NthValueComBox->currentIndex() == 0
				||m_ValueComBox->currentIndex() == 0 
				/*||m_RotateCenterLEdit->text().isEmpty()*/)//有空数据报错
			{
				QString strWarning;
				strWarning = strWarning.fromLocal8Bit("设置不全，请正确设置动画!");
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}
		}
		else if(m_pSelfInputValueCheckBox->isChecked() == true)//手动输入
		{
			if(m_ModelNameComBox->currentIndex() == 0
				||m_STimeLEdit->text().isEmpty()
				||m_ETimeLEdit->text().isEmpty()
				||m_AxisComBox->currentIndex() == 0
				||m_NthValueComBox->currentIndex() == 0
				//||m_ValueComBox->currentIndex() == 0 
				/*||m_RotateCenterLEdit->text().isEmpty()*/
				||m_SelfInputLEdit->text().isEmpty() )//有空数据报错
			{
				QString strWarning;
				strWarning = strWarning.fromLocal8Bit("设置不全，请正确设置动画!");
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}
		}
	}

	//判断是否生成管子
	if(m_pIsCreatPipeCheckBox->isChecked() == true)
	{
		if(m_PipeStyleComBox->currentIndex() == 1)//直管
		{
				if(m_PipeDirComBox->currentIndex() == 0
				||m_PipeLenLEdit->text().isEmpty()
				||m_PipeCenter_XLEdit->text().isEmpty()
				||m_PipeCenter_YLEdit->text().isEmpty()
				||m_PipeCenter_ZLEdit->text().isEmpty()
				)
				{
					QString strWarning;
					strWarning = strWarning.fromLocal8Bit("直管设置不全，请正确设置动画!");
					QMessageBox::warning(this,"",strWarning);
					LogInfo(strWarning);
					return;
				}
		}
		else if(m_PipeStyleComBox->currentIndex() == 2)//弯管
		{
			if(m_PipeDirComBox->currentIndex() == 0
				||m_PipeLenLEdit->text().isEmpty()
				||m_PipeCenter_XLEdit->text().isEmpty()
				||m_PipeCenter_YLEdit->text().isEmpty()
				||m_PipeCenter_ZLEdit->text().isEmpty()
				||m_CurvePipeRLEdit->text().isEmpty()
				||m_CurvePipeAngleLEdit->text().isEmpty()
				)
			{
				QString strWarning;
				strWarning = strWarning.fromLocal8Bit("弯管设置不全，请正确设置动画!");
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}
		}
		else if(m_PipeStyleComBox->currentIndex() == 0)
		{
			QString strWarning;
			strWarning = strWarning.fromLocal8Bit("未选择管子形状，请正确设置动画!");
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
	}


	//获取加工值
	float tmpvalue;
	if(m_pSelfInputValueCheckBox->isChecked() == true)
	{
		tmpvalue = m_SelfInputLEdit->text().toFloat();
	}
	else if(m_pSelfInputValueCheckBox->isChecked() == false)
	{
		if(m_ValueComBox->currentIndex() == 0)
		{
			QString strWarning;
			strWarning = strWarning.fromLocal8Bit("请正确选择加工值!");
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
		else
		{
			tmpvalue = m_ValueComBox->itemData(m_ValueComBox->currentIndex()).toFloat();//直接取数据
			
		}
	}
	//获取旋转中心
	osg::Vec3 tmpcenter;
	if(m_AnimationMethodComBox->currentIndex() == 2)
	{
		if(m_RotateCenter_XLEdit->text().isEmpty() == false 
			&& m_RotateCenter_YLEdit->text().isEmpty() == false 
			&& m_RotateCenter_ZLEdit->text().isEmpty() == false)
		{
			tmpcenter.x() = m_RotateCenter_XLEdit->text().toFloat();
			tmpcenter.y() = m_RotateCenter_YLEdit->text().toFloat();
			tmpcenter.z() = m_RotateCenter_ZLEdit->text().toFloat();

			m_RotateCenter = tmpcenter;
		}
		else
		{
			QString strWarning;
			strWarning = strWarning.fromLocal8Bit("请正确设置旋转中心!");
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
		

	}
	else//不是旋转的话
	{
		tmpcenter.x() = -100000.0;
		tmpcenter.y() = -100000.0;
		tmpcenter.z() = -100000.0;

		m_RotateCenter = tmpcenter;
	}

	//获取管子参数
	if(m_pIsCreatPipeCheckBox->isChecked() == true)
	{
		m_iPipeStyle = m_PipeStyleComBox->currentIndex();
		if(m_iPipeStyle !=0 && m_PipeDirComBox->currentIndex()!=0)
		{
			if(m_PipeDirComBox->currentIndex() == 1)
			{
				m_vec3PipeDir = osg::Vec3(1,0,0);
			}
			else if(m_PipeDirComBox->currentIndex() == 2)
			{
				m_vec3PipeDir = osg::Vec3(-1,0,0);
			}
			else if(m_PipeDirComBox->currentIndex() == 3)
			{
				m_vec3PipeDir = osg::Vec3(0,1,0);
			}
			else if(m_PipeDirComBox->currentIndex() == 4)
			{
				m_vec3PipeDir = osg::Vec3(0,-1,0);
			}
			else if(m_PipeDirComBox->currentIndex() == 5)
			{
				m_vec3PipeDir = osg::Vec3(0,0,1);
			}
			else if(m_PipeDirComBox->currentIndex() == 6)
			{
				m_vec3PipeDir = osg::Vec3(0,0,-1);
			}

			m_fPipeLen = m_PipeLenLEdit->text().toDouble();
			m_vec3PipeCenter = osg::Vec3(m_PipeCenter_XLEdit->text().toFloat(),m_PipeCenter_YLEdit->text().toFloat(),m_PipeCenter_ZLEdit->text().toFloat());

			if (m_iPipeStyle == 2)
			{
				m_fCurvePipeR = m_CurvePipeRLEdit->text().toFloat();
				m_fCurvePipeAngle = m_CurvePipeAngleLEdit->text().toFloat();
				
				m_iPipeSide = m_PipeSideComBox->currentIndex();

			}
			else if(m_iPipeStyle == 1)
			{
				m_fCurvePipeR = -100000.0;
				m_fCurvePipeAngle = -100000.0;
				m_iPipeSide = -100000.0;
			}
		}
		else
		{
			QString strWarning;
			strWarning = strWarning.fromLocal8Bit("请正确设置管子形状或方向!");
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
		
	}
	else
	{
		m_iPipeStyle = -100000.0;
		m_vec3PipeDir = osg::Vec3(-100000.0,-100000.0,-100000.0);//错误
		m_fPipeLen = -100000.0;
		m_vec3PipeCenter =  osg::Vec3(-100000.0,-100000.0,-100000.0);
		m_fCurvePipeR = -100000.0;
		m_fCurvePipeAngle = -100000.0;
	}


	m_Axis = m_AxisComBox->currentIndex();
	m_AnimationType = m_AnimationMethodComBox->currentIndex();
	m_curModelName = m_ModelNameComBox->currentText();
	m_fResultValue = tmpvalue;
	m_SelfInputLEdit->clear();
	m_pSelfInputValueCheckBox->setChecked(false);

	this->close();
	emit sigSettingAnimationOK();

}


void xSettingAnimation::SlotCancel()
{
	m_SelfInputLEdit->clear();

	//this->close();
}

void xSettingAnimation::SetInputLineEditEnable(int state)
{
	if( state == Qt::Unchecked)//未选中
	{
		m_SelfInputLEdit->setEnabled(false);
		m_SelfInputLEdit->clear();
	}
	else if( state == Qt::Checked)//选中
	{
		m_SelfInputLEdit->setEnabled(true);
		m_ValueComBox->setCurrentIndex(0);

		m_fResultValue = m_SelfInputLEdit->text().toFloat();
	}


}

void xSettingAnimation::SlotResetValueCombox(int index)
{
	if(index > 0)
	{
		//清除加工数值
		//int k= m_ValueComBox->count();
		//while (k>1)
		//{
		//	for(int j=1;j<m_ValueComBox->count();j++)
		//	{
		//		m_ValueComBox->removeItem(j);
		//	}

		//	k = m_ValueComBox->count();

		//} 
		//重设数值
		ResetValue(index);

		if(m_ProductionFileValues[index -1].ftwlc != 0)
		{
			m_pTwlcCheckBox->setChecked(false);
			m_pTwlcCheckBox->setEnabled(true);
		}
		else if(m_ProductionFileValues[index -1].ftwlc == 0)
		{
			m_pTwlcCheckBox->setChecked(false);
			m_pTwlcCheckBox->setEnabled(false);
		}


	}
	else if(index == 0)
	{
		//清除加工数值
		int k= m_ValueComBox->count();
		while (k>1)
		{
			for(int j=1;j<m_ValueComBox->count();j++)
			{
				m_ValueComBox->removeItem(j);
			}

			k = m_ValueComBox->count();

		} 

		m_pTwlcCheckBox->setChecked(false);
		m_pTwlcCheckBox->setEnabled(false);
		//QString strWarning;
		//strWarning = strWarning.fromLocal8Bit("请正确选择加工弯数!");
		//QMessageBox::warning(this,"",strWarning);
		//return;

	}
}


