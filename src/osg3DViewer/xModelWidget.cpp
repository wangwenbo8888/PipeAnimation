
#include "xModelWidget.h"

#include "MainWindow.h"
#include "xNormalUserWidget.h"

xModelWidget::xModelWidget(xNormalUserWidget *p)
{
	m_Widget = new QTableWidget(this);

	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置

	//setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
	//hide();

	//this->setParent(p);

}


xModelWidget::~xModelWidget()
{

}


void xModelWidget::SetMainWindow(MainWindow *p)
{
	m_pMainWindow = p;
}

std::map<QString,QString> xModelWidget::GetModelToType()
{
	return m_MapModelToType;
}

void xModelWidget::LoadModelPicture()
{
}


void xModelWidget::SetFilesDir(QString dir)
{
	m_sDir = dir;
}

void xModelWidget::ReadModelPictureNameMap()
{
	if (m_sDir.isEmpty())
	{
		return;
	}

	QString strFileName = m_sDir + QString::fromLocal8Bit("/ModelPictures/TypePictureNameMap.csv");	
	m_vTypeName = m_pMainWindow->ReadTypeNameMap(strFileName);
	m_MapModelToType = m_pMainWindow->GetModelToType();

#if 0
	m_Widget->resize(800,800);

	int size = m_vTypeName.size();
	int mod = size%4;
	int rows;
	if (mod==0)
	{
	    rows = size/4; 
	}
	else
	{
		rows = size/4+1;
	}

	m_Widget->setColumnCount(4);
	m_Widget->setRowCount(rows);
	// 四个一排
	for (int i = 0; i < size;++i)
	{
		int m = i/4;
		int n = i%4;
		QPushButton* button = new QPushButton(this);
		QString picture = m_sDir;
		picture.append("/ModelPictures/");
		picture.append(m_vTypeName[i].second);
		picture.append(".png");

		QPixmap image(picture);
		button->setIcon(QIcon(image));
		//button->setIconSize(QSize(200,200));
		m_Widget->setCellWidget(m,n,button);
		button->setStyleSheet("background:transparent;border:2px solid green;");

		connect(button,SIGNAL(clicked()),this,SLOT(Slot_Type_Selected()));

		m_mapTypeButtons[m_vTypeName[i].first] = button;
	}
#endif
}

// 设置当前操作对应表格中的label
void xModelWidget::setLabel(xMyLabel* label)
{
	QString strname;
	m_curLabel = label;
	//QString type = m_curLabel->GetType();
	//std::map<QString,QString>::iterator _it = m_MapModelToType.begin();
	//for (;_it!=m_MapModelToType.end();_it++)
	//{
	//	if (_it->second == type)
	//	{
	//		strname = _it->first;
	//	}
	//}
	strname = m_curLabel->GetName();
	std::map<QString,QPushButton*>::iterator iter = m_mapTypeButtons.begin();
	for (;iter != m_mapTypeButtons.end();++iter)
	{
		if (strname==iter->first)
		{
			iter->second->setStyleSheet("background:transparent;border:2px solid red;");
		}
		else
		{
			iter->second->setStyleSheet("background:transparent;border:2px solid green;");
		}
	}
}


void xModelWidget::RefreshTypeToBtnMap(std::vector<QString> ModelNames)
{
	//if (m_sDir.isEmpty())
	//{
	//	return;
	//}

	//QString strFileName = m_sDir + QString::fromLocal8Bit("/ModelPictures/TypePictureNameMap.csv");	
	//m_vTypeName = m_pMainWindow->ReadTypeNameMap(strFileName);
	m_vCurTypeNamePic = GetCurTypeNamePic(ModelNames,m_vTypeName);

	DeleteAllItems();
	m_mapTypeButtons.clear();
	m_Widget->resize(265,600);

	int size = ModelNames.size();
	/*int mod = size%4;
	int rows;
	if (mod==0)
	{
	rows = size/4; 
	}
	else
	{
	rows = size/4+1;
	}*/

	m_Widget->setColumnCount(1);
	m_Widget->setRowCount(size);
	// 四个一排
	for (int i = 0; i < size;++i)
	{
		//int m = i/4;
		//int n = i%4;
		QPushButton* button = new QPushButton(this);
		QString picture = m_sDir;
		picture.append("/ModelPictures/");
		picture.append(m_vCurTypeNamePic[i].second);
		picture.append(".png");

		QPixmap image(picture);
		button->setIcon(QIcon(image));
		button->setIconSize(QSize(230,150));
		m_Widget->setRowHeight(i,155);
		m_Widget->setColumnWidth(0,235);

		m_Widget->setCellWidget(i,0,button);
		button->setStyleSheet("background:transparent;border:2px solid green;");

		connect(button,SIGNAL(clicked()),this,SLOT(Slot_Type_Selected()));

		m_mapTypeButtons[m_vCurTypeNamePic[i].first] = button;
	}

}

void xModelWidget::DeleteAllItems()
{
	int a = m_Widget->rowCount();
	if(a==0)
		return;
	else
	{

		m_Widget->clearContents();
		for (int i = m_Widget->rowCount(); i > 0; i--)
		{
			m_Widget->removeRow(0);
		}
	}
}

std::vector<std::pair<QString,QString>> xModelWidget::GetCurTypeNamePic(std::vector<QString> ModelNames,std::vector<std::pair<QString,QString>> vTypeName)
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

void xModelWidget::Slot_Type_Selected()
{
	QPushButton* button = dynamic_cast<QPushButton*>(this->sender());
	if(NULL == button)
	{
		return;
	}

	int x = button->frameGeometry().x();
	int y = button->frameGeometry().y();
	QModelIndex index = m_Widget->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	std::map<QString,QPushButton*>::iterator iter = m_mapTypeButtons.begin();
	for (;iter != m_mapTypeButtons.end();++iter)
	{
		iter->second->setStyleSheet("background:transparent;border:2px solid green;");
	}

	QString type = m_vCurTypeNamePic[row].first;
	QPushButton* bt = m_mapTypeButtons.find(type)->second;
	bt->setStyleSheet("background:transparent;border:2px solid red;");

	// 更新表格中label表示的模型类型
	QString strType = m_MapModelToType.find(type)->second;	
	m_curLabel->setText(strType);
	m_curLabel->SetName(type);
	m_curLabel->SetType(strType);
	
	emit SigChangeType(m_curLabel);
}