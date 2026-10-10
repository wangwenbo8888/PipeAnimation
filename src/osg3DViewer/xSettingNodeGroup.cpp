#include "xSettingNodeGroup.h"
#include <QTextCodec>
#include <QColorDialog>
#include <QMessageBox>
#include <vector>
#include <string>
#include <osg/Group>
#include <QAbstractItemView>
#include <QWidget>
#include <QFrame>
//#include "language.h"

#include "log.h"

#include "MainWindow.h"
xSettingNodeGroupDlg::xSettingNodeGroupDlg(MainWindow *parent)
{
	m_MainWindow = parent;
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置

	InitDlg();
	connect(m_CancelBtn, SIGNAL(clicked()), this, SLOT(SlotCancel()));
	connect(m_okBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_addGroupBtn, SIGNAL(clicked()), this, SLOT(SlotAddGroup()));
	connect(m_delGroupBtn, SIGNAL(clicked()), this, SLOT(SlotDelGroup()));
	connect(m_addChildrenBtn, SIGNAL(clicked()), this, SLOT(SlotAddChildToGroup()));
	connect(m_delChildrenBtn, SIGNAL(clicked()), this, SLOT(SlotDelChildToGroup()));
	connect(m_allGroupNameCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotResetChildrenText()));
	connect(m_pChildItemsTable,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectTableItems()));
	connect(m_PosBtn, SIGNAL(clicked()), this, SLOT(SlotPosBtn()));
	connect(m_isGroupRBtn,SIGNAL(toggled(bool)),this,SLOT(SlotSetGroupUI()));
	connect(m_isSideRBtn,SIGNAL(toggled(bool)),this,SLOT(SlotSetSideUI()));

	connect(m_sideCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotResetSideChildrenText()));
	connect(m_JTNumComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotResetSideChildrenText()));
	connect(m_allchildrenNameCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotChangeChildrenCombox()));


}

xSettingNodeGroupDlg::~xSettingNodeGroupDlg()
{}

