#include "xCollisionDetectionDlg.h"

#include <QTextCodec>
#include <QColorDialog>
#include <QMessageBox>
#include <vector>
#include <string>
#include <osg/Group>
#include <QAbstractItemView>
#include <QFrame>
#include "MainWindow.h"

const int MAX_ROWS = 35;

xCollisionDetectionDlg::xCollisionDetectionDlg(MainWindow *pParent)
{
	InitDlg();

	int column = m_table->columnCount();
	int row = m_table->rowCount();

	connect(m_pCancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
	connect(m_pOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_addModelBtn, SIGNAL(clicked()), this, SLOT(SlotAddModel()));
	connect(m_delModelBtn, SIGNAL(clicked()), this, SLOT(SlotDelModel()));
	connect(m_table,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectTableItems()));
	connect(m_ChooseAllCheckBox,SIGNAL(stateChanged(int)),this,SLOT(SlotChooseAllDisplayName()));
	//connect(m_table, SIGNAL(cellChanged(int row,int col)), this, SLOT(SlotsettingTableChanged(int row, int col)));
	m_MainWindow = pParent;

}

xCollisionDetectionDlg::~xCollisionDetectionDlg(void)
{
	m_IndexToCheckBox.clear();
}

void xCollisionDetectionDlg::SetTable(std::map<QString,std::vector<std::pair<QString,int>>> filevalues,
	                                  std::map<QString,QString> SymbolToCurLanguage,
									  std::map<std::pair<QString,QString>,int>* CollPair,
									  std::map<QString,int>* models)
{
	m_MapNameToPos.clear();
	m_table->DeleteAllValueRow();
	bool isChooseAll = true;
	//m_table->setStyle(QStyleFactory::create("Cleanlooks"));
	m_table->setRowCount(MAX_ROWS);

	m_FileValues = filevalues;
	m_pMapCollisionPair = CollPair;
	m_pMapCollisionPair->clear();

	m_pMapCollisionModel = models;
	m_pMapCollisionModel->clear();

	int iColumn = 0;
	std::map<QString,std::vector<std::pair<QString,int>>>::iterator it = m_FileValues.begin();
	for(;it!=m_FileValues.end();it++)
	{
		QString strNeedDisplayName;
		QString strtmpName = it->first;
	
		// 文件里的模型名
		QString strRawName = strtmpName;

		std::map<QString,QString>::iterator iter;
		iter = SymbolToCurLanguage.find(strtmpName);
		if (iter!=SymbolToCurLanguage.end())
		{
			strtmpName = iter->second;
		}
		//QStringList value;
		//value<<strtmpName
		//	<< strtmpName
		//	<< strtmpName;
		//m_table->addRowValueToCollideTable(18,value);
		QTableWidgetItem* item = new QTableWidgetItem();
		item->setText(strtmpName);
		m_table->setHorizontalHeaderItem(iColumn*2,item);

		//QTableWidgetItem *checkBox = new QTableWidgetItem();
		//checkBox->setStyle(QStyleFactory::create("Cleanlooks"));
		std::vector<std::pair<QString,int>>::iterator iter0 = it->second.begin();
		// int i=m_IndexToCheckBox.size();
		int i = 0;
		for (;iter0 != it->second.end();++iter0)
		{

			QCheckBox *checkBox = new QCheckBox(this);
			checkBox->setStyle(QStyleFactory::create("Cleanlooks"));
			if(iter0->second == 1)
			{
				checkBox->setCheckState(Qt::Checked);
				(*m_pMapCollisionPair)[std::make_pair(strRawName,iter0->first)] = 1;
				(*m_pMapCollisionModel)[strRawName] = 1;
				(*m_pMapCollisionModel)[iter0->first] = 1;

			}
			else if(iter0->second == 0)
			{
				checkBox->setCheckState(Qt::Unchecked);
				isChooseAll = false;
			}

			//QStringList value;
			//value << iter0->first;
			QTableWidgetItem* item1 = new QTableWidgetItem();
			item1->setText(iter0->first);
			m_table->setItem(i,iColumn*2,item1);

			m_table->setCellWidget(i,iColumn*2+1, checkBox);
			connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(SlotSelected(int)));

			m_MapNameToPos[std::make_pair(iColumn,i)] = iter0->first;
			m_IndexToCheckBox[std::make_pair(iColumn,i)]=checkBox;
			i++;
		}

		iColumn++;
	}

	m_ChooseAllCheckBox->setChecked(isChooseAll);
}

