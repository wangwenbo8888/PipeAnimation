#include "xReplaceWheelModel.h"
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>
#include "MainWindow.h"

#include "log.h"

xReplaceWheelModel::xReplaceWheelModel(MainWindow *p)
{
	m_MainWindow = p;
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	//this->setWindowTitle(QString::fromLocal8Bit("修改模型"));

	InitDlg();
	connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_ModelComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotHighLightModel()));
	connect(m_ModelTypeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotChangeUIByModelType()));

}

xReplaceWheelModel::~xReplaceWheelModel()
{

}


void xReplaceWheelModel::SetLMFileValues(std::vector<LMR> values)
{
	m_vCurLMFileValues = values;
}

void xReplaceWheelModel::SetJMFileValues(std::vector<JM> values)
{
	m_vCurJMFileValues = values;
}

void xReplaceWheelModel::InitDlg()
{
	QString str;
	hModelLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("模型:");
	m_ModelLabel = new QLabel(str);
	m_ModelComBox = new QComboBox;
	m_ModelComBox->setMaxVisibleItems(35);
	str = str.fromLocal8Bit("--请选择模型--");
	m_ModelComBox->addItem(str);
	m_ModelComBox->view()->setFixedWidth(170);
	hModelLayout->addWidget(m_ModelLabel);
	hModelLayout->addWidget(m_ModelComBox);

	QDoubleValidator* lineDouble = new QDoubleValidator(-100000, 100000, 5, nullptr);

	hRLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("半径:");
	m_LMrLabel = new QLabel(str);
	m_LMrLEdit = new SoftKeyLineEdit;
	m_LMrLEdit->setValidator(lineDouble);
	hRLayout->addWidget(m_LMrLabel);
	hRLayout->addWidget(m_LMrLEdit);

	hUpLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("上端:");
	m_UpLenLabel = new QLabel(str);
	m_UpLenLEdit = new SoftKeyLineEdit;
	m_UpLenLEdit->setValidator(lineDouble);
	hUpLayout->addWidget(m_UpLenLabel);
	hUpLayout->addWidget(m_UpLenLEdit);

	hDownLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("下端:");
	m_DownLenLabel = new QLabel(str);
	m_DownLenLEdit = new SoftKeyLineEdit;
	m_DownLenLEdit->setValidator(lineDouble);
	hDownLayout->addWidget(m_DownLenLabel);
	hDownLayout->addWidget(m_DownLenLEdit);

	hBtnLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("确定");
	m_OkBtn = new QPushButton(str);
	str = str.fromLocal8Bit("取消");
	m_CancelBtn = new QPushButton(str);		
	hBtnLayout->addWidget(m_OkBtn);

	hTuLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("夹模长:");
	m_TuLabel = new QLabel(str);
	m_TuLenLEdit = new SoftKeyLineEdit;
	m_TuLenLEdit->setValidator(lineDouble);
	m_TuCheckBox = new QCheckBox();
	m_TuCheckBox->setStyle(QStyleFactory::create("Cleanlooks"));

	m_TuCheckBox->setChecked(false);

	m_TuSideCombox = new QComboBox;
	str = str.fromLocal8Bit("左:0");
	m_TuSideCombox->addItem(str);
	str = str.fromLocal8Bit("右:1");
	m_TuSideCombox->addItem(str);
	m_TuSideCombox->view()->setFixedWidth(100);
	hTuLayout->addWidget(m_TuLabel);
	hTuLayout->addWidget(m_TuLenLEdit);
	hTuLayout->addWidget(m_TuSideCombox);



	hModelTypeLayout = new QHBoxLayout;
	m_ModelTypeCombox = new QComboBox;
	str = str.fromLocal8Bit("轮模");
	m_ModelTypeCombox->addItem(str);
	str = str.fromLocal8Bit("夹模");
	m_ModelTypeCombox->addItem(str);
	str = str.fromLocal8Bit("类型:");	
	m_ModelTypeLabel = new QLabel(str);
	hModelTypeLayout->addWidget(m_ModelTypeLabel);
	hModelTypeLayout->addWidget(m_ModelTypeCombox);

	hOriginalJMLenLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("原始夹模长:");	
	m_OriginalLenLabel = new QLabel(str);
	m_OriginalLenLEdit = new SoftKeyLineEdit;
	m_OriginalLenLEdit->setValidator(lineDouble);
	hOriginalJMLenLayout->addWidget(m_OriginalLenLabel);
	hOriginalJMLenLayout->addWidget(m_OriginalLenLEdit);

	hCurJMLenLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("目标夹模长:");	
	m_CurLenLabel = new QLabel(str);
	m_CurLenLEdit = new SoftKeyLineEdit;
	m_CurLenLEdit->setValidator(lineDouble);
	hCurJMLenLayout->addWidget(m_CurLenLabel);
	hCurJMLenLayout->addWidget(m_CurLenLEdit);

	hJMLenLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("夹模长:");	
	m_JmLenLabel = new QLabel(str);
	m_JmLenLEdit = new SoftKeyLineEdit;
	m_JmLenLEdit->setValidator(lineDouble);
	m_AoSideCombox = new QComboBox;
	m_AidJMCombox = new QComboBox;
	str = str.fromLocal8Bit("主:0");
	m_AidJMCombox->addItem(str);
	str = str.fromLocal8Bit("辅:1");
	m_AidJMCombox->addItem(str);
	m_AidJMCombox->view()->setFixedWidth(100);

	str = str.fromLocal8Bit("左:0");
	m_AoSideCombox->addItem(str);
	str = str.fromLocal8Bit("右:1");
	m_AoSideCombox->addItem(str);
	m_AoSideCombox->view()->setFixedWidth(100);
	hJMLenLayout->addWidget(m_JmLenLabel);
	hJMLenLayout->addWidget(m_JmLenLEdit);
	hJMLenLayout->addWidget(m_AoSideCombox);
	hJMLenLayout->addWidget(m_AidJMCombox);



	hJMWideLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("夹模宽:");	
	m_JmWideLabel = new QLabel(str);
	m_JmWideLEdit = new SoftKeyLineEdit;
	m_JmWideLEdit->setValidator(lineDouble);
	hJMWideLayout->addWidget(m_JmWideLabel);
	hJMWideLayout->addWidget(m_JmWideLEdit);

	hJTNthLayout = new QHBoxLayout;
	str = str.fromLocal8Bit("机头编号");
	m_JiTouNthTable = new QLabel(str);
	m_JiTouNthLEdit = new SoftKeyLineEdit;
	hJTNthLayout->addWidget(m_JiTouNthTable);
	hJTNthLayout->addWidget(m_JiTouNthLEdit);


	/*QGridLayout **/gLayout = new QGridLayout;
	QVBoxLayout *VLayout = new QVBoxLayout;
	gLayout->addLayout(hModelTypeLayout,0,0);
	gLayout->addLayout(hModelLayout,1,0);
	gLayout->addLayout(hUpLayout,2,0);
	gLayout->addLayout(hDownLayout,3,0);
	gLayout->addLayout(hTuLayout,4,0);
	gLayout->addLayout(hRLayout,5,0);
	gLayout->addLayout(hJTNthLayout,6,0);
	gLayout->addLayout(hBtnLayout,7,0);


	this->setLayout(gLayout);
}

