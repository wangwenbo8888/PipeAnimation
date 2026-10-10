#include "xMoveRange.h"
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>

#include "log.h"

xMoveRange::xMoveRange()
{
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	//this->setWindowTitle(QString::fromLocal8Bit("设置模型运动范围"));

	InitDlg();
	connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_ModelComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotHighLightModel()));


}

xMoveRange::~xMoveRange()
{

}

void xMoveRange::InitDlg()
{
	QString str;
	str = str.fromLocal8Bit("模型:");
	m_ModelLabel = new QLabel(str);

	str = str.fromLocal8Bit("最小值:");
	m_MinLabel = new QLabel(str);

	str = str.fromLocal8Bit("最大值:");
	m_MaxLabel = new QLabel(str);

	m_ModelComBox = new QComboBox;
	str = str.fromLocal8Bit("--请选择模型--");
	m_ModelComBox->addItem(str);
	m_ModelComBox->view()->setFixedWidth(150);
	
	m_MinLEdit = new QLineEdit;
	m_MaxLEdit = new QLineEdit;
	QDoubleValidator* lineDouble = new QDoubleValidator(-100000, 100000, 5, nullptr);
	m_MinLEdit->setValidator(lineDouble);
	m_MaxLEdit->setValidator(lineDouble);

	QHBoxLayout *hModelLayout = new QHBoxLayout;
	hModelLayout->addWidget(m_ModelLabel);
	hModelLayout->addWidget(m_ModelComBox);

	QHBoxLayout *hMinLayout = new QHBoxLayout;
	hMinLayout->addWidget(m_MinLabel);
	hMinLayout->addWidget(m_MinLEdit);
	
	QHBoxLayout *hMaxLayout = new QHBoxLayout;
	hMaxLayout->addWidget(m_MaxLabel);
	hMaxLayout->addWidget(m_MaxLEdit);

	str = str.fromLocal8Bit("确定");
	m_OkBtn = new QPushButton(str);

	str = str.fromLocal8Bit("取消");
	m_CancelBtn = new QPushButton(str);

	QHBoxLayout *hBtnLayout = new QHBoxLayout;
	hBtnLayout->addWidget(m_OkBtn);
	//hBtnLayout->addWidget(m_CancelBtn);

	QVBoxLayout *VLayout = new QVBoxLayout;
	VLayout->addLayout(hModelLayout);
	VLayout->addLayout(hMinLayout);
	VLayout->addLayout(hMaxLayout);
	VLayout->addLayout(hBtnLayout);

	this->setLayout(VLayout);
}

void xMoveRange::InitModelCombox(std::vector<QString> vModelName)
{
	int count = m_ModelComBox->count();
	for(int i=1;i<count;i++)
	{
		m_ModelComBox->removeItem(i);
	}

	//
	for (size_t j=0;j<vModelName.size();j++)
	{
		m_ModelComBox->addItem(vModelName[j]);
	}

	m_ModelComBox->setCurrentIndex(0);
}

double xMoveRange::GetRangeMax()
{
	double max = m_MaxLEdit->text().toDouble();
	return max;
}

double xMoveRange::GetRangeMin()
{
	double min = m_MinLEdit->text().toDouble();
	return min;
}

QString xMoveRange::GetChosenModelName()
{
	QString str = m_ModelComBox->currentText();
	return str;
}

QString xMoveRange::GetHighLightModelName()
{
	return m_curModelName;
}

void xMoveRange::SlotOk()
{
	if(m_ModelComBox->currentIndex()!=0 && m_MaxLEdit->text().isEmpty()==false && m_MinLEdit->text().isEmpty()== false)
	{
		emit SigSettingMoveRangeFinished();
	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("设置不全!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}
}

void xMoveRange::SlotHighLightModel()
{
	if(m_ModelComBox->currentIndex()> 0)
	{
		m_curModelName = m_ModelComBox->currentText(); 
		emit SigSettingMoveRangeModelHighLight();
	}
}

