#include "xAdvanceUserDlg.h"
#include "MainWindow.h"
xAdvanceUserDlg::xAdvanceUserDlg(MainWindow *p)
{
	m_MainWindow = p;
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//´°¿ÚÇ°ÖÃ

	InitDlg();
	connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
}

xAdvanceUserDlg::~xAdvanceUserDlg()
{
}

void xAdvanceUserDlg::InitDlg()
{
	QString str;
	str = str.fromLocal8Bit("Password:");
	m_AdvancePasswordLabel = new QLabel(str);
	m_passwordLEdit = new SoftKeyLineEdit;
	m_passwordLEdit->setEchoMode(QLineEdit::Password);

	m_OkBtn = new QPushButton("OK");

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(m_AdvancePasswordLabel);
	hlayout->addWidget(m_passwordLEdit);

	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->addLayout(hlayout);
	vlayout->addWidget(m_OkBtn);

	this->setLayout(vlayout);
}

void xAdvanceUserDlg::clearPassword()
{
	m_passwordLEdit->clear();
}

void xAdvanceUserDlg::SetIsUseSoftKeyboard(bool var)
{
	m_passwordLEdit->setUseKeyboard(var);
}

void xAdvanceUserDlg::SlotOk()
{
	if (m_passwordLEdit->text().isEmpty() == false)
	{
		//emit SigInputAdvancePasswordFinished();
		emit accept();
		this->close();
	}
}