void xCollisionDetectionDlg::RefreshCollisionPair()
{
	m_pMapCollisionPair->clear();
	m_pMapCollisionModel->clear();

	std::map<QString,std::vector<std::pair<QString,int>>>::iterator it = m_FileValues.begin();
	for(;it!=m_FileValues.end();it++)
	{
		QString strtmpName = it->first;
	
		std::map<QString,QString>::iterator iter;
		std::vector<std::pair<QString,int>>::iterator iter0 = it->second.begin();
		int i = 0;
		for (;iter0 != it->second.end();++iter0)
		{
			if(iter0->second == 1)
			{
				(*m_pMapCollisionPair)[std::make_pair(strtmpName,iter0->first)] = 1;

				(*m_pMapCollisionModel)[strtmpName] = 1;
				(*m_pMapCollisionModel)[iter0->first] = 1;
			}
		}
	}
}

void xCollisionDetectionDlg::InitDlg()
{
	QString str;
	//this->resize(1000,600);
	str = str.fromLocal8Bit("设置干涉检查对象");
	//this->setWindowTitle(str);
	str = str.fromLocal8Bit("OK");
	m_pOk = new QPushButton(str);
	str = str.fromLocal8Bit("Cancel");
	m_pCancel = new QPushButton(str);

	str = str.fromLocal8Bit("模型:");
	m_pModelNameLabel = new QLabel(str);
	
	m_pModelComBox = new QComboBox;
	str = str.fromLocal8Bit("--请选择--");
	m_pModelComBox->addItem(str);

	str = str.fromLocal8Bit("增加");
	m_addModelBtn = new QPushButton(str);
	str = str.fromLocal8Bit("删除");
	m_delModelBtn = new QPushButton(str);

	//m_pCollideItemsTable = new QTableWidget(0,1);
	//m_pCollideItemsTable->horizontalHeader()->hide();
	//m_pCollideItemsTable->setFrameShape(QFrame::NoFrame);      //设置无边框
	//m_pCollideItemsTable->setShowGrid(false);                //设置不显示格子线
	//m_pCollideItemsTable->verticalHeader()->setVisible(false); //设置垂直头不可见
	//m_pCollideItemsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
	//m_pCollideItemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
	//m_pCollideItemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
	//m_pCollideItemsTable->setStyleSheet("selection-background-color:lightblue;");  //设置选中背景色
	//m_pCollideItemsTable->setColumnWidth(0,400);
	//m_pCollideItemsTable->verticalHeader()->setDefaultSectionSize(17);    //设置默认行高

	m_table = new Table;
	QStringList header;
	header <<QString::fromLocal8Bit("模型名1")
		   <<QString::fromLocal8Bit("选择")
	       <<QString::fromLocal8Bit("模型名2")
		   <<QString::fromLocal8Bit("选择")
		   <<QString::fromLocal8Bit("模型名3")
		   <<QString::fromLocal8Bit("选择")
		   <<QString::fromLocal8Bit("模型名4")
		   <<QString::fromLocal8Bit("选择");

	m_table->setColumnValue(8, header);

	m_table->setHeaderWidth(0, 400);
	m_table->setHeaderWidth(1, 70);
	m_table->setHeaderWidth(2, 400);
	m_table->setHeaderWidth(3, 70);
	m_table->setHeaderWidth(4, 400);
	m_table->setHeaderWidth(5, 70);
	m_table->setHeaderWidth(6, 400);
	m_table->setHeaderWidth(7, 70);
	
	m_table->setHeaderHeight(20);
	m_table->setSortingEnabled(false);
	m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_table->setFont(QFont("Microsoft YaHe", 10));

	m_table->setRowCount(MAX_ROWS);
	//QStringList value;
	//value << "a"<<" a" << "a" <<"a" <<"a" << "a" <<"a" <<"a";
	// 初始化最大35行表格
	//for (int i = 0; i < MAX_ROWS;++i)
	//{
	//	m_table->addRowValue(18,value);
	//}

	m_pAnimationSpeedComBox = new QComboBox;
	m_pAnimationSpeedComBox->setMaxVisibleItems(5);
	m_pAnimationSpeedComBox->setView(new QListView());
	m_pAnimationSpeedComBox->setStyleSheet("QComboBox QAbstractItemView::item { min-height: 18px;}");
	str = str.fromLocal8Bit("xSpeed");
	m_pSpeedLabel = new QLabel(str);
	str = str.fromLocal8Bit("1");
	m_pAnimationSpeedComBox->addItem(str);
	str = str.fromLocal8Bit("2");
	m_pAnimationSpeedComBox->addItem(str);
	str = str.fromLocal8Bit("3");
	m_pAnimationSpeedComBox->addItem(str);
	str = str.fromLocal8Bit("4");
	m_pAnimationSpeedComBox->addItem(str);
	str = str.fromLocal8Bit("5");
	m_pAnimationSpeedComBox->addItem(str);
	m_pAnimationSpeedComBox->setCurrentIndex(4);
	m_fAnimationSpeed = 5.0;

	str = str.fromLocal8Bit("全选");
	m_ChooseAllLabel = new QLabel(str);
	m_ChooseAllCheckBox = new QCheckBox;
	m_ChooseAllCheckBox->setStyle(QStyleFactory::create("Cleanlooks"));

	QHBoxLayout *hChooseAllLayout = new QHBoxLayout;
	hChooseAllLayout->addWidget(m_ChooseAllLabel);
	hChooseAllLayout->addWidget(m_ChooseAllCheckBox);
	m_ChooseAllCheckBox->setChecked(false);

	str = str.fromLocal8Bit("干涉");
	m_IsDetectedLabel = new QLabel(str);
	m_IsDetectedCheckBox = new QCheckBox;
	m_IsDetectedCheckBox->setStyle(QStyleFactory::create("Cleanlooks"));

	QHBoxLayout *hIsDetectedLayout = new QHBoxLayout;
	hIsDetectedLayout->addWidget(m_IsDetectedLabel);
	hIsDetectedLayout->addWidget(m_IsDetectedCheckBox);
	m_IsDetectedCheckBox->setChecked(false);

	QGridLayout *pGLayout = new QGridLayout();
	//pGLayout->addWidget(m_pModelNameLabel,0,0);
	//pGLayout->addWidget(m_pModelComBox,0,1);
	//pGLayout->addWidget(m_addModelBtn,0,2);
	//pGLayout->addWidget(m_delModelBtn,0,3);
	pGLayout->addWidget(m_table,0,0,4,4);
	//pGLayout->addWidget(m_pSpeedLabel,4,0);
	//pGLayout->addWidget(m_pAnimationSpeedComBox,4,1);
	pGLayout->addLayout(hChooseAllLayout,4,2);
	pGLayout->addLayout(hIsDetectedLayout,4,3);
	pGLayout->addWidget(m_pOk,5,3);

	this->setLayout(pGLayout);
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
}