void xSettingNodeGroupDlg::InitDlg()
{
	this->resize(380,270);
	QString str;
	str = str.fromLocal8Bit("组");
	m_isGroupRBtn = new QRadioButton(str);
	m_isGroupRBtn->setChecked(true);
	str = str.fromLocal8Bit("边");
	m_isSideRBtn = new QRadioButton(str);
	m_UiTypeBtnGroup = new QButtonGroup;
	m_UiTypeBtnGroup->addButton(m_isGroupRBtn);
	m_UiTypeBtnGroup->addButton(m_isSideRBtn);

	QHBoxLayout *hGS = new QHBoxLayout;
	hGS->addWidget(m_isGroupRBtn);
	hGS->addWidget(m_isSideRBtn);

	str = str.fromLocal8Bit("边:");
	m_sideLabel = new QLabel(str);
	m_sideCombox = new QComboBox;
	m_sideCombox->setMaxVisibleItems(3);
	str = str.fromLocal8Bit("LEFT");
	m_sideCombox->addItem(str);
	str = str.fromLocal8Bit("CENTER");
	m_sideCombox->addItem(str);
	str = str.fromLocal8Bit("RIGHT");
	m_sideCombox->addItem(str);
	hSideLayout = new QHBoxLayout;
	hSideLayout->addWidget(m_sideLabel);
	hSideLayout->addWidget(m_sideCombox);

	str = str.fromLocal8Bit("机头编号:");
	m_JTNumLabel = new QLabel(str);
	m_JTNumComBox = new QComboBox;
	hJTNumLayout = new QHBoxLayout;
	hJTNumLayout->addWidget(m_JTNumLabel);
	hJTNumLayout->addWidget(m_JTNumComBox);

	str = str.fromLocal8Bit("组名:");
	m_GroupNameLabel = new QLabel(str);
	m_GroupNameLEdit = new SoftKeyLineEdit;
	str = str.fromLocal8Bit("轴:");
	m_AxisLabel = new QLabel(str);
	m_AxisComBox = new QComboBox;
	str = str.fromLocal8Bit("--请选择轴--");
	m_AxisComBox->addItem(str);
	str = str.fromLocal8Bit("X轴");
	m_AxisComBox->addItem(str);
	str = str.fromLocal8Bit("Y轴");
	m_AxisComBox->addItem(str);
	str = str.fromLocal8Bit("Z轴");
	m_AxisComBox->addItem(str);
	/*QHBoxLayout **/gNLayout = new QHBoxLayout;
	gNLayout->addWidget(m_GroupNameLabel);
	gNLayout->addWidget(m_GroupNameLEdit);
	//gNLayout->addWidget(m_AxisLabel);
	//gNLayout->addWidget(m_AxisComBox);

	str = str.fromLocal8Bit("新建");
	m_addGroupBtn = new QPushButton(str);
	str = str.fromLocal8Bit("删除");
	m_delGroupBtn = new QPushButton(str);

	str = str.fromLocal8Bit("组:");
	m_allGroupNameLabel = new QLabel(str);
	m_allGroupNameCombox = new QComboBox;
	m_allGroupNameCombox->setMaxVisibleItems(35);
	str = str.fromLocal8Bit("--请选择--");
	m_allGroupNameCombox->addItem(str);
	m_allGroupNameCombox->view()->setFixedWidth(150);
	/*QHBoxLayout **/hgroupLayout = new QHBoxLayout;
	hgroupLayout->addWidget(m_allGroupNameLabel);
	hgroupLayout->addWidget(m_allGroupNameCombox);

	str = str.fromLocal8Bit("子:");
	m_allchildrenNameLabel = new QLabel(str);
	m_allchildrenNameCombox = new QComboBox;
	m_allchildrenNameCombox->setMaxVisibleItems(35);
	str = str.fromLocal8Bit("--请选择--");
	m_allchildrenNameCombox->addItem(str);
	m_allchildrenNameCombox->view()->setFixedWidth(150);

	
	/*QHBoxLayout **/hchildLayout = new QHBoxLayout;
	hchildLayout->addWidget(m_allchildrenNameLabel);
	hchildLayout->addWidget(m_allchildrenNameCombox);

	str = str.fromLocal8Bit("增加");
	m_addChildrenBtn = new QPushButton(str);
	str = str.fromLocal8Bit("删除");
	m_delChildrenBtn = new QPushButton(str);

	m_ChildenNameText = new QTextEdit;
	//m_ChildenNameText->setText("--NONE--!");
	m_pChildItemsTable = new QTableWidget(0,1);
	m_pChildItemsTable->horizontalHeader()->hide();
	m_pChildItemsTable->setFrameShape(QFrame::NoFrame);      //设置无边框
	m_pChildItemsTable->setShowGrid(false);                //设置不显示格子线
	m_pChildItemsTable->verticalHeader()->setVisible(false); //设置垂直头不可见
	m_pChildItemsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
	m_pChildItemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
	m_pChildItemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
	m_pChildItemsTable->setStyleSheet("selection-background-color:lightblue;");  //设置选中背景色
	m_pChildItemsTable->setColumnWidth(0,500);
	m_pChildItemsTable->verticalHeader()->setDefaultSectionSize(17);    //设置默认行高

	m_ChildenNameText->setReadOnly(true);

	str = str.fromLocal8Bit("确定");
	m_okBtn = new QPushButton(str);
	str = str.fromLocal8Bit("取消");
	m_CancelBtn = new QPushButton(str);

	str = str.fromLocal8Bit("重建:");
	m_RebuildLMLabel = new QLabel(str);
	m_RebuildLMCheckBox = new QCheckBox;
	m_RebuildLMCheckBox->setStyle(QStyleFactory::create("Cleanlooks"));

	m_RebuildLMCheckBox->setChecked(false);
	m_isRebuildLM = false;
	/*QHBoxLayout **/rebuildLayout = new QHBoxLayout;
	rebuildLayout->addWidget(m_RebuildLMLabel);
	rebuildLayout->addWidget(m_RebuildLMCheckBox);

	str = str.fromLocal8Bit("坐标:");
	m_PosLabel = new QLabel(str);

	QDoubleValidator* lineDouble = new QDoubleValidator(-100000, 100000, 5, nullptr);
	m_XPosLEdit = new SoftKeyLineEdit;
	m_YPosLEdit = new SoftKeyLineEdit;
	m_ZPosLEdit = new SoftKeyLineEdit;
	//m_XPosLEdit->setma
	m_XPosLEdit->setValidator(lineDouble);
	m_YPosLEdit->setValidator(lineDouble);
	m_ZPosLEdit->setValidator(lineDouble);
	/*QHBoxLayout **/posLayout = new QHBoxLayout;
	posLayout->addWidget(m_PosLabel);
	posLayout->addWidget(m_XPosLEdit);
	posLayout->addWidget(m_YPosLEdit);
	posLayout->addWidget(m_ZPosLEdit);

	str = str.fromLocal8Bit("修改");
	m_PosBtn = new QPushButton(str);
	posLayout->addWidget(m_PosBtn);

	SlotResetChildrenText();

	/*QGridLayout **/glayout = new QGridLayout;
	glayout->addLayout(hGS,0,0,1,1);
	glayout->addLayout(gNLayout,1,0,1,2);
	glayout->addWidget(m_addGroupBtn,1,2,1,1);
	glayout->addWidget(m_delGroupBtn,1,3,1,1);
	glayout->addLayout(hgroupLayout,2,0,1,1);
	glayout->addLayout(hchildLayout,2,1,1,1);
	glayout->addWidget(m_addChildrenBtn,2,2,1,1);
	glayout->addWidget(m_delChildrenBtn,2,3,1,1);
	glayout->addWidget(m_pChildItemsTable,3,0,4,4);
	//glayout->addWidget(m_PosLabel,6,0,1,1);
	glayout->addLayout(posLayout,7,0,1,2);
	glayout->addWidget(m_okBtn,7,3,1,1);

	this->setLayout(glayout);
}