void xReplaceWheelModel::InitModelCombox(std::vector<QString> vModelName)
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
	m_ModelTypeCombox->setCurrentIndex(0);
}

double xReplaceWheelModel::GetLmR()
{
	return m_curLMr;
}

double xReplaceWheelModel::GetUpLen()
{
	return m_upLen;
}

double xReplaceWheelModel::GetDownLen()
{
	return m_downLen;

}

QString xReplaceWheelModel::GetChosenModelName()
{
	return m_curModelName;
}

QString xReplaceWheelModel::GetHighLightModelName()
{
	return m_curModelName;

}

void xReplaceWheelModel::setReplaceDlgLanguage(std::vector<QString> vCurWords)
{
	m_ModelComBox->setItemText(0,vCurWords[84]);

	m_ModelTypeLabel->setText(vCurWords[56]);
	m_ModelTypeCombox->setItemText(0,vCurWords[57]);
	m_ModelTypeCombox->setItemText(1,vCurWords[58]);
	m_ModelLabel->setText(vCurWords[59]);
	m_UpLenLabel->setText(vCurWords[60]);
	m_DownLenLabel->setText(vCurWords[61]);
	m_TuLabel->setText(vCurWords[62]);
	m_TuSideCombox->setItemText(0,vCurWords[63]);
	m_TuSideCombox->setItemText(1,vCurWords[64]);
	m_AoSideCombox->setItemText(0,vCurWords[63]);
	m_AoSideCombox->setItemText(1,vCurWords[64]);
	m_LMrLabel->setText(vCurWords[65]);

	m_JmLenLabel->setText(vCurWords[62]);
	m_JmWideLabel->setText(vCurWords[66]);
	m_JiTouNthTable->setText(vCurWords[67]);
	m_OkBtn->setText(vCurWords[68]);

	m_vCurLanguageWords = vCurWords;
}

