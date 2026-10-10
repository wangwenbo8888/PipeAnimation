#include "xModifyLMTable.h"
#include "MainWindow.h"


xModifyLMTable::xModifyLMTable(MainWindow *pParent/*=NULL*/)
{
	m_MainWindow = pParent;
	m_table = new Table;
#if 1


	QStringList header;
	header<<QString::fromLocal8Bit("模型名")
		<<QString::fromLocal8Bit("轮模半径")
		<<QString::fromLocal8Bit("UpHeight")
		<<QString::fromLocal8Bit("DownHeight")
		<<QString::fromLocal8Bit("TuLen")
		<<QString::fromLocal8Bit("显示");
	m_table->setColumnValue(6, header);

	m_table->setHeaderWidth(0, 90);
	m_table->setHeaderWidth(1, 72);
	m_table->setHeaderWidth(2, 72);
	m_table->setHeaderWidth(3, 72);
	m_table->setHeaderWidth(4, 72);
	m_table->setHeaderWidth(5, 15);

	m_table->setHeaderHeight(20);
	m_table->setSortingEnabled(false);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->setFont(QFont("Microsoft YaHe", 10));

	m_JmTable = new Table;
	QStringList header2;
	header2<<QString::fromLocal8Bit("模型名")
		<<QString::fromLocal8Bit("UpHeight")
		<<QString::fromLocal8Bit("DownHeight")
		<<QString::fromLocal8Bit("Length")
		<<QString::fromLocal8Bit("Wide")
		<<QString::fromLocal8Bit("显示");
	m_JmTable->setColumnValue(6, header2);

	m_JmTable->setHeaderWidth(0, 90);
	m_JmTable->setHeaderWidth(1, 72);
	m_JmTable->setHeaderWidth(2, 72);
	m_JmTable->setHeaderWidth(3, 72);
	m_JmTable->setHeaderWidth(4, 72);
	m_JmTable->setHeaderWidth(5, 15);

	m_JmTable->setHeaderHeight(20);
	m_JmTable->setSortingEnabled(false);
	m_JmTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_JmTable->setFont(QFont("Microsoft YaHe", 10));


#endif


	//m_pMainTable = new Table;
	//QStringList header1;
	//header1<<QString::fromLocal8Bit("层数")
	//	<<QString::fromLocal8Bit("左导夹")
	//	<<QString::fromLocal8Bit("左主夹")
	//	<<QString::fromLocal8Bit("轮模")
	//	<<QString::fromLocal8Bit("右导夹")
	//	<<QString::fromLocal8Bit("右主夹")
	//	<<QString::fromLocal8Bit("显示");
	//m_pMainTable->setColumnValue(7, header1);
	//m_pMainTable->setHeaderWidth(0, 15);
	//m_pMainTable->setHeaderWidth(1, 90);
	//m_pMainTable->setHeaderWidth(2, 90);
	//m_pMainTable->setHeaderWidth(3, 90);
	//m_pMainTable->setHeaderWidth(4, 90);
	//m_pMainTable->setHeaderWidth(5, 90);
	//m_pMainTable->setHeaderWidth(6, 15);
	//m_pMainTable->setHeaderHeight(20);
	//m_pMainTable->setSortingEnabled(false);
	//m_pMainTable->setSelectionMode(QAbstractItemView::SingleSelection);
	//m_pMainTable->setFont(QFont("Microsoft YaHe", 10));

	//m_pSubTable = new Table;
	//QStringList header1;
	//header1<<QString::fromLocal8Bit("层数")
	//	<<QString::fromLocal8Bit("左导夹")
	//	<<QString::fromLocal8Bit("左主夹")
	//	<<QString::fromLocal8Bit("轮模")
	//	<<QString::fromLocal8Bit("右导夹")
	//	<<QString::fromLocal8Bit("右主夹")
	//	<<QString::fromLocal8Bit("显示");
	//m_pSubTable->setColumnValue(7, header1);
	//m_pSubTable->setHeaderWidth(0, 15);
	//m_pSubTable->setHeaderWidth(1, 90);
	//m_pSubTable->setHeaderWidth(2, 90);
	//m_pSubTable->setHeaderWidth(3, 90);
	//m_pSubTable->setHeaderWidth(4, 90);
	//m_pSubTable->setHeaderWidth(5, 90);
	//m_pSubTable->setHeaderWidth(6, 15);
	//m_pSubTable->setHeaderHeight(20);
	//m_pSubTable->setSortingEnabled(false);
	//m_pSubTable->setSelectionMode(QAbstractItemView::SingleSelection);
	//m_pSubTable->setFont(QFont("Microsoft YaHe", 10));





	QString str;
	str = str.fromLocal8Bit("确定");
	m_okBtn = new QPushButton(str);

	QVBoxLayout* layout = new QVBoxLayout;
	/*layout->addWidget(m_pMainTable);
	layout->addWidget(m_pSubTable);*/
	layout->addWidget(m_table);
	layout->addWidget(m_okBtn);

	this->setLayout(layout);
	this->resize(460,350);



	//m_table->setEditTriggers(QAbstractItemView::CurrentChanged);


	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置



	connect(m_JmTable,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectJMTableItems()));
	connect(m_table,SIGNAL(itemSelectionChanged()),this,SLOT(SlotSelectTableItems()));
	//connect(m_table, SIGNAL(cellChanged(int,int)), this, SLOT(SlotIsShow(int, int)));
	connect(m_okBtn, SIGNAL(clicked()), this, SLOT(SlotOk()));


}

