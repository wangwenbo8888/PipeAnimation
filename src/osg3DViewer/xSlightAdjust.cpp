#include "xSlightAdjust.h"
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>
#include "MainWindow.h"

#include "log.h"

// 显示精度，小数点后几位
const int preci = 2;
const QString initValue = "0.00";

xSlightAdjust::xSlightAdjust(MainWindow *pParent)
{
	m_pMainWindow = pParent;
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	//this->setWindowTitle(QString::fromLocal8Bit("调整模型初始位置"));
	InitDlg();

	this->resize(300,120);

	m_XValueLEdit->setEnabled(true);
	m_YValueLEdit->setEnabled(true);
	m_ZValueLEdit->setEnabled(true);
	float x = 0.0f;
	// 设置默认值
	m_XValueLEdit->setText(QString::number(x,'f',preci));
	m_YValueLEdit->setText(QString::number(x,'f',preci));
	m_ZValueLEdit->setText(QString::number(x,'f',preci));

	connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_ModelCombox,SIGNAL(currentIndexChanged(int)),this, SLOT(On_Item_changed(int)), Qt::UniqueConnection);
}

xSlightAdjust::~xSlightAdjust()
{
}

void xSlightAdjust::InitDlg()
{
	QString str;
	str = str.fromLocal8Bit("模型:");
	m_ModlLabel = new QLabel(str);
	m_ModelCombox = new QComboBox;
	m_ModelCombox->setMaxVisibleItems(35);
	str = str.fromLocal8Bit("--请选择模型--");
	m_ModelCombox->addItem(str);
	m_ModelCombox->view()->setFixedWidth(150);
	QHBoxLayout *hModelLayout = new QHBoxLayout;
	hModelLayout->addWidget(m_ModlLabel);
	hModelLayout->addWidget(m_ModelCombox);

	str = str.fromLocal8Bit("轴:");
	m_AxisLabel = new QLabel(str);
	m_AxisCombox = new QComboBox;
	str = str.fromLocal8Bit("--请选择轴--");
	m_AxisCombox->addItem(str);
	str = str.fromLocal8Bit("X轴");
	m_AxisCombox->addItem(str);
	str = str.fromLocal8Bit("Y轴");
	m_AxisCombox->addItem(str);
	str = str.fromLocal8Bit("Z轴");
	m_AxisCombox->addItem(str);
	QHBoxLayout *haxisLayout = new QHBoxLayout;
	haxisLayout->addWidget(m_AxisLabel);
	haxisLayout->addWidget(m_AxisCombox);

	str = str.fromLocal8Bit("数值(x,y,z):");
	m_ValueLabel = new QLabel(str);
	m_XValueLEdit = new SoftKeyLineEdit;
	m_YValueLEdit = new SoftKeyLineEdit;
	m_ZValueLEdit = new SoftKeyLineEdit;

	QDoubleValidator* lineDouble = new QDoubleValidator(-100000, 100000, 2, nullptr);

	m_XValueLEdit->setValidator(lineDouble);
	m_YValueLEdit->setValidator(lineDouble);
	m_ZValueLEdit->setValidator(lineDouble);

	QHBoxLayout *hvalueLayout = new QHBoxLayout;
	hvalueLayout->addWidget(m_XValueLEdit);
	hvalueLayout->addWidget(m_YValueLEdit);
	hvalueLayout->addWidget(m_ZValueLEdit);

	str = str.fromLocal8Bit("确定");
	m_OkBtn = new QPushButton(str);
	str = str.fromLocal8Bit("取消");
	m_CancelBtn = new QPushButton(str);
	QHBoxLayout *hBtnLayout = new QHBoxLayout;
	hBtnLayout->addWidget(m_OkBtn);

	str = str.fromLocal8Bit("中心位置");
	m_originalcenterLabel = new QLabel(str);
	QHBoxLayout *hchecklayout = new QHBoxLayout;
	hchecklayout->addWidget(m_originalcenterLabel);

	m_XCenterValueLEdit = new SoftKeyLineEdit;
	m_YCenterValueLEdit = new SoftKeyLineEdit;
	m_ZCenterValueLEdit = new SoftKeyLineEdit;

	m_XCenterValueLEdit->setValidator(lineDouble);
	m_YCenterValueLEdit->setValidator(lineDouble);
	m_ZCenterValueLEdit->setValidator(lineDouble);

	QHBoxLayout *hOriginalcentervalueLayout = new QHBoxLayout;
	hOriginalcentervalueLayout->addWidget(m_XCenterValueLEdit);
	hOriginalcentervalueLayout->addWidget(m_YCenterValueLEdit);
	hOriginalcentervalueLayout->addWidget(m_ZCenterValueLEdit);

	QGridLayout *gLayout = new QGridLayout;
	gLayout->addWidget(m_ModlLabel,0,0,1,1);
	gLayout->addWidget(m_ModelCombox,0,1,1,1);
	gLayout->addWidget(m_ValueLabel,1,0,1,1);
	gLayout->addLayout(hvalueLayout,1,1,1,1);
	gLayout->addLayout(hchecklayout,2,0,1,1);
	gLayout->addLayout(hOriginalcentervalueLayout,2,1,1,1);
	gLayout->addLayout(hBtnLayout,4,1,1,1);

	this->setLayout(gLayout);
}