bool xCollisionDetectionDlg::GetIsDetected()
{
	bool is;
	is = m_IsDetectedCheckBox->isChecked();
	return is;
}

void xCollisionDetectionDlg::ReBuildCollisionDetectionFile()
{
	char szUTF_8BOM[4] = {char(0xEF), char(0xBB), char(0xBF), char(0)};
	//locale &loc=locale::global(locale(locale(),"",LC_CTYPE));  // ②
	//	setlocale(LC_CTYPE, ""); 
	char collision[256] = "";
	m_MainWindow->GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ANIMATION_FILES_PATH, 3, collision);
	m_MainWindow->GetWholePath(collision);
	std::string tmp = collision;
	QString collisionPath = QString::fromLocal8Bit(tmp.c_str());
	collisionPath = QDir::fromNativeSeparators(collisionPath);//"\"转"/"
	std::ofstream outFile;
	QString file = collisionPath + QString::fromLocal8Bit("/CollisionDetection.csv");

	std::string str = std::string((const char *)file.toLocal8Bit().constData());

	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
//	outFile << szUTF_8BOM;
	outFile<<"ModelA"<<','<<"ModelB"<<std::endl;
	//写入数据-字符串
	std::map<QString,std::vector<std::pair<QString,int>>>::iterator it = m_FileValues.begin();
	for(;it!=m_FileValues.end();it++)
	{	
		std::string sr = it->first.toStdString();
		outFile<<sr;
		//QString str = (*it).first;
		//QByteArray cdata = str.toLocal8Bit();  
		//std::string sup = string((*it).first.toLocal8Bit());
		//outFile<<sr<<','<<sup<<',';

		outFile<<",";

		std::vector<std::pair<QString,int>>::iterator iter = it->second.begin();
		for (; iter != it->second.end();++iter)
		{
			outFile << "(";
			std::string sname = string(iter->first.toLocal8Bit());
			sname.append("?");
			// outFile << sname;
			//outFile << ","";
			std::string number = string(QString::number(iter->second).toLocal8Bit());
			sname.append(number);
			outFile << sname;
			outFile << ")";
		}

		outFile<<"\""<<std::endl;	
	}

	outFile.close();
}