xModifyLMTable::~xModifyLMTable()
{

}

std::map<QString,int> xModifyLMTable::GetModelIsShow()
{
	return m_MapModelNameToIsShow;
}

std::vector<LMR> xModifyLMTable::GetCurTableLMValues()
{
	return m_CurTableLmValues;
}

void xModifyLMTable::InitDlg(std::vector<LMR> values)
{
	m_table->DeleteAllValueRow();

	for (size_t i=0;i<values.size();i++)
	{
		m_table->addRowValueToLMTable(18,QStringList()<<values[i].strModelName<<QString::number(values[i].fLmr)<<QString::number(values[i].fupLen)<<QString::number(values[i].fdownLen)<<QString::number(values[i].ftuLen));
		QTableWidgetItem *checkBox = new QTableWidgetItem();
		if(values[i].isShow == 1)
		{
			checkBox->setCheckState(Qt::Checked);

		}
		else
		{
			checkBox->setCheckState(Qt::Unchecked);

		}
		m_table->setItem(i, 5, checkBox);

	}


	m_table->blockSignals(false);

	m_table->setEditTriggers(QAbstractItemView::DoubleClicked);





}

std::vector<JM> xModifyLMTable::GetCurTableJMValues()
{
	return m_CurTableJmValues;
}

void xModifyLMTable::InitJMTable(std::vector<JM> values)
{
	m_JmTable->DeleteAllValueRow();

	for (size_t i=0;i<values.size();i++)
	{
		m_JmTable->addRowValueToLMTable(18,QStringList()<<values[i].strModelName<<QString::number(values[i].fUpHeight)<<QString::number(values[i].fDownHeight)<<QString::number(values[i].fLength)<<QString::number(values[i].fWide));
		QTableWidgetItem *checkBox = new QTableWidgetItem();
		if(values[i].iShow == 1)
		{
			checkBox->setCheckState(Qt::Checked);

		}
		else
		{
			checkBox->setCheckState(Qt::Unchecked);

		}
		m_JmTable->setItem(i, 5, checkBox);

	}


	m_JmTable->blockSignals(false);

	m_JmTable->setEditTriggers(QAbstractItemView::DoubleClicked);
}

void xModifyLMTable::SlotIsShow(int irow, int icoloum)
{
	m_MapModelNameToIsShow.erase(m_MapModelNameToIsShow.begin(),m_MapModelNameToIsShow.end());
	m_CurTableLmValues.clear();

	if(icoloum == 5 )
	{
		if(m_table ->item(irow, icoloum)->checkState() == Qt::Checked) //选中
		{
			QString strModelName = m_table ->item(irow, 0)->text();
			m_MapModelNameToIsShow[strModelName] = 1;
		}
		else
		{
			QString strModelName = m_table ->item(irow, 0)->text();
			m_MapModelNameToIsShow[strModelName] = 0;
		}
		emit SigLMTableModelIsShow();
	}
	else
	{
		for(int i=0;i<m_table->rowCount();i++)
		{
			LMR tmp;
			tmp.strModelName = m_table ->item(i, 0)->text();
			tmp.fLmr = m_table->item(i,1)->text().toFloat();
			tmp.fupLen = m_table->item(i,2)->text().toFloat();
			tmp.fdownLen = m_table->item(i,3)->text().toFloat();
			tmp.ftuLen = m_table->item(i,4)->text().toFloat();
			tmp.isShow = m_table->item(i,5)->text().toInt();
			tmp.strType = m_table->item(i,7)->text();

			m_CurTableLmValues.push_back(tmp);
		}
		emit SigLMTableCurValues();
	}
	

}

