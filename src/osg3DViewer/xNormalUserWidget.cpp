#include "xNormalUserWidget.h"
#include "MainWindow.h"

xNormalUserWidget::xNormalUserWidget(MainWindow *p,bool keyboard)
: m_bIsUseKeyboard(keyboard)
{
	m_pMainWindow = p;
	InitDlg();

	numkeyboard = new NumKeyboard();
	// 读机头1的左，中，右边对应的模型
	//m_pMainWindow->ReadSideConfigFile(1,m_JT1SideToModels);
	//SetNewSideToModels(m_JT1SideToModels);
	//m_pMainWindow->ReadSideConfigFile(2,m_JT2SideToModels);
	//SetNewSideToModels(m_JT2SideToModels);

	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置

	m_table1Pictures = new xModelWidget(this);
	m_table1Pictures->SetMainWindow(m_pMainWindow);
	m_table2Pictures = new xModelWidget(this);
	m_table2Pictures->SetMainWindow(m_pMainWindow);

	connect(this,SIGNAL(SigChangeType(xMyLabel*)),this,SLOT(JT1ModelLeftTypeChanged(xMyLabel*)));

}

xNormalUserWidget::~xNormalUserWidget()
{
	//if (m_JT1_LTable!=NULL)
	//{
	//	delete m_JT1_LTable;
	//}

	//if (m_JT1_RTable!=NULL)
	//{
	//	delete m_JT1_RTable;
	//}
	//
	//if (m_JT1_CTable!=NULL)
	//{
	//	delete m_JT1_CTable;
	//}

}

void xNormalUserWidget::SetHideLMAsChosenSpecialModel()
{
	std::map<QString,std::vector<QString>>::iterator it;
	it = m_JT1SideToModels.find("LM");
	if (it!=m_JT1SideToModels.end())
	{
		std::vector<QString> vmodels = it->second;
		for (size_t i=0;i<vmodels.size();i++)
		{
			size_t j;
			for (j=0;j<m_JT1_CFileValues.size();j++)
			{
				if (vmodels[i] == m_JT1_CFileValues[j].strModelName)
				{
					break;
				}
			}

			if (j>=m_JT1_CFileValues.size())
			{
				m_pMainWindow->SetSTLNodeHideByName(vmodels[i]);
			}
		}
	}
}

void xNormalUserWidget::SetHideJMAsChosenSpecialModel()
{
	std::map<QString,std::vector<QString>>::iterator it;
	it = m_JT1SideToModels.find("JM");
	if (it!=m_JT1SideToModels.end())
	{
		std::vector<QString> vmodels = it->second;
		for (size_t i=0;i<vmodels.size();i++)
		{
			size_t j;
			for (j=0;j<m_JT1_JMFileValues.size();j++)
			{
				if (vmodels[i] == m_JT1_JMFileValues[j].strModelName)
				{
					break;
				}
			}

			if (j>=m_JT1_JMFileValues.size())
			{
				m_pMainWindow->SetSTLNodeHideByName(vmodels[i]);
			}
		}
	}
}

void xNormalUserWidget::ReSetJT1SideToModelsBySortFile()
{
	std::vector<QString> JMmodels;
	std::map<int,std::vector<QString>>::iterator iter = m_MapTovJT1JMSort.begin();
	for (;iter!=m_MapTovJT1JMSort.end();iter++)
	{
		for (size_t i=0;i<iter->second.size();i++)
		{
			JMmodels.push_back((iter->second[i]));
		}
	}

	std::vector<QString> LMmodels;
	std::map<int,std::vector<QString>>::iterator iter2 = m_MapTovJT1LMSort.begin();
	for (;iter2!=m_MapTovJT1LMSort.end();iter2++)
	{
		for (size_t i=0;i<iter2->second.size();i++)
		{
			LMmodels.push_back((iter2->second[i]));
		}
	}

	std::map<QString,std::vector<QString>>::iterator it;
	it = m_JT1SideToModels.find("JM");
	if (it!=m_JT1SideToModels.end())
	{
		it->second = JMmodels;
	}

	it = m_JT1SideToModels.find("LM");
	if (it!=m_JT1SideToModels.end())
	{
		it->second = LMmodels;
	}
}

void xNormalUserWidget::bubble_sort(std::vector<JM> &values,int n)
{
	for(int i=n-1; i>=0; i--) 
	{
		bool flag = true;
		for(int j=0; j<i; j++) 
		{
			if(values[j].iLayer>values[j+1].iLayer) 
			{
				flag = false;
				JM former = values[j];
			    values[j] = values[j+1];
				values[j+1] = former;


			}
		}
		if(flag)
			break;
	}
}

void xNormalUserWidget::LMbubble_sort(std::vector<LMR> &values,int n)
{
	for(int i=n-1; i>=0; i--) 
	{
		bool flag = true;
		for(int j=0; j<i; j++) 
		{
			if(values[j].iLayer>values[j+1].iLayer) 
			{
				flag = false;
				LMR former = values[j];
				values[j] = values[j+1];
				values[j+1] = former;
			}
		}
		if(flag)
			break;
	}
}

int xNormalUserWidget::GetPicTableRow(QString type,std::vector<std::pair<QString,QString>> PairTypeModelNameToPicName)
{
	int index = 0;
	//modelsPicMap;
	//for (int i=0;i<PairTypeModelNameToPicName.size();i++)
	//{
	//	index++;
	//	if (PairTypeModelNameToPicName[i].first == type)
	//	{
	//		return index;
	//	}
	//}

	for (size_t i = 0; i < modelsPicMap.size();++i)
	{
		for (size_t j = 0; j < modelsPicMap[i].second.size();++j)
		{
			if (modelsPicMap[i].second[j]==type)
			{
				return i+1;
			}
		}
	}
}

void xNormalUserWidget::SetNewSideToModels(std::map<QString,std::vector<QString>>& SideToModels)
{
	std::vector<QString> vJmNames;
	std::vector<QString> vLmNames;
	std::map<QString,std::vector<QString>>::iterator _itt =  SideToModels.begin();
	for (;_itt!=SideToModels.end();_itt++)
	{
		if (_itt->first == "LEFT"||_itt->first == "RIGHT")
		{
			std::vector<QString>::iterator _it = _itt->second.begin();
			for (;_it!=_itt->second.end();_it++)
			{
				vJmNames.push_back(*_it);
			}
		}
		else if (_itt->first == "CENTER")
		{
			std::vector<QString>::iterator _it = _itt->second.begin();
			for (;_it!=_itt->second.end();_it++)
			{
				vLmNames.push_back(*_it);
			}
		}
	}

	SideToModels.clear();
	QString TYPE1 = "JM";
	QString TYPE2 = "LM";

	SideToModels[TYPE1] = vJmNames;
	SideToModels[TYPE2] = vLmNames;
}

void xNormalUserWidget::DeleteAllItems()
{
	int a = m_JT1PicTable->rowCount();
	if(a == 0)
		return;
	else
	{

		m_JT1PicTable->clearContents();
		for (int i = m_JT1PicTable->rowCount(); i > 0; i--)
		{
			m_JT1PicTable->removeRow(0);
		}
	}
}

void xNormalUserWidget::ReadJT1ModelToLayerFileValues()
{
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1ModelToLayer.csv");	
	m_MapFileSetModelToLayer = m_pMainWindow->ReadModelToLayerFileValues(strFileName);
}

void xNormalUserWidget::ReadJT1JMSortFile(QString m_sDir)
{
	if (m_sDir.isEmpty())
	{
		return;
	}

	QString strFileName = m_sDir + QString::fromLocal8Bit("/JMSort.csv");	
	m_MapTovJT1JMSort = m_pMainWindow->ReadSortFile(strFileName);//JM
	strFileName = m_sDir + QString::fromLocal8Bit("/LMSort.csv");	
	m_MapTovJT1LMSort = m_pMainWindow->ReadLMSortFile(strFileName);//LM
}

void xNormalUserWidget::ReadModelPictureNameMap(QString m_sDir)
{
	if (m_sDir.isEmpty())
	{
		return;
	}

	QString strFileName = m_sDir + QString::fromLocal8Bit("/ModelPictures/TypePictureNameMap.csv");	
	m_vTypeName = m_pMainWindow->ReadTypeNameMap(strFileName);
	m_MapModelToType = m_pMainWindow->GetModelToType();
}

std::vector<std::pair<QString,QString>> xNormalUserWidget::GetCurTypeNamePic(std::vector<QString> ModelNames,std::vector<std::pair<QString,QString>> vTypeName)
{
	std::vector<std::pair<QString,QString>> curTypeName;
	for (size_t i=0;i<ModelNames.size();i++)
	{
		for (size_t j=0;j<vTypeName.size();j++)
		{
			if (ModelNames[i] == vTypeName[j].first)
			{
				curTypeName.push_back(vTypeName[j]);
			}
		}
	}

	return curTypeName;
}

void xNormalUserWidget::SetPicTableBtnFrame(xMyLabel *label)
{
	QString strname;
	m_curLabel = label;
	int irow;
	strname = m_curLabel->GetName();
	irow = m_curLabel->GetPicTableRow();
	std::map<QString,QPushButton*>::iterator iter = m_mapTypeButtons.begin();
	for (;iter != m_mapTypeButtons.end();++iter)
	{
		//if (strname == iter->first)
		//{
		//	iter->second->setStyleSheet("background:transparent;border:5px solid red;");
		//	m_JT1PicTable->verticalScrollBar()->setSliderPosition(irow-1);
		//}
		//else
		//{
			iter->second->setStyleSheet("background:transparent;border:5px solid green;");
		//}
	}

	iter = m_mapTypeButtons.begin();
	for (;iter != m_mapTypeButtons.end();++iter)
	{
		if (strname == iter->first)
		{
			iter->second->setStyleSheet("background:transparent;border:5px solid red;");
			m_JT1PicTable->verticalScrollBar()->setSliderPosition(irow-1);

			return;
		}
		//else
		//{
		//	iter->second->setStyleSheet("background:transparent;border:5px solid green;");
		//}
	}
}

void xNormalUserWidget::RefreshPicTable(std::vector<QString> ModelNames)
{
	m_vCurTypeNamePic = GetCurTypeNamePic(ModelNames,m_vTypeName);

	DeleteAllItems();
	m_mapTypeButtons.clear();
	modelsPicMap.clear();

	size_t size = m_vCurTypeNamePic.size();
	std::map<QString,int> tempTypes;
	std::vector<QString> tempPics;
	for (size_t m = 0; m < size;++m)
	{
		if (tempTypes.find(m_vCurTypeNamePic[m].second)==tempTypes.end())
		{
			tempTypes[m_vCurTypeNamePic[m].second] = 1;
			tempPics.push_back(m_vCurTypeNamePic[m].second);
		}
	}

	for (size_t i = 0; i < tempPics.size();++i)
	{
		std::vector<QString> modes;
		for (size_t n = 0; n < size;++n)
		{
			if (m_vCurTypeNamePic[n].second==tempPics[i])
			{
				modes.push_back(m_vCurTypeNamePic[n].first);
			}
		}

		modelsPicMap.push_back(make_pair(tempPics[i],modes));
	}

	m_JT1PicTable->setColumnCount(1);
	//m_JT1PicTable->setRowCount(size);
	m_JT1PicTable->setRowCount(modelsPicMap.size());
	// 四个一排
	//for (int i = 0; i < size;++i)
	for (size_t i = 0;i < modelsPicMap.size();++i)
	{
		//int m = i/4;
		//int n = i%4;
		QPushButton* button = new QPushButton(this);
		QString picture = m_strJT1Dir;
		picture.append("/ModelPictures/");
		//picture.append(m_vCurTypeNamePic[i].second);
		picture.append(modelsPicMap[i].first);
		picture.append(".png");

		QPixmap image(picture);
		button->setIcon(QIcon(image));
		button->setIconSize(QSize(100,70));
		m_JT1PicTable->setRowHeight(i,75);
		m_JT1PicTable->setColumnWidth(0,105);

		m_JT1PicTable->setCellWidget(i,0,button);
		button->setStyleSheet("background:transparent;border:5px solid green;");

		connect(button,SIGNAL(clicked()),this,SLOT(Slot_Type_Selected()));

		//m_mapTypeButtons[m_vCurTypeNamePic[i].first] = button;
		for (size_t m = 0; m < modelsPicMap[i].second.size();++m)
		{
			m_mapTypeButtons[modelsPicMap[i].second[m]] = button;
		}
	}
}

void xNormalUserWidget::ShowOrHideTable()
{

	//if (m_iJT1LTablePos<0)
	//{
	//	m_JT1_LTable->hide();
	//}
	//else
	//{
	//	m_JT1_LTable->show();
	//}

	//if (m_iJT1RTablePos<0)
	//{
	//	m_JT1_RTable->hide();
	//}
	//else
	//{
	//	m_JT1_RTable->show();
	//}


	if (m_iJT1CTablePos<0)
	{
		m_JT1_CTable->hide();
	}
	else
	{
		m_JT1_CTable->show();
	}
}

