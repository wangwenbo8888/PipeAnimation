#include "xAddModelDlg.h"
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>
#include "MainWindow.h"

#include "log.h"

xAddModel::xAddModel(MainWindow *p)
{
	m_MainWindow = p;
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	//this->setWindowTitle(QString::fromLocal8Bit("累加模型"));

	InitDlg();
	connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_ModelComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotHighLightModel()));
}

xAddModel::~xAddModel()
{

}

void xAddModel::InitDlg()
{
	QString str;
	str = str.fromLocal8Bit("模型:");
	m_ModelLabel = new QLabel(str);
	m_ModelComBox = new QComboBox;
	m_ModelComBox->setMaxVisibleItems(35);
	str = str.fromLocal8Bit("--请选择模型--");
	m_ModelComBox->addItem(str);
	m_ModelComBox->view()->setFixedWidth(170);
	QHBoxLayout *hModelLayout = new QHBoxLayout;
	hModelLayout->addWidget(m_ModelLabel);
	hModelLayout->addWidget(m_ModelComBox);


	str = str.fromLocal8Bit("新增模型名:");
	m_NewModelNameLabel = new QLabel(str);
	m_NewModelNameLEdit = new SoftKeyLineEdit;
	QHBoxLayout *hNewModelLayout = new QHBoxLayout;
	hNewModelLayout->addWidget(m_NewModelNameLabel);
	hNewModelLayout->addWidget(m_NewModelNameLEdit);


	str = str.fromLocal8Bit("新模型向上:");
	m_NewUpUnitsLabel = new QLabel(str);
	m_NewUpUnitsLEdit = new SoftKeyLineEdit;
	QHBoxLayout *hNewUpModelLayout = new QHBoxLayout;
	hNewUpModelLayout->addWidget(m_NewUpUnitsLabel);
	hNewUpModelLayout->addWidget(m_NewUpUnitsLEdit);

	str = str.fromLocal8Bit("机头编号");
	m_JTNthLabel = new QLabel(str);
	m_JTNthLEdit = new SoftKeyLineEdit;
	QHBoxLayout *hNthJTLayout = new QHBoxLayout;
	hNthJTLayout->addWidget(m_JTNthLabel);
	hNthJTLayout->addWidget(m_JTNthLEdit);


	str = str.fromLocal8Bit("确定");
	m_OkBtn = new QPushButton(str);

	str = str.fromLocal8Bit("取消");
	m_CancelBtn = new QPushButton(str);

	QHBoxLayout *hBtnLayout = new QHBoxLayout;
	hBtnLayout->addWidget(m_OkBtn);



	QVBoxLayout *VLayout = new QVBoxLayout;
	VLayout->addLayout(hModelLayout);
	VLayout->addLayout(hNewModelLayout);
	VLayout->addLayout(hNewUpModelLayout);
	VLayout->addLayout(hNthJTLayout);
	VLayout->addLayout(hBtnLayout);
	this->setLayout(VLayout);
}

void xAddModel::InitModelCombox(std::vector<QString> vModelName)
{
	size_t count = m_ModelComBox->count();
	for(size_t i=count-1;i>0;i--)
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

QString xAddModel::GetChosenModelName()
{
	return m_CopyModelName;
}

void xAddModel::SetAccumulateDlgLanguage(std::vector<QString> vCurWords)
{
	m_ModelComBox->setItemText(0,vCurWords[84]);


	m_ModelLabel->setText(vCurWords[51]);
	m_NewModelNameLabel->setText(vCurWords[52]);
	m_NewUpUnitsLabel->setText(vCurWords[53]);
	m_JTNthLabel->setText(vCurWords[54]);
	m_OkBtn->setText(vCurWords[55]);

	m_vCurLanguageWords = vCurWords;
}

void xAddModel::SetIsUseSoftKeyboard(bool var)
{
	m_NewModelNameLEdit->setUseKeyboard(var);
	m_JTNthLEdit->setUseKeyboard(var);
	m_NewUpUnitsLEdit->setUseKeyboard(var);
}

void xAddModel::SlotOk()
{
	if(m_ModelComBox->currentIndex()!=0 && m_NewModelNameLEdit->text().isEmpty()==false && m_NewUpUnitsLEdit->text().isEmpty()==false)
	{
		m_NewModelName = m_NewModelNameLEdit->text();
		m_CopyModelName	= m_ModelComBox->currentText(); 
		m_curUpUnits = m_NewUpUnitsLEdit->text().toInt();
		m_iNthJT = m_JTNthLEdit->text().toInt();

		emit SigSettingAddModelFinished();
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

void xAddModel::SlotHighLightModel()
{
	if(m_ModelComBox->currentIndex()==0)
	{
		m_MainWindow->ResumeAllModelColor();

	}
	else 
	{
		if(m_MainWindow != NULL)
		{
			QString name = m_ModelComBox->currentText();
			m_MainWindow->On_modelColor_change(name);
		}
	}
}

void xAddModel::closeEvent(QCloseEvent *event)
{
	m_MainWindow->ResumeAllModelColor();

}