void xModifyLMTable::SlotSelectTableItems()
{
	std::vector<QString> vmodelName;
	vmodelName.clear();
	QList<QTableWidgetItem*> items = m_table->selectedItems();
	int count = items.count();
	for(int i = 0; i < count/6; i++)
	{
		//int row = m_table->row(items.at(i));
		//QTableWidgetItem* item = ui->tableWidget_right->item(row,column);
		QTableWidgetItem *item = items.at(i*6);
		//item->
		QString text = item->text(); //获取内容
		
		vmodelName.push_back(text);
	}
	m_vCurSelectItemNames = vmodelName;
	m_MainWindow->ChangeMutiModelColor(vmodelName);
}

void xModifyLMTable::SlotSelectJMTableItems()
{
	std::vector<QString> vmodelName;
	vmodelName.clear();
	QList<QTableWidgetItem*> items = m_JmTable->selectedItems();
	int count = items.count();
	for(int i = 0; i < count/6; i++)
	{
		//int row = m_table->row(items.at(i));
		//QTableWidgetItem* item = ui->tableWidget_right->item(row,column);
		QTableWidgetItem *item = items.at(i*6);
		//item->
		QString text = item->text(); //获取内容

		vmodelName.push_back(text);
	}
	m_vCurSelectJMItemNames = vmodelName;
	m_MainWindow->ChangeMutiModelColor(vmodelName);
}

void xModifyLMTable::SlotOk()
{
	m_CurTableLmValues.clear();
	m_CurTableJmValues.clear();
		
	for(int i=0;i<m_table->rowCount();i++)
	{
		LMR tmp;
		tmp.strModelName = m_table ->item(i, 0)->text();
		tmp.fLmr = m_table->item(i,1)->text().toFloat();
		tmp.fupLen = m_table->item(i,2)->text().toFloat();
		tmp.fdownLen = m_table->item(i,3)->text().toFloat();
		tmp.ftuLen = m_table->item(i,4)->text().toFloat();
		tmp.strType = m_table->item(i,7)->text();
		
		int isshow;
		if(m_table->item(i,5)->checkState() == Qt::Checked)
		{
			isshow = 1;
		}
		else if(m_table->item(i,5)->checkState() == Qt::Unchecked)
		{
			isshow = 0;
		}
		tmp.isShow = isshow;
		m_CurTableLmValues.push_back(tmp);
	}

	if (m_vCurSelectItemNames.size()>0)
	{
		m_MainWindow->ChangeMutiModelColor(m_vCurSelectItemNames);
	}

	for(int j=0;j<m_JmTable->rowCount();j++)
	{
		JM tmp2;
		tmp2.strModelName = m_JmTable ->item(j, 0)->text();
		tmp2.fUpHeight = m_JmTable->item(j,1)->text().toFloat();
		tmp2.fDownHeight = m_JmTable->item(j,2)->text().toFloat();
		tmp2.fLength = m_JmTable->item(j,3)->text().toFloat();
		tmp2.fWide = m_JmTable->item(j,4)->text().toFloat();
		tmp2.strType = m_JmTable->item(j,7)->text();

		int isshow;
		if(m_JmTable->item(j,5)->checkState() == Qt::Checked)
		{
			isshow = 1;
		}
		else if(m_JmTable->item(j,5)->checkState() == Qt::Unchecked)
		{
			isshow = 0;
		}
		tmp2.iShow = isshow;
		m_CurTableJmValues.push_back(tmp2);
	}

	if (m_vCurSelectJMItemNames.size()>0)
	{
		m_MainWindow->ChangeMutiModelColor(m_vCurSelectJMItemNames);
	}

	emit SigSetLMTableFinished();
}