void xNormalUserWidget::InitDlg()
{
	/*m_pTabWidget = new QTabWidget;*/
	m_pTabWidget = new QTabWidget(this);
	//m_pTabWidget->setStyleSheet("QTabWidget:pane {border:1px solid #e8f3f9; clolr:white; background:transparent;}");
	//m_pTabWidget->setTabShape(QTabWidget::Rounded);//设置选项卡的形状 Rounded
	//m_pTabWidget->setGeometry(20, 10, 400, 200);
	//m_pTabWidget->setTabPosition(QTabWidget::East);
	char num[64]="";
	m_pMainWindow->GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JITOU_NUM, 3, num);
	m_iJTNum = atoi(num);

	tab1 = new QWidget(this);
	tab2 = new QWidget(this);

	//m_table = new Table;
	//m_JT1_LTable = new Table;
	//m_JT1_RTable = new Table;
	m_JT1_CTable = new Table;
	m_JT1_JMTable = new Table;
	m_JT1_JMTable->setAlternatingRowColors(false);
	m_JT1_CTable->setAlternatingRowColors(false);

	char JT1LTable[64]="";
	m_pMainWindow->GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_LTable, 3, JT1LTable);
	m_iJT1LTablePos = atoi(JT1LTable);

	char JT1CTable[64]="";
	m_pMainWindow->GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_CTable, 3, JT1CTable);
	m_iJT1CTablePos = atoi(JT1CTable);

	char JT1RTable[64]="";
	m_pMainWindow->GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_RTable, 3, JT1RTable);
	m_iJT1RTablePos = atoi(JT1RTable);

	gLayout = new QGridLayout(tab1);
	QVBoxLayout *vlayout = new QVBoxLayout(tab1);
	m_JT1PicTable = new QTableWidget;

	//gLayout->addWidget(m_JT1_LTable,m_iJT1LTablePos,0);
	gLayout->addWidget(m_JT1_CTable,0,0);
	gLayout->addWidget(m_JT1_JMTable,1,0);
	gLayout->addWidget(m_JT1PicTable,0,1,2,3);

	hLayout1 = new QHBoxLayout();
	hLayout1->addLayout(gLayout);
	tab1->setLayout(hLayout1);

	//ShowOrHideTable();

	SetJT1();

	m_pTabWidget->addTab(tab1, QIcon(""), QString("1"));
	m_pTabWidget->addTab(tab2, QIcon(""), QString("2"));

	QHBoxLayout *hwidget = new QHBoxLayout;
	hwidget->addWidget(m_pTabWidget);
	this->setLayout(hwidget);

	this->resize(880,700);
}

void xNormalUserWidget::SetJT1()
{
#if 0
	//QStringList header;
	//header<<m_vCurLanguageWords[20]
	//<<m_vCurLanguageWords[21]
	//<<m_vCurLanguageWords[22]
	//<<m_vCurLanguageWords[23]
	//<<m_vCurLanguageWords[24]
	//<<m_vCurLanguageWords[25]
	//<<m_vCurLanguageWords[26]
	//<<m_vCurLanguageWords[27];


	QStringList header;
	header<<QString::fromLocal8Bit("模程")
		  <<QString::fromLocal8Bit("UpHeight")
		  <<QString::fromLocal8Bit("DownHeight")
		  <<QString::fromLocal8Bit("Length")
		  <<QString::fromLocal8Bit("Wide")
		  <<QString::fromLocal8Bit("AoSide")
		  <<QString::fromLocal8Bit("Type")
		  <<QString::fromLocal8Bit("显示")
		  <<QString::fromLocal8Bit("Modifiable")
		  ;
	

	m_JT1_LTable->setColumnValue(9, header);
	m_JT1_LTable->setHeaderWidth(0, 40);
	//m_JT1_LTable->setHeaderWidth(1, 130);
	m_JT1_LTable->setHeaderWidth(1, 72);
	m_JT1_LTable->setHeaderWidth(2, 72);
	m_JT1_LTable->setHeaderWidth(3, 72);
	m_JT1_LTable->setHeaderWidth(4, 72);
	m_JT1_LTable->setHeaderWidth(5, 72);
	m_JT1_LTable->setHeaderWidth(6, 130);
	m_JT1_LTable->setHeaderWidth(7, 15);
	m_JT1_LTable->setHeaderWidth(8, 15);

	m_JT1_LTable->setHeaderHeight(20);
	m_JT1_LTable->setSortingEnabled(false);
	m_JT1_LTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_JT1_LTable->setFont(QFont("Microsoft YaHe", 10));

	QStringList header1;
	header1<<QString::fromLocal8Bit("模程")
		  //<<QString::fromLocal8Bit("模型名")
		  <<QString::fromLocal8Bit("UpHeight")
		  <<QString::fromLocal8Bit("DownHeight")
		  <<QString::fromLocal8Bit("Length")
		  <<QString::fromLocal8Bit("Wide")
		  <<QString::fromLocal8Bit("AoSide")
		  <<QString::fromLocal8Bit("Type")
		  <<QString::fromLocal8Bit("显示")
		  <<QString::fromLocal8Bit("Modifiable")
		  ;

	m_JT1_RTable->setColumnValue(9, header1);
	m_JT1_RTable->setHeaderWidth(0, 40);
	//m_JT1_RTable->setHeaderWidth(1, 130);
	m_JT1_RTable->setHeaderWidth(1, 72);
	m_JT1_RTable->setHeaderWidth(2, 72);
	m_JT1_RTable->setHeaderWidth(3, 72);
	m_JT1_RTable->setHeaderWidth(4, 72);
	m_JT1_RTable->setHeaderWidth(5, 72);
	m_JT1_RTable->setHeaderWidth(6, 130);
	m_JT1_RTable->setHeaderWidth(7, 15);
	m_JT1_RTable->setHeaderWidth(7, 15);
	m_JT1_RTable->setHeaderHeight(20);
	m_JT1_RTable->setSortingEnabled(false);
	m_JT1_RTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_JT1_RTable->setFont(QFont("Microsoft YaHe", 10));
#endif
	//QStringList header2;
	//header2<<m_vCurLanguageWords[20]
	//<<m_vCurLanguageWords[21]
	//<<m_vCurLanguageWords[22]
	//<<m_vCurLanguageWords[28]
	//<<m_vCurLanguageWords[29]
	//<<m_vCurLanguageWords[30]
	//<<m_vCurLanguageWords[26]
	//<<m_vCurLanguageWords[27];

	QStringList header2;
	header2<<QString::fromLocal8Bit("  ")
		  <<QString::fromLocal8Bit("模程")		  
		  <<QString::fromLocal8Bit("UpHeight")
		  <<QString::fromLocal8Bit("DownHeight")
		  <<QString::fromLocal8Bit("LM_R")
		  <<QString::fromLocal8Bit("TuLen")
		  <<QString::fromLocal8Bit("TuSide")
		  <<QString::fromLocal8Bit("Type")
		  <<QString::fromLocal8Bit("显示")
		  <<QString::fromLocal8Bit("Modifiable");

	m_JT1_CTable->setColumnValue(10, header2);
	m_JT1_CTable->setHeaderWidth(0, 40);
	m_JT1_CTable->setHeaderWidth(1, 72);
	m_JT1_CTable->setHeaderWidth(2, 72);
	m_JT1_CTable->setHeaderWidth(3, 72);
	m_JT1_CTable->setHeaderWidth(4, 72);
	m_JT1_CTable->setHeaderWidth(5, 72);
	m_JT1_CTable->setHeaderWidth(6, 72);
	m_JT1_CTable->setHeaderWidth(7, 130);
	m_JT1_CTable->setHeaderWidth(8, 15);
	m_JT1_CTable->setHeaderWidth(9, 15);

	m_JT1_CTable->setHeaderHeight(20);
	m_JT1_CTable->setSortingEnabled(false);
	m_JT1_CTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_JT1_CTable->setFont(QFont("Microsoft YaHe", 10));

	QStringList header0;
	header0<<QString::fromLocal8Bit("  ")
		<<QString::fromLocal8Bit("模程")
		<<QString::fromLocal8Bit("UpHeight")
		<<QString::fromLocal8Bit("DownHeight")
		<<QString::fromLocal8Bit("Length")
		<<QString::fromLocal8Bit("Wide")
		<<QString::fromLocal8Bit("AoSide")
		<<QString::fromLocal8Bit("Type")
		<<QString::fromLocal8Bit("显示")
		<<QString::fromLocal8Bit("Modifiable")
		;

	m_JT1_JMTable->setColumnValue(10, header0);
	m_JT1_JMTable->setHeaderWidth(0, 40);
	m_JT1_JMTable->setHeaderWidth(1, 72);
	m_JT1_JMTable->setHeaderWidth(2, 72);
	m_JT1_JMTable->setHeaderWidth(3, 72);
	m_JT1_JMTable->setHeaderWidth(4, 72);
	m_JT1_JMTable->setHeaderWidth(5, 72);
	m_JT1_JMTable->setHeaderWidth(6, 72);
	m_JT1_JMTable->setHeaderWidth(7, 130);
	m_JT1_JMTable->setHeaderWidth(8, 15);
	m_JT1_JMTable->setHeaderWidth(9, 15);
	m_JT1_JMTable->setHeaderHeight(20);
	m_JT1_JMTable->setSortingEnabled(false);
	m_JT1_JMTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_JT1_JMTable->setFont(QFont("Microsoft YaHe", 10));
}

void xNormalUserWidget::InitTable()
{
	 //读机头1的左，中，右边对应的模型
	m_pMainWindow->ReadSideConfigFile(1,m_JT1SideToModels);
	SetNewSideToModels(m_JT1SideToModels);

	m_pMainWindow->ReadSideConfigFile(2,m_JT2SideToModels);
	SetNewSideToModels(m_JT2SideToModels);

	QString strtmp;
	std::string stmp;
	char sJT1_FILES_PATH[256] = "";
	m_pMainWindow->GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_FILES_PATH, 3, sJT1_FILES_PATH);
	m_pMainWindow->GetWholePath(sJT1_FILES_PATH);
	stmp = sJT1_FILES_PATH;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	m_strJT1Dir = strtmp;

	char sJT2_FILES_PATH[256] = "";
	m_pMainWindow->GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT2_FILES_PATH, 3, sJT2_FILES_PATH);
	m_pMainWindow->GetWholePath(sJT2_FILES_PATH);
	stmp = sJT2_FILES_PATH;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	m_strJT2Dir = strtmp;

	m_AllSTLs = m_pMainWindow->GetAllSTLs();
	for (int i=1;i<=m_iJTNum;i++)
	{
		if (i==1)
		{
			//m_table1Pictures->SetFilesDir(m_strJT1Dir);
			//m_table1Pictures->ReadModelPictureNameMap();//构造类型表格
			ReadJT1JMSortFile(m_strJT1Dir);
			ReSetJT1SideToModelsBySortFile();

			ReadModelPictureNameMap(m_strJT1Dir);
			InitJT1Table();

		}
		else if (i==2)
		{
			//m_table2Pictures->SetFilesDir(m_strJT2Dir);
			//m_table2Pictures->ReadModelPictureNameMap();
			//InitJT2Table();

		}
	}
}

void xNormalUserWidget::InitTab1(std::vector<LMR> values)
{
#if 0


	vector<QString> types;
	for (int i = 0; i< values.size();++i)
	{
		types.push_back(values[i].strModelName);	
	}

	m_table->DeleteAllValueRow();
	for (int i=0;i<values.size();i++)
	{
		m_table->addRowValueToLMTable(18,QStringList()<<QString::number(i+1)
													  <<values[i].strModelName
													  <<QString::number(values[i].fLmr)
													  <<QString::number(values[i].fupLen)
													  <<QString::number(values[i].fdownLen)
													  <<QString::number(values[i].ftuLen));
													  //<<values[i].strModelName);

		QComboBox* comBox = new QComboBox(this);
		for (int j = 0; j < types.size();++j)
		{
			comBox->addItem(types[j]);
		}
		comBox->setCurrentIndex(i);
		//comBox->setStyleSheet("QComboBox{margin:3px};")
		m_table->setCellWidget(i, 6, comBox);

		connect(comBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ModelTypeChanged(int)));
		ComboBoxs.push_back(comBox);

		QCheckBox *checkBox = new QCheckBox(this);
		if(values[i].isShow == 1)
		{
			checkBox->setCheckState(Qt::Checked);

		}
		else
		{
			checkBox->setCheckState(Qt::Unchecked);

		}
		m_table->setCellWidget(i, 7, checkBox);

		connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(ModelShowChanged(int)));

	}

	//m_table->blockSignals(false);

	//connect(m_table, SIGNAL(cellChanged(int,int)), this, SLOT(changeTest(int, int)));

	m_table->setEditTriggers(QAbstractItemView::DoubleClicked);