void xReplaceWheelModel::SetIsUseSoftKeyboard(bool var)
{
	m_LMrLEdit->setUseKeyboard(var);
	m_UpLenLEdit->setUseKeyboard(var);
	m_DownLenLEdit->setUseKeyboard(var);
	m_TuLenLEdit->setUseKeyboard(var);
	//m_OriginalLenLEdit->setUseKeyboard(var);
	//m_CurLenLEdit->setUseKeyboard(var);
	m_JmLenLEdit->setUseKeyboard(var);
	m_JmWideLEdit->setUseKeyboard(var);
	//m_MinLEdit->setUseKeyboard(var);
	//m_MaxLEdit->setUseKeyboard(var);
	m_JiTouNthLEdit->setUseKeyboard(var);


}

void xReplaceWheelModel::SlotOk()
{
	if (m_ModelTypeCombox->currentIndex() == 0)
	{

		if(m_ModelComBox->currentIndex()!=0 && m_LMrLEdit->text().isEmpty()==false && m_UpLenLEdit->text().isEmpty()==false && m_DownLenLEdit->text().isEmpty()==false && m_TuLenLEdit->text().isEmpty()==false)
		{
			m_curModelName = m_ModelComBox->currentText(); 
			m_curLMr = m_LMrLEdit->text().toDouble();
			m_upLen = m_UpLenLEdit->text().toDouble();
			m_downLen = m_DownLenLEdit->text().toDouble();
			m_isHaveTu = m_TuCheckBox->isChecked();
			m_TuLen = m_TuLenLEdit->text().toDouble();
			m_TuSide = m_TuSideCombox->currentIndex();
			m_JTNth = m_JiTouNthLEdit->text().toInt();
			emit SigSettingLMrFinished();
		}
		else
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = QString::fromLocal8Bit("设置不全");
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
	else if (m_ModelTypeCombox->currentIndex() == 1 )
	{
		if (m_ModelComBox->currentIndex()!=0 && m_UpLenLEdit->text().isEmpty()==false && m_DownLenLEdit->text().isEmpty()==false && m_JmLenLEdit->text().isEmpty()==false &&  m_JmWideLEdit->text().isEmpty()==false)
		{
			m_curModelName = m_ModelComBox->currentText();
			m_upLen = m_UpLenLEdit->text().toDouble();
			m_downLen = m_DownLenLEdit->text().toDouble();
			m_JmLen = m_JmLenLEdit->text().toDouble();
			m_JmWide = m_JmWideLEdit->text().toDouble();
			m_AoSide = m_AoSideCombox->currentIndex();//0:左，1有
			m_AidJM = m_AidJMCombox->currentIndex();//0zhu,1fu
			m_JTNth = m_JiTouNthLEdit->text().toInt();

			//m_originalJMLen = m_OriginalLenLEdit->text().toDouble();
			//m_curJMLen = m_CurLenLEdit->text().toDouble();
			emit SigSettingJMFinished();
		}
		else
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = QString::fromLocal8Bit("设置不全");
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
}

void xReplaceWheelModel::SlotHighLightModel()
{
	if(m_ModelComBox->currentIndex()==0)
	{
		m_MainWindow->ResumeAllModelColor();
	     
		m_LMrLEdit->clear();
		m_UpLenLEdit->clear();
		m_DownLenLEdit->clear();	
		m_TuLenLEdit->clear();
	}
	else 
	{
		if(m_MainWindow != NULL)
		{
			if (m_ModelTypeCombox->currentIndex() == 0)//导模
			{
				QString name = m_ModelComBox->currentText();
				m_MainWindow->On_modelColor_change(name);
				size_t i;
				for (i=0;i<m_vCurLMFileValues.size();i++)
				{
					if (m_vCurLMFileValues[i].strModelName == name)
					{
						break;
					}
				}
				if (i<m_vCurLMFileValues.size())
				{
					m_LMrLEdit->clear();
					m_UpLenLEdit->clear();
					m_DownLenLEdit->clear();	
					m_TuLenLEdit->clear();
					m_JiTouNthLEdit->clear();

					m_LMrLEdit->setText(QString::number(m_vCurLMFileValues[i].fLmr));
					m_UpLenLEdit->setText(QString::number(m_vCurLMFileValues[i].fupLen));
					m_DownLenLEdit->setText(QString::number(m_vCurLMFileValues[i].fdownLen));
					m_TuLenLEdit->setText(QString::number(m_vCurLMFileValues[i].ftuLen));
					m_JiTouNthLEdit->setText(QString::number(m_vCurLMFileValues[i].iJTNth));
					int index = m_vCurLMFileValues[i].iSide;
					m_TuSideCombox->setCurrentIndex(index);
				}
				else
				{
					m_LMrLEdit->clear();
					m_UpLenLEdit->clear();
					m_DownLenLEdit->clear();	
					m_TuLenLEdit->clear();
					m_JiTouNthLEdit->clear();
				}
			}
			else if (m_ModelTypeCombox->currentIndex() == 1)
			{
				QString name = m_ModelComBox->currentText();
				m_MainWindow->On_modelColor_change(name);

				size_t i;
				for (i=0;i<m_vCurJMFileValues.size();i++)
				{
					if (m_vCurJMFileValues[i].strModelName == name)
					{
						break;
					}
				}
				if (i<m_vCurJMFileValues.size())
				{
					m_UpLenLEdit->clear();
					m_DownLenLEdit->clear();	
					m_JmLenLEdit->clear();
					m_JmWideLEdit->clear();
					m_JiTouNthLEdit->clear();

					m_JmLenLEdit->setText(QString::number(m_vCurJMFileValues[i].fLength));
					m_UpLenLEdit->setText(QString::number(m_vCurJMFileValues[i].fUpHeight));
					m_DownLenLEdit->setText(QString::number(m_vCurJMFileValues[i].fDownHeight));
					m_JmWideLEdit->setText(QString::number(m_vCurJMFileValues[i].fWide));
					m_JiTouNthLEdit->setText(QString::number(m_vCurJMFileValues[i].iJTNth));
					int index = m_vCurJMFileValues[i].iSide;
					m_AoSideCombox->setCurrentIndex(index);
					index = m_vCurJMFileValues[i].iAid;
					m_AidJMCombox->setCurrentIndex(index);

				}
				else
				{
					m_UpLenLEdit->clear();
					m_DownLenLEdit->clear();	
					m_JmLenLEdit->clear();
					m_JmWideLEdit->clear();
					m_JiTouNthLEdit->clear();
				}
			}

		}
	}
}

void xReplaceWheelModel::closeEvent(QCloseEvent *event)
{
	m_MainWindow->ResumeAllModelColor();
}

void xReplaceWheelModel::SlotChangeUIByModelType()
{
	if (m_ModelTypeCombox->currentIndex() == 0)//轮模UI
	{	
		m_TuLabel->show();
		m_TuLenLEdit->show();
		m_TuSideCombox->show();
		m_LMrLabel->show();
		m_LMrLEdit->show();

		m_JmLenLabel->hide();
		m_JmLenLEdit->hide();
		m_AoSideCombox->hide();
		m_AidJMCombox->hide();
		m_JmWideLabel->hide();	
		m_JmWideLEdit->hide();

		gLayout->addLayout(hTuLayout,4,0);
		gLayout->addLayout(hRLayout,5,0);
		
	}
	else if (m_ModelTypeCombox->currentIndex() == 1)//夹模UI
	{
		
	
		m_TuLabel->hide();
		m_TuLenLEdit->hide();
		m_TuSideCombox->hide();
		m_LMrLabel->hide();
		m_LMrLEdit->hide();


		m_JmLenLabel->show();
		m_JmLenLEdit->show();
		m_AoSideCombox->show();
		m_AidJMCombox->show();
		m_JmWideLabel->show();	
		m_JmWideLEdit->show();


		gLayout->addLayout(hJMLenLayout,4,0);
		gLayout->addLayout(hJMWideLayout,5,0);

	}
}
