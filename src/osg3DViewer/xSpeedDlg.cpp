#include "xSpeedDlg.h"
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>

xSpeedDlg::xSpeedDlg()
{
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	InitDlg();

	//connect(m_okBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
}

xSpeedDlg::~xSpeedDlg()
{
}

void xSpeedDlg::InitDlg()
{
	QString str;
	str = str.fromLocal8Bit("速度");
	m_SpeedLabel = new QLabel(str);
	m_SpeedValue = new QLabel;
	m_SpeedSlider = new QSlider(Qt::Horizontal);

	connect(m_SpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(RefreshLabel(int)));

	//m_SpeedSlider->setMinimum(1);
	m_SpeedSlider->setRange(1,5);
	m_SpeedSlider->setTickInterval(1);
	//m_SpeedSlider->setSingleStep(1);
	m_SpeedSlider->setPageStep(1);

	m_SpeedSlider->setValue(5);
	m_SpeedValue->setText("5");

	QHBoxLayout *Speedlayout = new QHBoxLayout;
	Speedlayout->addWidget(m_SpeedLabel);
	Speedlayout->addWidget(m_SpeedSlider);
	Speedlayout->addWidget(m_SpeedValue);

	//str = str.fromLocal8Bit("确定");
	//m_okBtn = new QPushButton(str);

	QVBoxLayout *all = new QVBoxLayout;
	all->addLayout(Speedlayout);
	//all->addWidget(m_okBtn);

	this->setLayout(all);
}

void xSpeedDlg::RefreshLabel(int i)
{
	m_SpeedValue->setText(QString::number(i));
}

void xSpeedDlg::SlotOk()
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