#endif
}

void xNormalUserWidget::InitJT1Table()
{
	//获取左中右文件数据
	//ReadJT1LeftFileValues();
	//ReadJT1RightFileValues();
	ReadJT1ModelToLayerFileValues();
	ReadJT1JMFileValues();
	ReadJT1CenterFileValues();
}

void xNormalUserWidget::InitJT2Table()
{
}

void xNormalUserWidget::ReFreshJT1LeftTable()
{
}

void xNormalUserWidget::ReFreshJT1RightTable()
{
}

void xNormalUserWidget::ReFreshJT1CenterTable()
{
}

void xNormalUserWidget::GetAimJMValues(std::vector<JM> values,QString aimName,JM& aim)
{
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LEFT.csv");	
	m_JT1_LFileValues = m_pMainWindow->ReadJT1LeftFileValues(strFileName);

	strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_RIGHT.csv");	
	m_JT1_RFileValues = m_pMainWindow->ReadJT1RightFileValues(strFileName);

	std::vector<JM> tmpValues;
	tmpValues = m_JT1_LFileValues;
	for (size_t j=0;j<m_JT1_RFileValues.size();j++)
	{
		tmpValues.push_back(m_JT1_RFileValues[j]);
	}

	for (size_t i=0;i<tmpValues.size();i++)
	{
		if (tmpValues[i].strModelName == aimName)
		{
			aim.fLength = tmpValues[i].fLength;
			aim.fWide = tmpValues[i].fWide;
			aim.iSide = tmpValues[i].iSide;
		}
	}
}

void xNormalUserWidget::GetAimLMValues(std::vector<LMR> values,QString aimName,LMR& aim)
{
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_CENTER.csv");
	values = m_pMainWindow->ReadJT1CenterFileValues(strFileName);

	for (size_t i=0;i<values.size();i++)
	{
		if (values[i].strModelName == aimName)
		{
			aim.fLmr = values[i].fLmr;
			aim.ftuLen = values[i].ftuLen;
			aim.iSide = values[i].iSide;
		}
	}
}

void xNormalUserWidget::GetJMTableValues(std::vector<JM>& values,Table *table)
{
	values.clear();
	for(int i=0;i<table->rowCount();i++)
	{
		JM tmp;
		//表格可以修改的内容
		tmp.fLength = table->item(i,4)->text().toFloat();
		tmp.fWide = table->item(i,5)->text().toFloat();

		values.push_back(tmp);
	}

	for(size_t i=0;i<values.size();i++)
	{
		size_t j;
		for(j=0;j<m_JT1_LFileValues.size();j++)
		{
			if (values[i].strModelName == m_JT1_LFileValues[j].strModelName)
			{
				break;
			}
		}

		if (j<m_JT1_LFileValues.size())
		{

		}
	}
}

void xNormalUserWidget::SetIsUseSoftKeyboard(bool var)
{
	m_bIsUseKeyboard = var;
}

void xNormalUserWidget::SetChooseLabel(std::vector<xMyLabel*> vLabels,xMyLabel * label,std::vector<xMyLabel*> vOther)
{
	for (size_t i=0;i<vLabels.size();i++)
	{
		if (vLabels[i] == label)
		{
			label->setStyleSheet("background-color:yellow"); 
		}
		else
		{
			vLabels[i]->setStyleSheet("background-color:white");
		}
	}

	for (size_t i=0;i<vOther.size();i++)
	{
		vOther[i]->setStyleSheet("background-color:white");
	}
}

void xNormalUserWidget::JT1ModelLeftTypeChanged(xMyLabel * label)
{
	xMyLabel *comBox_ = dynamic_cast<xMyLabel*>(label);
	if(NULL == comBox_)
	{
		return;
	}

	// 刷新后台文件的值
	QString strCurRowToModelName;
	QString strName = label->GetName();//切换类型对应的模型名
	QString strSide = label->GetSide();
	int JTNth = label->GetJTNth();
	if (JTNth == 1)
	{
		if (strSide == "JM")
		{
			int x = comBox_->frameGeometry().x();
			int y = comBox_->frameGeometry().y();
			QModelIndex index = m_JT1_JMTable->indexAt(QPoint(x, y));
			int row = index.row();
			int column = index.column();

			int iSpecialType = m_pMainWindow->GetTypeModelIsReplaceSpecialByName(strName);
			if (iSpecialType == 1)
			{
				// 直接用特殊模型替代掉现有模型，特殊类型模型只有一个实例
				m_pMainWindow->ReplaceBySpecialModel(m_JT1_JMFileValues[row].strModelName,strName);
				m_JT1_JMFileValues[row].strType=strName;

				QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");
				WriteToSideFile(strFileName,m_JT1_JMFileValues);
			}
			else if (iSpecialType == 2)
			{
				// 将特殊类型移动到现有模型位置，特殊模型形状不变，特殊类型可以有多个实例
				m_pMainWindow->ReplaceByNewInstance(m_JT1_JMFileValues[row].strModelName,strName);
				m_JT1_JMFileValues[row].strType=strName;

				QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");
				WriteToSideFile(strFileName,m_JT1_JMFileValues);
			}
			else
			{
				QString name = m_JT1_JMFileValues[row].strModelName; //当前行的模型
				for (size_t i = 0; i < m_JT1_JMFileValues.size();++i)
				{
					if (m_JT1_JMFileValues[i].strModelName==name)
					{
						JM aim;
						std::map<QString,QString> mapNameToType = m_MapModelToType;
						//strName = label->GetName();
						strCurRowToModelName = label->GetCurRowToModelName();

						GetAimJMValues(m_JT1_JMFileValues,strName,aim);
						m_JT1_JMFileValues[i].strType = strName;
						//m_JT1_JMFileValues[i].fLength = aim.fLength;
						//m_JT1_JMFileValues[i].fWide = aim.fWide;
						m_JT1_JMFileValues[i].iSide = aim.iSide;
						m_JT1_JMTable->item(row,4)->setText(QString::number(m_JT1_JMFileValues[i].fLength));
						m_JT1_JMTable->item(row,5)->setText(QString::number(m_JT1_JMFileValues[i].fWide));
						m_JT1_JMTable->item(row,6)->setText(QString::number(m_JT1_JMFileValues[i].iSide));
					}
				}

				// 刷新文件
				QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");
				WriteToSideFile(strFileName,m_JT1_JMFileValues);
				////更新视图模型
				//m_pMainWindow->RebuildJT1JMModel(m_JT1_JMFileValues);
			}

			//更新视图模型
			m_pMainWindow->RebuildJT1JMModel(m_JT1_JMFileValues);
		}
		else if (strSide == "LM")
		{
			int x = comBox_->frameGeometry().x();
			int y = comBox_->frameGeometry().y();
			QModelIndex index = m_JT1_CTable->indexAt(QPoint(x, y));
			int row = index.row();
			int column = index.column();

			int iSpecialType = m_pMainWindow->GetTypeModelIsReplaceSpecialByName(strName);
			if (iSpecialType == 1)
			{
				// 不能改变位置和形状的轮模
				m_pMainWindow->ReplaceBySpecialModel(m_JT1_CFileValues[row].strModelName,strName);
				m_JT1_CFileValues[row].strType = strName;

				QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");
				WriteToCenterFile(strFileName,m_JT1_CFileValues,1);
			}
			else if (iSpecialType==2)
			{
				// 能改变位置不能改变形状的轮模
				m_pMainWindow->ReplaceByNewInstance(m_JT1_CFileValues[row].strModelName,strName);

				// 保存修改的类型
				m_JT1_CFileValues[row].strType = strName;

				QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");
				WriteToCenterFile(strFileName,m_JT1_CFileValues,1);
			}
			else
			{
				QString name = m_JT1_CFileValues[row].strModelName; //获取内容

				for (size_t i = 0; i < m_JT1_CFileValues.size();++i)
				{
					if (m_JT1_CFileValues[i].strModelName==name)
					{

						LMR aim;
						strName = label->GetName();
						strCurRowToModelName = label->GetCurRowToModelName();

						GetAimLMValues(m_JT1_CFileValues,strName,aim);
						m_JT1_CFileValues[i].strType = strName;

						// 按照初始值设置轮模形状
						if (fabs(m_JT1_CFileValues[i].fLmr)<0.00001)
						{
							m_JT1_CFileValues[i].fLmr = aim.fLmr;
						}

						if (fabs(m_JT1_CFileValues[i].ftuLen)<0.00001 || fabs(aim.ftuLen)<0.00001)
						{
							m_JT1_CFileValues[i].ftuLen = aim.ftuLen;
						}

						m_JT1_CFileValues[i].iSide = aim.iSide;

						m_JT1_CTable->item(row,4)->setText(QString::number(m_JT1_CFileValues[i].fLmr));
						m_JT1_CTable->item(row,5)->setText(QString::number(m_JT1_CFileValues[i].ftuLen));
						m_JT1_CTable->item(row,6)->setText(QString::number(m_JT1_CFileValues[i].iSide));

					}
				}
				// 刷新文件
				QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");
				WriteToCenterFile(strFileName,m_JT1_CFileValues,1);

				//更新视图模型
				m_pMainWindow->RebuildJT1LMModel(m_JT1_CFileValues);
			}
			
		}
	}
	else if (JTNth == 2)
	{
	}

	if(m_pMainWindow != NULL)
	{
		m_pMainWindow->On_modelColor_change(strCurRowToModelName);
	}
}

void xNormalUserWidget::ReadJT1LeftFileValues()
{
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LEFT.csv");	
	m_JT1_LFileValues = m_pMainWindow->ReadJT1LeftFileValues(strFileName);
	std::vector<QString> types;
	for (size_t i=0;i<m_JT1_LFileValues.size();i++)
	{
		types.push_back(m_JT1_LFileValues[i].strModelName);
	}

#if 0

	QStringList header;
	header<<m_vCurLanguageWords[20]
		<<m_vCurLanguageWords[21]
		<<m_vCurLanguageWords[22]
		<<m_vCurLanguageWords[23]
		<<m_vCurLanguageWords[24]
		<<m_vCurLanguageWords[25]
		<<m_vCurLanguageWords[26]
		<<m_vCurLanguageWords[27];

	m_JT1_LTable->setColumnValue(8, header);
	m_JT1_LTable->setHeaderWidth(0, 40);
	//m_JT1_LTable->setHeaderWidth(1, 130);
	m_JT1_LTable->setHeaderWidth(1, 72);
	m_JT1_LTable->setHeaderWidth(2, 72);
	m_JT1_LTable->setHeaderWidth(3, 72);
	m_JT1_LTable->setHeaderWidth(4, 72);
	m_JT1_LTable->setHeaderWidth(5, 72);
	m_JT1_LTable->setHeaderWidth(6, 130);
	m_JT1_LTable->setHeaderWidth(7, 15);

	m_JT1_LTable->setHeaderHeight(20);
	m_JT1_LTable->setSortingEnabled(false);
	m_JT1_LTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_JT1_LTable->setFont(QFont("Microsoft YaHe", 10));


	//写入数据
	m_JT1_LTable->DeleteAllValueRow();
	m_JT1LeftLabels.clear();
	for (int i=0;i<m_JT1_LFileValues.size();i++)
	{
		m_JT1_LTable->addRowValueToLMTable(18,QStringList()<<QString::number(m_JT1_LFileValues[i].iLayer)
															//<<m_JT1_LFileValues[i].strModelName
															<<QString::number(m_JT1_LFileValues[i].fUpHeight)
															<<QString::number(m_JT1_LFileValues[i].fDownHeight)
															<<QString::number(m_JT1_LFileValues[i].fLength)
															<<QString::number(m_JT1_LFileValues[i].fWide)
															<<QString::number(m_JT1_LFileValues[i].iSide)
															<<QString::number(m_JT1_LFileValues[i].iModifiable)
															);




		//QComboBox* comBox = new QComboBox(this);
		//int currentIndex = -1;
		int needindex;
		for (int j = 0; j < types.size();++j)
		{		
			//comBox->addItem(types[j]);
			if (m_JT1_LFileValues[i].strType == types[j])
			{
				needindex = j;
			}
		}
		//comBox->setCurrentIndex(needindex);
		xMyLabel* label = new xMyLabel(this);

		std::map<QString,QString> mapNameToType = m_table1Pictures->GetModelToType();
		QString strType = mapNameToType.find(types[needindex])->second;
		label->setText(strType);
		//label->text()
		label->SetType(strType);
		label->SetName(types[needindex]);
		label->SetJTNth(1);
		label->SetSide(QString("LEFT"));
		label->SetModifiable(m_JT1_LFileValues[i].iModifiable);
		
		m_JT1LeftLabels.push_back(label);
		m_JT1_LTable->setCellWidget(i, 6, label);

		//connect(comBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ModelLeftTypeChanged(int)));

		QCheckBox *checkBox = new QCheckBox(this);
		checkBox->setStyle(QStyleFactory::create("Cleanlooks"));
		if(m_JT1_LFileValues[i].iShow == 1)
		{
			checkBox->setCheckState(Qt::Checked);
		}
		else
		{
			checkBox->setCheckState(Qt::Unchecked);
		}
		m_JT1_LTable->setCellWidget(i, 7, checkBox);

		connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(ModelLeftShowChanged(int)));

	}

	connect(m_JT1_LTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(LeftItemDoubleClicked(int,int))/*,Qt::QueuedConnection*/);
	connect(m_JT1_LTable,SIGNAL(cellChanged(int,int)),this,SLOT(LeftTableChanged(int,int))/*,Qt::QueuedConnection*/);
	connect(m_JT1_LTable,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectJT1LTableItems())/*,Qt::QueuedConnection*/);

	//m_table->blockSignals(false);

	//connect(m_table, SIGNAL(cellChanged(int,int)), this, SLOT(changeTest(int, int)));

	//m_table->setEditTriggers(QAbstractItemView::DoubleClicked);
	m_JT1_LTable->setEditTriggers(QAbstractItemView::DoubleClicked);
	m_JT1_LTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//列自适应宽度
	m_JT1_LTable->setColumnHidden(8,true);

	#endif
}

