#include "xSettingColor.h"
#include "MainWindow.h"
xSettingColor::xSettingColor(MainWindow *p)
{
	m_pMainWindow = p;
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//窗口前置
	InitDlg();

	connect(m_pColorBtn, SIGNAL(clicked()), this, SLOT(SlotChangeColor()));
	connect(m_ModelCombox,SIGNAL(currentIndexChanged(int)),this, SLOT(On_Item_changed(int)), Qt::UniqueConnection);

}

xSettingColor::~xSettingColor()
{

}

void xSettingColor::InitDlg()
{
	m_pColorBtn = new QPushButton("Choose");
	m_ModelLabel = new QLabel("ModelName");
	m_ModelCombox = new QComboBox;
	m_ModelCombox->setMaxVisibleItems(35);
	m_ModelCombox->addItem("please choose one");

	QHBoxLayout *h = new QHBoxLayout;
	h->addWidget(m_ModelLabel);
	h->addWidget(m_ModelCombox);
	
	QGridLayout *g = new QGridLayout;
	g->addLayout(h,0,0);
	g->addWidget(m_pColorBtn,1,0);

	this->setLayout(g);

}

void xSettingColor::InitModelComBox(std::vector<QString> vModelName)
{
	size_t count = m_ModelCombox->count();
	for(size_t i=count -1;i>0;i--)
	{
		m_ModelCombox->removeItem(i);
	}


	for (size_t j=0;j<vModelName.size();j++)
	{
		m_ModelCombox->addItem(vModelName[j]);
	}

	m_ModelCombox->setCurrentIndex(0);
}

void xSettingColor::SetColorDlgLanguage(std::vector<QString> vCurWords)
{
	m_ModelLabel->setText(vCurWords[74]);
	m_pColorBtn->setText(vCurWords[94]);
	m_ModelCombox->setItemText(0,vCurWords[84]);
}

void xSettingColor::RebuildModelColorFile()
{
	char lang[256] = "";
	m_pMainWindow->GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, STL_PATH, 3, lang);
	m_pMainWindow->GetWholePath(lang);
	std::string tmp = lang;
	QString langPath = QString::fromLocal8Bit(tmp.c_str());
	langPath = QDir::fromNativeSeparators(langPath);//"\"转"/"

	QString strlanfgFile = langPath + QString::fromLocal8Bit("/ModelColor.csv");
	std::string str = std::string((const char *)strlanfgFile.toLocal8Bit().constData());
	std::ofstream outFile;
	outFile.open(str,std::ios::out | std::ios::trunc);

	outFile<<"ModelName"<<','<<"RGB"<<std::endl;
	std::map<QString,RGB_COLOE>::iterator it = m_MapModelNameToRgb.begin();
	for (;it!=m_MapModelNameToRgb.end();it++)
	{
		std::string sname =string(it->first.toLocal8Bit());
		outFile<<sname<<','<<"\"(";
		std::string sR = QString::number(it->second.r).toStdString();
		std::string sG = QString::number(it->second.g).toStdString();
		std::string sB = QString::number(it->second.b).toStdString();
		outFile<<sR<<','<<sG<<','<<sB<<"\")"<<std::endl;
	}

	outFile.close();

}

void xSettingColor::SetMapValue(std::map<QString,RGB_COLOE> value)
{
	m_MapModelNameToRgb = value;
}

void xSettingColor::closeEvent(QCloseEvent *event)
{
	m_pMainWindow->ResumeAllModelColor();
}

void xSettingColor::SlotChangeColor()
{
	QString strname = m_ModelCombox->currentText();

	//颜色对话框设置
	QColorDialog::setCustomColor(0,QRgb(0x0000FF));
	//定义 QColor 接收值
	m_color = QColorDialog::getColor(/*QColor(0,255,0)*/Qt::black);
	//定义 QPalette(调色板类)
	QPalette p = palette();
	//调色板接收颜色
	p.setColor(QPalette::WindowText,QColor(m_color));
	//给 label 绑定颜色
	m_pColorBtn->setPalette(p);


	float r = m_color.red()/255.0;
	float g = m_color.green()/255.0;
	float b = m_color.blue()/255.0;
	osg::Vec4 color = osg::Vec4(r,g,b,1.0f);
	m_pMainWindow->SetColor(strname,color);


	std::map<QString,RGB_COLOE>::iterator it;
	it=m_MapModelNameToRgb.find(strname);
	if (it!=m_MapModelNameToRgb.end())
	{
		RGB_COLOE c;
		c.r = m_color.red();
		c.g = m_color.green();
		c.b = m_color.blue();

		it->second = c;
	}
	else
	{
		RGB_COLOE c;
		c.r = m_color.red();
		c.g = m_color.green();
		c.b = m_color.blue();
		m_MapModelNameToRgb[strname] = c;
	}


	RebuildModelColorFile();
}

void xSettingColor::On_Item_changed(int index)
{
	if (index == 0)
	{
		m_pMainWindow->ResumeAllModelColor();
	}
	else
	{
		QString strname = m_ModelCombox->currentText();
		m_pMainWindow->On_modelColor_change(strname);
	}

}
