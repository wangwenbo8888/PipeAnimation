#include "xSettingNode.h"

#include <QTextCodec>
#include <QColorDialog>
#include <QMessageBox>
#include <vector>
#include <string>
#include <osg/Group>
#include <QAbstractItemView>

#include "log.h"

xSettingNodeDlg::xSettingNodeDlg(MainWindow *mw)
{
	m_pMainWin = mw;

	InitDialog();
	connect(m_pCancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
	connect(m_pOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(m_pAddNodeBtn,SIGNAL(clicked()),this,SLOT(SlotAddChildToFather()));
	connect(m_pDelNodeBtn,SIGNAL(clicked()),this,SLOT(SlotDelChildToFather()));
	connect(m_pChooseFatherNode,SIGNAL(currentIndexChanged(int)),this,SLOT(ResetChildrenText()));

	this->setWindowModality(Qt::WindowModal);

}

xSettingNodeDlg::~xSettingNodeDlg(void)
{
}

void xSettingNodeDlg::InitDialog()
{
	this->resize(320,250);

	QString str ;
	str = str.fromLocal8Bit("选择颜色");
	m_pColorBtn = new QPushButton(str);
	str = str.fromLocal8Bit("确定");
	m_pOk = new QPushButton(str);
	str = str.fromLocal8Bit("取消");
	m_pCancel = new QPushButton(str);


	str = str.fromLocal8Bit("可选父节点：");
	m_pFatherNodeLabel = new QLabel(str);

	m_pBtnGLayout = new QGridLayout();
	str = str.fromLocal8Bit("选择节点类型：");
	m_pNodeTypeLabel = new QLabel(str);
	str = str.fromLocal8Bit("选择节点运动方式：");
	m_pOperaTypeLabel = new QLabel(str);

	m_pNodeTypeCombox = new QComboBox;
	str = str.fromLocal8Bit("---选择节点类型---");
	m_pNodeTypeCombox->addItem(str);
	str = str.fromLocal8Bit("作为父节点");
	m_pNodeTypeCombox->addItem(str);
	str = str.fromLocal8Bit("作为子节点");
	m_pNodeTypeCombox->addItem(str);



	m_pOperaTypeCombox = new QComboBox;
	str = str.fromLocal8Bit("---选择节点运动方式---");
	m_pOperaTypeCombox->addItem(str);
	str = str.fromLocal8Bit("平动");
	m_pOperaTypeCombox->addItem(str);
	str = str.fromLocal8Bit("转动");
	m_pOperaTypeCombox->addItem(str);
	str = str.fromLocal8Bit("静止");
	m_pOperaTypeCombox->addItem(str);


	m_pChooseFatherNode = new QComboBox;
	m_pChooseFatherNode->show();
	str = str.fromLocal8Bit("选择父节点");
	QString Nonestr = str;
	m_pChooseFatherNode->addItem(str);
	m_pChooseFatherNode->view()->setFixedWidth(150);
	str = str.fromLocal8Bit("节点名：");
	m_pNodeNameLabel = new QLabel(str);
	m_pNodeNameLE = new QLineEdit;


	str = str.fromLocal8Bit("增加此节点");
	m_pAddNodeBtn = new QPushButton(str);
	str = str.fromLocal8Bit("删除此节点");
	m_pDelNodeBtn = new QPushButton(str);
	m_ChildenNameText = new QTextEdit;
	str = str.fromLocal8Bit("NONE!");
	m_ChildenNameText->setText("NONE!");
	std::vector<QString> vNono;
	vNono.push_back(str);
	m_FatherToChildrenNameMap[Nonestr] = vNono;

	QHBoxLayout *hfathernode = new QHBoxLayout;
	hfathernode->addWidget(m_pFatherNodeLabel);
	hfathernode->addWidget(m_pChooseFatherNode);

	m_pBtnGLayout->addLayout(hfathernode,0,0,1,2);
	//m_pBtnGLayout->addWidget(m_pChooseFatherNode,0,1,1,1);
	m_pBtnGLayout->addWidget(m_pAddNodeBtn,0,2,1,1);
	m_pBtnGLayout->addWidget(m_pDelNodeBtn,0,3,1,1);
	m_pBtnGLayout->addWidget(m_ChildenNameText,1,0,4,4);
	m_pBtnGLayout->addWidget(m_pCancel,5,0,1,1);
	m_pBtnGLayout->addWidget(m_pOk,5,3,1,1);

	this->setLayout(m_pBtnGLayout);

	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置

	m_color.setRgb(255,0,0);
	QFrame *buttonFrame = new QFrame;
	QPalette p = buttonFrame->palette();
	p.setColor(QPalette::Button,m_color);
	m_pColorBtn->setPalette(p);

	m_IsDel = false;
	m_IsAdd = false;
}

void xSettingNodeDlg::SlotColor()
{
	QString strColor;
	strColor = strColor.fromLocal8Bit("选择颜色");
	m_color = QColorDialog::getColor(Qt::red, this, strColor, QColorDialog::DontUseNativeDialog);
	if (m_color.isValid())
	{
		// color.name() 获取的是 html 表示的颜色名称，比如 #ff0403
		// color.red()、color.green()、color.blue() 获取的是 RGB 对应的3色数值
		QFrame *buttonFrame = new QFrame;

		QPalette p = buttonFrame->palette();
		p.setColor(QPalette::Button,m_color);
		m_pColorBtn->setPalette(p);
	}

}

void xSettingNodeDlg::SlotOpeType()
{

}

void xSettingNodeDlg::SlotNodeType()
{
}

void xSettingNodeDlg::SlotOk()
{	
	//
	if(m_IsDel == true)
	{
		//emit sigDelChildToFather(m_strCurNodeName,m_strCurFatherName);		
	}
	if(m_IsAdd == true)
	{
		//emit sigAddChildToFather(m_strCurNodeName,m_strCurFatherName);
	}

	this->close();
	//m_iNodeType = m_pNodeTypeCombox->currentIndex();
	//m_iOpeType = m_pOperaTypeCombox->currentIndex();
	//m_NodeColor = m_color;
	//m_strFatherNodeName = m_pChooseFatherNode->currentText();
	//m_strAddNodeName = m_pNodeNameLE->text();


	//if(!m_pNodeNameLE->text().isEmpty())
	//{
	//	if(m_iNodeType > 0 && m_iOpeType > 0 && m_NodeColor.isValid())
	//	{
	//		emit sigSetiingNodeFinished(m_Node,m_strAddNodeName);
	//		this->close();
	//	}

	//	else
	//	{
	//		QString strWarning;
	//		strWarning = strWarning.fromLocal8Bit("请正确设置节点属性!");
	//		QMessageBox::warning(this,"",strWarning);
	//	}
	//}
	//else
	//{
	//	QString strWarning;
	//	strWarning = strWarning.fromLocal8Bit("节点名不能为空!");
	//	QMessageBox::warning(this,"",strWarning);
	//}



}


void xSettingNodeDlg::SlotCancel()
{
	m_strCurNodeName = "";
	m_strCurFatherName = "";
	m_IsDel = false;
	m_IsAdd = false;

	//this->close();
}

void xSettingNodeDlg::AddChoics(int index)
{
	//m_pChooseFatherNode->clear();
	//if(index == 1 || index == 2)
	//{
	//	std::vector<osg::Group*> ::iterator iter= m_vNodes.begin();
	//	for(;iter!=m_vNodes.end();iter++)
	//	{
	//		QString tmp = QString::fromStdString((*iter)->getName());
	//		m_pChooseFatherNode->addItem(tmp);
	//	}

	//}

}



void xSettingNodeDlg::SlotAddChildToFather()
{
	int i;
	QString strnew;
	//j = m_pChooseFatherNode->currentIndex();
	if(m_pChooseFatherNode->currentIndex() != 0 && m_pChooseFatherNode->currentText() != m_strCurNodeName)//第一條索引無用
	{
		std::map<QString,std::vector<QString>>::iterator fatherFind = m_FatherToChildrenNameMap.find(m_strCurNodeName);
		if(fatherFind == m_FatherToChildrenNameMap.end())//节点不存在
		{
			std::map<QString,std::vector<QString>>::iterator iter = m_FatherToChildrenNameMap.begin();
			for (;iter!=m_FatherToChildrenNameMap.end();++iter)
			{
				if(iter->first == m_pChooseFatherNode->currentText())
				{

					//std::vector<QString> vtmpchildren = iter->second;
					//std::vector<QString>::iterator result = find( vtmpchildren.begin(), vtmpchildren.end(), m_strCurNodeName);
					//if ( result == L.end() )//子节点不存在
					//{
						iter->second.push_back(m_strCurNodeName); 
						/*std::vector<QString> vtmp;
						m_FatherToChildrenNameMap[m_strCurNodeName] = vtmp;*/
						m_vFatherNodesName.push_back(m_strCurNodeName);
						m_AddChildToFatherPair = make_pair(m_strCurNodeName,iter->first);
						m_IsDel = false;
						m_IsAdd = true;
						m_strCurFatherName = iter->first;
						i = m_pChooseFatherNode->currentIndex();
						/*	break;
						}
						else
						{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("当前父节点下已存在此子节点!");
						QMessageBox::warning(this,"",strWarning);
						return;
						}*/
				
				}
			}
		}
		else//节点已存在
		{
			QString strWarning;
			strWarning = strWarning.fromLocal8Bit("当前节点已存在!");
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
		



		AddFatherCombox(m_strCurNodeName,i);
		ResetChildrenText();
	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("请正确选择父节点!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;

	}
	
}

void xSettingNodeDlg::SlotDelChildToFather()
{
	int i;
	vtmpChildren.clear();
	//获取当前选择父节点下的所有子节点，保存当前选择的父节点
	if(m_pChooseFatherNode->currentIndex() != 0 && m_pChooseFatherNode->currentText() != m_strCurNodeName)//第一條索引不能删
	{
		std::map<QString,std::vector<QString>>::iterator iter = m_FatherToChildrenNameMap.begin();
		for (;iter!=m_FatherToChildrenNameMap.end();++iter)
		{
			if(iter->first == m_pChooseFatherNode->currentText())//通过选择的父节点，获得其子节点数组
			{
				vtmpChildren = iter->second;//父节点下的所有子节点  都需要删除（递归删除孙子节点）
				std::vector<QString>::iterator it = iter->second.begin();
				for (; it != iter->second.end(); ++it) 
				{
					if (*it == m_strCurNodeName) 
					{
						//iter->second.erase(it);//选择的父节点下 删除直接选择的节点
						m_DelChildToFatherPair = make_pair(m_strCurNodeName,iter->first);
						m_IsDel = true;
						m_IsAdd = false;
						m_strCurFatherName = iter->first;
						m_strDelCurFatherName = iter->first;
						m_strDelCurChildName = m_strCurNodeName;
						i = m_pChooseFatherNode->currentIndex();//删除节点的父节点索引
						break;
					}
				}

				if(it == iter->second.end() )
				{
					QString strWarning;
					strWarning = strWarning.fromLocal8Bit("当前父节点下不存在此子节点!");
					QMessageBox::warning(this,"",strWarning);
					LogInfo(strWarning);
					return;
				}
			}
		}
		m_vChildren = vtmpChildren;

		//子节点下的子节点
		for(size_t t=0;t<vtmpChildren.size();t++)
		{
			QString str;
			str = vtmpChildren[t];
			GetAllGChildrenByFather(str);
		}



		//删除父节点数组里的节点
		std::vector<QString>::iterator it_needDel = m_vChildren.begin();
		for (;it_needDel!=m_vChildren.end();++it_needDel)
		{
			std::vector<QString>::iterator iterf = m_vFatherNodesName.begin();
			for (;iterf!=m_vFatherNodesName.end();++iterf)
			{
				if(*it_needDel == *iterf)
				{
					m_vFatherNodesName.erase(iterf);
					break;
				}
			}
		}
		
		//更新下拉框和父子节点对应关系
		DelFatherCombox(m_vChildren,i);
		ResetChildrenText();
	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("请正确选择父节点!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;

	}
}

void xSettingNodeDlg::setNodeValue(osg::Node* node)
{
	m_Node = node;
}

int xSettingNodeDlg::GetOpeType()
{
	return m_iOpeType;
}

int xSettingNodeDlg::GetNodeType()
{
	return m_iNodeType;
}

QColor xSettingNodeDlg::GetNodeColor()
{
	return m_NodeColor;
}

void xSettingNodeDlg::SetVecGNodes(std::vector<osg::Group *> pgroup)
{
	m_vNodes = pgroup;
}

QString xSettingNodeDlg::getFatherNodeName()
{
	return m_strFatherNodeName;
}

QString xSettingNodeDlg::getAddNodeName()
{
	return m_strAddNodeName;
}

QComboBox* xSettingNodeDlg::getNodeTypeCombox()
{
	return m_pNodeTypeCombox;
}

void xSettingNodeDlg::setCurLineEditFileName(QString str)
{
	m_pNodeNameLE->setText(str);
	m_pNodeNameLE->setFocusPolicy(Qt::NoFocus); 
}


void xSettingNodeDlg::setCurNodeName(QString name)
{
	m_strCurNodeName = name;
}

void xSettingNodeDlg::initCombox()
{
	m_pChooseFatherNode->setCurrentIndex(0);


	m_pNodeTypeCombox->setCurrentIndex(1);
	m_pOperaTypeCombox->setCurrentIndex(1);
}

void xSettingNodeDlg::ResetChildrenText()
{
	//ResetFatherCombox();


	m_ChildenNameText->clear();
	std::map<QString,std::vector<QString>>::iterator iter = m_FatherToChildrenNameMap.begin();
	for (;iter!=m_FatherToChildrenNameMap.end();++iter)
	{
		if(iter->first == m_pChooseFatherNode->currentText())
		{
			for (auto it = iter->second.begin(); it != iter->second.end(); ++it) 
			{
				m_ChildenNameText->insertPlainText(*it+'\n');
			}
		}
	}
}

void xSettingNodeDlg::ResetFatherCombox(QString strFather)
{

}

void xSettingNodeDlg::AddFatherCombox(QString strFather,int i)
{
	m_pChooseFatherNode->addItem(strFather);
	m_pChooseFatherNode->setCurrentIndex(i);

	std::vector<QString> vtmp;
	m_FatherToChildrenNameMap[m_strCurNodeName] = vtmp;
}

void xSettingNodeDlg::DelFatherCombox(std::vector<QString> vstrFather,int i)
{
	//下拉框 删除选择的节点及其子节点
	std::vector<QString>::iterator del_it =  vstrFather.begin();
	for(;del_it!=vstrFather.end();del_it++)
	{	
		int index = m_pChooseFatherNode->findText(*del_it);
		if(index != -1)
		{
			m_pChooseFatherNode->removeItem(index);
		}
		
	}
	m_pChooseFatherNode->setCurrentIndex(0);

	//更新map数据结构:删除掉作为父节点的结构
	for(size_t b=0;b<m_vChildren.size();b++)
	{
		std::map<QString,std::vector<QString>>::iterator iter2 = m_FatherToChildrenNameMap.begin();
		for (;iter2!=m_FatherToChildrenNameMap.end();/*++iter2*/)
		{
			if(iter2->first == m_vChildren[b])
			{
				m_FatherToChildrenNameMap.erase(iter2++);//iter2++会先备份一个iter2++，删除iter2后，iter2++仍然有效

			}
			else
			{
				iter2++;
			}
		}
	}

	//删除目标模型作为子节点
	std::map<QString,std::vector<QString>>::iterator it2 = m_FatherToChildrenNameMap.begin();
	for (;it2!=m_FatherToChildrenNameMap.end();it2++)
	{
		if(it2->first == m_strDelCurFatherName)
		{
			std::vector<QString>::iterator it3 = it2->second.begin();
			for (; it3 != it2->second.end(); ++it3) 
			{
				if(*it3 == m_strDelCurChildName)
				{
					it2->second.erase(it3);
					break;
				}
			}
		}
	}


}

void xSettingNodeDlg::InitFatherCombox()
{
	QString str;
	str = str.fromLocal8Bit("NodeScene");
	m_pChooseFatherNode->addItem(str);//场景节点
	m_vFatherNodesName.push_back(str);
	std::vector<QString> tmp;
	m_FatherToChildrenNameMap[str] = tmp;

	//std::vector<QString> tmp;
	//tmp.push_back(m_strCurNodeName);
	//m_FatherToChildrenNameMap[str] = tmp;
	//m_pChooseFatherNode->addItem(m_strCurNodeName);//第一个模型节点
	//m_vFatherNodesName.push_back(m_strCurNodeName);
	str = str.fromLocal8Bit("选择父节点");
	m_vFatherNodesName.push_back(str);

	m_IsDel = false;
	m_IsAdd = false;
}

const QString xSettingNodeDlg::GetCurFatherName()
{
	return m_strCurFatherName;
}

const QString xSettingNodeDlg::GetCurChildName()
{
	return m_strCurNodeName;
}

void xSettingNodeDlg::GetAllGChildrenByFather(QString strFa)
{
	std::map<QString,std::vector<QString>>::iterator iter = m_FatherToChildrenNameMap.begin();
	for (;iter!=m_FatherToChildrenNameMap.end();++iter)
	{
		if(iter->first == strFa)
		{
			if( iter->second.size()>0)
			{
				std::vector<QString> tmp;
				tmp = iter->second;

				for(size_t a=0;a<tmp.size();a++)
				{
					m_vChildren.push_back(tmp[a]);
					GetAllGChildrenByFather(tmp[a]);
				}
			}

		}
	}
	if(iter==m_FatherToChildrenNameMap.end())
	{
		return;
	}
}

std::vector<QString> xSettingNodeDlg::GetAllDelNode()
{
	return m_vChildren;
}

void xSettingNodeDlg::AddFatherToChildrenMap(QString strFa,QString strChild)
{
	std::map<QString,std::vector<QString>>::iterator iter = m_FatherToChildrenNameMap.begin();
	for (;iter!=m_FatherToChildrenNameMap.end();++iter)
	{
		if(iter->first == strFa)
		{
			iter->second.push_back(strChild);
			break;
		}
	}

	if(iter == m_FatherToChildrenNameMap.end())
	{
		std::vector<QString> tmp;
		m_FatherToChildrenNameMap[strFa] = tmp; 
	}
}