void xNormalUserWidget::ReadJT1RightFileValues()
{
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_RIGHT.csv");
	m_JT1_RFileValues = m_pMainWindow->ReadJT1RightFileValues(strFileName);
	std::vector<QString> types;
	for (size_t i=0;i<m_JT1_RFileValues.size();i++)
	{
		types.push_back(m_JT1_RFileValues[i].strModelName);
	}

	//表头
	//QStringList header;
	//header <<QString::fromLocal8Bit("模程")
	//	 // <<QString::fromLocal8Bit("模型名")
	//	  <<QString::fromLocal8Bit("UpHeight")
	//	  <<QString::fromLocal8Bit("DownHeight")
	//	  <<QString::fromLocal8Bit("Length")
	//	  <<QString::fromLocal8Bit("Wide")
	//	  <<QString::fromLocal8Bit("AoSide")
	//	  <<QString::fromLocal8Bit("Type")
	//	  <<QString::fromLocal8Bit("显示");
#if 0



	QStringList header;
	header<<m_vCurLanguageWords[20]
		<<m_vCurLanguageWords[21]
		<<m_vCurLanguageWords[22]
		<<m_vCurLanguageWords[23]
		<<m_vCurLanguageWords[24]
		<<m_vCurLanguageWords[25]
		<<m_vCurLanguageWords[26]
		<<m_vCurLanguageWords[27];



	m_JT1_RTable->setColumnValue(8, header);
	m_JT1_RTable->setHeaderWidth(0, 40);
	//m_JT1_RTable->setHeaderWidth(1, 130);
	m_JT1_RTable->setHeaderWidth(1, 72);
	m_JT1_RTable->setHeaderWidth(2, 72);
	m_JT1_RTable->setHeaderWidth(3, 72);
	m_JT1_RTable->setHeaderWidth(4, 72);
	m_JT1_RTable->setHeaderWidth(5, 72);
	m_JT1_RTable->setHeaderWidth(6, 130);
	m_JT1_RTable->setHeaderWidth(7, 15);

	m_JT1_RTable->setHeaderHeight(20);
	m_JT1_RTable->setSortingEnabled(false);
	m_JT1_RTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_JT1_RTable->setFont(QFont("Microsoft YaHe", 10));


	//写入数据
	m_JT1RightLabels.clear();
	m_JT1_RTable->DeleteAllValueRow();
	for (int i=0;i<m_JT1_RFileValues.size();i++)
	{
		m_JT1_RTable->addRowValueToLMTable(18,QStringList()<<QString::number(m_JT1_RFileValues[i].iLayer)
			//<<m_JT1_RFileValues[i].strModelName
			<<QString::number(m_JT1_RFileValues[i].fUpHeight)
			<<QString::number(m_JT1_RFileValues[i].fDownHeight)
			<<QString::number(m_JT1_RFileValues[i].fLength)
			<<QString::number(m_JT1_RFileValues[i].fWide)
			<<QString::number(m_JT1_RFileValues[i].iSide)
			<<QString::number(m_JT1_RFileValues[i].iModifiable))
			;

		//QComboBox* comBox = new QComboBox(this);
		int needindex;
		for (int j = 0; j < types.size();++j)
		{
			//comBox->addItem(types[j]);
			if (m_JT1_RFileValues[i].strType == types[j])
			{
				needindex = j;
			}
		}
		xMyLabel* label = new xMyLabel(this);
		std::map<QString,QString> mapNameToType = m_table1Pictures->GetModelToType();
		QString strType = mapNameToType.find(types[needindex])->second;
		label->setText(strType);
		label->SetType(strType);
		label->SetName(types[needindex]);
		label->SetJTNth(1);
		label->SetSide(QString("RIGHT"));
		label->SetModifiable(m_JT1_RFileValues[i].iModifiable);
		m_JT1RightLabels.push_back(label);
		m_JT1_RTable->setCellWidget(i, 6, label);






		//comBox->setCurrentIndex(needindex);
		//m_JT1_RTable->setCellWidget(i, 6, comBox);
		//connect(comBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ModelRightTypeChanged(int)));

		QCheckBox *checkBox = new QCheckBox(this);
		checkBox->setStyle(QStyleFactory::create("Cleanlooks"));

		if(m_JT1_RFileValues[i].iShow == 1)
		{
			checkBox->setCheckState(Qt::Checked);

		}
		else
		{
			checkBox->setCheckState(Qt::Unchecked);

		}
		m_JT1_RTable->setCellWidget(i, 7, checkBox);

		connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(ModelRightShowChanged(int)));
	}

	connect(m_JT1_RTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(RightItemDoubleClicked(int,int)));
	connect(m_JT1_RTable,SIGNAL(cellChanged(int,int)),this,SLOT(RightTableChanged(int,int)));
	connect(m_JT1_RTable,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectJT1LRightItems()));

	m_JT1_RTable->setEditTriggers(QAbstractItemView::DoubleClicked);
	m_JT1_RTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//列自适应宽度

	m_JT1_RTable->setColumnHidden(8,true);

	#endif
}

std::vector<JM> xNormalUserWidget::SetLayerByFileValues(std::vector<JM> JmFileValues,std::map<QString,int> MapNameToLayerFileValues)
{
	for (size_t i=0;i<JmFileValues.size();i++)
	{
		QString strname = JmFileValues[i].strModelName;
		std::map<QString,int>::iterator it;
		it = MapNameToLayerFileValues.find(strname);
		if (it!=MapNameToLayerFileValues.end())
		{
			JmFileValues[i].iLayer = it->second;
		}
	}

	return JmFileValues;
}

std::vector<JM> xNormalUserWidget::AddSortNameToValues(std::vector<JM> JmFileValues,std::map<QString,std::vector<QString>> MapSortNameTovJT1JMNames)
{
	std::map<QString,std::vector<QString>>::iterator it = MapSortNameTovJT1JMNames.begin();
	for (;it!=MapSortNameTovJT1JMNames.end();it++)
	{
		for (size_t i=0;i<it->second.size();i++)
		{
			for (size_t j=0;j<JmFileValues.size();j++)
			{
				if (JmFileValues[j].strModelName == (it->second)[i])
				{
					if (JmFileValues[j].strSortName == QString::fromLocal8Bit("RenameSort"))
					{
						JmFileValues[j].strSortName = it->first;
					}
				}
			}
		}
	}

	return JmFileValues;
}

std::vector<JM> xNormalUserWidget::ReSortJMValues(std::vector<JM> JmFileValues,std::map<int,std::vector<QString>> Mapsort)
{
	//按照分类顺序重新排列
	m_vJT1JMTableColorRow.clear();
	std::vector<JM> vTmp = JmFileValues;
	std::vector<JM> vNew;
	std::vector<std::vector<JM>> vTovsortModelName;

	std::map<int,std::vector<QString>>::iterator _it = Mapsort.begin();
	for (;_it!=Mapsort.end();_it++)
	{
		std::vector<JM> vsort;
		
		for (size_t j=0;j<_it->second.size();j++)
		{
			std::vector<JM>::iterator _itt = vTmp.begin();
			for (;_itt!=vTmp.end();_itt++)
			{
				if (_it->second[j] == (*_itt).strModelName)
				{
					vsort.push_back(*_itt);
					//break;
				}
			}
		}

		vTovsortModelName.push_back(vsort);
	}

	for (size_t m=0;m<vTovsortModelName.size();m++)
	{
		for(size_t n=0;n<vTovsortModelName[m].size();n++)
		{
			bubble_sort(vTovsortModelName[m],vTovsortModelName[m].size());
		}
	}

	for (size_t m=0;m<vTovsortModelName.size();m++)
	{
		for(size_t n=0;n<vTovsortModelName[m].size();n++)
		{
			if(n==0)
			{
				vTovsortModelName[m][n].strSortName = QString::fromLocal8Bit("RenameSort");
			}
			vNew.push_back(vTovsortModelName[m][n]);
		}
	}

	return vNew;
}

std::vector<LMR> xNormalUserWidget::SetLMLayerByFileValues(std::vector<LMR> LmFileValues,std::map<QString,int> MapNameToLayerFileValues)
{
	for (size_t i=0;i<LmFileValues.size();i++)
	{
		QString strname = LmFileValues[i].strModelName;
		std::map<QString,int>::iterator it;
		it = MapNameToLayerFileValues.find(strname);
		if (it!=MapNameToLayerFileValues.end())
		{
			LmFileValues[i].iLayer = it->second;
		}
	}
	return LmFileValues;
}

std::vector<LMR> xNormalUserWidget::AddSortNameToLMValues(std::vector<LMR> LmFileValues,std::map<QString,std::vector<QString>> MapSortNameTovJT1LMNames)
{
	std::map<QString,std::vector<QString>>::iterator it = MapSortNameTovJT1LMNames.begin();
	for (;it!=MapSortNameTovJT1LMNames.end();it++)
	{
		for (size_t i=0;i<it->second.size();i++)
		{
			for (size_t j=0;j<LmFileValues.size();j++)
			{
				if (LmFileValues[j].strModelName == (it->second)[i])
				{
					if (LmFileValues[j].strSortName == QString::fromLocal8Bit("RenameSort"))
					{
						LmFileValues[j].strSortName = it->first;

					}
				}
			}
		}
	}

	return LmFileValues;
}

std::vector<LMR> xNormalUserWidget::ReSortLMValues(std::vector<LMR> LmFileValues,std::map<int,std::vector<QString>> Mapsort)
{
	//按照分类顺序重新排列
	m_vJT1LMTableColorRow.clear();
	std::vector<LMR> vTmp = LmFileValues;
	std::vector<LMR> vNew;
	std::vector<std::vector<LMR>> vTovsortModelName;

	std::map<int,std::vector<QString>>::iterator _it = Mapsort.begin();
	for (;_it!=Mapsort.end();_it++)
	{
		std::vector<LMR> vsort;

		for (size_t j=0;j<_it->second.size();j++)
		{
			std::vector<LMR>::iterator _itt = vTmp.begin();
			for (;_itt!=vTmp.end();_itt++)
			{
				if (_it->second[j] == (*_itt).strModelName)
				{
					vsort.push_back(*_itt);
				}
			}
		}

		vTovsortModelName.push_back(vsort);
	}

	for (size_t m=0;m<vTovsortModelName.size();m++)
	{
		for(size_t n=0;n<vTovsortModelName[m].size();n++)
		{
			LMbubble_sort(vTovsortModelName[m],vTovsortModelName[m].size());
		}
	}

	for (size_t m=0;m<vTovsortModelName.size();m++)
	{
		for(size_t n=0;n<vTovsortModelName[m].size();n++)
		{
			if(n==0)
			{
				vTovsortModelName[m][n].strSortName = QString::fromLocal8Bit("RenameSort");
			}
			vNew.push_back(vTovsortModelName[m][n]);
		}
	}

	return vNew;
}