void xSlightAdjust::SetCurFileValue(std::vector<struct InitModelCenter> vInitModelCenterFileValues)
{
	m_vInitPosFileValues = vInitModelCenterFileValues;
}

void xSlightAdjust::InitModelCombox(std::vector<QString> vModelName)
{
	int count = m_ModelCombox->count();
	for(int i=count-1;i>0;i--)
	{
		m_ModelCombox->removeItem(i);
	}

	//
	for (size_t j=0;j<vModelName.size();j++)
	{
		m_ModelCombox->addItem(vModelName[j]);
	}

	m_ModelCombox->setCurrentIndex(0);
	m_AxisCombox->setCurrentIndex(0);
}

int xSlightAdjust::GetAxis()
{
	int index = m_AxisCombox->currentIndex();
	return index;
}

QString xSlightAdjust::GetChosenModelName()
{
	QString str = m_ModelCombox->currentText();
	return str;
}

osg::Vec3 xSlightAdjust::GetAdjustValue()
{
	double Xvalue = m_XValueLEdit->text().toDouble();
	double Yvalue = m_YValueLEdit->text().toDouble();
	double Zvalue = m_ZValueLEdit->text().toDouble();

	osg::Vec3 moveValue = osg::Vec3(Xvalue,Yvalue,Zvalue);
	return moveValue;
}

osg::Vec3 xSlightAdjust::GetModelCenter()
{
	double Xvalue = m_XCenterValueLEdit->text().toDouble();
	double Yvalue = m_YCenterValueLEdit->text().toDouble();
	double Zvalue = m_ZCenterValueLEdit->text().toDouble();

	osg::Vec3 center = osg::Vec3(Xvalue,Yvalue,Zvalue);
	return center;
}

void xSlightAdjust::Before()
{
	m_XValueLEdit->clear();
	m_YValueLEdit->clear();
	m_ZValueLEdit->clear();
	m_XCenterValueLEdit->clear();
	m_YCenterValueLEdit->clear();
	m_ZCenterValueLEdit->clear();

	m_XCenterValueLEdit->setEnabled(false);
	m_YCenterValueLEdit->setEnabled(false);
	m_ZCenterValueLEdit->setEnabled(false);
}

void xSlightAdjust::SetSlightAdjustLanguage(std::vector<QString> vCurWords)
{
	m_ModelCombox->setItemText(0,vCurWords[84]);

	m_ModlLabel->setText(vCurWords[69]);
	m_ValueLabel->setText(vCurWords[70]);
	m_originalcenterLabel->setText(vCurWords[71]);
	//m_centerLabel->setText(vCurWords[72]);
	m_OkBtn->setText(vCurWords[73]);

	m_vCurLanguageWords = vCurWords;
}

void xSlightAdjust::SetCurrentItem(const QString& name)
{
	int count = m_ModelCombox->count();
	for (int i = 0; i < count;++i)
	{
		if (name==m_ModelCombox->itemText(i))
		{
			m_ModelCombox->setCurrentIndex(i);
			On_Item_changed(i);

			return;
		}
	}
}

void xSlightAdjust::SetCurrentPos(osg::Vec3 pos)
{
	m_XCenterValueLEdit->setText(QString::number(pos.x(),'f',preci));
	m_YCenterValueLEdit->setText(QString::number(pos.y(),'f',preci));
	m_ZCenterValueLEdit->setText(QString::number(pos.z(),'f',preci));
}

void xSlightAdjust::SetIsUseSoftKeyboard(bool var)
{
	m_XValueLEdit->setUseKeyboard(var);
	m_YValueLEdit->setUseKeyboard(var);
	m_ZValueLEdit->setUseKeyboard(var);
	m_XCenterValueLEdit->setUseKeyboard(var);
	m_YCenterValueLEdit->setUseKeyboard(var);
	m_ZCenterValueLEdit->setUseKeyboard(var);
}