void xCollisionDetectionDlg::InitAllChildrenCombox(std::vector<QString> strAllName)
{
	//清空
	int count = m_pModelComBox->count();
	for(int i=count-1;i>0;i--)
	{
		m_pModelComBox->removeItem(i);
	}

	//
	for (size_t j=0;j<strAllName.size();j++)
	{
		m_pModelComBox->addItem(strAllName[j]);
	}

	m_pModelComBox->setCurrentIndex(0);
}

void xCollisionDetectionDlg::ReSetTable()
{
	m_MapNameToPos.clear();
	m_table->DeleteAllValueRow();
	bool isChooseAll = true;
	//m_table->setStyle(QStyleFactory::create("Cleanlooks"));
	m_table->setRowCount(MAX_ROWS);

	int iColumn = 0;
	std::map<QString,std::vector<std::pair<QString,int>>>::iterator it = m_FileValues.begin();
	for(;it!=m_FileValues.end();it++)
	{
		QString strNeedDisplayName;
		QString strtmpName = it->first;
	
		//std::map<QString,QString>::iterator iter;
		//iter = SymbolToCurLanguage.find(strtmpName);
		//if (iter!=SymbolToCurLanguage.end())
		//{
		//	strtmpName = iter->second;
		//}
		//QStringList value;
		//value<<strtmpName
		//	<< strtmpName
		//	<< strtmpName;
		//m_table->addRowValueToCollideTable(18,value);
		QTableWidgetItem* item = new QTableWidgetItem();
		item->setText(strtmpName);
		m_table->setHorizontalHeaderItem(iColumn*2,item);

		//QTableWidgetItem *checkBox = new QTableWidgetItem();
		//checkBox->setStyle(QStyleFactory::create("Cleanlooks"));
		std::vector<std::pair<QString,int>>::iterator iter0 = it->second.begin();
		// int i=m_IndexToCheckBox.size();
		int i = 0;
		for (;iter0 != it->second.end();++iter0)
		{

			QCheckBox *checkBox = new QCheckBox(this);
			checkBox->setStyle(QStyleFactory::create("Cleanlooks"));
			if(iter0->second == 1)
			{
				checkBox->setCheckState(Qt::Checked);
			}
			else if(iter0->second == 0)
			{
				checkBox->setCheckState(Qt::Unchecked);
				isChooseAll = false;
			}

			//QStringList value;
			//value << iter0->first;
			QTableWidgetItem* item1 = new QTableWidgetItem();
			item1->setText(iter0->first);
			m_table->setItem(i,iColumn*2,item1);

			m_table->setCellWidget(i,iColumn*2+1, checkBox);
			connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(SlotSelected(int)));

			m_MapNameToPos[std::make_pair(iColumn,i)] = iter0->first;
			m_IndexToCheckBox[std::make_pair(iColumn,i)]=checkBox;
			i++;
		}

		iColumn++;
	}

	m_ChooseAllCheckBox->setChecked(isChooseAll);
}