void xNormalUserWidget::ResetColorByJT1SortFile(Table * table,std::vector<int> vColorRow)
{
	int tableRowCount = table->rowCount();

	for (int t=0;t<tableRowCount;t++)
	{
		for (int j=0;j<6;j++)
		{

			QTableWidgetItem *item = table->item(t,j);
			item->setBackgroundColor(QColor(255,255,255));
		}

		//QWidget *cellwidget = table->cellWidget(t,7);

		//cellwidget->setAutoFillBackground(true);
		//QPalette pal;
		//pal.setColor(QPalette::Base,QColor(255,255,255));//色值自定义
		//cellwidget->setPalette(pal);
	}


	for (int i=0;i<tableRowCount;i++)
	{

		for (size_t k=0;k<vColorRow.size();k++)
		{
			if (i==vColorRow[k])
			{
				for (int j=0;j<6;j++)
				{

					QTableWidgetItem *item = table->item(i,j);
					item->setBackgroundColor(QColor(200,200,200));
				}
				//QWidget *cellwidget1 = table->cellWidget(i,7);
				//cellwidget1->setAutoFillBackground(true);
				//QPalette pal1;
				//pal1.setColor(QPalette::Base,QColor(200,200,200));//色值自定义
				//cellwidget1->setPalette(pal1);
			}
		}
		
	}
}

void xNormalUserWidget::ReadJT1JMFileValues()
{
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");	
	m_JT1_JMFileValues = m_pMainWindow->ReadJT1LeftFileValues(strFileName);
	m_MapSortNameTovJT1JMNames = m_pMainWindow->GetSortNameToJT1JMNamesMap();

	m_JT1_JMFileValues = ReSortJMValues(m_JT1_JMFileValues,m_MapTovJT1JMSort);
	m_JT1_JMFileValues = AddSortNameToValues(m_JT1_JMFileValues,m_MapSortNameTovJT1JMNames);
	m_JT1_JMFileValues = SetLayerByFileValues(m_JT1_JMFileValues,m_MapFileSetModelToLayer);

	QString side = "JM";
	std::vector<QString> ModelNames = m_JT1SideToModels.find(side)->second;
	m_vCurTypeNamePic = GetCurTypeNamePic(ModelNames,m_vTypeName);

	modelsPicMap.clear();

	size_t size = m_vCurTypeNamePic.size();
	std::map<QString,int> tempTypes;
	std::vector<QString> tempPics;
	for (size_t m = 0; m < size;++m)
	{
		if (tempTypes.find(m_vCurTypeNamePic[m].second)==tempTypes.end())
		{
			tempTypes[m_vCurTypeNamePic[m].second] = 1;
			tempPics.push_back(m_vCurTypeNamePic[m].second);
		}
	}

	for (size_t i = 0;i < tempPics.size();++i)
	{
		std::vector<QString> modes;
		for (size_t n = 0; n < size;++n)
		{
			if (m_vCurTypeNamePic[n].second==tempPics[i])
			{
				modes.push_back(m_vCurTypeNamePic[n].first);
			}
		}

		modelsPicMap.push_back(make_pair(tempPics[i],modes));
	}

	std::vector<QString> types;//类型对应的模型名
	//for (int i=0;i<m_JT1_JMFileValues.size();i++)
	//{
	//	types.push_back(m_JT1_JMFileValues[i].strModelName);
	//}
	std::map<QString,QString>::iterator iter = m_MapModelToType.begin();
	for (;iter != m_MapModelToType.end();++iter)
	{
		types.push_back(iter->first);
	}

	int countc =  m_JT1_JMTable->columnCount();
	//写入数据
	m_JT1_JMTable->DeleteAllValueRow();
	m_JT1JMLabels.clear();

	if (m_JT1_JMFileValues.empty())
	{
		return;
	}

	QString curSort = m_JT1_JMFileValues[0].strSortName;
	QString lastSort = m_JT1_JMFileValues[0].strSortName;
	for (size_t i=0;i<m_JT1_JMFileValues.size();i++)
	{
		m_JT1_JMTable->addRowValueToLMTable(18,QStringList()<<m_JT1_JMFileValues[i].strSortName
			<<QString::number(m_JT1_JMFileValues[i].iLayer)		
			<<QString::number(m_JT1_JMFileValues[i].fUpHeight)
			<<QString::number(m_JT1_JMFileValues[i].fDownHeight)
			<<QString::number(m_JT1_JMFileValues[i].fLength)
			<<QString::number(m_JT1_JMFileValues[i].fWide)
			<<QString::number(m_JT1_JMFileValues[i].iSide)
			<<QString::number(m_JT1_JMFileValues[i].iModifiable)
			);

		int needindex = -1;
		for (size_t j = 0; j < types.size();++j)
		{		
			if (m_JT1_JMFileValues[i].strType == types[j])
			{
				needindex = j;
			}
		}

		xMyLabel* label = new xMyLabel(this);
		std::map<QString,QString> mapNameToType = m_MapModelToType;
		QString strType = mapNameToType.find(types[needindex])->second;
		int picRow = GetPicTableRow(types[needindex],m_vCurTypeNamePic);
		label->SetPicTableRow(picRow);
		label->setText(QString::number(picRow));
		label->SetType(strType);
		label->SetName(types[needindex]);
		label->SetCurRowToModelName(m_JT1_JMFileValues[i].strModelName);
		label->SetJTNth(1);
		label->SetSide(QString("JM"));
		label->SetModifiable(m_JT1_JMFileValues[i].iModifiable);
		//label->setStyleSheet("background-color:white");
		m_JT1JMLabels.push_back(label);
		m_JT1_JMTable->setCellWidget(i, 7, label);

		QCheckBox *checkBox = new QCheckBox(this);
		checkBox->setStyle(QStyleFactory::create("Cleanlooks"));
		if(m_JT1_JMFileValues[i].iShow == 1)
		{
			checkBox->setCheckState(Qt::Checked);
		}
		else
		{
			checkBox->setCheckState(Qt::Unchecked);
		}
		m_JT1_JMTable->setCellWidget(i, 8, checkBox);
		connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(ModelLeftShowChanged(int)));
	}

	connect(m_JT1_JMTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(LeftItemDoubleClicked(int,int))/*,Qt::QueuedConnection*/);
	connect(m_JT1_JMTable,SIGNAL(cellChanged(int,int)),this,SLOT(LeftTableChanged(int,int))/*,Qt::QueuedConnection*/);
	connect(m_JT1_JMTable,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectJT1LTableItems())/*,Qt::QueuedConnection*/);

	m_JT1_JMTable->setEditTriggers(QAbstractItemView::DoubleClicked);
	m_JT1_JMTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//列自适应宽度
	m_JT1_JMTable->setColumnHidden(9,true);
	//m_JT1_JMTable->setColumnHidden(6,true);
	//ResetColorByJT1SortFile(m_JT1_JMTable,m_vJT1JMTableColorRow);

	SetHideJMAsChosenSpecialModel();
}

void xNormalUserWidget::ReadJT1CenterFileValues()
{
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");
	m_JT1_CFileValues = m_pMainWindow->ReadJT1CenterFileValues(strFileName);
	m_MapSortNameTovJT1LMNames = m_pMainWindow->GetSortNameToJT1LMNamesMap();

	m_JT1_CFileValues = ReSortLMValues(m_JT1_CFileValues,m_MapTovJT1LMSort);
	m_JT1_CFileValues = AddSortNameToLMValues(m_JT1_CFileValues,m_MapSortNameTovJT1LMNames);
	m_JT1_CFileValues = SetLMLayerByFileValues(m_JT1_CFileValues,m_MapFileSetModelToLayer);

	std::vector<QString> types;
	for (size_t i=0;i<m_JT1_CFileValues.size();i++)
	{
		//types.push_back(m_JT1_CFileValues[i].strModelName);
		types.push_back(m_JT1_CFileValues[i].strType);
	}

	QString side = "LM";
	std::vector<QString> ModelNames = m_JT1SideToModels.find(side)->second;
	m_vCurTypeNamePic = GetCurTypeNamePic(ModelNames,m_vTypeName);

	modelsPicMap.clear();
	size_t size = m_vCurTypeNamePic.size();
	std::map<QString,int> tempTypes;
	std::vector<QString> tempPics;
	for (size_t m = 0; m < size;++m)
	{
		if (tempTypes.find(m_vCurTypeNamePic[m].second)==tempTypes.end())
		{
			tempTypes[m_vCurTypeNamePic[m].second] = 1;
			tempPics.push_back(m_vCurTypeNamePic[m].second);
		}
	}

	for (size_t i = 0; i < tempPics.size();++i)
	{
		std::vector<QString> modes;
		for (size_t n = 0; n < size;++n)
		{
			if (m_vCurTypeNamePic[n].second==tempPics[i])
			{
				modes.push_back(m_vCurTypeNamePic[n].first);
			}
		}

		modelsPicMap.push_back(make_pair(tempPics[i],modes));
	}

	//写入数据
	m_JT1_CTable->DeleteAllValueRow();
	m_JT1CenterLabels.clear();
	for (size_t i=0;i<m_JT1_CFileValues.size();i++)
	{
		m_JT1_CTable->addRowValueToLMTable(18,QStringList()<<m_JT1_CFileValues[i].strSortName
															<<QString::number(m_JT1_CFileValues[i].iLayer)
															//<<m_JT1_CFileValues[i].strModelName															
															<<QString::number(m_JT1_CFileValues[i].fupLen)
															<<QString::number(m_JT1_CFileValues[i].fdownLen)
															<<QString::number(m_JT1_CFileValues[i].fLmr)
															<<QString::number(m_JT1_CFileValues[i].ftuLen)
															<<QString::number(m_JT1_CFileValues[i].iSide)
															<<QString::number(m_JT1_CFileValues[i].iModifiable)
															);

		int needindex;
		for (size_t j = 0; j < types.size();++j)
		{
			if (m_JT1_CFileValues[i].strType == types[j])
			{
				needindex = j;
			}
		}

		xMyLabel* label = new xMyLabel(this);
		std::map<QString,QString> mapNameToType = m_MapModelToType;
		QString strType = mapNameToType.find(types[needindex])->second;
		int picRow = GetPicTableRow(types[needindex],m_vCurTypeNamePic);
		label->SetPicTableRow(picRow);
		label->setText(QString::number(picRow));
		//label->setText(strType);
		label->SetType(strType);
		label->SetName(types[needindex]);
		label->SetCurRowToModelName(m_JT1_CFileValues[i].strModelName);
		label->SetJTNth(1);
		label->SetSide(QString("LM"));
		label->SetModifiable(m_JT1_CFileValues[i].iModifiable);

		m_JT1CenterLabels.push_back(label);
		m_JT1_CTable->setCellWidget(i, 7, label);

		QCheckBox *checkBox = new QCheckBox(this);
		checkBox->setStyle(QStyleFactory::create("Cleanlooks"));

		if(m_JT1_CFileValues[i].isShow == 1)
		{
			checkBox->setCheckState(Qt::Checked);

		}
		else
		{
			checkBox->setCheckState(Qt::Unchecked);

		}
		m_JT1_CTable->setCellWidget(i, 8, checkBox);

		connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(ModelCenterShowChanged(int)));
	}

	connect(m_JT1_CTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(CenterItemDoubleClicked(int,int)));
	connect(m_JT1_CTable,SIGNAL(cellChanged(int,int)),this,SLOT(CenterTableChanged(int,int)));
	connect(m_JT1_CTable,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectJT1LCenterItems()));
	
	m_JT1_CTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//列自适应宽度
	m_JT1_CTable->setEditTriggers(QAbstractItemView::DoubleClicked);

	m_JT1_CTable->setColumnHidden(9,true);
	//m_JT1_CTable->setColumnHidden(6,true);

	SetHideLMAsChosenSpecialModel();
}

void xNormalUserWidget::ResetJT1LeftModelVector()
{
	m_JT1_JMFileSTLs.clear();
	for(size_t i=0;i<m_JT1_JMFileValues.size();i++)
	{
		size_t j;
		for (j=0;j<m_AllSTLs.size();j++)
		{
			if(m_JT1_JMFileValues[i].strModelName == m_AllSTLs[j]->GetSTLName())
				break;
		}

		if(j<m_AllSTLs.size())
		{
			m_AllSTLs[j]->SetType(m_JT1_JMFileValues[i].strType);
			m_AllSTLs[j]->SetLMupLen(m_JT1_JMFileValues[i].fUpHeight);
			m_AllSTLs[j]->SetLMdownLen(m_JT1_JMFileValues[i].fDownHeight);
			m_AllSTLs[j]->SetJMLen(m_JT1_JMFileValues[i].fLength);
			m_AllSTLs[j]->SetJMWide(m_JT1_JMFileValues[i].fWide);
			m_AllSTLs[j]->SetLMShow(m_JT1_JMFileValues[i].iShow);
			m_AllSTLs[j]->SetJMAoSide(m_JT1_JMFileValues[i].iSide);
			m_AllSTLs[j]->SetLMNthJT(m_JT1_JMFileValues[i].iJTNth);
			m_AllSTLs[j]->SetCanChanged(m_JT1_JMFileValues[i].iModifiable);

			m_JT1_JMFileSTLs.push_back(m_AllSTLs[j]);
		}
	}

	ReBuildJT1LeftModel();
}

