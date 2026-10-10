#include "xLanguageDlg.h"
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>

xLanguageDlg::xLanguageDlg()
{
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	InitDlg();

	connect(m_okBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
}

xLanguageDlg::~xLanguageDlg()
{

}

void xLanguageDlg::InitDlg()
{
	QString str;
	str = str.fromLocal8Bit("语言");
	m_LanguageLabel = new QLabel(str);
	m_LanguageComBox = new QComboBox;
	m_LanguageComBox->setMaxVisibleItems(10);
	//m_LanguageComBox->addItem("-------");
	m_LanguageComBox->view()->setFixedWidth(150);
	QHBoxLayout *languagelayout = new QHBoxLayout;
	languagelayout->addWidget(m_LanguageLabel);
	languagelayout->addWidget(m_LanguageComBox);

	str = str.fromLocal8Bit("确定");
	m_okBtn = new QPushButton(str);

	QVBoxLayout *all = new QVBoxLayout;
	all->addLayout(languagelayout);
	all->addWidget(m_okBtn);

	this->setLayout(all);
}

void xLanguageDlg::InitLanguageCombox(std::vector<std::vector<QString>> vLanguageName,QString strLanguage)
{
	int count = m_LanguageComBox->count();
	for(int i=count -1;i>=0;i--)
	{
		m_LanguageComBox->removeItem(i);
	}

	std::vector<std::vector<QString>>::iterator it = vLanguageName.begin();
	for(;it!=vLanguageName.end();it++)
	{
		std::vector<QString>::iterator itt = (*it).begin();	
		m_LanguageComBox->addItem(itt[1]);

		int key = (*it)[0].toInt();
		if ((*it)[1] == strLanguage)
		{
			//m_NumToDefalutLanguagePair = make_pair(key,m_strDefaultLanguageType);

			int count = m_LanguageComBox->count();
			m_LanguageComBox->setCurrentIndex(count-1);
		}
	}

}

void xLanguageDlg::SetLanguageDlgLanguage(std::vector<QString> vCurWords)
{
	//m_LanguageComBox->setItemText(0,vCurWords[52]);

	m_LanguageLabel->setText(vCurWords[31]);
	m_okBtn->setText(vCurWords[32]);

	m_vCurLanguageWords = vCurWords;

}

void xLanguageDlg::SlotOk()
{
	//if(m_LanguageComBox->currentIndex()>0)
	//{
		m_curLanguage = m_LanguageComBox->currentText();
		emit SigSettingLanguageFinished();

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
