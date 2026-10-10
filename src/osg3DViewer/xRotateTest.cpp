#include "xRotateTest.h"
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>
#include "MainWindow.h"

#include "log.h"

xRotateTest::xRotateTest(MainWindow *pParent)
{
	m_MainWindow = pParent;
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	//this->setWindowTitle(QString::fromLocal8Bit("测试旋转中心"));

	InitDlg();
	this->resize(300,120);
	connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_ModelCombox,SIGNAL(currentIndexChanged(int)),this, SLOT(On_Item_changed(int)), Qt::UniqueConnection);

}

xRotateTest::~xRotateTest()
{

}

void xRotateTest::InitDlg()
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
	//m_AxisCombox->view()->setFixedWidth(150);
	QHBoxLayout *haxisLayout = new QHBoxLayout;
	haxisLayout->addWidget(m_AxisLabel);
	haxisLayout->addWidget(m_AxisCombox);


	str = str.fromLocal8Bit("数值:");
	m_RotateValueLabel = new QLabel(str);
	m_RotateValueLEdit = new SoftKeyLineEdit;
	QDoubleValidator* lineDouble = new QDoubleValidator(-100000, 100000, 5, nullptr);
	m_RotateValueLEdit->setValidator(lineDouble);



	str = str.fromLocal8Bit("中心(x,y,z):");
	m_RotateCenterLabel = new QLabel(str);
	m_XRotateCenterLEdit = new SoftKeyLineEdit(this);
	m_YRotateCenterLEdit = new SoftKeyLineEdit(this);
	m_ZRotateCenterLEdit = new SoftKeyLineEdit(this);

	m_XRotateCenterLEdit->setValidator(lineDouble);
	m_YRotateCenterLEdit->setValidator(lineDouble);
	m_ZRotateCenterLEdit->setValidator(lineDouble);
	QHBoxLayout *hvalueLayout = new QHBoxLayout;
	//hvalueLayout->addWidget(m_ValueLabel);
	hvalueLayout->addWidget(m_XRotateCenterLEdit);
	hvalueLayout->addWidget(m_YRotateCenterLEdit);
	hvalueLayout->addWidget(m_ZRotateCenterLEdit);



	str = str.fromLocal8Bit("确定");
	m_OkBtn = new QPushButton(str);
	str = str.fromLocal8Bit("取消");
	m_CancelBtn = new QPushButton(str);
	QHBoxLayout *hBtnLayout = new QHBoxLayout;
	//hBtnLayout->addWidget(m_CancelBtn);
	hBtnLayout->addWidget(m_OkBtn);


	QGridLayout *gLayout = new QGridLayout;
	gLayout->addWidget(m_ModlLabel,0,0,1,1);
	gLayout->addWidget(m_ModelCombox,0,1,1,1);
	gLayout->addWidget(m_RotateValueLabel,1,0,1,1);
	gLayout->addWidget(m_RotateValueLEdit,1,1,1,1);
	gLayout->addWidget(m_AxisLabel,2,0,1,1);
	gLayout->addWidget(m_AxisCombox,2,1,1,1);
	gLayout->addWidget(m_RotateCenterLabel,3,0,1,1);
	gLayout->addLayout(hvalueLayout,3,1,1,1);
	gLayout->addLayout(hBtnLayout,4,1,1,1);

	this->setLayout(gLayout);
}

void xRotateTest::InitModelCombox(std::vector<QString> vModelName)
{
	size_t count = m_ModelCombox->count();
	for(size_t i=count -1;i>0;i--)
	{
		m_ModelCombox->removeItem(i);
	}

	
	for (size_t j=0;j<vModelName.size();j++)
	{
		m_ModelCombox->addItem(vModelName[j]);
	}

	m_ModelCombox->setCurrentIndex(0);
	m_AxisCombox->setCurrentIndex(0);
}

double xRotateTest::GetRotateAngle()
{
	m_curRotateAngle = m_RotateValueLEdit->text().toDouble();
	return m_curRotateAngle;
}

int xRotateTest::GetAxis()
{
	int index = m_AxisCombox->currentIndex();
	return index;
}

QString xRotateTest::GetChosenModelName()
{
	QString str = m_ModelCombox->currentText();
	return str;
}

osg::Vec3 xRotateTest::GetRotateCenter()
{
	double Xvalue = m_XRotateCenterLEdit->text().toDouble();
	double Yvalue = m_YRotateCenterLEdit->text().toDouble();
	double Zvalue = m_ZRotateCenterLEdit->text().toDouble();

	osg::Vec3 center = osg::Vec3(Xvalue,Yvalue,Zvalue);
	return center;
}

void xRotateTest::SetRotateTestDlgLanguage(std::vector<QString> vCurWords)
{
	m_ModelCombox->setItemText(0,vCurWords[84]);


	m_ModlLabel->setText(vCurWords[74]);
	m_RotateValueLabel->setText(vCurWords[75]);
	m_AxisLabel->setText(vCurWords[76]);
	m_RotateCenterLabel->setText(vCurWords[77]);
	m_OkBtn->setText(vCurWords[78]);

	m_vCurLanguageWords = vCurWords;

}

void xRotateTest::SetIsUseSoftKeyboard(bool var)
{
	m_XRotateCenterLEdit->setUseKeyboard(var);
	m_YRotateCenterLEdit->setUseKeyboard(var);
	m_ZRotateCenterLEdit->setUseKeyboard(var);
	m_RotateValueLEdit->setUseKeyboard(var);

}

void xRotateTest::SlotOk()
{
	if(m_AxisCombox->currentIndex()!=0 && m_ModelCombox->currentIndex()!=0 
		&& m_XRotateCenterLEdit->text().isEmpty()==false &&  m_YRotateCenterLEdit->text().isEmpty()==false&&  m_ZRotateCenterLEdit->text().isEmpty()==false
		&&m_RotateValueLEdit->text().isEmpty() == false)
	{
		emit SigSettingRotateTestFinished();
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

void xRotateTest::closeEvent(QCloseEvent *event)
{
	m_MainWindow->ResumeAllModelColor();
}

void xRotateTest::On_Item_changed(int index)
{
	if(index == 0)
	{
		m_MainWindow->ResumeAllModelColor();

	}
	else 
	{
		if(m_MainWindow != NULL)
		{
			QString name = m_ModelCombox->currentText();
			m_MainWindow->On_modelColor_change(name);
		}
	}
}