void xNormalUserWidget::ReBuildJT1LeftModel()
{
	for (size_t i=0;i<m_JT1_JMFileSTLs.size();i++)
	{
		float a = m_JT1_JMFileSTLs[i]->GetJMLen();
		float b = m_JT1_JMFileSTLs[i]->GetJMWide();

		int side = m_JT1_JMFileSTLs[i]->GetJMAoSide();
		bool AoSide; 
		if(side == 0)//zuo
		{
			AoSide = false;
		}
		else if (side == 1)
		{
			AoSide = true;
		}

		//Clip * tmpClipModel = new Clip(m_JT1_LFileSTLs[i]->GetLMupLen(),m_JT1_LFileSTLs[i]->GetLMdownLen(),m_NorWidget_param.fPipeDiameter/2,40,m_JT1_LFileSTLs[i]->GetJMLen(),m_JT1_LFileSTLs[i]->GetJMWide(),AoSide);

		//osg::ref_ptr<osg::Geode> JMModel = tmpClipModel->Create(m_JT1_LFileSTLs[i]->GetModelCopyFatherCenter(),osg::Vec3(0,0,1));
		//JMModel->setName(m_JT1_LFileSTLs[i]->GetSTLName().toStdString());
		//m_JT1_LFileSTLs[i]->replaceCurNode((osg::Node*)JMModel);
		//m_JT1_LFileSTLs[i]->SetIsNew(true);

		int ishow = m_JT1_JMFileSTLs[i]->GetIsShow();
		if (ishow == 1)
		{
			m_JT1_JMFileSTLs[i]->GetSTLNode()->setNodeMask(-1);
		}
		else if (ishow == 0)
		{
			m_JT1_JMFileSTLs[i]->GetSTLNode()->setNodeMask(0);
		}
	}
}

void xNormalUserWidget::ResetJT1RightModelVector()
{
	m_JT1_RFileSTLs.clear();
	for(size_t i=0;i<m_JT1_RFileValues.size();i++)
	{
		size_t j;
		for (j=0;j<m_AllSTLs.size();j++)
		{
			if(m_JT1_RFileValues[i].strModelName == m_AllSTLs[j]->GetSTLName())
				break;
		}

		if(j<m_AllSTLs.size())
		{
			m_AllSTLs[j]->SetType(m_JT1_RFileValues[i].strType);
			m_AllSTLs[j]->SetLMupLen(m_JT1_RFileValues[i].fUpHeight);
			m_AllSTLs[j]->SetLMdownLen(m_JT1_RFileValues[i].fDownHeight);
			m_AllSTLs[j]->SetJMLen(m_JT1_RFileValues[i].fLength);
			m_AllSTLs[j]->SetJMWide(m_JT1_RFileValues[i].fWide);
			m_AllSTLs[j]->SetLMShow(m_JT1_RFileValues[i].iShow);
			m_AllSTLs[j]->SetJMAoSide(m_JT1_RFileValues[i].iSide);
			m_AllSTLs[j]->SetLMNthJT(m_JT1_RFileValues[i].iJTNth);

			m_JT1_RFileSTLs.push_back(m_AllSTLs[j]);
		}
	}

	ReBuildJT1RightModel();
}

void xNormalUserWidget::ReBuildJT1RightModel()
{
	for (size_t i=0;i<m_JT1_RFileSTLs.size();i++)
	{
		float a = m_JT1_RFileSTLs[i]->GetJMLen();
		float b = m_JT1_RFileSTLs[i]->GetJMWide();

		int side = m_JT1_RFileSTLs[i]->GetJMAoSide();
		bool AoSide; 
		if(side == 0)//zuo
		{
			AoSide = false;
		}
		else if (side == 1)
		{
			AoSide = true;
		}

		//Clip * tmpClipModel = new Clip(m_JT1_LFileSTLs[i]->GetLMupLen(),m_JT1_LFileSTLs[i]->GetLMdownLen(),m_NorWidget_param.fPipeDiameter/2,40,m_JT1_LFileSTLs[i]->GetJMLen(),m_JT1_LFileSTLs[i]->GetJMWide(),AoSide);

		//osg::ref_ptr<osg::Geode> JMModel = tmpClipModel->Create(m_JT1_LFileSTLs[i]->GetModelCopyFatherCenter(),osg::Vec3(0,0,1));
		//JMModel->setName(m_JT1_LFileSTLs[i]->GetSTLName().toStdString());
		//m_JT1_LFileSTLs[i]->replaceCurNode((osg::Node*)JMModel);
		//m_JT1_LFileSTLs[i]->SetIsNew(true);

		int ishow = m_JT1_RFileSTLs[i]->GetIsShow();
		if (ishow == 1)
		{
			m_JT1_RFileSTLs[i]->GetSTLNode()->setNodeMask(-1);
		}
		else if (ishow == 0)
		{
			m_JT1_RFileSTLs[i]->GetSTLNode()->setNodeMask(0);
		}

		// 此处为什么颜色会变红
		//m_pMainWindow->On_modelColor_change(m_JT1_LFileSTLs[i]->GetSTLName());
	}
}

void xNormalUserWidget::ResetJT1CenterModelVector()
{
	m_JT1_CFileSTLs.clear();
	for(size_t i=0;i<m_JT1_CFileValues.size();i++)
	{
		size_t j;
		for (j=0;j<m_AllSTLs.size();j++)
		{
			if(m_JT1_CFileValues[i].strModelName == m_AllSTLs[j]->GetSTLName())
				break;
		}

		if(j<m_AllSTLs.size())
		{
			m_AllSTLs[j]->SetType(m_JT1_CFileValues[i].strType);
			m_AllSTLs[j]->SetLMupLen(m_JT1_CFileValues[i].fupLen);
			m_AllSTLs[j]->SetLMdownLen(m_JT1_CFileValues[i].fdownLen);
			m_AllSTLs[j]->SetLMr(m_JT1_CFileValues[i].fLmr);
			m_AllSTLs[j]->SetLMtuLen(m_JT1_CFileValues[i].ftuLen);
			//m_AllSTLs[j]->SetJMLen(m_JT1_CFileValues[i].fLength);
			//m_AllSTLs[j]->SetJMWide(m_JT1_CFileValues[i].fWide);
			m_AllSTLs[j]->SetLMShow(m_JT1_CFileValues[i].isShow);
			m_AllSTLs[j]->SetJMAoSide(m_JT1_CFileValues[i].iSide);
			m_AllSTLs[j]->SetLMNthJT(m_JT1_CFileValues[i].iJTNth);
			m_AllSTLs[j]->SetCanChanged(m_JT1_CFileValues[i].iModifiable);

			m_JT1_CFileSTLs.push_back(m_AllSTLs[j]);
		}
	}

	ReBuildJT1CenterModel();
}

void xNormalUserWidget::ReBuildJT1CenterModel()
{
	for (size_t i=0;i<m_JT1_CFileSTLs.size();i++)
	{
		float a = m_JT1_CFileSTLs[i]->GetJMLen();
		float b = m_JT1_CFileSTLs[i]->GetJMWide();

		int side = m_JT1_CFileSTLs[i]->GetJMAoSide();
		bool AoSide; 
		if(side == 0)//zuo
		{
			AoSide = false;
		}
		else if (side == 1)
		{
			AoSide = true;
		}

		int ishow = m_JT1_CFileSTLs[i]->GetIsShow();
		if (ishow == 1)
		{
			m_JT1_CFileSTLs[i]->GetSTLNode()->setNodeMask(-1);
		}
		else if (ishow == 0)
		{
			m_JT1_CFileSTLs[i]->GetSTLNode()->setNodeMask(0);
		}
	}
}

void xNormalUserWidget::SetNormalUserWidgetLanguage(std::vector<QString> vCurWords)
{
	m_vCurLanguageWords = vCurWords;

#if 1
	//表头
	QStringList header;
	header<<QString::fromLocal8Bit("  ")
		<<vCurWords[20]
		<<vCurWords[21]
		<<vCurWords[22]
		<<vCurWords[23]
		<<vCurWords[24]
		<<vCurWords[25]
		<<vCurWords[26]
		<<vCurWords[27]
		<<QString::fromLocal8Bit("Modifiable");

	//m_JT1_LTable->setColumnValue(8, header);
	//m_JT1_LTable->setFont(QFont("Microsoft YaHe", 10));
	//
	//m_JT1_RTable->setColumnValue(8, header);
	//m_JT1_RTable->setFont(QFont("Microsoft YaHe", 10));

	//m_JT1_JMTable->setColumnValue(10, header);
	//m_JT1_JMTable->setFont(QFont("Microsoft YaHe", 10));

		//m_JT1_CTable->setColumnValue(8, header);
		//m_JT1_CTable->setFont(QFont("Microsoft YaHe", 10));

		//m_JT1_RTable->setColumnValue(8, header);
		//m_JT1_RTable->setFont(QFont("Microsoft YaHe", 10));

		m_JT1_JMTable->setColumnValue(10, header);
		m_JT1_JMTable->setFont(QFont("Microsoft YaHe", 10));

	QStringList header1;
	header1<<QString::fromLocal8Bit("  ")
	<<vCurWords[20]
	<<vCurWords[21]
	<<vCurWords[22]
	<<vCurWords[28]
	<<vCurWords[29]
	<<vCurWords[30]
	<<vCurWords[26]
	<<vCurWords[27]
	<<QString::fromLocal8Bit("Modifiable");
	m_JT1_CTable->setColumnValue(10, header1);
	m_JT1_CTable->setFont(QFont("Microsoft YaHe", 10));

#endif

}

void xNormalUserWidget::ModelLeftTypeChanged(int i)
{
#if 0

	QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());
	if(NULL == comBox_)
	{
		return;
	}
	int x = comBox_->frameGeometry().x();
	int y = comBox_->frameGeometry().y();
	QModelIndex index = m_JT1_LTable->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	qDebug() << "选择第几行:" << row << "选择第几列:" << column;
	//qDebug() << "选择内容:" << text;

	// 刷新后台文件的值
	//QString name = m_JT1_LTable->item(row,1)->text();

	QString name = m_JT1_LFileValues[row].strModelName; //获取内容

	for (int i = 0; i < m_JT1_LFileValues.size();++i)
	{
		if (m_JT1_LFileValues[i].strModelName==name)
		{
			JM aim;
			GetAimJMValues(m_JT1_LFileValues,comBox_->currentText(),aim);
			m_JT1_LFileValues[i].strType = comBox_->currentText();
			m_JT1_LFileValues[i].fLength = aim.fLength;
			m_JT1_LFileValues[i].fWide = aim.fWide;
			m_JT1_LFileValues[i].iSide = aim.iSide;
			m_JT1_LTable->item(row,3)->setText(QString::number(m_JT1_LFileValues[i].fLength));
			m_JT1_LTable->item(row,4)->setText(QString::number(m_JT1_LFileValues[i].fWide));
			m_JT1_LTable->item(row,5)->setText(QString::number(m_JT1_LFileValues[i].iSide));

		}
	}
	// 刷新文件
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LEFT.csv");
	WriteToSideFile(strFileName,m_JT1_LFileValues);

	//更新视图模型
	m_pMainWindow->RebuildJT1JMModel(m_JT1_LFileValues);
	
	if(m_pMainWindow != NULL)
	{
		m_pMainWindow->On_modelColor_change(comBox_->currentText());
	}
#endif
}

void xNormalUserWidget::ModelLeftShowChanged(int i)
{
	QCheckBox* checkBox = dynamic_cast<QCheckBox*>(this->sender());
	if(NULL == checkBox)
	{
		return;
	}

	int x = checkBox->frameGeometry().x();
	int y = checkBox->frameGeometry().y();
	QModelIndex index = m_JT1_JMTable->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	qDebug() << "选择第几行:" << row << "选择第几列:" << column;
	Qt::CheckState state = checkBox->checkState();
	// 刷新后台文件的值
	//QString name = m_JT1_LTable->item(row,1)->text();
	//m_JT1_LFileValues
	QString name = m_JT1_JMFileValues[row].strModelName; //获取内容

	for (size_t i = 0; i < m_JT1_JMFileValues.size();++i)
	{
		if (m_JT1_JMFileValues[i].strModelName==name)
		{
			if (state==Qt::Unchecked)
			{
				m_JT1_JMFileValues[i].iShow = 0;
			}
			else if (state==Qt::Checked)
			{
				m_JT1_JMFileValues[i].iShow = 1;
			}
		}
	}

	// 刷新文件
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");
	WriteToSideFile(strFileName,m_JT1_JMFileValues);

	m_JT1_JMFileValues = m_pMainWindow->ReadJT1LeftFileValues(strFileName);
	ResetJT1LeftModelVector();
}