void xCollisionDetectionDlg::SetCollisionDlgLanguage(std::vector<QString> vCurWords)
{
	//m_pModelComBox->setItemText(0,vCurWords[73]);

	//m_pModelNameLabel->setText(vCurWords[14]);
	//m_addModelBtn->setText(vCurWords[23]);
	//m_delModelBtn->setText(vCurWords[24]);
	//QStringList header;
	//header<< vCurWords[14]
	//	  << vCurWords[15]
	//	  << vCurWords[14]
	//	  << vCurWords[15]
	//	  << vCurWords[14]
	//	  << vCurWords[15]
	//	  << vCurWords[14]
	//	  << vCurWords[15];

	//m_table->setColumnValue(10, header);
	//m_table->setFont(QFont("Microsoft YaHe", 10));
	m_pSpeedLabel->setText(vCurWords[16]);
	m_ChooseAllLabel->setText(vCurWords[17]);
	m_IsDetectedLabel->setText(vCurWords[18]);
	m_pOk->setText(vCurWords[19]);
	m_vCurLanguageWords = vCurWords;
}

void xCollisionDetectionDlg::SlotOk()
{
	m_vDetectiveItemName.clear();

	QString str;
	m_fAnimationSpeed = m_pAnimationSpeedComBox->currentIndex()+1.0;

	std::map<std::pair<int,int>,QCheckBox*>::iterator iterBox = m_IndexToCheckBox.begin();
	std::map<std::pair<int,int>,QString>::iterator iterStr = m_MapNameToPos.begin();
	for(;iterBox != m_IndexToCheckBox.end();++iterBox,++iterStr)
	{		
		QString strName = iterStr->second/*m_table ->item(i, 0)->text()*/;
		int iselected;

		if(iterBox->second->checkState() == Qt::Checked)
		{
			iselected = 1;
		}
		else if(iterBox->second->checkState() == Qt::Unchecked)
		{
			iselected = 0;
		}
		
		std::map<QString,std::vector<std::pair<QString,int>>>::iterator it = m_FileValues.begin();
		for (int i = 0; i <iterBox->first.first;++i)
		{
			++it;
		}
		std::vector<std::pair<QString,int>>::iterator iter = it->second.begin();
		for (;iter != it->second.end();++iter)
		{
			if (strName==iter->first)
			{
				iter->second = iselected;
			}
		}
	}

	ReBuildCollisionDetectionFile();

	// 刷新碰撞检查检测对
	RefreshCollisionPair();

	emit SigSetDetectedFinished();
	this->close();
}

void xCollisionDetectionDlg::SlotCancel()
{
	this->close();
}

void xCollisionDetectionDlg::SlotAddModel()
{
	if(m_pModelComBox->currentIndex()>0)
	{
		m_vCurCollideItemsName.push_back(m_pModelComBox->currentText());
	}

	ReSetTable();
}

void xCollisionDetectionDlg::SlotDelModel()
{
	if(m_pModelComBox->currentIndex()>0)
	{
		std::vector<QString>::iterator it = m_vCurCollideItemsName.begin();
		for(;it!=m_vCurCollideItemsName.end();)
		{
			if((*it) == m_pModelComBox->currentText())
			{
				 it =  m_vCurCollideItemsName.erase(it);
				//vec.erase(remove(vec.begin(), vec.end(), 3), vec.end());
				
			}
			else
			{
				it++;
			}
	
		}
	}

	ReSetTable();
}