void xSettingNodeGroupDlg::InitJTNum(int num)
{
	//清空
	int count = m_JTNumComBox->count();
	for(int i = count-1;i>=0;i--)
	{
		m_JTNumComBox->removeItem(i);
	}

	for (int j=0;j<num;j++)
	{
		QString str = QString::number(j+1);
		m_JTNumComBox->addItem(str);
	}

}

void xSettingNodeGroupDlg::InitAllChildrenCombox(std::vector<QString> vChildrenName)
{
	//清空
	size_t count = m_allchildrenNameCombox->count();
	for(size_t i=count - 1;i>0;i--)
	{
		m_allchildrenNameCombox->removeItem(i);
	}
	
	//
	for (size_t j=0;j<vChildrenName.size();j++)
	{
		m_allchildrenNameCombox->addItem(vChildrenName[j]);
	}

	m_allchildrenNameCombox->setCurrentIndex(0);
	m_allGroupNameCombox->setCurrentIndex(0);
	SlotResetChildrenText();
	SlotResetSideChildrenText();

}

void xSettingNodeGroupDlg::DivideSide()
{

	int iJTNum = m_JTNumComBox->currentIndex()+1;
	QString strSide = m_sideCombox->currentText();
	QString strChild = m_allchildrenNameCombox->currentText();

	if(m_allchildrenNameCombox->currentIndex()!=0)
	{	
		if (m_JTNumMapToSideToModelNames.empty())
		{
			std::map<QString,std::vector<QString>> t;
			std::vector<QString> vt;
			vt.push_back(strChild);
			t[strSide] = vt;


			m_JTNumMapToSideToModelNames[iJTNum] = t;
			SlotResetSideChildrenText();

		}
		else
		{
			std::map<QString,std::vector<QString>>::iterator iter2;//边对应模型
			std::vector<QString>::iterator itt2;//模型
			std::map<QString,std::vector<QString>> tmp;
			std::map<int,std::map<QString,std::vector<QString>>>::iterator it2 = m_JTNumMapToSideToModelNames.begin();
			for(;it2!=m_JTNumMapToSideToModelNames.end();it2++)
			{
				tmp.erase(tmp.begin(),tmp.end());
				if(it2->first == iJTNum)
				{
					iter2 = it2->second.begin();
					for (;iter2!=it2->second.end();iter2++)//添加边成员
					{
						if (strSide == iter2->first)
						{
							itt2 = iter2->second.begin();
							for(;itt2!=iter2->second.end();itt2++)
							{
								if (*itt2 == strChild)
								{
									break;//找到子成员
								}
							}
							if (itt2 == iter2->second.end())//没找到,新添加子成员
							{
								iter2->second.push_back(strChild);
							}

							break;//找到对应边
						}	

					}
					if(iter2 == it2->second.end())//没找到，新添加边
					{		
						std::vector<QString> vt;
						vt.push_back(strChild);
						(it2->second)[strSide] = vt;
					}
					else
					{
						(it2->second)[strSide] = iter2->second;
					}
					break;
				}		
			}
			if (it2 == m_JTNumMapToSideToModelNames.end())//没找到，新添加机头
			{
				std::vector<QString> vt;
				vt.push_back(strChild);
				tmp[strSide] = vt;
				m_JTNumMapToSideToModelNames[iJTNum] = tmp;

			}
			else if (it2 != m_JTNumMapToSideToModelNames.end())
			{
				tmp = it2->second;
				m_JTNumMapToSideToModelNames[iJTNum] = tmp;

			}

			SlotResetSideChildrenText();
		}
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("请正确选择子对象!");
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

std::map<QString,int> xSettingNodeGroupDlg::GetGroupToAxisMap()
{
	return m_GroupToAxisMap;
}

void xSettingNodeGroupDlg::DelGroupToAxis(QString str)
{
	std::map<QString,int>::iterator iter ;
	iter = m_GroupToAxisMap.find(str);
	if (iter != m_GroupToAxisMap.end())
		m_GroupToAxisMap.erase (iter);
}

//void xSettingNodeGroupDlg::SetGroupToChildrenMap(std::map<QString,std::vector<QString>> GroupToChildrenMap)
//{
//	m_GroupToChildrenMap = GroupToChildrenMap;
//	UpdataDlg();
//}

void xSettingNodeGroupDlg::SetJTSideModelsMap(std::map<int,std::map<QString,std::vector<QString>>> map)
{
	m_JTNumMapToSideToModelNames = map;
	//UpdataDlg();
}

void xSettingNodeGroupDlg::SetGroupToChildrenMap(std::map<QString,GroupChildren> GroupToGroupChildrenMap)
{
	m_GroupToGroupChildrenMap = GroupToGroupChildrenMap;
	UpdataDlg();
}

void xSettingNodeGroupDlg::UpdataDlg()
{
	//更新组下拉框
	int count = m_allGroupNameCombox->count();
	for(int i=count - 1;i > 0;i--)
	{
		m_allGroupNameCombox->removeItem(i);
	}

	std::map<QString,GroupChildren>::iterator it = m_GroupToGroupChildrenMap.begin();
	for(;it!=m_GroupToGroupChildrenMap.end();it++)
	{
		m_allGroupNameCombox->addItem(it->first);
	}
}

void xSettingNodeGroupDlg::setGroupNodeDlgLanguage(std::vector<QString> vcurWords)
{
	 m_isGroupRBtn->setText(vcurWords[33]);
	 m_isSideRBtn->setText(vcurWords[34]);


	 m_allGroupNameCombox->setItemText(0,vcurWords[84]);
	 m_allchildrenNameCombox->setItemText(0,vcurWords[84]);

	 m_GroupNameLabel->setText(vcurWords[35]);
	 m_addGroupBtn->setText(vcurWords[36]);
	 m_delGroupBtn->setText(vcurWords[37]);
	 m_allGroupNameLabel->setText(vcurWords[38]);
	 m_allchildrenNameLabel->setText(vcurWords[39]);
	 m_addChildrenBtn->setText(vcurWords[40]);
	 m_delChildrenBtn->setText(vcurWords[41]);
	 m_PosLabel->setText(vcurWords[42]);
	 m_PosBtn->setText(vcurWords[43]);
	 m_okBtn->setText(vcurWords[44]);

	 m_JTNumLabel->setText(vcurWords[45]);
	 m_sideLabel->setText(vcurWords[46]);
	 m_allchildrenNameLabel->setText(vcurWords[47]);
	 m_addChildrenBtn->setText(vcurWords[48]);
	 m_delChildrenBtn->setText(vcurWords[49]);
	 m_okBtn->setText(vcurWords[50]);


	 m_vCurLanguageWords = vcurWords;
}

void xSettingNodeGroupDlg::SetIsUseSoftKeyboard(bool var)
{
	m_XPosLEdit->setUseKeyboard(var);
	m_YPosLEdit->setUseKeyboard(var);
	m_ZPosLEdit->setUseKeyboard(var);
	m_GroupNameLEdit->setUseKeyboard(var);
}

void xSettingNodeGroupDlg::SlotChangeChildrenCombox()
{
	std::vector<QString> vmodelName;
	QString strName = m_allchildrenNameCombox->currentText();
	vmodelName.push_back(strName);
	m_MainWindow->ChangeMutiModelColor(vmodelName);
	m_pChildItemsTable->clearSelection();
}

void xSettingNodeGroupDlg::SlotSetSideUI()
{
	m_GroupNameLabel->hide();
	m_GroupNameLEdit->hide();
	m_addGroupBtn->hide();
	m_delGroupBtn->hide();
	m_allGroupNameLabel->hide();
	m_allGroupNameCombox->hide();
	m_PosLabel->hide();
	m_XPosLEdit->hide();
	m_YPosLEdit->hide();
	m_ZPosLEdit->hide();
	m_PosBtn->hide();

	m_sideLabel->show();
	m_sideCombox->show();
	m_JTNumLabel->show();
	m_JTNumComBox->show();

	glayout->addLayout(hSideLayout,2,0,1,1);
	glayout->addLayout(hJTNumLayout,1,0,1,1);

	SlotResetSideChildrenText();
}

void xSettingNodeGroupDlg::SlotSetGroupUI()
{
	m_sideLabel->hide();
	m_sideCombox->hide();
	m_JTNumLabel->hide();
	m_JTNumComBox->hide();

	m_GroupNameLabel->show();
	m_GroupNameLEdit->show();
	m_addGroupBtn->show();
	m_delGroupBtn->show();
	m_allGroupNameLabel->show();
	m_allGroupNameCombox->show();
	m_PosLabel->show();
	m_XPosLEdit->show();
	m_YPosLEdit->show();
	m_ZPosLEdit->show();
	m_PosBtn->show();

	glayout->addLayout(hJTNumLayout,1,0,1,1);
	glayout->addLayout(hgroupLayout,2,0,1,1);


	SlotResetChildrenText();
	//SlotResetChildrenText();

}

void xSettingNodeGroupDlg::SlotPosBtn()
{
	if (m_XPosLEdit->text().isEmpty() == false && m_YPosLEdit->text().isEmpty() == false && m_ZPosLEdit->text().isEmpty() == false && m_allGroupNameCombox->currentIndex()!=0)
	{
		std::map<QString,GroupChildren>::iterator it;
		it = m_GroupToGroupChildrenMap.find(m_allGroupNameCombox->currentText());
		if(it!=m_GroupToGroupChildrenMap.end())
		{
			//osg::Vec3 tmpGPos(m_XPosLEdit->text().toFloat(),m_YPosLEdit->text().toFloat(),m_ZPosLEdit->text().toFloat());
			it->second.fx = m_XPosLEdit->text().toFloat();
			it->second.fy = m_YPosLEdit->text().toFloat();
			it->second.fz = m_ZPosLEdit->text().toFloat();


		}
		else
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = QString::fromLocal8Bit("组名不存在!");
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
			strWarning = QString::fromLocal8Bit("坐标设置不全!");
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

void xSettingNodeGroupDlg::SlotOk()
{
	if (m_isGroupRBtn->isChecked() == true)
	{
		m_isRebuildLM = m_RebuildLMCheckBox->isChecked();
		m_GroupPos.x() = m_XPosLEdit->text().toFloat(); 
		m_GroupPos.y() = m_YPosLEdit->text().toFloat(); 
		m_GroupPos.z() = m_ZPosLEdit->text().toFloat(); 

		emit SigSettingGroupFinished();
		this->close();
	}
	else if (m_isSideRBtn->isChecked() == true)
	{
		emit SigSettingJTSideFinished();
		this->close();
	}
}

void xSettingNodeGroupDlg::SlotCancel()
{

}

void xSettingNodeGroupDlg::SlotAddGroup()
{
	QString str;
	str = m_GroupNameLEdit->text();

	std::vector<QString> vtmp;
	GroupChildren tmpGC;
	if(str.isEmpty() == false)
	{
		std::map<QString,GroupChildren>::iterator it = m_GroupToGroupChildrenMap.begin();
		for(;it!=m_GroupToGroupChildrenMap.end();it++)
		{
			if(it->first == str)
			{
				break;
			}
		}

		if(it != m_GroupToGroupChildrenMap.end())//已存在
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("当前组已存在!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[85];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
		else
		{
				//更新数据
				m_GroupToGroupChildrenMap[str] = tmpGC;

				//更新下拉款选项
				m_allGroupNameCombox->addItem(str);
				SlotResetChildrenText();
		}
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("新建组名不能为空!");
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

void xSettingNodeGroupDlg::SlotDelGroup()
{
	QString str;
	str = m_GroupNameLEdit->text();
	std::map<QString,GroupChildren>::iterator it = m_GroupToGroupChildrenMap.begin();

	for(;it!=m_GroupToGroupChildrenMap.end();it++)
	{
		if(it->first == str)
		{
			//m_GroupToChildrenMap.erase(it);
			break;
		}
	}

	if((it)==m_GroupToGroupChildrenMap.end())//不存在
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("当前组不存在!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[85];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}
	else
	{
		m_GroupToGroupChildrenMap.erase(it);
		//DelGroupToAxis(str);//删除组轴map
		int index = m_allGroupNameCombox->findText(str);
		m_allGroupNameCombox->removeItem(index);
		m_XPosLEdit->clear();
		m_YPosLEdit->clear();
		m_ZPosLEdit->clear();

		m_allGroupNameCombox->setCurrentIndex(0);

		SlotResetChildrenText();
	}
}

void xSettingNodeGroupDlg::SlotDelChildToGroup()
{
	if (m_isGroupRBtn->isChecked() == true)
	{
		QString strGroup = m_allGroupNameCombox->currentText();
		QString strChild = m_allchildrenNameCombox->currentText();

		if(m_allGroupNameCombox->currentIndex()!=0)
		{
			if(m_allchildrenNameCombox->currentIndex()!=0)
			{	
				std::map<QString,GroupChildren>::iterator it = m_GroupToGroupChildrenMap.begin();
				for(;it!=m_GroupToGroupChildrenMap.end();it++)
				{
					if(it->first == strGroup)
					{
						std::vector<QString>::iterator iter = it->second.vChildrenNames.begin();
						for(;iter!=it->second.vChildrenNames.end();iter++)
						{
							if(*iter == strChild)//是否存在
							{
								break;
							}
						}
						//删除
						if(iter != it->second.vChildrenNames.end())
						{
							it->second.vChildrenNames.erase(iter);
							SlotResetChildrenText();

						}
						else
						{
							QString strWarning;
							if (m_vCurLanguageWords.empty())
							{
								strWarning = strWarning.fromLocal8Bit("当前组不存在!");
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
			}
			else
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("请正确选择子对象!");
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
				strWarning = strWarning.fromLocal8Bit("请正确选择组对象!");
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
	else if (m_isSideRBtn->isChecked() == true)
	{
		DeleteSideMember();
	}
}

void xSettingNodeGroupDlg::DeleteSideMember()
{
	int iJTNum = m_JTNumComBox->currentIndex()+1;
	QString strSide = m_sideCombox->currentText();
	//QString strChild = m_pChildItemsTable->currentItem()->text();
	QString strChild = m_allchildrenNameCombox->currentText();

	if(m_allchildrenNameCombox->currentIndex()!=0)
	{	
		std::map<int,std::map<QString,std::vector<QString>>>::iterator iter = m_JTNumMapToSideToModelNames.find(iJTNum);
		if (iter != m_JTNumMapToSideToModelNames.end())
		{
			std::map<QString,std::vector<QString>>::iterator iter1 = iter->second.find(strSide);
			if (iter1 != iter->second.end())
			{
				std::vector<QString> vTemp = iter1->second;
				iter1->second.clear();
				for (size_t i = 0; i < vTemp.size();++i)
				{
					if (vTemp[i]!=strChild)
					{
						iter1->second.push_back(vTemp[i]);
					}
				}
				
				SlotResetSideChildrenText();
			}
		}
	}
}

void xSettingNodeGroupDlg::SlotAddChildToGroup()
{
	if (m_isGroupRBtn->isChecked() == true)
	{

		QString strGroup = m_allGroupNameCombox->currentText();
		QString strChild = m_allchildrenNameCombox->currentText();

		if(m_allGroupNameCombox->currentIndex()!=0)
		{
			if(m_allchildrenNameCombox->currentIndex()!=0)
			{	
				std::map<QString,GroupChildren>::iterator it = m_GroupToGroupChildrenMap.begin();
				for(;it!=m_GroupToGroupChildrenMap.end();it++)
				{
					if(it->first == strGroup)
					{
						std::vector<QString>::iterator iter = it->second.vChildrenNames.begin();
						for(;iter!=it->second.vChildrenNames.end();iter++)
						{
							if(*iter == strChild)//是否存在
							{
								break;
							}
						}

						//添加
						if(iter == it->second.vChildrenNames.end())
						{
							it->second.vChildrenNames.push_back(strChild);
							SlotResetChildrenText();

						}
						else
						{
							QString strWarning;
							if (m_vCurLanguageWords.empty())
							{
								strWarning = strWarning.fromLocal8Bit("当前子对象已存在!");

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
			}
			else
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("请正确选择子对象!");


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
				strWarning = strWarning.fromLocal8Bit("请正确选择组对象!");
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
	else if (m_isSideRBtn->isChecked() == true)
	{
		DivideSide();
	}

}

void xSettingNodeGroupDlg::SlotResetChildrenText()
{

	int count = m_pChildItemsTable->rowCount();
	for(int j =count-1;j>=0;j--)
	{
		m_pChildItemsTable->removeRow(j);
	}

	if(m_allGroupNameCombox->currentIndex()== 0)
	{
		m_pChildItemsTable->clear();
		m_XPosLEdit->clear();
		m_YPosLEdit->clear();
		m_ZPosLEdit->clear();

	}
	else if(m_allGroupNameCombox->currentIndex()!=0)
	{
		//判断子成员
		std::map<QString,GroupChildren>::iterator iter = m_GroupToGroupChildrenMap.begin();
		for (;iter!=m_GroupToGroupChildrenMap.end();++iter)
		{

			if(iter->first == m_allGroupNameCombox->currentText())
			{
				//重新生成表格
				m_pChildItemsTable->clear();
				
				for(size_t i=0;i<iter->second.vChildrenNames.size();i++)
				{
					m_pChildItemsTable->setRowHeight(i,22);

					QTableWidgetItem *item = new QTableWidgetItem;
					QString txt = QString("%1").arg(iter->second.vChildrenNames[i]); 
					item->setText(txt);
					int index = m_pChildItemsTable->rowCount();
					m_pChildItemsTable->insertRow(index);
					m_pChildItemsTable->setItem(index,0,item);
				}

				//坐标
				m_XPosLEdit->clear();
				m_YPosLEdit->clear();
				m_ZPosLEdit->clear();
				m_XPosLEdit->setText(QString::number(iter->second.fx));
				m_YPosLEdit->setText(QString::number(iter->second.fy));
				m_ZPosLEdit->setText(QString::number(iter->second.fz));

			}
		}
	}
}

void xSettingNodeGroupDlg::SlotResetSideChildrenText()
{
	//清空
	int count = m_pChildItemsTable->rowCount();
	for(int j =count-1;j>=0;j--)
	{
		m_pChildItemsTable->removeRow(j);
	}

	int iJTNum = m_JTNumComBox->currentIndex()+1;
	QString strSide = m_sideCombox->currentText();
	QString strChild = m_allchildrenNameCombox->currentText();

	//判断子成员
	std::map<int,std::map<QString,std::vector<QString>>>::iterator iter = m_JTNumMapToSideToModelNames.begin();

	//std::map<QString,GroupChildren>::iterator iter = m_GroupToGroupChildrenMap.begin();
	for (;iter!=m_JTNumMapToSideToModelNames.end();++iter)
	{

		if(iter->first == iJTNum)
		{
			std::map<QString,std::vector<QString>>::iterator it = iter->second.begin();
			for (;it!=iter->second.end();it++)
			{
				if (it->first == strSide)
				{
					//重新生成表格
					m_pChildItemsTable->clear();
					for(size_t i=0;i<it->second.size();i++)
					{
						m_pChildItemsTable->setRowHeight(i,22);

						QTableWidgetItem *item = new QTableWidgetItem;
						QString txt = QString("%1").arg(it->second[i]); 
						item->setText(txt);
						int index = m_pChildItemsTable->rowCount();
						m_pChildItemsTable->insertRow(index);
						m_pChildItemsTable->setItem(index,0,item);
					}
				}
			}
		}

	}
}

void xSettingNodeGroupDlg::SlotSelectTableItems()
{
	std::vector<QString> vmodelName;
	vmodelName.clear();
	QList<QTableWidgetItem*> items = m_pChildItemsTable->selectedItems();
	int count = items.count();
	for(int i = 0; i < count; i++)
	{
		int row = m_pChildItemsTable->row(items.at(i));
		QTableWidgetItem *item = items.at(i);
		QString text = item->text(); //获取内容
		vmodelName.push_back(text);
	}

	m_MainWindow->ChangeMutiModelColor(vmodelName);

}

void xSettingNodeGroupDlg::closeEvent(QCloseEvent *event)
{
	m_MainWindow->ResumeAllModelColor();
}
