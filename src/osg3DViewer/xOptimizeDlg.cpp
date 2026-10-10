#include "xOptimizeDlg.h"
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QCheckBox>

xOptimizeDlg::xOptimizeDlg()
{
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	InitDlg();

	//connect(m_okBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
}

xOptimizeDlg::~xOptimizeDlg()
{
}

void xOptimizeDlg::InitDlg()
{
	QString str;
	//str = str.fromLocal8Bit("速度");
	//m_SpeedLabel = new QLabel(str);
	//m_SpeedValue = new QLabel;

	str = str.fromLocal8Bit("弯管变直管");
	m_PushButtonCurveToStraight = new QPushButton(str);

	str = str.fromLocal8Bit("修改YBC");
	m_PushButtonModifyYBC = new QPushButton(str);

	str = str.fromLocal8Bit("退出计算");
	m_PushButtonExit = new QPushButton(str);

	connect(m_PushButtonCurveToStraight, SIGNAL(clicked()), this, SLOT(On_Curve_To_Straight()));
	connect(m_PushButtonModifyYBC, SIGNAL(clicked()), this, SLOT(On_ModifyYBC()));
	connect(m_PushButtonExit, SIGNAL(clicked()), this, SLOT(On_Exit()));

//	m_SpeedValue->setText("5");

	QHBoxLayout *Speedlayout = new QHBoxLayout;
	//Speedlayout->addWidget(m_SpeedLabel);
	Speedlayout->addWidget(m_PushButtonCurveToStraight);
	Speedlayout->addWidget(m_PushButtonModifyYBC);
	Speedlayout->addWidget(m_PushButtonExit);
	//str = str.fromLocal8Bit("确定");
	//m_okBtn = new QPushButton(str);

	QVBoxLayout *all = new QVBoxLayout;
	all->addLayout(Speedlayout);
	//all->addWidget(m_okBtn);

	this->setLayout(all);
}

void xOptimizeDlg::On_Curve_To_Straight()
{
	m_OptimizeType = 0;
	close();
}

void xOptimizeDlg::On_ModifyYBC()
{
	m_OptimizeType = 1;
	close();
}

void xOptimizeDlg::On_Exit()
{
	m_OptimizeType = 2;
	close();
}

void xOptimizeDlg::RefreshLabel(int i)
{
//	m_SpeedValue->setText(QString::number(i));
}

void xOptimizeDlg::SlotOk()
{
	//if(m_LanguageComBox->currentIndex()>0)
	//{
		emit SigSettingSpeedFinished();

		this->close();
	//}
		/*else
		{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
		strWarning = QString::fromLocal8Bit("设置不全");
		}
		else
		{
		strWarning = m_vCurLanguageWords[53];
		}
		QMessageBox::warning(this,"",strWarning);
		return;
		}*/
	
}