void xNormalUserWidget::LeftItemDoubleClicked(int i,int j)
{
#if 0

	if (j==6)
	{
#if 0
		// 设置模型类型
		//QString type =  m_JT1LeftLabels[i]->text();
		SetChooseLabel(m_JT1JMLabels,m_JT1JMLabels[i],m_JT1CenterLabels);
		int modify = m_JT1JMLabels[i]->GetModifiable();
		if (modify == 1)
		{

			m_JT1PicTable->show();
			QString side = "JM";
			std::vector<QString> ModelNames = m_JT1SideToModels.find(side)->second;
			RefreshPicTable(ModelNames);
			SetPicTableBtnFrame(m_JT1JMLabels[i]);
		}
		else
		{
			m_JT1PicTable->hide();
		}

#endif	

	}
	else
	{
		if (!m_bIsUseKeyboard)
		{
			numkeyboard->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
			numkeyboard->setStyleSheet("border:2px solid white;");
			//numkeyboard->setGeometry(0,300);
			numkeyboard->move(80,80);

			numkeyboard->setText(m_JT1_LTable->item(i,j)->text());  //当前的文本框的内容，this->指向的是SoftKeyLineEdit        
			numkeyboard->exec();
			if(numkeyboard->valid)
			{
				QString str = numkeyboard->getText();
				m_JT1_LTable->item(i,j)->setText(numkeyboard->getText());  //此处的setText是继承QLineEdit的，numkeyboard->setTex是自定义的，目的是获取当前的文本
			}

			//QString name = m_JT1_LTable->item(i,1)->text();
			QString name = m_JT1_LFileValues[i].strModelName; //获取内容

			// 刷新后台文件的值
			for (int i = 0; i < m_JT1_LFileValues.size();++i)
			{
				if (m_JT1_LFileValues[i].strModelName==name)
				{
					m_JT1_LFileValues[i].fLength = m_JT1_LTable->item(i,3)->text().toFloat();
					m_JT1_LFileValues[i].fWide = m_JT1_LTable->item(i,4)->text().toFloat();
					m_JT1_LFileValues[i].iSide = m_JT1_LTable->item(i,5)->text().toInt();
				}
			}
			// 刷新文件
			QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");
			WriteToSideFile(strFileName,m_JT1_LFileValues);

			//更新视图模型
			m_pMainWindow->RebuildJT1JMModel(m_JT1_LFileValues);

			if(m_pMainWindow != NULL)
			{
				m_pMainWindow->On_modelColor_change(name);
			}
		}
		else
		{
			//LeftTableChanged(i,j);
		}
	}

#endif

}

// 修改表格值响应函数
void xNormalUserWidget::LeftTableChanged(int i, int j)
{
		QString name = m_JT1_JMFileValues[i].strModelName; //获取内容

		for (size_t i = 0; i < m_JT1_JMFileValues.size();++i)
		{
			if (m_JT1_JMFileValues[i].strModelName == name)
			{
				float value = m_JT1_JMTable->item(i,j)->text().toFloat();
				switch (j)
				{
				case 4:
					m_JT1_JMFileValues[i].fLength = value;
					break;
				case 5:
					m_JT1_JMFileValues[i].fWide = value;
					break;
				case 6:
					m_JT1_JMFileValues[i].iSide = value;
					break;
				}

				break;
			}
		}

		// 刷新文件
		QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");
		WriteToSideFile(strFileName,m_JT1_JMFileValues);

		//更新视图模型
		m_pMainWindow->RebuildJT1JMModel(m_JT1_JMFileValues);
		if(m_pMainWindow != NULL)
		{
			m_pMainWindow->On_modelColor_change(name);
		}

}

void xNormalUserWidget::ModelCenterTypeChanged(int)
{
	QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());
	if(NULL == comBox_)
	{
		return;
	}
	int x = comBox_->frameGeometry().x();
	int y = comBox_->frameGeometry().y();
	QModelIndex index = m_JT1_CTable->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	qDebug() << "选择第几行:" << row << "选择第几列:" << column;
	//qDebug() << "选择内容:" << text;
	//QString name = m_JT1_CTable->item(row,1)->text();
	QString name = m_JT1_CFileValues[row].strModelName; //获取内容

	for (size_t i = 0; i < m_JT1_CFileValues.size();++i)
	{
		if (m_JT1_CFileValues[i].strModelName==name)
		{

			LMR aim;
			GetAimLMValues(m_JT1_CFileValues,comBox_->currentText(),aim);
			m_JT1_CFileValues[i].strType = comBox_->currentText();

			m_JT1_CFileValues[i].fLmr = aim.fLmr;
			m_JT1_CFileValues[i].ftuLen = aim.ftuLen;
			m_JT1_CFileValues[i].iSide = aim.iSide;


			m_JT1_CTable->item(row,3)->setText(QString::number(m_JT1_CFileValues[i].fLmr));
			m_JT1_CTable->item(row,4)->setText(QString::number(m_JT1_CFileValues[i].ftuLen));
			m_JT1_CTable->item(row,5)->setText(QString::number(m_JT1_CFileValues[i].iSide));

		}
	}
	// 刷新文件
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_CENTER.csv");
	WriteToCenterFile(strFileName,m_JT1_CFileValues,1);

	//更新视图模型
	m_pMainWindow->RebuildJT1LMModel(m_JT1_CFileValues);

	if(m_pMainWindow != NULL)
	{
		m_pMainWindow->On_modelColor_change(comBox_->currentText());
	}
}

void xNormalUserWidget::ModelCenterShowChanged(int)
{
	QCheckBox* checkBox = dynamic_cast<QCheckBox*>(this->sender());
	if(NULL == checkBox)
	{
		return;
	}

	int x = checkBox->frameGeometry().x();
	int y = checkBox->frameGeometry().y();
	QModelIndex index = m_JT1_CTable->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	qDebug() << "选择第几行:" << row << "选择第几列:" << column;
	Qt::CheckState state = checkBox->checkState();
	// 刷新后台文件的值
	//QString name = m_JT1_CTable->item(row,1)->text();
	QString name = m_JT1_CFileValues[row].strModelName; //获取内容

	for (size_t i = 0; i < m_JT1_CFileValues.size();++i)
	{
		if (m_JT1_CFileValues[i].strModelName==name)
		{
			if (state==Qt::Unchecked)
			{
				m_JT1_CFileValues[i].isShow = 0;
			}
			else
			{
				m_JT1_CFileValues[i].isShow = 1;
			}
		}
	}

	// 刷新文件
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");
	WriteToCenterFile(strFileName,m_JT1_CFileValues,1);

	m_JT1_CFileValues = m_pMainWindow->ReadJT1CenterFileValues(strFileName);
	ResetJT1CenterModelVector();
}

// 修改表格值响应函数
void xNormalUserWidget::CenterTableChanged(int i, int j)
{
	//if (j==2 || j==5 || j==6)
	{

		//QString name = m_JT1_CTable->item(i,1)->text();
		QString name = m_JT1_CFileValues[i].strModelName; //获取内容

		for (size_t i = 0; i < m_JT1_CFileValues.size();++i)
		{
			if (m_JT1_CFileValues[i].strModelName==name)
			{
				float value = m_JT1_CTable->item(i,j)->text().toFloat();
				switch (j)
				{
				case 4:
					m_JT1_CFileValues[i].fLmr = value;
					m_pMainWindow->RebuildParamFile(m_JT1_CFileValues);
					break;
				case 5:
					m_JT1_CFileValues[i].ftuLen = value;
					break;
				case 6:
					m_JT1_CFileValues[i].iSide = value;
					break;
				}

				break;
			}
		}

		// 刷新文件
		QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");
		WriteToCenterFile(strFileName,m_JT1_CFileValues,1);

		//更新视图模型
		m_pMainWindow->RebuildJT1LMModel(m_JT1_CFileValues);
		if(m_pMainWindow != NULL)
		{
			m_pMainWindow->On_modelColor_change(name);
		}
	}
}

void xNormalUserWidget::CenterItemDoubleClicked(int i,int j)
{
	if (j==6)
	{
#if 0
		// 设置模型类型
		SetChooseLabel(m_JT1CenterLabels,m_JT1CenterLabels[i],m_JT1JMLabels);
		int modify = m_JT1CenterLabels[i]->GetModifiable();
		if (modify == 1)
		{
			m_JT1PicTable->show();

			QString side = "LM";
			std::vector<QString> ModelNames = m_JT1SideToModels.find(side)->second;
			RefreshPicTable(ModelNames);
			SetPicTableBtnFrame(m_JT1CenterLabels[i]);

		}
		else
		{
			m_JT1PicTable->hide();
		}
#endif
	}
	else
	{

		if (!m_bIsUseKeyboard)
		{
			numkeyboard->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
			numkeyboard->setStyleSheet("border:2px solid white;");
			//numkeyboard->setGeometry(0,300);
			numkeyboard->move(80,80);

			numkeyboard->setText(m_JT1_CTable->item(i,j)->text());  //当前的文本框的内容，this->指向的是SoftKeyLineEdit        
			numkeyboard->exec();
			if(numkeyboard->valid)
			{
				m_JT1_CTable->item(i,j)->setText(numkeyboard->getText());  //此处的setText是继承QLineEdit的，numkeyboard->setTex是自定义的，目的是获取当前的文本
			}

			//QString name = m_JT1_CTable->item(i,1)->text();
			QString name = m_JT1_CFileValues[i].strModelName; //获取内容

			// 刷新后台文件的值
			for (size_t i = 0; i < m_JT1_CFileValues.size();++i)
			{
				if (m_JT1_CFileValues[i].strModelName==name)
				{
					m_JT1_CFileValues[i].fLmr = m_JT1_CTable->item(i,3)->text().toFloat();
					m_JT1_CFileValues[i].ftuLen = m_JT1_CTable->item(i,4)->text().toFloat();
					m_JT1_CFileValues[i].iSide = m_JT1_CTable->item(i,5)->text().toInt();
				}
			}
			// 刷新文件
			QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");
			WriteToCenterFile(strFileName,m_JT1_CFileValues,1);

			//更新视图模型
			m_pMainWindow->RebuildJT1LMModel(m_JT1_CFileValues);
			if(m_pMainWindow != NULL)
			{
				m_pMainWindow->On_modelColor_change(name);
			}
		}
	}
}

void xNormalUserWidget::ModelRightTypeChanged(int)
{
#if 0
	QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());
	if(NULL == comBox_)
	{
		return;
	}
	int x = comBox_->frameGeometry().x();
	int y = comBox_->frameGeometry().y();
	QModelIndex index = m_JT1_RTable->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	qDebug() << "选择第几行:" << row << "选择第几列:" << column;
	//qDebug() << "选择内容:" << text;

	//QString name = m_JT1_RTable->item(row,1)->text();
	QString name = m_JT1_RFileValues[row].strModelName; //获取内容

	for (int i = 0; i < m_JT1_RFileValues.size();++i)
	{
		if (m_JT1_RFileValues[i].strModelName==name)
		{
			m_JT1_RFileValues[i].strType = comBox_->currentText();


			JM aim;
			GetAimJMValues(m_JT1_RFileValues,comBox_->currentText(),aim);
			m_JT1_RFileValues[i].strType = comBox_->currentText();
			m_JT1_RFileValues[i].fLength = aim.fLength;
			m_JT1_RFileValues[i].fWide = aim.fWide;
			m_JT1_RFileValues[i].iSide = aim.iSide;
			m_JT1_RTable->item(row,3)->setText(QString::number(m_JT1_RFileValues[i].fLength));
			m_JT1_RTable->item(row,4)->setText(QString::number(m_JT1_RFileValues[i].fWide));
			m_JT1_RTable->item(row,5)->setText(QString::number(m_JT1_RFileValues[i].iSide));


		}
	}
	// 刷新文件
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_RIGHT.csv");
	WriteToSideFile(strFileName,m_JT1_RFileValues);


	//更新视图模型
	m_pMainWindow->RebuildJT1JMModel(m_JT1_RFileValues);

	if(m_pMainWindow != NULL)
	{
		m_pMainWindow->On_modelColor_change(comBox_->currentText());
	}
#endif
}