void xCollisionDetectionDlg::SlotSelectTableItems()
{
	std::vector<QString> vDisplayName;
	std::vector<QString> vmodelName;
	vDisplayName.clear();
	vmodelName.clear();

	QList<QTableWidgetItem*> items = m_table->selectedItems();
	int count = items.count();
	for(int i = 0; i < count; i++)
	{
		
		QTableWidgetItem *item = items.at(i);

		QString text = m_MapNameToPos[std::make_pair(item->column(),item->row())]/*item->text()*/; //获取内容
		vDisplayName.push_back(text);
	}

	for (size_t i=0;i<vDisplayName.size();i++)
	{
		std::map<QString,std::vector<std::pair<QString,int>>>::iterator it = m_FileValues.begin();
		for(;it!=m_FileValues.end();it++)
		{
			if (vDisplayName[i] == it->first)
			{
				std::vector<std::pair<QString,int>>::iterator itt = it->second.begin();
				for (;itt!=it->second.end();itt++)
				{
					vmodelName.push_back(itt->first);
				}
			}
		}
	}

	m_MainWindow->ChangeMutiModelColor(vmodelName);

	////设置是否全选
	//bool isChooseAll = true;
	//for(int i=0;i<m_table->rowCount();i++)
	//{
	//	if (m_table->item(i,1)->checkState()== Qt::Unchecked)
	//	{
	//		isChooseAll = false;
	//	}
	//}
	//m_ChooseAllCheckBox->setChecked(isChooseAll);
}

void xCollisionDetectionDlg::SlotChooseAllDisplayName()
{
	if (m_ChooseAllCheckBox->isChecked() == true)
	{
		std::map<std::pair<int,int>,QCheckBox*>::iterator iter = m_IndexToCheckBox.begin();
		for(; iter != m_IndexToCheckBox.end();++iter)
		{
			//m_table->item(i,1)->setCheckState(Qt::Checked);
			iter->second->setCheckState(Qt::Checked);
		}
	}
	else
	{
		std::map<std::pair<int,int>,QCheckBox*>::iterator iter = m_IndexToCheckBox.begin();
		for(; iter != m_IndexToCheckBox.end();++iter)
		{
			//m_table->item(i,1)->setCheckState(Qt::Checked);
			iter->second->setCheckState(Qt::Unchecked);
		}
	}
}

void xCollisionDetectionDlg::SlotsettingTableChanged(int,int)
{
	int count = m_table->rowCount();
	bool isAll = true;
	for(int i=0;i<count;i++)
	{		
		if(m_table->item(i,1)->checkState() == Qt::Unchecked)
		{
			isAll = false;
			break;
		}
	}

	if (isAll == true)
	{
		m_ChooseAllCheckBox->setChecked(true);		
		SlotChooseAllDisplayName();
	}
	else if (isAll == false)
	{
		m_ChooseAllCheckBox->setChecked(false);
		SlotChooseAllDisplayName();
	}
}

void xCollisionDetectionDlg::SlotSelected(int selected)
{
	int count = m_table->rowCount();
	bool isAll = true;

	std::map<std::pair<int,int>,QCheckBox*>::iterator iter = m_IndexToCheckBox.begin();
	for(; iter != m_IndexToCheckBox.end();++iter)
	{
		if(iter->second->checkState() == Qt::Unchecked)
		{
			isAll = false;
			break;
		}
	}

	if (isAll == true)
	{
		disconnect(m_ChooseAllCheckBox,SIGNAL(stateChanged(int)),this,SLOT(SlotChooseAllDisplayName()));
		m_ChooseAllCheckBox->setChecked(true);		
		//SlotChooseAllDisplayName();
	}
	else if (isAll == false)
	{
		disconnect(m_ChooseAllCheckBox,SIGNAL(stateChanged(int)),this,SLOT(SlotChooseAllDisplayName()));
		m_ChooseAllCheckBox->setChecked(false);
		//SlotChooseAllDisplayName();
	}
	connect(m_ChooseAllCheckBox,SIGNAL(stateChanged(int)),this,SLOT(SlotChooseAllDisplayName()));
}

void xCollisionDetectionDlg::closeEvent(QCloseEvent *event)
{
	m_MainWindow->ResumeAllModelColor();
}

void xCollisionDetectionDlg::SlotSetAllCheckbox(int)
{
}