void xSlightAdjust::SlotOk()
{
	if(m_ModelCombox->currentIndex()!=0 
		&& m_XValueLEdit->text().isEmpty()==false 
		&& m_YValueLEdit->text().isEmpty()==false 
		&& m_ZValueLEdit->text().isEmpty()==false)
	{
		if(m_XCenterValueLEdit->text().isEmpty()==false
			&& m_XCenterValueLEdit->text().isEmpty()==false
			&& m_XCenterValueLEdit->text().isEmpty()==false)
		{
			double x_ = m_XCenterValueLEdit->text().toDouble();
			double y_ = m_YCenterValueLEdit->text().toDouble();
			double z_ = m_ZCenterValueLEdit->text().toDouble();

			x_ += m_XValueLEdit->text().toDouble();
			y_ += m_YValueLEdit->text().toDouble();
			z_ += m_ZValueLEdit->text().toDouble();

			m_XCenterValueLEdit->setText(QString::number(x_,'f',preci));
			m_YCenterValueLEdit->setText(QString::number(y_,'f',preci));
			m_ZCenterValueLEdit->setText(QString::number(z_,'f',preci));

			emit SigSettingSlightAdjustFinished();
		}
		else
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("原始中心未填写!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[85];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("设置不全!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[85];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}
}

void xSlightAdjust::On_Item_changed(int index)
{
	Before();

	if(index>0)
	{
		if(m_pMainWindow != NULL)
		{
			QString name = m_ModelCombox->currentText();
			m_pMainWindow->On_modelColor_change(name);
			//m_pMainWindow->On_modelColor_change(name);

			size_t i;
			for (i=0;i<m_vInitPosFileValues.size();i++)
			{
				if (m_vInitPosFileValues[i].ModelName == name)
				{
					break;
				}
			}

			if (i<m_vInitPosFileValues.size())
			{
				//m_XValueLEdit->clear();
				//m_YValueLEdit->clear();
				//m_ZValueLEdit->clear();

				SetDefaultValue();

				m_XCenterValueLEdit->setText(QString::number(m_vInitPosFileValues[i].ModelCenter.x(),'f',preci));
				m_YCenterValueLEdit->setText(QString::number(m_vInitPosFileValues[i].ModelCenter.y(),'f',preci));
				m_ZCenterValueLEdit->setText(QString::number(m_vInitPosFileValues[i].ModelCenter.z(),'f',preci));
			}
			else
			{
				m_XValueLEdit->setText(initValue);;
				m_YValueLEdit->setText(initValue);
				m_ZValueLEdit->setText(initValue);
			}
		}
	}
	else if(index == 0)
	{
		m_pMainWindow->ResumeAllModelColor();
	}
}

void xSlightAdjust::SlotSetCenter(int state)
{
	if( state == Qt::Unchecked)//未选中
	{
		m_XCenterValueLEdit->setEnabled(false);
		m_YCenterValueLEdit->setEnabled(false);
		m_ZCenterValueLEdit->setEnabled(false);

		m_XCenterValueLEdit->clear();
		m_YCenterValueLEdit->clear();
		m_ZCenterValueLEdit->clear();

	}
	else if(state == Qt::Checked)
	{
		m_XCenterValueLEdit->setEnabled(true);
		m_YCenterValueLEdit->setEnabled(true);
		m_ZCenterValueLEdit->setEnabled(true);

		m_XCenterValueLEdit->setText(initValue);
		m_YCenterValueLEdit->setText(initValue);
		m_ZCenterValueLEdit->setText(initValue);
	}
}

// 设置各输入框默认值
void xSlightAdjust::SetDefaultValue()
{
	m_XValueLEdit->setText(initValue);
	m_YValueLEdit->setText(initValue);
	m_ZValueLEdit->setText(initValue);

	m_XCenterValueLEdit->setText(initValue);
	m_YCenterValueLEdit->setText(initValue);
	m_ZCenterValueLEdit->setText(initValue);
}

void xSlightAdjust::showEvent(QShowEvent *event)
{
	if (m_pMainWindow)
	{
		m_pMainWindow->SetAdjusting(true);

		SetDefaultValue();
	}
}

void xSlightAdjust::closeEvent(QCloseEvent *event)
{
	m_pMainWindow->ResumeAllModelColor();
	if (m_pMainWindow)
	{
		m_pMainWindow->SetAdjusting(false);
	}
}