void xNormalUserWidget::ModelRightShowChanged(int)
{
#if 0
	QCheckBox* checkBox = dynamic_cast<QCheckBox*>(this->sender());
	if(NULL == checkBox)
	{
		return;
	}

	int x = checkBox->frameGeometry().x();
	int y = checkBox->frameGeometry().y();
	QModelIndex index = m_JT1_RTable->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	qDebug() << "选择第几行:" << row << "选择第几列:" << column;
	Qt::CheckState state = checkBox->checkState();

	// 刷新后台文件的值
	//QString name = m_JT1_RTable->item(row,1)->text();
	QString name = m_JT1_RFileValues[row].strModelName; //获取内容

	for (int i = 0; i < m_JT1_RFileValues.size();++i)
	{
		if (m_JT1_RFileValues[i].strModelName==name)
		{
			if (state==Qt::Unchecked)
			{
				m_JT1_RFileValues[i].iShow = 0;
			}
			else
			{
				m_JT1_RFileValues[i].iShow = 1;
			}
		}
	}

	// 刷新文件
	QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_RIGHT.csv");
	WriteToSideFile(strFileName,m_JT1_RFileValues);

	m_JT1_RFileValues = m_pMainWindow->ReadJT1RightFileValues(strFileName);
	ResetJT1RightModelVector();
#endif
}

void xNormalUserWidget::RightTableChanged(int i, int j)
{
#if 0
	//if (j>=4 && j<=6)
	{
		//QString name = m_JT1_RTable->item(i,1)->text();
		QString name = m_JT1_RFileValues[i].strModelName; //获取内容

		for (int i = 0; i < m_JT1_RFileValues.size();++i)
		{
			if (m_JT1_RFileValues[i].strModelName==name)
			{
				float value = m_JT1_RTable->item(i,j)->text().toFloat();
				switch (j)
				{
					/*case 2:
					m_JT1_RFileValues[i].fUpHeight = value;
					break;
					case 3:
					m_JT1_RFileValues[i].fDownHeight = value;
					break;*/
				case 3:
					m_JT1_RFileValues[i].fLength = value;
					break;
				case 4:
					m_JT1_RFileValues[i].fWide = value;
					break;
				case 5:
					m_JT1_RFileValues[i].iSide = value;
					break;
				}

				break;
			}
		}

		// 刷新文件
		QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_RIGHT.csv");
		WriteToSideFile(strFileName,m_JT1_RFileValues);

		//更新视图模型
		m_pMainWindow->RebuildJT1JMModel(m_JT1_RFileValues);
		if(m_pMainWindow != NULL)
		{
			m_pMainWindow->On_modelColor_change(name);
		}
	}

#endif
}

void xNormalUserWidget::RightItemDoubleClicked(int i,int j)
{
#if 0
	if (j==6)
	{
		// 设置模型类型
		int modify = m_JT1RightLabels[i]->GetModifiable();
		if (modify == 1)
		{

			QString side = "RIGHT";
			std::vector<QString> ModelNames = m_JT1SideToModels.find(side)->second;
			m_table1Pictures->RefreshTypeToBtnMap(ModelNames);
			m_table1Pictures->setLabel(m_JT1RightLabels[i]);//设置表格中当前选项为红框
			m_table1Pictures->show();
		}
	}
	else
	{
		if (!m_bIsUseKeyboard)
		{
			numkeyboard->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
			numkeyboard->setStyleSheet("border:2px solid white;");
			//numkeyboard->setGeometry(0,300);
			numkeyboard->move(80,80);

			numkeyboard->setText(m_JT1_RTable->item(i,j)->text());  //当前的文本框的内容，this->指向的是SoftKeyLineEdit        
			numkeyboard->exec();
			if(numkeyboard->valid)
			{
				m_JT1_RTable->item(i,j)->setText(numkeyboard->getText());  //此处的setText是继承QLineEdit的，numkeyboard->setTex是自定义的，目的是获取当前的文本
			}

			QString name = m_JT1_RFileValues[i].strModelName; //获取内容

			//QString name = m_JT1_RTable->item(i,1)->text();
			// 刷新后台文件的值
			for (int i = 0; i < m_JT1_RFileValues.size();++i)
			{
				if (m_JT1_RFileValues[i].strModelName==name)
				{
					m_JT1_RFileValues[i].fLength = m_JT1_RTable->item(i,3)->text().toFloat();
					m_JT1_RFileValues[i].fWide = m_JT1_RTable->item(i,4)->text().toFloat();
					m_JT1_RFileValues[i].iSide = m_JT1_RTable->item(i,5)->text().toInt();
				}
			}
			// 刷新文件
			QString strFileName = m_strJT1Dir + QString::fromLocal8Bit("/JT1_RIGHT.csv");
			WriteToSideFile(strFileName,m_JT1_RFileValues);

			//更新视图模型
			m_pMainWindow->RebuildJT1JMModel(m_JT1_RFileValues);

			if(m_pMainWindow != NULL)
			{
				m_pMainWindow->On_modelColor_change(name);
			}
		}
	}
#endif 
}

void xNormalUserWidget::SlotSelectJT1LTableItems()
{

	//m_JT1_JMTable->setStyleSheet("selection-background-color:lightblue;");
	m_JT1_CTable->clearSelection();

	std::vector<QString> vmodelName;
	vmodelName.clear();

	QList<QTableWidgetItem*> items = m_JT1_JMTable->selectedItems();
	int count = items.count();
	for(int i = 0; i < count; i++)
	{
		int row = m_JT1_JMTable->row(items.at(i));
		//QTableWidgetItem *item = items.at(i);
		QString text = m_JT1_JMFileValues[row].strModelName; //获取内容
		vmodelName.push_back(text);

		// 设置模型类型
		//QString type =  m_JT1LeftLabels[i]->text();
		//SetChooseLabel(m_JT1JMLabels,m_JT1JMLabels[row],m_JT1CenterLabels);
		int modify = m_JT1JMLabels[row]->GetModifiable();
		if (modify == 1)
		{

			m_JT1PicTable->show();
			QString side = "JM";
			std::vector<QString> ModelNames = m_JT1SideToModels.find(side)->second;
			RefreshPicTable(ModelNames);
			SetPicTableBtnFrame(m_JT1JMLabels[row]);
			m_JT1PicTable->setSelectionMode(QAbstractItemView::SingleSelection);
		}
		else
		{
			//m_JT1PicTable->hide();
			DeleteAllItems();
		}
	}

	m_pMainWindow->ChangeMutiModelColor(vmodelName);
}

void xNormalUserWidget::SlotSelectJT1LRightItems()
{
#if 0

	std::vector<QString> vmodelName;
	vmodelName.clear();
	QList<QTableWidgetItem*> items = m_JT1_RTable->selectedItems();
	int count = items.count();
	for(int i = 0; i < count; i++)
	{
		int row = m_JT1_RTable->row(items.at(i));
		//QTableWidgetItem *item = items.at(i);
		QString text = m_JT1_RFileValues[row].strModelName; //获取内容
		vmodelName.push_back(text);
	}

	m_pMainWindow->ChangeMutiModelColor(vmodelName);
#endif
}

void xNormalUserWidget::SlotSelectJT1LCenterItems()
{
	//m_JT1_CTable->setStyleSheet("selection-background-color:lightblue;");

	m_JT1_JMTable->clearSelection();

	std::vector<QString> vmodelName;
	vmodelName.clear();
	QList<QTableWidgetItem*> items = m_JT1_CTable->selectedItems();
	int count = items.count();
	for(int i = 0; i < count; i++)
	{
		int row = m_JT1_CTable->row(items.at(i));
		//QTableWidgetItem *item = items.at(i);
		QString text = m_JT1_CFileValues[row].strModelName; //获取内容
		vmodelName.push_back(text);

		// 设置模型类型
		//SetChooseLabel(m_JT1CenterLabels,m_JT1CenterLabels[row],m_JT1JMLabels);
		int modify = m_JT1CenterLabels[row]->GetModifiable();
		if (modify == 1)
		{
			m_JT1PicTable->show();

			QString side = "LM";
			std::vector<QString> ModelNames = m_JT1SideToModels.find(side)->second;
			RefreshPicTable(ModelNames);
			SetPicTableBtnFrame(m_JT1CenterLabels[row]);
			m_JT1PicTable->setSelectionMode(QAbstractItemView::SingleSelection);
		}
		else
		{
			//m_JT1PicTable->hide();
			DeleteAllItems();
		}
	}

	m_pMainWindow->ChangeMutiModelColor(vmodelName);
//	ShowPicTable();
}

void xNormalUserWidget::closeEvent(QCloseEvent *event)
{
	m_pMainWindow->ResumeAllModelColor();
	m_table1Pictures->close();
	m_table1Pictures->close();
}

void xNormalUserWidget::Slot_Type_Selected()
{
	QPushButton* button = dynamic_cast<QPushButton*>(this->sender());
	if(NULL == button)
	{
		return;
	}

	int x = button->frameGeometry().x();
	int y = button->frameGeometry().y();
	QModelIndex index = m_JT1PicTable->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	std::map<QString,QPushButton*>::iterator iter = m_mapTypeButtons.begin();
	for (;iter != m_mapTypeButtons.end();++iter)
	{
		iter->second->setStyleSheet("background:transparent;border:5px solid green;");
	}

	//QString type = m_vCurTypeNamePic[row].first;
	QString type = 	modelsPicMap[row].second.front();
	QPushButton* bt = m_mapTypeButtons.find(type)->second;
	bt->setStyleSheet("background:transparent;border:5px solid red;");

	// 更新表格中label表示的模型类型
	QString strType = m_MapModelToType.find(type)->second;	
	m_curLabel->setText(QString::number(row+1));
	m_curLabel->SetPicTableRow(row+1);
	//m_curLabel->setText(strType);
	m_curLabel->SetName(type);
	m_curLabel->SetType(strType);

	emit SigChangeType(m_curLabel);
}

void xNormalUserWidget::WriteToSideFile(QString fileName,const std::vector<JM>& values)
{
	std::ofstream outFile;
	std::string str = std::string((const char *)fileName.toLocal8Bit().constData());

	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"Layer"<<','<<"ModelName"<<','<<"Type"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"Length"<<','<<"Wide"<<','<<"AoSide"<<','<<"IsAid"<<','<<"IsShow"<<','<<"Modifiable"<<std::endl;
	//写入数据-字符串
	for(size_t i = 0; i < values.size();++i)
	{	
		std::string sL = QString::number(values[i].iLayer).toStdString();
		std::string sName = string( values[i].strModelName.toLocal8Bit());      /* values[i].strModelName.toStdString();*/
		std::string sType = string( values[i].strType.toLocal8Bit());      /* values[i].strType.toStdString();*/
		std::string sUp = QString::number(values[i].fUpHeight).toStdString();
		std::string sDown = QString::number(values[i].fDownHeight).toStdString();
		std::string sLen = QString::number(values[i].fLength).toStdString();
		std::string sWide = QString::number(values[i].fWide).toStdString();
		std::string sAo = QString::number(values[i].iSide).toStdString();
		std::string sAid = QString::number(values[i].iAid).toStdString();
		std::string sShow = QString::number(values[i].iShow).toStdString();
		std::string sModify = QString::number(values[i].iModifiable).toStdString();
		outFile<<sL<<','<<sName<<','<<sType<<','<<sUp<<','<<sDown<<','<<sLen<<','<<sWide<<','<<sAo<<','<<sAid<<','<<sShow<<','<<sModify<<std::endl;

	}

	outFile.close();
}

void xNormalUserWidget::WriteToCenterFile(QString fileName,const std::vector<LMR>& values,int num)
{
	std::ofstream outFile;
	std::string str = std::string((const char *)fileName.toLocal8Bit().constData());
	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"Layer"<<','<<"ModelName"<<','<<"Type"<<','<<"LM_R"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"TuLen"<<','<<"TuSide"<<','<<"IsShow"<<','<<"Modifiable"<<std::endl;
	//写入数据-字符串
	for(size_t i = 0; i < values.size();++i)
	{	
		std::string sL = QString::number(values[i].iLayer).toStdString();
		std::string sName = string( values[i].strModelName.toLocal8Bit());   /* values[i].strModelName.toStdString();*/
		std::string sType = string( values[i].strType.toLocal8Bit());    /*values[i].strType.toStdString();*/
		std::string sR =  QString::number(values[i].fLmr).toStdString();
		std::string sUp = QString::number(values[i].fupLen).toStdString();
		std::string sDown = QString::number(values[i].fdownLen).toStdString();
		std::string sTuLen = QString::number(values[i].ftuLen).toStdString();
		std::string sTuSide = QString::number(values[i].iSide).toStdString();
		std::string sShow = QString::number(values[i].isShow).toStdString();
		std::string sModify = QString::number(values[i].iModifiable).toStdString();

		outFile<<sL<<','<<sName<<','<<sType<<','<<sR<<','<<sUp<<','<<sDown<<','<<sTuLen<<','<<sTuSide<<','<<sShow<<','<<sModify<<std::endl;
	}

	outFile.close();

}
