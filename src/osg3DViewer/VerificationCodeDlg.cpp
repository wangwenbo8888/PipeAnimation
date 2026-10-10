#include "VerificationCodeDlg.h"

//#include "CPUID.h"
//#include "MacAddress.h"
//#include "Encode.h"

VerificationCodeDlg::VerificationCodeDlg()
{
	InitDialog();
	connect(m_pOk, SIGNAL(clicked()), this, SLOT(SlotOk()));

}

VerificationCodeDlg::~VerificationCodeDlg(void)
{

}

void VerificationCodeDlg::setUserNum(std::string strnum)
{
	QString str = QString::fromStdString(strnum);
	m_UserNumLEdit->setText(str);
}

void VerificationCodeDlg::InitDialog()
{
	m_pBtnGLayout = new QGridLayout;
	m_pOk = new QPushButton("OK");
	m_pCancel = new QPushButton("Cancel");

	m_userLabel = new QLabel("User");
	m_userCombox = new QComboBox;
	m_userCombox->addItem("Normal");
	m_userCombox->addItem("Administrator");

	m_userNumLabel = new QLabel("UserNum:");
	m_UserNumLEdit = new QLineEdit;
	m_UserNumLEdit->setReadOnly(true);
	m_pCodeLabel = new QLabel("Code:");
	m_pCodeLineEdit = new QLineEdit;
	m_pBtnGLayout->addWidget(m_pCodeLabel,1,0);
	m_pBtnGLayout->addWidget(m_pCodeLineEdit,1,1);
	m_pBtnGLayout->addWidget(m_userNumLabel,0,0);
	m_pBtnGLayout->addWidget(m_UserNumLEdit,0,1);
	m_pBtnGLayout->addWidget(m_pCancel,2,0);
	m_pBtnGLayout->addWidget(m_pOk,2,1);
	this->setLayout(m_pBtnGLayout);


}

void VerificationCodeDlg::SetIsUseSoftKeyboard(bool var)
{
	//m_pCodeLineEdit->setUseKeyboard(var);

}

void VerificationCodeDlg::SlotOk()
{
	//emit sigGetCode(m_pCodeLineEdit->text());
	emit accept();
}

