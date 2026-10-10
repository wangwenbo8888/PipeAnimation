/******************************************************************************
osg3DViewer: a 3D file viewer
Copyright(C) 2015-2015  ronggenmiao@163.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software

*******************************************************************************/

#include "MainWindow.h"
#include "xAppSettings.h"
#include "xLogHandler.h"
#include "xFindNode.h"
#include "JdcMath.h"
#include "xTravelNode.h"
#include <iostream> //输入输出流
#include <iomanip>
#include <fstream> //文件流
#include <regex>
#include <stdlib.h>
#include <atlconv.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <cctype> //判断字符类型需要的头文件
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <QtGui/qwindowdefs.h>
#include <QStandardItemModel>
//#include <QStringLiteral>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QFileDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLabel>
#include <QtGui/QAction>
#include <QtCore/QTextStream>
#include <QtGui/QCompleter>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>
#include <QtGui/QGridLayout>

#include <QtCore/QStringListIterator>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>//Geode是专门管理可绘制几何体的总闸
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/Material>
#include <osg/NodeVisitor>
#include<sstream>
#include<QHeaderView>

#include <QColor>

#include "xSceneView.h"
#include "xSceneModel.h"
#include "xTreeModel.h"
#include "xTreeView.h"
#include "xPropertyWidget.h"
#include "xSearchLineEdit.h"
#include "CollisionCallBack.h"

#include "log.h"

#include <regex>

#include "qtimer.h"

//#include "PreferencesWidget.h"
#include "xThreadPool.h"
#include "xObjectLoader.h"
//#include "SceneViewState.h"
//#include "BookmarkItem.h"
//#include "BookmarkDialog.h"
//#include "PosterImageDialog.h"
//#include "AboutDialog.h"
#include "xShaderSettingDialog.h"
//#include "MiscFunctions.h"
#include "xFindNameListVisitor.h"
#include "NodeCollisionDetect.h"

#include "SoftKeylineEdit.h"

#include "CreateNode.h"

#include "ExportStl.h"

using namespace std;
std::regex reg(",");

int g_modelType = 1;//20130510定义一个变量，用来全局标志是第一种类型还是第二种模型
const int maxRecentlyOpenedFile = 10;
int g_topLunMoNum = 0;
double g_finalCylinderLen = 2000.0;
// 轮模高度为55mm
float g_fModelHeight = 80;
bool g_xiaocheGansheMove=false;
// 最底下的轮模半径为60mm
float g_fBottomRadius = 60;

const float freturnDis = 2000;

char twlc_path[64] = "";
char param_path[64] = "";
char stlname[128] = "";
char stlpath[256] = "";
char allfilepath[64] = "";
char production_values_files_path[256] ="";
char animation_files_path[256] ="";

std::string& trim(std::string &s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(0,s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

void MainWindow::SetSTLNodeHideByName(QString str)
{   
	for (size_t i=0;i<m_vSTLs.size();i++)
	{
		if (m_vSTLs[i]->GetSTLName() == str)
		{
			m_vSTLs[i]->GetSTLNode()->setNodeMask(0);
			m_vSTLs[i]->SetLMShow(false);
		}
	}
}

void MainWindow::ReplaceBySpecialModel(QString strCurName,QString strSpecialName)
{
	for (size_t i=0;i<m_vSTLs.size();i++)
	{
		if (m_vSTLs[i]->GetSTLName() == strCurName)
		{
			STL *special = GetCurSTLByName(strSpecialName);
			//m_vSTLs[i]->replaceCurNode(special->GetSTLNode());
			m_vSTLs[i]->replaceCurNodeBySpecial(special);
			special->GetSTLNode()->setNodeMask(-1);
			special->RemoveCurNode();
			//m_vSTLs[i]->GetSTLNode()->setNodeMask(-1);
		}
	}
}

osg::Vec3 MainWindow::GetReplacedModelInitPos(QString name,int layer)
{
	InitModelCenter temp;
	for (size_t i = 0; i < m_vInitModelCenterFileValues.size();++i)
	{
		temp = m_vInitModelCenterFileValues[i];
		if (temp.ModelName==name && temp.ModelLayer==layer)
		{
			return temp.ModelCenter;
		}
	}

	// 开始设置模型，默认模层为1
	for (size_t i = 0; i < m_vInitModelCenterFileValues.size();++i)
	{
		temp = m_vInitModelCenterFileValues[i];
		if (temp.ModelName==name && temp.ModelLayer==1)
		{
			return temp.ModelCenter;
		}
	}

	QMessageBox::warning(NULL, "warning", QString(("error in find center for %1 in layer %2")).arg(name).arg(QString::number(layer)), QMessageBox::Yes);
	LogInfo(QString("error with find center replace %1 in layer %2").arg(name).arg(QString::number(layer)));
}

void MainWindow::ReplaceByNewInstance(QString strCurName,QString strSpecialName)
{
	for (size_t i=0;i<m_vSTLs.size();i++)
	{
		if (m_vSTLs[i]->GetSTLName() == strCurName)
		{
			STL *special = GetCurSTLByName(strSpecialName);
			//m_vSTLs[i]->replaceCurNode(special->GetSTLNode());
			// osg::ref_ptr<osg::Node> node = (osg::Node *)(special->GetSTLNode()->clone(osg::CopyOp::DEEP_COPY_ALL));
			osg::ref_ptr<osg::Material> material;
			osg::ref_ptr<osg::Node> node = CloneStl(special,m_vSTLs[i]->GetSTLColor(),material);
			node->setName(strCurName.toStdString().c_str());
			m_vSTLs[i]->replaceCurNode(node);
			m_vSTLs[i]->SetMaterial(material);

			int layer = m_vSTLs[i]->GetCurLayer();
			// 设置切换后模型位置
			osg::Vec3 moveValue = GetReplacedModelInitPos(strSpecialName,layer)/* - curCenter*/;
			osg::ref_ptr<osg::MatrixTransform> tmpMatrix = m_vSTLs[i]->GetPosMatrix();
			//osg::Matrixd mat0 = tmpMatrix->getMatrix();
			double shift = GetJMShift(strCurName);
			moveValue[0] += shift;

			// 保存因管径不同造成的平移
			m_vSTLs[i]->SetShift(shift);
			osg::Matrixd mat1 = osg::Matrix::translate(moveValue);
			tmpMatrix->setMatrix(mat1);
			//更新模型中心
			m_vSTLs[i]->SetModelCenter(moveValue);
		}
	}
}

void MainWindow::ReadAsSpecialReplaceModelFile()
{
	m_MapModelNameToIsAsSpecial.clear();

	QString strFile = need_filePath + QString::fromLocal8Bit("/JT1AsSpecialReplaceModel.csv");
	QByteArray ba = strFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			QString strname = QString::fromLocal8Bit(Res[0].c_str());
			int iSpecial = QString::fromLocal8Bit(Res[1].c_str()).toInt();
			m_MapModelNameToIsAsSpecial[strname] = iSpecial;
		}
	}

	fp.close();
}

int MainWindow::GetTypeModelIsReplaceSpecialByName(QString strName)
{
	std::map<QString,int>::iterator it /*= m_MapModelNameToIsAsSpecial.begin()*/;
	it = m_MapModelNameToIsAsSpecial.find(strName);
	if (it!=m_MapModelNameToIsAsSpecial.end())
	{
		return it->second;
	}
	else
	{
		return 0;//默认是非特殊模型
	}
}

void MainWindow::RebuildParamFile(std::vector<LMR> vNormalUserLMValues)
{
	std::map<int,float> vMapLayerToNewR;
	for (size_t i=0;i<vNormalUserLMValues.size();i++)
	{
		for (size_t j=0;j<m_vLMRFileValues.size();j++)
		{
			if (vNormalUserLMValues[i].strModelName == m_vLMRFileValues[j].strModelName)
			{
				vMapLayerToNewR[m_vLMRFileValues[j].iLayer] = vNormalUserLMValues[i].fLmr;
			}
		}
	}

	std::map<int,float>::iterator it = vMapLayerToNewR.begin();
	for (;it!=vMapLayerToNewR.end();it++)
	{
		m_param.LmRadius[it->first - 1] = it->second;
	}

	//更新参数文件
	std::ofstream outFile;

	QString strName;
	QString str;
	std::string s;	
	s = param_path;
	str = QString::fromLocal8Bit(s.c_str());
	strName = QDir::fromNativeSeparators(str);//"\"转"/"
	strName = strName + QString::fromLocal8Bit("/参数.csv");	
	std::string str2 = std::string((const char *)strName.toLocal8Bit().constData());

	outFile.open(str2,std::ios::out | std::ios::trunc);

	float _fydp;
	if (m_param.fYdPos < 0)
	{
		_fydp = -(m_param.fYdPos-m_pipe_z);
	}
	else
	{
		_fydp = m_param.fYdPos+m_pipe_z;
	}

	float _fZgPos;
	if (m_param.fZgPos < 0)
	{
		_fZgPos = -m_param.fZgPos + m_pipe_z;
	}
	else
	{
		_fZgPos = m_param.fZgPos + m_pipe_z;
	}

	float _fDywPos;
	if (m_param.fDywPos < 0)
	{
		_fDywPos = -m_param.fDywPos + m_pipe_z;
	}
	else
	{
		_fDywPos = m_param.fDywPos+ m_pipe_z;
	}


	float _fWftwPos;
	if (m_param.fWftwPos < 0)
	{
		_fWftwPos = -m_param.fWftwPos + m_pipe_z;
	}
	else
	{
		_fWftwPos = m_param.fWftwPos + m_pipe_z;
	}

	outFile<<"R1"<<','<<QString::number(m_param.LmRadius[0]).toStdString()<<std::endl;
	outFile<<"R2"<<','<<QString::number(m_param.LmRadius[1]).toStdString()<<std::endl;
	outFile<<"R3"<<','<<QString::number(m_param.LmRadius[2]).toStdString()<<std::endl;
	outFile<<"R4"<<','<<QString::number(m_param.LmRadius[3]).toStdString()<<std::endl;
	outFile<<"R5"<<','<<QString::number(m_param.LmRadius[4]).toStdString()<<std::endl;
	outFile<<"D"<<','<<QString::number(m_param.fPipeDiameter).toStdString()<<std::endl;
	outFile<<"L"<<','<<QString::number(m_param.fPipeLength).toStdString()<<std::endl;
	outFile<<"mn"<<','<<QString::number(m_param.imn).toStdString()<<std::endl;
	outFile<<"ydp"<<','<<QString::number(_fydp).toStdString()<<std::endl;
	outFile<<"zgp"<<','<<QString::number(_fZgPos).toStdString()<<std::endl;
	outFile<<"dywp"<<','<<QString::number(_fDywPos).toStdString()<<std::endl;
	outFile<<"wqmr"<<','<<QString::number(m_param.fWqmRadius).toStdString()<<std::endl;
	outFile<<"mjl"<<','<<QString::number(m_param.fMjDistance).toStdString()<<std::endl;
	outFile<<"zxmr"<<','<<QString::number(m_param.fZxmRadius).toStdString()<<std::endl;
	outFile<<"dmal"<<','<<QString::number(m_param.fDmaqDistance).toStdString()<<std::endl;
	outFile<<"wtwp"<<','<<QString::number(_fWftwPos).toStdString()<<std::endl;
	outFile<<"zm1"<<','<<QString::number(m_param.Zmlength[0]).toStdString()<<std::endl;
	outFile<<"zm2"<<','<<QString::number(m_param.Zmlength[1]).toStdString()<<std::endl;
	outFile<<"zm3"<<','<<QString::number(m_param.Zmlength[2]).toStdString()<<std::endl;
	outFile<<"zm4"<<','<<QString::number(m_param.Zmlength[3]).toStdString()<<std::endl;
	outFile<<"zm5"<<','<<QString::number(m_param.Zmlength[4]).toStdString()<<std::endl;
	outFile<<"ydm1"<<','<<QString::number(m_param.RDmlength[0]).toStdString()<<std::endl;
	outFile<<"ydm2"<<','<<QString::number(m_param.RDmlength[1]).toStdString()<<std::endl;
	outFile<<"ydm3"<<','<<QString::number(m_param.RDmlength[2]).toStdString()<<std::endl;
	outFile<<"ydm4"<<','<<QString::number(m_param.RDmlength[3]).toStdString()<<std::endl;
	outFile<<"ydm5"<<','<<QString::number(m_param.RDmlength[4]).toStdString()<<std::endl;
	outFile<<"zdm1"<<','<<QString::number(m_param.LDmlength[0]).toStdString()<<std::endl;
	outFile<<"zdm2"<<','<<QString::number(m_param.LDmlength[1]).toStdString()<<std::endl;
	outFile<<"zdm3"<<','<<QString::number(m_param.LDmlength[2]).toStdString()<<std::endl;
	outFile<<"zdm4"<<','<<QString::number(m_param.LDmlength[3]).toStdString()<<std::endl;
	outFile<<"zdm5"<<','<<QString::number(m_param.LDmlength[4]).toStdString()<<std::endl;

	outFile.close();
}

void MainWindow::SetCurrentModel(const QString& name)
{
	if (m_pSlightAdjustDlg!=nullptr)
	{
		m_pSlightAdjustDlg->SetCurrentItem(name);
	}
}

void MainWindow::SetCurrentPos(osg::Vec3 pos)
{
	if (m_pSlightAdjustDlg!=nullptr)
	{
		QString name = m_pSlightAdjustDlg->GetChosenModelName();
		STL* curSTL = GetCurSTLByName(name);
		if (curSTL!=nullptr)
		{
			osg::Vec3 curPos = (curSTL->GetSTLNode())->getBound().center()*osg::computeLocalToWorld((curSTL->GetSTLNode())->getParentalNodePaths()[0]);
			m_pSlightAdjustDlg->SetCurrentPos(curPos);
		}
		else
		{
			//QMessageBox::warning(NULL, "Error", "Error get model position !", QMessageBox::Yes);
			//LogInfo(QString("Error get model %1 center pos ! ").arg(name));
			m_pSlightAdjustDlg->SetCurrentPos(pos);

			return;
		}
	}
}

// 开始的时候设置为普通用户默认的设置
void MainWindow::RefreshNormalUserSetting()
{
	if (m_pNormalUserWidget!=NULL)
	{
		m_pNormalUserWidget->InitTable();
		m_pNormalUserWidget->show();
		m_pNormalUserWidget->close();
	}
}

// LMR.csv文件里的轮模半径根据参数.csv里刷新过，
// 根据LMR.csv文件刷新机头1 JT1_LMValues.csv文件里的轮模半径
void MainWindow::RefreshJT1LMValueFile()
{
	QString strtmp;
	std::string stmp;
	char sJT1_FILES_PATH[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_FILES_PATH, 3, sJT1_FILES_PATH);
	GetWholePath(sJT1_FILES_PATH);
	stmp = sJT1_FILES_PATH;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	QString JT1Dir;
	JT1Dir = strtmp;

	QString strFileName = JT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");	
	m_JT1_LMFileValues = ReadJT1CenterFileValues(strFileName);

	if (m_JT1_LMFileValues.empty())
	{
		return;
	}

	for (size_t i = 0; i < m_JT1_LMFileValues.size();++i)
	{
		LMR& lm0 = m_JT1_LMFileValues[i];
		for (size_t j = 0; j < m_vLMRFileValues.size();++j)
		{
			LMR& lm1 = m_vLMRFileValues[j];
			if (lm1.strModelName == lm0.strModelName)
			{
				lm0.fLmr = lm1.fLmr;
				break;
			}
		}
	}

	if (m_pNormalUserWidget!=NULL)
	{
		m_pNormalUserWidget->WriteToCenterFile(strFileName,m_JT1_LMFileValues,1);
	}

	// m_JT1_LMFileValues.clear();
}

void MainWindow::RefreshLMrByParamFile(std::vector<LMR>& vLMValues)
{
	for (size_t i=0;i<vLMValues.size();i++)
	{
		if(vLMValues[i].iLayer == 1)
		{
			vLMValues[i].fLmr = m_param.LmRadius[0];
		}
		else if (vLMValues[i].iLayer == 2)
		{
			vLMValues[i].fLmr = m_param.LmRadius[1];
		}
		else if(vLMValues[i].iLayer == 3)
		{
			vLMValues[i].fLmr = m_param.LmRadius[2];
		}
		else if(vLMValues[i].iLayer == 4)
		{
			vLMValues[i].fLmr = m_param.LmRadius[3];
		}
		else if(vLMValues[i].iLayer == 5)
		{
			vLMValues[i].fLmr = m_param.LmRadius[4];
		}
	}

	//更新文件
	std::vector<LMR> vtmp = vLMValues;
	std::ofstream outFile;
	QString file = need_filePath + QString::fromLocal8Bit("/LMR.csv");
	std::string str = std::string((const char *)file.toLocal8Bit().constData());
	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"ModelName"<<','<<"LM_Type"<<','<<"LM_R"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"TuLen"<<','<<"TuSide"<<','<<"IsShow"<<','<<"JTNth"<<','<<"Layer"<<std::endl;
	//写入数据-字符串
	std::vector<LMR>::iterator it = vtmp.begin();
	for(;it!=vtmp.end();it++)
	{	std::string sType = string((*it).strType.toLocal8Bit());
	std::string sr = QString::number((*it).fLmr).toStdString();
	std::string sup = QString::number((*it).fupLen).toStdString();
	std::string sdown = QString::number((*it).fdownLen).toStdString();
	std::string stu = QString::number((*it).ftuLen).toStdString();
	std::string sshow =  QString::number((*it).isShow).toStdString();
	std::string sside =  QString::number((*it).iSide).toStdString();
	std::string sJTnth =  QString::number((*it).iJTNth).toStdString();
	std::string sname = string((*it).strModelName.toLocal8Bit());
	std::string sLayer = QString::number((*it).iLayer).toStdString();

	outFile<<sname<<','<<sType<<','<<sr<<','<<sup<<','<<sdown<<','<<stu<<','<<sside<<','<<sshow<<','<<sJTnth<<','<<sLayer<<std::endl;
	}

	outFile.close();
}

void MainWindow::Trim(std::string &s)
{
	int index = 0;
	if( !s.empty())
	{
		while( (index = s.find(' ',index)) != string::npos)
		{
			s.erase(index,1);
		}
	}
}

osg::Camera* createHUD(osg::GraphicsContext* gc)
{
	if (gc)
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		camera->setRenderOrder(osg::Camera::POST_RENDER,2);
		camera->setAllowEventFocus(true);
		camera->setGraphicsContext(gc);
		const osg::GraphicsContext::Traits* traits = gc->getTraits();
		camera->setViewport(new osg::Viewport(0,0, traits->width, traits->height));
		GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
		camera->setDrawBuffer(buffer);
		camera->setReadBuffer(buffer);

		return camera.release();
	}

	return 0;
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	m_treeModel(NULL),
	m_treeView(NULL),
	//m_currentSnapshotNum(0),
	//m_lastSnapshotName("snapshot"),
	m_appName(PACKAGE_NAME),
	m_version(PACKAGE_VERSION),
	//m_recentFilesMenu(NULL),
	//m_logLevel(LogHandler::LOG_DEBUG),
	//m_splashscreenAtStartup(true),
	//m_splashscreenTransparentBackground(true),
	//m_resetConfig(false),
	//m_watermark(true),
	//m_keepCompassState(false),
	//m_inverseMouseWheel(false),
	//m_displayTrackballHelper(false),
	//m_optimize(false),
	m_lineEditSearch(NULL),
	m_sceneModel(NULL),
	m_LODFactorLabel(NULL),
	m_LODFactor(1.0f),
	m_isOptimized(false),
	m_objectLoader(NULL),
	m_aspectRatioLabel(NULL),
	m_pSettingAnimationDlg(NULL),
	NodeDlg(NULL),
	m_pSettingNodeGroupDlg(NULL),
	m_pSlightAdjustDlg(NULL),
	m_pReplaceWheelModelDlg(NULL),
	m_pCollisionDlg(NULL),
	m_pRotateTestDlg(NULL),
	m_productionTable(NULL),
	m_twTable(NULL),
	m_isCaseSensitive(false),
	m_completerSearch(0),
    m_dPipePos(0.0),
	//m_typeSnapshot(SNAPSHOT_FOR_FILE),
	//m_bmkDialog(NULL),
	//m_posterDialog(NULL),
	//m_about(NULL)
	m_bOnlyLeft(false),
	m_bIsAdjusting(false)
{
	ui.setupUi(this);
	//loadSettings();

	m_dTDistance = -1.0;

	this->setMouseTracking(true);

	//右键表格
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	cmenu = new QMenu;
	TableAction = cmenu->addAction("Values");
	BtnsAction = cmenu->addAction("Btns");
	SetViewAsLookAt = cmenu->addAction("SetLookAt");

	TableAction->setCheckable(true);
	TableAction->setChecked(true);
	BtnsAction->setCheckable(true);
	BtnsAction->setChecked(true);	
	cmenu->setStyle(QStyleFactory::create("Cleanlooks"));
	//SetViewAsLookAt->setCheckable(true);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(SlotRightMenu(const QPoint &)), Qt::UniqueConnection);
	connect(TableAction, SIGNAL(toggled(bool)), this, SLOT(SlotMenuTablecheckbox_clicked(bool)));
	connect(BtnsAction, SIGNAL(toggled(bool)), this, SLOT(SlotMenuBtnscheckbox_clicked(bool)));
	connect(SetViewAsLookAt, SIGNAL(triggered(bool)), this, SLOT(SlotMenuLookAt_clicked(bool)));

	m_pTree = NULL;
	//m_pTree = new CTree("Root");

	QGridLayout* grid = new QGridLayout;
	grid->setContentsMargins(1,1,1,1);
	grid->setSpacing(0);
	ui.centralwidget->setLayout(grid);

	m_sceneView = new xSceneView(ui.centralwidget);
	grid->addWidget(m_sceneView, 0, 0);

	m_propertyWidget = new xPropertyWidget(ui.dockWidget_Properties);
	m_propertyWidget->hide();
	//ui.dockWidget_Properties->setWidget(m_propertyWidget);

	//加入表格界面
	QWidget *widget = new QWidget(ui.dockWidget_Properties);
	//m_productionTable = new Table(widget);
	//m_twTable = new Table(widget);
	QVBoxLayout *_vLayout = new QVBoxLayout(widget);
	//_vLayout->addWidget(m_productionTable);
	//_vLayout->setMargin(0);
	//_vLayout->addWidget(m_twTable);
	_vLayout->addWidget(m_productionTable);
	widget->setLayout(_vLayout);
	ui.dockWidget_Properties->setWidget(widget);

	QDockWidget *dock2 = new QDockWidget();
	m_productionTable = new Table();
	dock2->setWidget(m_productionTable);
	dock2->setFeatures(QDockWidget::AllDockWidgetFeatures); 
	dock2->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::LeftDockWidgetArea, dock2); /* 初始化显示在窗口顶端 */

	QDockWidget *dock = new QDockWidget();
	m_twTable = new Table();
	dock->setWidget(m_twTable);
	dock->setFeatures(QDockWidget::AllDockWidgetFeatures); 
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::LeftDockWidgetArea, dock); /* 初始化显示在窗口顶端 */

	//加入平板界面
	m_NormalPadWidget = new QWidget(ui.dockWidget_Model);
	ui.dockWidget_Model->setWidget(m_NormalPadWidget);
	InitNormalUserPadWidget();

	loadSettings();
	updateUi();

	addDockWidget(Qt::RightDockWidgetArea, ui.dockWidget_Model);//添加到界面右侧
	ui.dockWidget_Model->show();
	ui.dockWidget_Properties->hide();
	ui.actionBackFace->setVisible(false);
	//ui.dockWidget_Properties->setSizePolicy(QSizePolicy::);
	ui.logDock->hide();
	ui.menubar->hide();

	m_IsDetected = false;
	m_bIsAutoPlan = false;
	nodecount = 0;
	m_bIsFirstSTL = true;

	m_closeTimer = new QTimer(this);

	//SetTimer = new MMTimer(1000,0);//设置定时22ms

	//m_closeTimer->setSingleShot(true);

	m_collisionTimer = new QTimer(this);
	//m_collisionTimer->setInterval(5000/m_v);
	//m_collisionTimer->setSingleShot(true);

	m_mark = 0;
	m_showLongStraightPipe = 0;
	m_isFirstOpenGroupDlg = true;
	m_isFirstOpenAdjustDlg = true;
	m_isFirstOpenMoveRangeDlg = true;
	m_isFirstOpenRotateTestDlg = true;
	m_isFirstOpenCollisionDlg = true;
	connect(this,SIGNAL(sigPartsPause(bool)),this,SLOT(on_actionLight_triggered(bool)));

	m_dRat = 1.0;
	m_dX = 0.0;
	m_dY = 0.0;
	m_dDegree = 0.0;
	m_bFirstSet = true;

	SetQToolBarSize(50,50);

	//fengtest 0229
	/*_debugGeode = new osg::Geode;
	_debugGeode->setCullingActive(false);
	osg::ref_ptr<osgCollisionDetection::DebugDraw> drawable = new osgCollisionDetection::DebugDraw();
	drawable->setCollisionWorld(osgCollisionDetection::CollisionInterface::Instence()->getCollisionWorld());
	_debugGeode->addDrawable(drawable.get());
	_debugGeode->setNodeMask(0x2);
	m_sceneModel->getScene()->asSwitch()->addChild(_debugGeode.get());*/

	m_sceneView->getView(0)->addEventHandler(new MouseHandle(this));

	osgDragger::ShowBound::Instence()->setNodeMask(~0x1);
	m_sceneModel->getScene()->asSwitch()->addChild(osgDragger::ShowBound::Instence()->getRoot());

	osg::ref_ptr<osg::Camera> hudCamera = 0;

	osgViewer::Viewer::Windows windows;
	m_sceneView->getWindows(windows);
	
	int a = windows.size();
	if (windows.size() != 0)
	{
		hudCamera = createHUD(windows[0]);
		hudCamera->addChild(osgVirtualAssembly::CustomDraggerManager::Instence()->getRoot());
		m_sceneView->getView(0)->addSlave(hudCamera.get(), false);
		_hudCamera = new osgDragger::HudCamera(hudCamera.get(), 60, 60);//左下角坐标系
	}
	else
	{
		osgVirtualAssembly::CustomDraggerManager::Instence()->getRoot()->getOrCreateStateSet()->setAttributeAndModes(new osgDragger::ClearDepth);
		osgVirtualAssembly::CustomDraggerManager::Instence()->getRoot()->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		osgVirtualAssembly::CustomDraggerManager::Instence()->getRoot()->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		m_sceneModel->getScene()->asSwitch()->addChild(osgVirtualAssembly::CustomDraggerManager::Instence()->getRoot());
		_hudCamera = new osgDragger::HudCamera(m_sceneView->getView(0)->getCamera(), 60, 60);//左下角坐标系
	}
	
	osgDragger::MoveDragger* move = new osgDragger::MoveDragger;
	move->setName("move");
	move->setIntersectionMask(0x2);
	move->setIntersectionCamera(hudCamera.get());
	move->setupDefaultGeometry();
	move->setHandleEvents(true);

	osgVirtualAssembly::CustomDraggerManager::Instence()->addDragger(move, true);
	osgVirtualAssembly::CustomDraggerManager::Instence()->setIntersectionMask(0x2);

	osgDragger::RotateDragger* rotate = new osgDragger::RotateDragger;
	rotate->setName("rotate");
	rotate->setIntersectionMask(0x2);
	rotate->setIntersectionCamera(hudCamera.get());
	rotate->setupDefaultGeometry();
	rotate->setHandleEvents(true);
	osgVirtualAssembly::CustomDraggerManager::Instence()->addDragger(rotate, false);
	osgVirtualAssembly::CustomDraggerManager::Instence()->setIntersectionMask(0x2);
#if 0
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(), 5.0f)));
	SelectionList sl;
	{
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		mt->setName("mt");
		mt->addChild(geode.get());
		sl.push_back(mt.get());
		m_sceneModel->getScene()->asSwitch()->addChild(mt.get());
	}
	{
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		mt->setName("mt");
		mt->setMatrix(osg::Matrix::translate(osg::Vec3(20,0,0)));
		mt->addChild(geode.get());
		m_sceneModel->getScene()->asSwitch()->addChild(mt.get());
	}
	CustomDraggerManager::Instence()->setSelections(sl);
#endif

	m_sceneView->getView(0)->setCameraManipulator(new osgGA::TrackballManipulator);
	//fengtest end
}

MainWindow::~MainWindow()
{
	saveSettings();

	if (m_pCollisionDlg!=NULL)
	{
		delete m_pCollisionDlg;
		m_pCollisionDlg = NULL;
	}

	if (NodeDlg!=NULL)
	{
		delete NodeDlg;
		NodeDlg = NULL;
	}

	if (m_pSettingNodeGroupDlg!=NULL)
	{
		delete m_pSettingNodeGroupDlg;
		m_pSettingNodeGroupDlg = NULL;
	}

	if (m_pSlightAdjustDlg!=NULL)
	{
		delete m_pSlightAdjustDlg;
		m_pSlightAdjustDlg = NULL;
	}

	if (m_pRotateTestDlg!=NULL)
	{
		delete m_pRotateTestDlg;
		m_pRotateTestDlg = NULL;
	}

	if (m_productionTable!=NULL)
	{
		delete m_productionTable;
		m_productionTable = NULL;
	}

	if (m_twTable!=NULL)
	{
		delete m_twTable;
		m_twTable = NULL;
	}

	if (m_pReplaceWheelModelDlg!=NULL)
	{
		delete m_pReplaceWheelModelDlg;
		m_pReplaceWheelModelDlg = NULL;
	}

	if (m_pSettingAnimationDlg!=NULL)
	{
		delete m_pSettingAnimationDlg;
		m_pSettingAnimationDlg = NULL;
	}

	if (m_pNormalUserWidget!=NULL)
	{
		delete m_pNormalUserWidget;
		m_pNormalUserWidget = NULL;
	}

	for (size_t i=0;i<m_vSTLs.size();i++)
	{
		delete m_vSTLs[i];
		m_vSTLs[i] = NULL;
	}
	for (size_t i=0;i<m_vOriginalSTLs.size();i++)
	{
		delete m_vOriginalSTLs[i];
		m_vOriginalSTLs[i] = NULL;
	}


	for (size_t j=0;j<m_vNewWheelModel.size();j++)
	{
		delete m_vNewWheelModel[j];
		m_vNewWheelModel[j] = NULL;
	}

	for(size_t i=0;i<m_Groups.size();i++)
	{
		delete m_Groups[i];//释放组对象
	}

	if (m_pTree!=NULL)
	{
		delete m_pTree;
		m_pTree = NULL;
	}
}

int MainWindow::GetLMLayerByName(QString str)
{
	int iLayer;
	for (size_t i=0;i<m_vSTLs.size();i++)
	{
		if (str == m_vSTLs[i]->GetSTLName())
		{
			iLayer = m_vSTLs[i]->GetCurLayer();
		}
	}

	return iLayer;
}

std::map<QString,std::vector<QString>> MainWindow::GetSortNameToJT1LMNamesMap()
{
	return m_MapSortNameTovJT1LMNames;
}

std::map<QString,std::vector<QString>> MainWindow::GetSortNameToJT1JMNamesMap()
{
	return m_MapSortNameTovJT1JMNames;
}

std::map<int,std::vector<QString>> MainWindow::ReadSortFile(QString strPathFile)
{
	std::map<int,std::vector<QString>> tmpMap;
	m_MapSortNameTovJT1JMNames.clear();
	QByteArray ba = strPathFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		//memset(buf, 0, maxChar*sizeof(wchar_t));
		//fp.getline(buf, maxChar);
		int num=0;
		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;	
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);
			num++;

			std::vector<QString> vtmp;
			for (size_t i=1;i<Res.size();i++)
			{
				QString strname = QString::fromLocal8Bit(Res[i].c_str()); 
				vtmp.push_back(strname);
			}


			tmpMap[num] = vtmp;

			QString strsortName = QString::fromLocal8Bit(Res[0].c_str()); 
			m_MapSortNameTovJT1JMNames[strsortName] = vtmp;

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	m_MapTovJT1JMSort = tmpMap;

	return tmpMap;

}

std::map<int,std::vector<QString>> MainWindow::ReadLMSortFile(QString strPathFile)
{
	std::map<int,std::vector<QString>> tmpMap;
	m_MapSortNameTovJT1LMNames.clear();
	QByteArray ba = strPathFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		//memset(buf, 0, maxChar*sizeof(wchar_t));
		//fp.getline(buf, maxChar);
		int num=0;
		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;	
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);
			num++;
			if (Res.size()<2)
			{
				continue;
			}

			std::vector<QString> vtmp;
			for (size_t i=1;i<Res.size();i++)
			{
				QString strname = QString::fromLocal8Bit(Res[i].c_str()); 
				vtmp.push_back(strname);
			}


			tmpMap[num] = vtmp;

			QString strsortName = QString::fromLocal8Bit(Res[0].c_str()); 
			m_MapSortNameTovJT1LMNames[strsortName] = vtmp;

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();
	m_MapTovJT1LMSort = tmpMap;

	return tmpMap;

}

void MainWindow::RebuildProductionTable()
{
	//加工值表格
	m_productionTable->DeleteAllValueRow();
	//m_productionTable->setRowCount(0);
	//m_productionTable->clearContents();
	for(int i=0;i<m_param.imn+1;i++)
	{
		m_productionTable->addRowValue(18,QStringList()<<QString::number(i+1)<<QString::number(m_vproductionvalues[i].fmy)<<QString::number(m_vproductionvalues[i].fmb)<<QString::number(m_vproductionvalues[i].fmc)<<QString::number(m_vproductionvalues[i].fmm));
	}

	//推弯表格
	SetTwValue();
}

std::map<QString,QString> MainWindow::GetModelToType()
{
	return m_MapModelNameToType;
}

void MainWindow::ReadModelColorFileValue()
{
	m_MapModelNameToRgbColor.erase(m_MapModelNameToRgbColor.begin(),m_MapModelNameToRgbColor.end());
	char display[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, STL_PATH, 3, display);
	GetWholePath(display);
	std::string tmp = display;
	QString displayPath = QString::fromLocal8Bit(tmp.c_str());
	displayPath = QDir::fromNativeSeparators(displayPath);//"\"转"/"

	QString strFile = displayPath + QString::fromLocal8Bit("/ModelColor.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			if (Res.size() == 4)
			{
				QString modelName = QString::fromLocal8Bit(Res[0].c_str());
				RGB_COLOE c;
				c.r = QString::fromLocal8Bit(Res[1].c_str()).toInt();
				c.g = QString::fromLocal8Bit(Res[2].c_str()).toInt();
				c.b = QString::fromLocal8Bit(Res[3].c_str()).toInt();
				m_MapModelNameToRgbColor[modelName] = c;
			}

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	m_pSettingColor->SetMapValue(m_MapModelNameToRgbColor);
	if (m_MapModelNameToRgbColor.empty() == false)
	{
		std::map<QString,RGB_COLOE>::iterator it = m_MapModelNameToRgbColor.begin();
		for (;it!=m_MapModelNameToRgbColor.end();it++)
		{
			float r = it->second.r/255.0;
			float g = it->second.g/255.0;
			float b = it->second.b/255.0;
			osg::Vec4 color = osg::Vec4(r,g,b,1.0f);
			SetColor(it->first,color);
		}
	}
}

void MainWindow::ReBuildLanguageDefaultFile()
{
	char lang[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LANGUAGE_FILE_PATH, 3, lang);
	GetWholePath(lang);
	std::string tmp = lang;
	QString langPath = QString::fromLocal8Bit(tmp.c_str());
	langPath = QDir::fromNativeSeparators(langPath);//"\"转"/"

	QString strlanfgFile = langPath + QString::fromLocal8Bit("/Default.csv");
	std::string str = std::string((const char *)strlanfgFile.toLocal8Bit().constData());
	std::ofstream outFile;
	outFile.open(str,std::ios::out | std::ios::trunc);
	std::string strLanguage =string(m_strDefaultLanguageType.toLocal8Bit());/* m_strDefaultLanguageType.toStdString();*/

	outFile<<strLanguage<<std::endl;

	outFile.close();
}

QString MainWindow::GetDefaultLanguage()
{
	return m_strDefaultLanguageType;
}

void MainWindow::SetQStringLen(QString &str,int len)
{
	if(str.length()>len)
	{
		QString strformer;
		for (int i=0;i<len;i++)
		{
			strformer.append(str.at(i));
		}
		strformer.append("\n");

		QString strlaster;
		for (int j=len;j<str.length();j++)
		{
			strlaster.append(str.at(j));
		}
		SetQStringLen(strlaster,len);
		str = strformer + strlaster;

	}
	else
	{
		int k = len - str.length();
		for (int n=0;n<k;n++)
		{
			str = str + " ";
		}
	}

	return ;
}

void MainWindow::ReadModelDiplayNameFile()
{
	m_ModelNameToDisplayName.erase(m_ModelNameToDisplayName.begin(),m_ModelNameToDisplayName.end());
	char display[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ANIMATION_FILES_PATH, 3, display);
	GetWholePath(display);
	std::string tmp = display;
	QString displayPath = QString::fromLocal8Bit(tmp.c_str());
	displayPath = QDir::fromNativeSeparators(displayPath);//"\"转"/"

	QString strFile = displayPath + QString::fromLocal8Bit("/DisplayModelName.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			//if (Res.size() == 2)
			{
				QString modelName = QString::fromLocal8Bit(Res[0].c_str());
				QString displayName /*= QString::fromLocal8Bit(Res[1].c_str())*/;
				std::vector<QString> vlanguages;
				for(size_t j=1;j<Res.size();j++)
				{
					vlanguages.push_back(QString::fromLocal8Bit(Res[j].c_str()));
				}
				int languagenum = m_NumToDefalutLanguagePair.first;
				displayName = vlanguages[languagenum-1];
				m_ModelNameToDisplayName[modelName] = displayName;
			}


			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();
}

void MainWindow::SetViewLookAtByFileValue()
{
	char view[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ANIMATION_FILES_PATH, 3, view);
	GetWholePath(view);
	std::string tmp = view;
	QString viewPath = QString::fromLocal8Bit(tmp.c_str());
	viewPath = QDir::fromNativeSeparators(viewPath);//"\"转"/"

	QString strFile = viewPath + QString::fromLocal8Bit("/ViewLookAt.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			if (Res.size() == 9)
			{
				osg::Vec3f eye1,center1,up1;
				eye1.x() = GetPosValueByString(Res[0]);
				eye1.y() = GetPosValueByString(Res[1]);
				eye1.z() = GetPosValueByString(Res[2]);

				center1.x() = GetPosValueByString(Res[3]);
				center1.y() = GetPosValueByString(Res[4]);
				center1.z() = GetPosValueByString(Res[5]);

				up1.x() = GetPosValueByString(Res[6]);
				up1.y() = GetPosValueByString(Res[7]);
				up1.z() = GetPosValueByString(Res[8]);


				//m_sceneView->getOsgView()->getCameraManipulator()->setHomePosition(eye1,center1,up1);
				//m_sceneView->home();

				//m_sceneView->getOsgView()->getCamera()->setViewMatrixAsLookAt(eye1,center1,up1);
			}


			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();
	//std::map<QString,std::pair<int,std::vector<QString>>> a = m_MapDisplayNameToPairModelNames;
}

void MainWindow::SetDefaultLanguage()
{
	char lang[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LANGUAGE_FILE_PATH, 3, lang);
	GetWholePath(lang);
	std::string tmp = lang;
	QString langPath = QString::fromLocal8Bit(tmp.c_str());
	langPath = QDir::fromNativeSeparators(langPath);//"\"转"/"

	QString strlanfgFile = langPath + QString::fromLocal8Bit("/Default.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strlanfgFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 1024;
		wchar_t buf[maxChar];

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if (Res.size() == 1)
			{

				m_strDefaultLanguageType = QString::fromLocal8Bit(Res[0].c_str());
			}
			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	std::vector<std::vector<QString>>::iterator it = m_vLanguagevWords.begin();
	for (;it!=m_vLanguagevWords.end();it++)
	{
		int key = (*it)[0].toInt();
		if ((*it)[1] == m_strDefaultLanguageType)
		{
			m_NumToDefalutLanguagePair = make_pair(key,m_strDefaultLanguageType);
			m_vCurLanguageWords = (*it);
			SetLanguageInMainWindow(m_vCurLanguageWords);
			break;
		}
	}

}

void MainWindow::SetQToolBarSize(int wide,int height)
{
	//ui.fileToolBar->setFixedSize(100,100);


	//ui.actionLight->setIc
	//ui.actionFull_Screen->

	ui.fileToolBar->setIconSize(QSize(wide,height));
	ui.sceneToolBar->setIconSize(QSize(wide,height));

	//ui.actionLight->setFixedSize(50,50);
}

float MainWindow::GetBiggestLMRadius(int iNthJT)
{
	float biggestR = 0;
	if (iNthJT == 1)
	{
		std::map<int,float>::iterator it = m_MapJT1LayerToR.begin();

		for (;it!=m_MapJT1LayerToR.end();it++)
		{
			if (it->second > biggestR)
			{
				biggestR = it->second;
			}
		}
	}

	return biggestR;

}

void MainWindow::BuildLayerToRadiusFile(std::map<int,float> MapJTLayerToR,int iNth)
{
	QString strFileName;

	if (iNth == 1)
	{
		strFileName = need_filePath + QString::fromLocal8Bit("/JT1/JT1_LayerToRadius.csv");

	}
	else
	{
		strFileName = need_filePath + QString::fromLocal8Bit("/JT2/JT2_LayerToRadius.csv");

	}


	std::map<int,float>::iterator it;
	std::ofstream outFile;

	std::string str = std::string((const char *)strFileName.toLocal8Bit().constData());
	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"Layer"<<','<<"Radius"<<std::endl;


	//写数据
	it = MapJTLayerToR.begin();

	for (;it!=MapJTLayerToR.end();it++)
	{
		std::string sNth = QString::number(it->first).toStdString();
		std::string strDR = QString::number(it->second).toStdString();

		outFile<<sNth<<','<<strDR<<std::endl;				
	}

	outFile.close();
}

void MainWindow::BuildSumOfTwoRadius(std::map<int,float> MapJTLayerToR,int iNth)
{

	if (MapJTLayerToR.size()==0)
	{
		return;
	}

	QString strFileName;

	if (iNth == 1)
	{
		strFileName = need_filePath + QString::fromLocal8Bit("/JT1/JT1_SRadius.csv");

	}
	else
	{
		strFileName = need_filePath + QString::fromLocal8Bit("/JT2/JT2_SRadius.csv");

	}


	std::map<int,float>::iterator it;
	std::ofstream outFile;

	std::string str = std::string((const char *)strFileName.toLocal8Bit().constData());
	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"Layer"<<',';
	it = MapJTLayerToR.begin();
	auto end =MapJTLayerToR.end();
	end--;
	for (;it!=MapJTLayerToR.end();it++)
	{
		std::string sNth = QString::number(it->first).toStdString();
		if (it == end)
		{
			outFile<<sNth<<std::endl;							
		}
		else
		{
			outFile<<sNth<<',';
		}
	}

	//写数据
	it = MapJTLayerToR.begin();
	for (;it!=MapJTLayerToR.end();it++)
	{
		std::string sNth = QString::number(it->first).toStdString();
		outFile<<sNth<<',';

		std::string strDR;
		std::map<int,float>::iterator iter = MapJTLayerToR.begin();
		auto end2 =MapJTLayerToR.end();
		end2--;
		for (;iter!=MapJTLayerToR.end();iter++)
		{
			float fdr = it->second + iter->second;
			strDR =QString::number(fdr).toStdString();
			if (iter == end2)
			{
				outFile<<strDR<<std::endl;					
			}
			else
			{
				outFile<<strDR<<',';
			}
		}	
	}

	outFile.close();
}

void MainWindow::BuildSemidiameterFile(std::map<int,float> MapJTLayerToR,int iNth)
{

	if (MapJTLayerToR.size()==0)
	{
		return;
	}

	QString strFileName;

	if (iNth == 1)
	{
		strFileName = need_filePath + QString::fromLocal8Bit("/JT1/JT1_DRadius.csv");

	}
	else
	{
		strFileName = need_filePath + QString::fromLocal8Bit("/JT2/JT2_DRadius.csv");

	}


	std::map<int,float>::iterator it;
	std::ofstream outFile;

	std::string str = std::string((const char *)strFileName.toLocal8Bit().constData());
	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"Layer"<<',';
	it = MapJTLayerToR.begin();
	auto end =MapJTLayerToR.end();
	end--;
	for (;it!=MapJTLayerToR.end();it++)
	{
		std::string sNth = QString::number(it->first).toStdString();
		if (it == end)
		{
			outFile<<sNth<<std::endl;							
		}
		else
		{
			outFile<<sNth<<',';
		}
	}

	//写数据
	it = MapJTLayerToR.begin();
	for (;it!=MapJTLayerToR.end();it++)
	{
		std::string sNth = QString::number(it->first).toStdString();
		outFile<<sNth<<',';

		std::string strDR;
		std::map<int,float>::iterator iter = MapJTLayerToR.begin();
		auto end2 =MapJTLayerToR.end();
		end2--;
		for (;iter!=MapJTLayerToR.end();iter++)
		{
			float fdr = it->second - iter->second;
			strDR =QString::number(fdr).toStdString();
			if (iter == end2)
			{
				outFile<<strDR<<std::endl;					
			}
			else
			{
				outFile<<strDR<<',';
			}

		}	
	}

	outFile.close();
}

void MainWindow::SetAllJTLayerToRadiusAtLoad(std::map<int,std::map<QString,std::vector<QString>>> values)
{
	//判断子成员
	std::map<int,std::map<QString,std::vector<QString>>>::iterator iter = values.begin();

	for (;iter!=values.end();++iter)
	{
		if (iter->first == 1)
		{
			std::vector<STL*> vstls;
			std::map<QString,std::vector<QString>> vModels = iter->second;
			std::map<QString,std::vector<QString>>::iterator it = vModels.begin();
			for (;it!=vModels.end();it++)
			{
				if (it->first == "CENTER")
				{
					//m_LayerNum = it->second.size();
					vstls = GetSideSTLsBySideName(it->second);
					//SetJT1LayerValues(vstls);

					std::vector<STL*> needSTLs;
					for (size_t j=0;j<vstls.size();j++)
					{
						if (vstls[j]->GetIsShow() == true)
						{
							needSTLs.push_back(vstls[j]);
						}
					}

					int max=1;
					for (size_t k=0;k<needSTLs.size();k++)
					{
						int itmp = needSTLs[k]->GetCurLayer();
						if (itmp>max)
						{
							max = itmp;
						}
					}

					m_LayerNum = max;
					SetJT1LayerValues(needSTLs);
				}
			}
		}
	}
}

void MainWindow::SetJT1LayerValues(std::vector<STL*> stl)
{
	m_MapJT1LayerToR.erase(m_MapJT1LayerToR.begin(),m_MapJT1LayerToR.end());

	for(size_t i=0;i<stl.size();i++)
	{

		if (stl[i]->GetCanChanged() == 1)
		{
			int iLayer = stl[i]->GetCurLayer();
			float fRadius = stl[i]->GetLMr();
			if (stl[i]->GetIsShow() == 1)
			{
				m_MapJT1LayerToR[iLayer] = fRadius;
			}
		}
	}

	//BuildSemidiameterFile(m_MapJT1LayerToR,1);
	BuildSumOfTwoRadius(m_MapJT1LayerToR,1);
	//BuildLayerToRadiusFile(m_MapJT1LayerToR,1);
}

void MainWindow::SetJT2LayerValues(std::vector<STL*> stl)
{
	m_MapJT2LayerToR.erase(m_MapJT2LayerToR.begin(),m_MapJT2LayerToR.end());

	for(size_t i=0;i<stl.size();i++)
	{
		int iLayer = stl[i]->GetCurLayer();
		float fRadius = stl[i]->GetLMr();
		m_MapJT2LayerToR[iLayer] = fRadius;
	}
}

float MainWindow::GetJT1CurLayerLMRadius(int layer)
{
	std::map<int,float>::iterator it;
	it = m_MapJT1LayerToR.find(layer);
	if (it!=m_MapJT1LayerToR.end())
	{
		return it->second;
	}
	else
	{
		return m_param.LmRadius[0];
		//return -10000.0;
	}
}

std::vector<STL*> MainWindow::GetAllSTLs()
{
	std::vector<STL*> a = m_vSTLs;
	return m_vSTLs;
}

Param MainWindow::GetParamFileValues()
{
	Param a = m_param;
	return m_param;
}

void MainWindow::GetLayerToR(std::vector<LMR> values)
{
	std::vector<QString> vnames;
	for (size_t i=0;i<values.size();i++)
	{
		vnames.push_back(values[i].strModelName);
	}

	std::vector<STL*> vstls = GetSideSTLsBySideName(vnames);
	std::vector<STL*> needSTLs;
	for (size_t j=0;j<vstls.size();j++)
	{
		if (vstls[j]->GetIsShow() == true)
		{
			if (vstls[j]->GetCanChanged() == 1)
			{
				needSTLs.push_back(vstls[j]);
			}
		}
	}

	int max=1;
	for (size_t k=0;k<needSTLs.size();k++)
	{
		int itmp = needSTLs[k]->GetCurLayer();
		if (itmp>max)
		{
			max = itmp;
		}
	}

	m_LayerNum = max;
	SetJT1LayerValues(needSTLs);
}

void MainWindow::InitNormalUserPadWidget()
{
	//获得widget的palette
	//m_NormalPadWidget->setAttribute(Qt::WA_StyledBackground,true);
	//m_NormalPadWidget->setStyleSheet("background-color: rgb(255,255, 255)");
	/*QPalette palette = m_NormalPadWidget->palette();
	palette.setColor(QPalette::Background, Qt::blue);
	m_NormalPadWidget->setPalette(palette);      
	*/

	QString imagepath;
	QString fileName = QCoreApplication::applicationDirPath();		
	QString str;
	str = str.fromLocal8Bit("↑");
	m_PadUpMoveBtn = new QPushButton();
	m_PadUpMoveBtn->setFixedSize(50,50);
	m_PadUpMoveBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/Up.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/Up-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/Up-2.png);}");

	//imagepath = /*fileName +*/ "E:/Osg/osg3DViewer-master/proj/bin/Debug/stop.png";
	//QPixmap image0(imagepath);
	//QPixmap image = image0.scaled(QSize(this->width(),image0.height()));
	//m_PadUpMoveBtn->setIcon(QIcon(image0));

	str = str.fromLocal8Bit("↓");
	m_PadDownMoveBtn = new QPushButton();
	m_PadDownMoveBtn->setFixedSize(50,50);
	m_PadDownMoveBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/Down.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/Down-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/Down-2.png);}");

	str = str.fromLocal8Bit("←");
	m_PadLeftMoveBtn = new QPushButton();
	m_PadLeftMoveBtn->setFixedSize(50,50);
	m_PadLeftMoveBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/Left.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/Left-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/Left-2.png);}");

	str = str.fromLocal8Bit(" ");
	m_PadRightMoveBtn = new QPushButton();
	//	QPixmap pixmap(":/osg3DViewer/right.png");
	//	m_PadRightMoveBtn->setIcon(QIcon(pixmap));
	//	m_PadRightMoveBtn->setIconSize(QSize(48,48));
	m_PadRightMoveBtn->setFixedSize(50,50);
	// m_PadRightMoveBtn->setStyleSheet/*("border-image:url(:/osg3DViewer/Right.png)");*/
	m_PadRightMoveBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/Right.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/Right-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/Right-2.png);}");

	str = str.fromLocal8Bit("+");
	m_PadZoomInBtn = new QPushButton();
	m_PadZoomInBtn->setFixedSize(50,50);
	m_PadZoomInBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/ZoomIn.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/ZoomIn-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/ZoomIn-2.png);}");

	str = str.fromLocal8Bit("-");
	m_PadZoomOutBtn = new QPushButton();
	m_PadZoomOutBtn->setFixedSize(50,50);
	m_PadZoomOutBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/ZoomOut.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/ZoomOut-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/ZoomOut-2.png);}");

	str = str.fromLocal8Bit("CW");
	m_PadCWRotateBtn = new QPushButton();
	m_PadCWRotateBtn->setFixedSize(50,50);
	m_PadCWRotateBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/S.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/S-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/S-2.png);}");

	str = str.fromLocal8Bit("CCW");
	m_PadCCWRotateBtn = new QPushButton();
	m_PadCCWRotateBtn->setFixedSize(50,50);
	m_PadCCWRotateBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/N.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/N-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/N-2.png);}");

	str = str.fromLocal8Bit("Resume");
	m_PadResumeBtn = new QPushButton();
	m_PadResumeBtn->setFixedSize(50,50);
	m_PadResumeBtn->setStyleSheet("QPushButton{border-image: url(:/osg3DViewer/Resume.png);}"
		"QPushButton:hover{border-image: url(:/osg3DViewer/Resume-1.png);}"
		"QPushButton:pressed{border-image: url(:/osg3DViewer/Resume-2.png);}");


	QGridLayout *pBtn = new QGridLayout(m_NormalPadWidget);
	pBtn->addWidget(m_PadZoomInBtn,0,0,1,1);
	pBtn->addWidget(m_PadZoomOutBtn,0,1,1,1);
	pBtn->addWidget(m_PadUpMoveBtn,1,0,1,1);
	pBtn->addWidget(m_PadDownMoveBtn,1,1,1,1);
	pBtn->addWidget(m_PadLeftMoveBtn,2,0,1,1);
	pBtn->addWidget(m_PadRightMoveBtn,2,1,1,1);
	pBtn->addWidget(m_PadCWRotateBtn,3,0,1,1);
	pBtn->addWidget(m_PadCCWRotateBtn,3,1,1,1);
	pBtn->addWidget(m_PadResumeBtn,4,0,1,1);

	m_NormalPadWidget->setLayout(pBtn);

	connect(m_PadZoomInBtn, SIGNAL(clicked()), this, SLOT(SlotZoomIn()));
	connect(m_PadZoomOutBtn,SIGNAL(clicked()),this,SLOT(SlotZoomOut()));
	connect(m_PadUpMoveBtn, SIGNAL(clicked()), this, SLOT(SlotUpMove()));
	connect(m_PadDownMoveBtn,SIGNAL(clicked()),this,SLOT(SlotDownMove()));
	connect(m_PadLeftMoveBtn, SIGNAL(clicked()), this, SLOT(SlotLeftMove()));
	connect(m_PadRightMoveBtn,SIGNAL(clicked()),this,SLOT(SlotRightMove()));
	connect(m_PadCWRotateBtn,SIGNAL(clicked()),this,SLOT(SlotCWRotate()));
	connect(m_PadCCWRotateBtn,SIGNAL(clicked()),this,SLOT(SlotCCWRotate()));
	connect(m_PadResumeBtn,SIGNAL(clicked()),this,SLOT(SlotResume()));
}

void MainWindow::HideAction(bool var)
{
	//ui.actionTexture->setVisible(var);
	//ui.actionShadow->setVisible(var);
	//ui.actionFull_Screen->setVisible(var);
	//ui.actionHighLight->setVisible(var);
	//ui.actionBackFace->setVisible(var);

	ui.sceneToolBar->setVisible(var);
}

void MainWindow::ReadCollisionDectionFile()
{
	m_MapDisplayNameToPairModelNames.erase(m_MapDisplayNameToPairModelNames.begin(),m_MapDisplayNameToPairModelNames.end());
	m_MapCollisonDectionFileDisplayNameToLanguage.clear();
	char collision[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ANIMATION_FILES_PATH, 3, collision);
	GetWholePath(collision);
	std::string tmp = collision;
	QString collisionPath = QString::fromLocal8Bit(tmp.c_str());
	collisionPath = QDir::fromNativeSeparators(collisionPath);//"\"转"/"

	QString strFile = collisionPath + QString::fromLocal8Bit("/CollisionDetection.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			std::pair<int,std::vector<QString>> tmpPair;
			std::vector<std::pair<QString,int>> vPairs;
			for(size_t i=1;i<Res.size();i+=2)
			{
				QString name = QString::fromLocal8Bit(Res[i].c_str());
				vPairs.push_back(std::make_pair(name,QString(Res[i+1].c_str()).toInt()));
			}

			QString key = Res[0].c_str();
			m_MapDisplayNameToPairModelNames[key] = vPairs;

			//diaplay to language
			/*std::vector<QString> vlanguage;
			for (int j=pos+1;j<Res.size();j++)
			{
			QString strlanguage = QString::fromLocal8Bit(Res[j].c_str());

			vlanguage.push_back(strlanguage);
			}*/

			//int curDisplaylanguage = m_NumToDefalutLanguagePair.first;
			//QString disName = vlanguage[curDisplaylanguage-1];
			//m_MapDisplayNameToPairModelNames[disName] = tmpPair;
			//m_MapCollisonDectionFileDisplayNameToLanguage[disName] = vlanguage;
			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	std::map<QString,std::vector<std::pair<QString,int>>> a = m_MapDisplayNameToPairModelNames;

	m_pCollisionDlg->SetTable(m_MapDisplayNameToPairModelNames,m_MapSymbolToCurLanguage,
		&m_MapCollisionPair,&m_MapCollisionModel);

}

void MainWindow::ReadAlterModelName()
{
	m_MapModelNameToAlterModelName.clear();

	QString filepath = need_filePath + QString::fromLocal8Bit("/Alter_CollisionDetection.csv");
	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();

	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t* buf = new wchar_t[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);
			if (Res.size() == 2)
			{
				QString modelname = QString::fromLocal8Bit(Res[0].c_str());
				QString altername = QString::fromLocal8Bit(Res[1].c_str());

				m_MapModelNameToAlterModelName[modelname] = altername;
			}

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}

		delete []buf;
	}

	fp.close();

	//构造m_vAlterModelSTL
	// SetAlterSTL();


}

void MainWindow::SetAlterSTL()
{
	for(size_t i=0;i<m_vAlterModelSTL.size();i++)
	{
		delete m_vAlterModelSTL[i];
		m_vAlterModelSTL[i] = NULL;
	}
	m_vAlterModelSTL.clear();

	char alter[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, STL_PATH, 3, alter);
	GetWholePath(alter);
	std::string tmpalter = alter;
	QString tmpalterPath = QString::fromLocal8Bit(tmpalter.c_str());
	tmpalterPath = QDir::fromNativeSeparators(tmpalterPath);//"\"转"/"
	tmpalterPath = tmpalterPath + QString::fromLocal8Bit("/AlterModels/");

	std::map<QString,QString>::iterator it = m_MapModelNameToAlterModelName.begin();
	for (;it!=m_MapModelNameToAlterModelName.end();it++)
	{


		QString file = tmpalterPath + it->second;

		if (file.isEmpty() || !QFileInfo(file).exists())
		{
			//QMessageBox::warning(NULL, "warning", tr("%1 does not exist").arg(file), QMessageBox::Ok);
			LogInfo(QString(("%1 does not exist")).arg(file));
			return;
		}

		m_objectLoader->slotNewObjectToLoad(file);
		osg::ref_ptr<osg::Node> node = m_objectLoader->GetLoadedModel();
		node->setName(it->second.toStdString());
		//m_sceneModel->setData(node);
		// 隐藏做碰撞检测的小模型
		//node->setNodeMask(0);

		osg::ref_ptr<osg::Material> material = new osg::Material;

		STL *alterSTL = new STL(node,material,node->getName());

		m_sceneModel->GetNodeScene()->addChild(alterSTL->GetModelMatrix());

		alterSTL->GetSTLNode()->setNodeMask(0);
		m_vAlterModelSTL.push_back(alterSTL);



		////与原对象矩阵相连
		//for (int a=0;a<m_vSTLs.size();a++)
		//{
		//	if (m_vSTLs[a]->GetSTLName() == it->first)
		//	{
		//		m_vSTLs[a]->GetPosMatrix()->addChild(alterSTL->GetModelMatrix());
		//		m_vSTLs[a]->GetSTLNode()->setNodeMask(0);
		//		alterSTL->GetSTLNode()->setNodeMask(0);
		//	}
		//}

	}
}

void MainWindow::ReadCollisionDectionLanguageFile()
{
	m_MapSymbolToCollisionLanguage.clear();
	char collision[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ANIMATION_FILES_PATH, 3, collision);
	GetWholePath(collision);
	std::string tmp = collision;
	QString collisionPath = QString::fromLocal8Bit(tmp.c_str());
	collisionPath = QDir::fromNativeSeparators(collisionPath);//"\"转"/"

	QString strFile = collisionPath + QString::fromLocal8Bit("/CollisionDetectionLanguage.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			if(Res.empty())
			{
				continue;
			}

			std::vector<QString> vLanguage;
			for (size_t i=1;i<Res.size();i++)
			{
				QString str = QString::fromLocal8Bit(Res[i].c_str());
				vLanguage.push_back(str);
			}

			QString strsymbol = QString::fromLocal8Bit(Res[0].c_str());

			m_MapSymbolToCollisionLanguage[strsymbol] = vLanguage;

			int need = m_NumToDefalutLanguagePair.first;
			m_MapSymbolToCurLanguage[strsymbol] = vLanguage[need-1];
			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	std::map<QString,std::vector<QString>> a = m_MapSymbolToCollisionLanguage;
}

void MainWindow::ReadLanguageFile()
{
	m_vLanguagevWords.clear();

	char lang[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LANGUAGE_FILE_PATH, 3, lang);
	GetWholePath(lang);
	std::string tmp = lang;
	QString langPath = QString::fromLocal8Bit(tmp.c_str());
	langPath = QDir::fromNativeSeparators(langPath);//"\"转"/"

	QString strlanfgFile = langPath + QString::fromLocal8Bit("/Language.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strlanfgFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 1024;
		wchar_t buf[maxChar];

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			std::vector<QString> tmpWords;
			for(size_t i=0;i<Res.size();i++)
			{
				QString str = QString::fromLocal8Bit(Res[i].c_str());
				tmpWords.push_back(str);
			}

			m_vLanguagevWords.push_back(tmpWords);
			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	SetDefaultLanguage();

	// 为了调试，先注释掉 wangwenbo 20241028
	// m_vLanguagevWords.clear();
	//std::vector<std::vector<QString>> a = m_vLanguagevWords;
}

void MainWindow::SetLanguageInMainWindow(std::vector<QString> vCurWords)
{
#if 1
	// m_vCurLanguageWords = vCurWords;


	//for (int i=2;i<=13;i++)
	//{
	//	SetQStringLen(vCurWords[i],6);

	//}
	ui.actionOpen->setText(vCurWords[8]);
	ui.actionUnload->setText(vCurWords[2]);

	ui.actionSave_As->setText(vCurWords[6]);
	ui.actionReset_View->setText(vCurWords[4]);
	ui.actionFull_Screen->setText(vCurWords[11]);
	ui.actionTexture->setText(vCurWords[9]);
	ui.actionLight->setText(vCurWords[3]);	
	ui.actionHighLight->setText(vCurWords[12]);
	ui.actionShadow->setText(vCurWords[10]);
	ui.actionQuit->setText(vCurWords[5]);
	ui.actionOptimize->setText(vCurWords[7]);
	ui.actionLoad_Shader->setText(vCurWords[13]);

	ui.actionEnableNode->setText(vCurWords[101]);
	ui.actionDisableNode->setText(vCurWords[102]);

	TableAction->setText(vCurWords[95]);
	BtnsAction->setText(vCurWords[96]);
	SetViewAsLookAt->setText(vCurWords[97]);

	QStringList header;
	header<<vCurWords[79]
	<<vCurWords[80]
	<<vCurWords[81]
	<<vCurWords[82]
	<<vCurWords[83];
	m_productionTable->setColumnValue(5, header);
	m_productionTable->setFont(QFont("Microsoft YaHe", 10));

	m_pCollisionDlg->SetCollisionDlgLanguage(vCurWords);
	m_pNormalUserWidget->SetNormalUserWidgetLanguage(vCurWords);
	m_pSettingNodeGroupDlg->setGroupNodeDlgLanguage(vCurWords);
	m_pSlightAdjustDlg->SetSlightAdjustLanguage(vCurWords);
	m_pAddModelDlg->SetAccumulateDlgLanguage(vCurWords);
	m_pReplaceWheelModelDlg->setReplaceDlgLanguage(vCurWords);
	m_pRotateTestDlg->SetRotateTestDlgLanguage(vCurWords);	
	m_pLanguageDlg->SetLanguageDlgLanguage(vCurWords);
	m_pSettingColor->SetColorDlgLanguage(vCurWords);

#endif
}

void MainWindow::ResetAllNodeInitPos()
{
	std::map<QString,GroupChildren>::iterator it = m_MapGroupToGroupChildren.begin();
	for(;it!=m_MapGroupToGroupChildren.end();it++)
	{
		GroupChildren temp = it->second;
		it->second.currentPos = osg::Vec3d(temp.fx,temp.fy,temp.fz);
		it->second.m_fAngle = 0.0;
	}
}

void MainWindow::ResetAllModelAbsPos()
{
	std::map<QString,GroupChildren>::iterator it = m_MapGroupToGroupChildren.begin();
	for(;it!=m_MapGroupToGroupChildren.end();it++)
	{
		std::map<QString,std::vector<STL*>>::iterator itt = m_MapGroupToSTL.begin();
		for (;itt!=m_MapGroupToSTL.end();itt++)
		{
			if(it->first == itt->first)
			{
				osg::Vec3 tmpPos(it->second.fx,it->second.fy,it->second.fz);
				int isize = itt->second.size();
				for (int i=0;i<isize;i++)
				{
					itt->second[i]->ResetAbsolutePos(tmpPos);
				}
			}
		}
	}
}

void MainWindow::SetGroupToSTLS()
{
	if (!m_MapGroupToSTL.empty())
	{
		return;
	}

	m_MapGroupToSTL.erase(m_MapGroupToSTL.begin(),m_MapGroupToSTL.end());

	m_Groups.clear();
	for(size_t i=0;i<m_Groups.size();i++)
	{
		delete m_Groups[i];//释放组对象
	}

	//重新存储
	if(!m_MapGroupToGroupChildren.empty())
	{
		std::map<QString,GroupChildren>::iterator it = m_MapGroupToGroupChildren.begin();
		for(;it!=m_MapGroupToGroupChildren.end();it++)
		{
			std::vector<STL*> vtmp;

			std::vector<QString>::iterator iter = it->second.vChildrenNames.begin();
			for(;iter!=it->second.vChildrenNames.end();iter++)
			{
				for(size_t i=0;i<m_vSTLs.size();i++)
				{
					if(*iter == m_vSTLs[i]->GetSTLName())
					{
						osg::Vec3 tmPos(it->second.fx,it->second.fy,it->second.fz);
						m_vSTLs[i]->SetSTLAbsolutePos(tmPos);
						vtmp.push_back(m_vSTLs[i]);
					}
				}
			}

			m_MapGroupToSTL[it->first] = vtmp;
		}
	}

	//生成组对象
	std::map<QString,std::vector<STL*>>::iterator iter = m_MapGroupToSTL.begin();
	for(;iter!=m_MapGroupToSTL.end();iter++)
	{
		xModelGroup *pGroup = new xModelGroup(iter->first,iter->second);
		m_Groups.push_back(pGroup);
	}
}

void MainWindow::ReadLMRFILE()
{
	m_vLMRFileValues.clear();

	QString strLMrFile = need_filePath + QString::fromLocal8Bit("/LMR.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strLMrFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;	
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=7)
			{
				QMessageBox::warning(NULL, "warning", "error with LMR File", QMessageBox::Yes);
				LogInfo("error with LMR File");
				return;
			}	

			LMR tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.fLmr =  GetPosValueByString(Res[1]);	
			tmp.fupLen =  GetPosValueByString(Res[2]);	
			tmp.fdownLen =  GetPosValueByString(Res[3]);	
			tmp.ftuLen =  GetPosValueByString(Res[4]);	
			tmp.iSide = QString::fromStdString(Res[5]).toInt();
			tmp.isShow = QString::fromStdString(Res[6]).toInt();

			m_vLMRFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();
}

void MainWindow::LoadAccumulateModelsFromTheFile()
{
	//m_vAccumulateNewModelFileValues.clear();
	//char lm_h[64]="";
	//GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HEIGHT, 3, lm_h);
	//m_flm_h = atof(lm_h);

	//std::string stmp;
	//QString strtmp;
	//QString strtmpName;
	//GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ANIMATION_FILES_PATH, 3, animation_files_path);
	//GetWholePath(animation_files_path);
	//stmp = animation_files_path;
	//strtmp = QString::fromLocal8Bit(stmp.c_str());
	//strtmpName = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	//need_filePath = strtmpName;


	QString strAccumulateFile = need_filePath + QString::fromLocal8Bit("/AccumulateNewModel.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strAccumulateFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);
			if(Res.size()!=4)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with AccumulateNewModel.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[93];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}	

			AccumulateModel tmp;
			tmp.strNewModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.strCopyModelName = QString::fromStdString(Res[1].c_str()); 
			//tmp.strCopyModelName = QString::fromStdString(Res[12].c_str()); 

			float f = GetPosValueByString(Res[2]);	
			tmp.iUpUints = (int)f;
			tmp.iJTNth = QString::fromStdString(Res[3]).toInt(); 


			m_vAccumulateNewModelFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	//m_vAccumulateFileSTLs
	m_vAccumulateFileSTLs.clear();
	for(size_t i=0;i<m_vAccumulateNewModelFileValues.size();i++)
	{
		size_t j;
		STL* pExistNewSTL;
		for (j=0;j<m_vSTLs.size();j++)
		{
			if(m_vAccumulateNewModelFileValues[i].strNewModelName == m_vSTLs[j]->GetSTLName())
			{
				pExistNewSTL = m_vSTLs[j];
				break;
			}

		}

		if(j<m_vSTLs.size())//newmodel存在,直接上移存在的
		{

		}
		else//不存在  重新生成，再上移
		{

			ResumeAllModelColor();

			//获取目标模型
			STL *CopySTL;
			size_t k;
			for (k=0;k<m_vSTLs.size();k++)
			{
				if(m_vAccumulateNewModelFileValues[i].strCopyModelName == m_vSTLs[k]->GetSTLName())
					break;
			}

			if(k<m_vSTLs.size())//目标存在
			{
				CopySTL = m_vSTLs[k];
			}
			else
			{

				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with AccumulateNewModel.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[93];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;


				//QMessageBox::warning(NULL, "warning", QString::fromLocal8Bit("error with AccumulateNewModelFile's %1").arg(m_vAccumulateNewModelFileValues[i].strCopyModelName), QMessageBox::Yes);
				//return;
			}


			float fuplen,fdownlen;
			if(CopySTL->GetIsNew() == true)
			{
				fuplen = CopySTL->GetLMupLen();
				fdownlen = CopySTL->GetLMdownLen();
			}
			else
			{
				char lm_up[64]="";
				char lm_down[64]="";


				GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_UPLEN, 3, lm_up);
				GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_DOWNLEN, 3, lm_down);

				fuplen = atof(lm_up);
				fdownlen = atof(lm_down);

			}

			//生成一个STLNode
			osg::Vec3 newcenter;
			newcenter = CopySTL->GetCurModelCenter();
			int CopyModelLayer = CopySTL->GetCurLayer();
			int NewModelLayer = CopyModelLayer + m_vAccumulateNewModelFileValues[i].iUpUints;

			float Aunit = fuplen+fdownlen;
			newcenter.y()+=Aunit*m_vAccumulateNewModelFileValues[i].iUpUints;

			osg::ref_ptr<osg::Geode> aGeode;
			osg::ref_ptr<osg::Material> mat;
			CreateSameModel(CopySTL->GetSTLNode(),aGeode,mat);
			STL *newModelSTL = new STL(aGeode.get(),mat,m_vAccumulateNewModelFileValues[i].strNewModelName.toStdString());
			newModelSTL->SetSTLColor(CopySTL->GetSTLColor());
			newModelSTL->SetPosMatrix(newModelSTL->GetPosMatrix());

			m_sceneModel->GetNodeScene()->addChild(newModelSTL->GetModelMatrix());

			osg::ref_ptr<osg::MatrixTransform> tmpMatrix = newModelSTL->GetPosMatrix();
			float ft = Aunit*m_vAccumulateNewModelFileValues[i].iUpUints; 
			tmpMatrix->setMatrix(osg::Matrix::translate(osg::Vec3(0,Aunit*m_vAccumulateNewModelFileValues[i].iUpUints,0)));
			newModelSTL->SetModelCenter(newcenter);//随模型运动的中心
			newModelSTL->SetModelOriginalCenter(newcenter);//原始中心
			newModelSTL->SetModelCopyFatherCenter(CopySTL->GetCurModelCenter());//

			newModelSTL->SetCurUpUnit(m_vAccumulateNewModelFileValues[i].iUpUints);
			newModelSTL->SetLMupLen(CopySTL->GetLMupLen());
			newModelSTL->SetLMdownLen(CopySTL->GetLMdownLen());
			newModelSTL->SetLMr(CopySTL->GetLMr());
			newModelSTL->SetLMShow(CopySTL->GetIsShow());
			newModelSTL->SetLMHOLE_R(CopySTL->GetLMHOLE_R());
			newModelSTL->SetLMtuLen(CopySTL->GetLMtuLen());
			newModelSTL->SetLMTuSide(CopySTL->GetLMTuSide());

			newModelSTL->SetJMCurLen(CopySTL->GetJMCurLen());
			newModelSTL->SetJMOriginalLen(CopySTL->GetJMOriginalLen());
			newModelSTL->SetJMAid(CopySTL->GetJMAid());
			newModelSTL->SetJMAoSide(CopySTL->GetJMAoSide());
			newModelSTL->SetJMLen(CopySTL->GetJMLen());
			newModelSTL->SetJMWide(CopySTL->GetJMWide());

			newModelSTL->SetLMNthJT(CopySTL->GetLMNthJT());

			newModelSTL->SetCurLayer(NewModelLayer);
			newModelSTL->SetCanChanged(1);
			newModelSTL->SetAsSpecialReplaceModel(0);

			m_vSTLs.push_back(newModelSTL);
			m_vAllNodeName.push_back(newModelSTL->GetSTLName());
			m_vAccumulateFileSTLs.push_back(newModelSTL);

			On_modelColor_change(newModelSTL->GetSTLName());
		}
	}

	std::vector<STL*> a = m_vSTLs;
}

void MainWindow::UpdateJMFile(QString ModelName)
{
	QString strJMFile = need_filePath + QString::fromLocal8Bit("/JM.csv");
	//获取当前文件里的所有模型及初始位置
	std::vector<JM> vtmpJMFileValues;
	QByteArray ba = strJMFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=11)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with JM.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[91];//xbc
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}	

			JM tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.strType = QString::fromStdString(Res[1].c_str()); 
			tmp.strType = QString::fromStdString(Res[2].c_str());
			tmp.fUpHeight =  GetPosValueByString(Res[3]);	
			tmp.fDownHeight =  GetPosValueByString(Res[4]);	
			tmp.fLength = GetPosValueByString(Res[5]);
			tmp.fWide = GetPosValueByString(Res[6]);
			tmp.iShow = QString::fromStdString(Res[7]).toInt();
			tmp.iSide = QString::fromStdString(Res[8]).toInt();
			tmp.iJTNth = QString::fromStdString(Res[9]).toInt();
			tmp.iAid = QString::fromStdString(Res[10]).toInt();

			vtmpJMFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	//判断文件里是否有这个模型	
	std::vector<JM>::iterator it = vtmpJMFileValues.begin();
	for(;it!=vtmpJMFileValues.end();it++)
	{
		if((*it).strModelName == ModelName)
		{
			break;
		}
	}
	if(it!=vtmpJMFileValues.end())//已存在，再次修改
	{
		((*it).fUpHeight = (float)m_LMupLen);
		((*it).fDownHeight = (float)m_LMdownLen);
		((*it)).fLength = (float)m_dJMLen;
		((*it)).fWide = (float)m_dJMWide;
		((*it)).iSide = (int)m_JMAoSide;
		((*it)).iJTNth = (float)m_JMJTNth;
		((*it)).iAid = (int)m_JMAid;
	}
	else//新增
	{
		JM newTmp;	
		newTmp.strModelName = ModelName;
		newTmp.fUpHeight = (float)m_LMupLen;
		newTmp.fDownHeight = (float)m_LMdownLen;
		newTmp.fLength = (float)m_dJMLen;
		newTmp.fWide = (float)m_dJMWide;
		newTmp.iShow = 1;
		newTmp.iSide = (int)m_JMAoSide;
		newTmp.iJTNth = (float)m_JMJTNth;
		newTmp.iAid = (int)m_JMAid;

		vtmpJMFileValues.push_back(newTmp);

	}

	//重新生成文件
	ReBuildJMFile(vtmpJMFileValues);
}

void MainWindow::ReBuildJMFile(std::vector<JM> vtmp)
{
	std::ofstream outFile;
	QString file = need_filePath + QString::fromLocal8Bit("/JM.csv");

	std::string str = std::string((const char *)file.toLocal8Bit().constData());

	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"ModelName"<<','<<"JM_Type"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"Length"<<','<<"Wide"<<','<<"IsShow"<<','<<"AoSide"<<','<<"JTNth"<<','<<"IsAid"<<std::endl;
	//写入数据-字符串
	std::vector<JM>::iterator it = vtmp.begin();
	for(;it!=vtmp.end();it++)
	{	
		std::string sup = QString::number((*it).fUpHeight).toStdString();
		std::string sdown = QString::number((*it).fDownHeight).toStdString();
		std::string slen = QString::number((*it).fLength).toStdString();
		std::string swide = QString::number((*it).fWide).toStdString();
		std::string ss = QString::number((*it).iShow).toStdString();
		std::string sside = QString::number((*it).iSide).toStdString();
		std::string snth = QString::number((*it).iJTNth).toStdString();
		std::string said = QString::number((*it).iAid).toStdString();

		std::string sType = string((*it).strType.toLocal8Bit());
		std::string sname = string((*it).strModelName.toLocal8Bit());
		outFile<<sname<<','<<sType<<','<<sup<<','<<sdown<<','<<slen<<','<<swide<<','<<ss<<','<<sside<<','<<snth<<','<<said<<std::endl;
	}
	outFile.close();

	ReBuildJMVector();
}

void MainWindow::ReBuildJMVector()
{
	m_vJMFileValues.clear();

	QString strJMFile = need_filePath + QString::fromLocal8Bit("/JM.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strJMFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=11)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with JM.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[91];//xbc
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}	


			JM tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str());
			tmp.strType = QString::fromStdString(Res[1].c_str());
			tmp.fUpHeight =  GetPosValueByString(Res[2]);	
			tmp.fDownHeight =  GetPosValueByString(Res[3]);	
			tmp.fLength =  GetPosValueByString(Res[4]);	
			tmp.fWide =  GetPosValueByString(Res[5]);	
			tmp.iShow = QString::fromStdString(Res[6]).toInt(); 
			tmp.iSide = QString::fromStdString(Res[7]).toInt(); 
			tmp.iJTNth = QString::fromStdString(Res[8]).toInt(); 
			tmp.iAid = QString::fromStdString(Res[9]).toInt(); 
			tmp.iLayer = QString::fromStdString(Res[10]).toInt();

			m_vJMFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}
	fp.close();

	//m_vLMRFileSTLs
	m_vJMFileSTLs.clear();
	for(size_t i=0;i<m_vJMFileValues.size();i++)
	{
		size_t j;
		for (j=0;j<m_vSTLs.size();j++)
		{
			if(m_vJMFileValues[i].strModelName == m_vSTLs[j]->GetSTLName())
				break;
		}

		if(j<m_vSTLs.size())
		{
			m_vSTLs[j]->SetLMupLen(m_vJMFileValues[i].fUpHeight);
			m_vSTLs[j]->SetLMdownLen(m_vJMFileValues[i].fDownHeight);
			m_vSTLs[j]->SetJMLen(m_vJMFileValues[i].fLength);
			m_vSTLs[j]->SetJMWide(m_vJMFileValues[i].fWide);
			m_vSTLs[j]->SetLMShow(m_vJMFileValues[i].iShow);
			m_vSTLs[j]->SetJMAoSide(m_vJMFileValues[i].iSide);
			m_vSTLs[j]->SetLMNthJT(m_vJMFileValues[i].iJTNth);
			m_vSTLs[j]->SetJMAid(m_vJMFileValues[i].iAid);


			m_vJMFileSTLs.push_back(m_vSTLs[j]);
		}
		else
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("Error with JM.csv!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[91];//xbc
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
	}


	ReBuildJMModel();

	//m_pModifyLMTable->InitJMTable(m_vJMFileValues);

}

void MainWindow::OnlyReadJMFile()
{
	m_vJMFileValues.clear();

	QString strJMFile = need_filePath + QString::fromLocal8Bit("/JM.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strJMFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=10)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with JM.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[91];//xbc
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}	


			JM tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.strType = QString::fromStdString(Res[1].c_str()); 
			tmp.fUpHeight =  GetPosValueByString(Res[2]);	
			tmp.fDownHeight =  GetPosValueByString(Res[3]);	
			tmp.fLength =  GetPosValueByString(Res[4]);	
			tmp.fWide =  GetPosValueByString(Res[5]);	
			tmp.iShow = QString::fromStdString(Res[6]).toInt(); 
			tmp.iSide = QString::fromStdString(Res[7]).toInt(); 
			tmp.iJTNth = QString::fromStdString(Res[8]).toInt(); 
			tmp.iAid = QString::fromStdString(Res[9]).toInt(); 
			tmp.iLayer = QString::fromStdString(Res[10]).toInt();

			m_vJMFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();
}

void MainWindow::ReBuildJMModel()
{

	for (size_t i=0;i<m_vJMFileSTLs.size();i++)
	{
		float a = m_vJMFileSTLs[i]->GetJMLen();
		float b = m_vJMFileSTLs[i]->GetJMWide();

		int side = m_vJMFileSTLs[i]->GetJMAoSide();
		int aid = m_vJMFileSTLs[i]->GetJMAid();
		bool bAid;
		bool AoSide; 
		if(side == 0)//zuo
		{
			AoSide = false;
		}
		else if (side == 1)
		{
			AoSide = true;

		}

		if (aid == 0)//zhu
		{
			bAid = false;
		}
		else if (aid == 1)//fu
		{
			bAid = true;
		}

		Clip * tmpClipModel = new Clip(m_vJMFileSTLs[i]->GetLMupLen(),m_vJMFileSTLs[i]->GetLMdownLen(),m_param.fPipeDiameter/2,40,m_vJMFileSTLs[i]->GetJMLen(),m_vJMFileSTLs[i]->GetJMWide(),AoSide,bAid);

		osg::ref_ptr<osg::Geode> JMModel = tmpClipModel->Create(m_vJMFileSTLs[i]->GetModelCopyFatherCenter(),osg::Vec3(0,0,1));
		JMModel->setName(m_vJMFileSTLs[i]->GetSTLName().toStdString());
		m_vJMFileSTLs[i]->replaceCurNode((osg::Node*)JMModel);
		m_vJMFileSTLs[i]->SetMaterial(tmpClipModel->GetMaterial());
		m_vJMFileSTLs[i]->SetIsNew(true);
		//m_vJMFileSTLs[i]->SetJMAid(aid);

		if (m_vJMFileSTLs[i]->GetSTLName() == "dj2.1.stl")
		{
			int k;
			k=0;
		}
		m_vJMFileSTLs[i]->SetCanChanged(1);

		osg::Vec3 newcenter =(m_vJMFileSTLs[i]->GetSTLNode())->getBound().center()*osg::computeLocalToWorld((m_vJMFileSTLs[i]->GetSTLNode())->getParentalNodePaths()[0]);
		m_vJMFileSTLs[i]->SetModelCenter(newcenter);


		int ishow = m_vJMFileSTLs[i]->GetIsShow();
		if (ishow == 1)
		{
			m_vJMFileSTLs[i]->GetSTLNode()->setNodeMask(-1);
		}
		else if (ishow == 0)
		{
			m_vJMFileSTLs[i]->GetSTLNode()->setNodeMask(0);
		}




		On_modelColor_change(m_vJMFileSTLs[i]->GetSTLName());
	}

	std::vector<STL*> a = m_vSTLs;
}

void MainWindow::ReBuildJMVectorAtLoad()
{
	m_vJMFileValues.clear();

	QString strJMFile = need_filePath + QString::fromLocal8Bit("/JM.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strJMFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=11)
			{

				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with JM.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[91];//xbc
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}	

			JM tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.strType = QString::fromStdString(Res[1].c_str()); 
			tmp.fUpHeight =  GetPosValueByString(Res[2]);	
			tmp.fDownHeight =  GetPosValueByString(Res[3]);	
			tmp.fLength = GetPosValueByString(Res[4]);
			tmp.fWide = GetPosValueByString(Res[5]);
			tmp.iShow = QString::fromStdString(Res[6]).toInt();
			tmp.iSide = QString::fromStdString(Res[7]).toInt();
			tmp.iJTNth = QString::fromStdString(Res[8]).toInt();
			tmp.iAid = QString::fromStdString(Res[9]).toInt();
			tmp.iLayer = QString::fromStdString(Res[10]).toInt();

			m_vJMFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	//m_vJMFileSTLs
	m_vJMFileSTLs.clear();
	for(size_t i=0;i<m_vJMFileValues.size();i++)
	{
		size_t j;
		for (j=0;j<m_vSTLs.size();j++)
		{
			if(m_vJMFileValues[i].strModelName == m_vSTLs[j]->GetSTLName())
				break;
		}

		if(j<m_vSTLs.size())
		{
			m_vSTLs[j]->SetLMupLen(m_vJMFileValues[i].fUpHeight);
			m_vSTLs[j]->SetLMdownLen(m_vJMFileValues[i].fDownHeight);
			m_vSTLs[j]->SetJMLen(m_vJMFileValues[i].fLength);
			m_vSTLs[j]->SetJMWide(m_vJMFileValues[i].fWide);
			m_vSTLs[j]->SetLMShow(m_vJMFileValues[i].iShow);
			m_vSTLs[j]->SetJMAoSide(m_vJMFileValues[i].iSide);
			m_vSTLs[j]->SetLMNthJT(m_vJMFileValues[i].iJTNth);
			m_vSTLs[j]->SetJMAid(m_vJMFileValues[i].iAid);
			m_vSTLs[j]->SetCurLayer(m_vJMFileValues[i].iLayer);

			m_vJMFileSTLs.push_back(m_vSTLs[j]);
		}
		else if (j>m_vSTLs.size())
		{ 
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("error with JM File!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[91];//xbc
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
	}

	ReBuildJMModelAtLoad();

	//m_pModifyLMTable->InitJMTable(m_vJMFileValues);

}

void MainWindow::ReBuildJMModelAtLoad()
{
	for (size_t i=0;i<m_vJMFileSTLs.size();i++)
	{
		int side = m_vJMFileSTLs[i]->GetJMAoSide();
		int aid = m_vJMFileSTLs[i]->GetJMAid();
		bool bAid;
		bool AoSide; 
		if(side == 0)//zuo
		{
			AoSide = false;
		}
		else if (side == 1)
		{
			AoSide = true;

		}

		if (aid == 0)//zhu
		{
			bAid = false;
		}
		else if (aid == 1)
		{
			bAid = true;
		}

		Clip * tmpClipModel = new Clip(m_vJMFileSTLs[i]->GetLMupLen(),m_vJMFileSTLs[i]->GetLMdownLen(),m_param.fPipeDiameter/2,40,m_vJMFileSTLs[i]->GetJMLen(),m_vJMFileSTLs[i]->GetJMWide(),AoSide,bAid);

		osg::ref_ptr<osg::Geode> JMModel = tmpClipModel->Create(m_vJMFileSTLs[i]->GetModelCopyFatherCenter(),osg::Vec3(0,0,1));
		JMModel->setName(m_vJMFileSTLs[i]->GetSTLName().toStdString());
		m_vJMFileSTLs[i]->replaceCurNode((osg::Node*)JMModel);
		m_vJMFileSTLs[i]->SetMaterial(tmpClipModel->GetMaterial());
		m_vJMFileSTLs[i]->SetIsNew(true);

		m_vJMFileSTLs[i]->SetCanChanged(1);

		osg::Vec3 newcenter =(m_vJMFileSTLs[i]->GetSTLNode())->getBound().center()*osg::computeLocalToWorld((m_vJMFileSTLs[i]->GetSTLNode())->getParentalNodePaths()[0]);
		//m_vJMFileSTLs[i]->SetModelOriginalCenter(newcenter);
		//m_vJMFileSTLs[i]->GetScaleMatrix()->setMatrix(osg::Matrixd::scale(1.0, 1.0,m_vJMFileSTLs[i]->GetJMCurLen()/m_vJMFileSTLs[i]->GetJMOriginalLen()));


		int ishow = m_vJMFileSTLs[i]->GetIsShow();
		if (ishow == 1)
		{
			m_vJMFileSTLs[i]->GetSTLNode()->setNodeMask(-1);
		}
		else if (ishow == 0)
		{
			m_vJMFileSTLs[i]->GetSTLNode()->setNodeMask(0);
		}

		On_modelColor_change(m_vJMFileSTLs[i]->GetSTLName());
	}
}

void MainWindow::RebuildJT1JMModel(std::vector<JM> vtmp)
{
	std::vector<STL*> JT1JMSTLs;
	QString modelName;
	QString typeName;
	int specialType;
	for(size_t i=0;i<vtmp.size();i++)
	{
		size_t j;
		for (j=0;j<m_vSTLs.size();j++)
		{
			if(vtmp[i].strModelName == m_vSTLs[j]->GetSTLName())
			{
				modelName = vtmp[i].strModelName;
				typeName = vtmp[i].strType;
				specialType = GetTypeModelIsReplaceSpecialByName(typeName);
				break;
			}
		}

		// 将STL数组里的参数设置为文件保存的参数
		if(j<m_vSTLs.size())
		{
			m_vSTLs[j]->SetLMupLen(vtmp[i].fUpHeight);
			m_vSTLs[j]->SetLMdownLen(vtmp[i].fDownHeight);
			m_vSTLs[j]->SetJMLen(vtmp[i].fLength);
			m_vSTLs[j]->SetJMWide(vtmp[i].fWide);
			m_vSTLs[j]->SetLMShow(vtmp[i].iShow);
			m_vSTLs[j]->SetJMAoSide(vtmp[i].iSide);
			m_vSTLs[j]->SetJMAid(vtmp[i].iAid);
			m_vSTLs[j]->SetLMNthJT(vtmp[i].iJTNth);
			m_vSTLs[j]->SetCanChanged(vtmp[i].iModifiable);
			m_vSTLs[j]->SetType(typeName);
			m_vSTLs[j]->SetSpecialType(specialType);

			//m_vSTLs[j]->SetJMAid(vtmp[i].iAid);

			JT1JMSTLs.push_back(m_vSTLs[j]);
		}
	}

	for (size_t i=0;i<JT1JMSTLs.size();i++)
	{
		int side = JT1JMSTLs[i]->GetJMAoSide();
		int aid = JT1JMSTLs[i]->GetJMAid();

		bool AoSide; 
		bool bAid;

		if(side == 0)//zuo
		{
			AoSide = false;
		}
		else if (side == 1)
		{
			AoSide = true;
		}

		if (aid == 0)//zhu
		{
			bAid = false;
		}
		else if (aid)
		{
			bAid = true;
		}

		// 只显示，不能改变尺寸
		if (JT1JMSTLs[i]->GetCanChanged() == 0)
		{
			int ishow = JT1JMSTLs[i]->GetIsShow();
			if (ishow == 1)
			{
				JT1JMSTLs[i]->GetSTLNode()->setNodeMask(-1);
			}
			else if (ishow == 0)
			{
				JT1JMSTLs[i]->GetSTLNode()->setNodeMask(0);
			}
		}
		else if (JT1JMSTLs[i]->GetCanChanged() == 1)
		{
			modelName = JT1JMSTLs[i]->GetSTLName();
			typeName = JT1JMSTLs[i]->GetType();
			specialType = JT1JMSTLs[i]->GetSpecialType();
			if (specialType == 1)
			{
				// 直接用特殊模型替代掉现有模型，特殊类型模型只有一个实例
				ReplaceBySpecialModel(modelName, typeName);
			}
			else if (specialType == 2)
			{
				// 将特殊类型移动到现有模型位置，特殊模型形状不变，特殊类型可以有多个实例
				ReplaceByNewInstance(modelName, typeName);

				int ishow = JT1JMSTLs[i]->GetIsShow();
				if (ishow == 1)
				{
					JT1JMSTLs[i]->GetSTLNode()->setNodeMask(-1);
				}
				else if (ishow == 0)
				{
					JT1JMSTLs[i]->GetSTLNode()->setNodeMask(0);
				}
			}
			else
			{
				Clip * tmpClipModel = new Clip(JT1JMSTLs[i]->GetLMupLen(),JT1JMSTLs[i]->GetLMdownLen(),m_param.fPipeDiameter*0.5,40,JT1JMSTLs[i]->GetJMLen(),JT1JMSTLs[i]->GetJMWide(),AoSide,bAid);
				osg::ref_ptr<osg::Geode> JMModel = tmpClipModel->Create(JT1JMSTLs[i]->GetModelCopyFatherCenter(),osg::Vec3(0,0,1));
				JMModel->setName(JT1JMSTLs[i]->GetSTLName().toStdString());
				JT1JMSTLs[i]->replaceCurNode((osg::Node*)JMModel);
				JT1JMSTLs[i]->SetMaterial(tmpClipModel->GetMaterial());
				JT1JMSTLs[i]->SetIsNew(true);
				JT1JMSTLs[i]->SetCanChanged(1);
				//osg::Vec3 newcenter =(JT1JMSTLs[i]->GetSTLNode())->getBound().center()*osg::computeLocalToWorld((JT1JMSTLs[i]->GetSTLNode())->getParentalNodePaths()[0]);

				int layer = JT1JMSTLs[i]->GetCurLayer();
				// 设置切换后模型位置
				osg::Vec3 moveValue = GetReplacedModelInitPos(JT1JMSTLs[i]->GetSTLName(),layer)/* - curCenter*/;
				osg::ref_ptr<osg::MatrixTransform> tmpMatrix = JT1JMSTLs[i]->GetPosMatrix();
				//osg::Matrixd mat0 = tmpMatrix->getMatrix();

				// 夹模根据轮模半径平移
				double shift = GetJMShift(JT1JMSTLs[i]->GetSTLName());
				moveValue[0] += shift; 

				osg::Matrixd mat1 = osg::Matrix::translate(moveValue);
				tmpMatrix->setMatrix(mat1);

				//更新模型中心
				JT1JMSTLs[i]->SetModelCenter(moveValue);

				int ishow = JT1JMSTLs[i]->GetIsShow();
				if (ishow == 1)
				{
					JT1JMSTLs[i]->GetSTLNode()->setNodeMask(-1);
				}
				else if (ishow == 0)
				{
					JT1JMSTLs[i]->GetSTLNode()->setNodeMask(0);
				}
			}
		}
	}
}

void MainWindow::RebuildJT1LMModel(std::vector<LMR> vtmp)
{
	m_MapJT1LayerToR.erase(m_MapJT1LayerToR.begin(),m_MapJT1LayerToR.end());

	std::vector<STL*> LMSTLs;
	QString modelName;
	QString typeName;
	int specialType;
	for(size_t i=0;i<vtmp.size();i++)
	{
		size_t j;
		for (j=0;j<m_vSTLs.size();j++)
		{
			if(vtmp[i].strModelName == m_vSTLs[j]->GetSTLName())
			{
				modelName = vtmp[i].strModelName;
				typeName = vtmp[i].strType;
				specialType = GetTypeModelIsReplaceSpecialByName(typeName);
				break;
			}
		}

		if(j<m_vSTLs.size())
		{
			m_vSTLs[j]->SetLMr(vtmp[i].fLmr);
			m_vSTLs[j]->SetLMupLen(vtmp[i].fupLen);
			m_vSTLs[j]->SetLMdownLen(vtmp[i].fdownLen);
			m_vSTLs[j]->SetLMtuLen(vtmp[i].ftuLen);
			m_vSTLs[j]->SetLMShow(vtmp[i].isShow);
			m_vSTLs[j]->SetLMTuSide(vtmp[i].iSide);
			m_vSTLs[j]->SetLMNthJT(vtmp[i].iJTNth);
			m_vSTLs[j]->SetCanChanged(vtmp[i].iModifiable);
			m_vSTLs[j]->SetType(typeName);
			m_vSTLs[j]->SetCurLayer(vtmp[i].iLayer);
			m_vSTLs[j]->SetSpecialType(specialType);

			LMSTLs.push_back(m_vSTLs[j]);
		}
	}

	char hole_r[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HOLE_R, 3, hole_r);
	m_fhole_r = atof(hole_r);
	for (size_t i=0;i<LMSTLs.size();i++)
	{
		osg::Vec3 center = LMSTLs[i]->GetCurModelCenter();
		bool side;
		if(LMSTLs[i]->GetLMTuSide() == 0)
		{
			side = false;
		}
		else if (LMSTLs[i]->GetLMTuSide() == 1)
		{
			side = true;
		}

		if (LMSTLs[i]->GetCanChanged() == 0)
		{
			//显示隐藏
			int ishow = LMSTLs[i]->GetIsShow();
			if (ishow == 1)
			{
				LMSTLs[i]->GetSTLNode()->setNodeMask(-1);
			}
			else if (ishow == 0)
			{
				LMSTLs[i]->GetSTLNode()->setNodeMask(0);
			}
		}
		else if (LMSTLs[i]->GetCanChanged() == 1)
		{
			modelName = LMSTLs[i]->GetSTLName();
			typeName = LMSTLs[i]->GetType();
			specialType = LMSTLs[i]->GetSpecialType();
			if (specialType == 1)
			{
				// 直接用特殊模型替代掉现有模型，特殊类型模型只有一个实例
				ReplaceBySpecialModel(modelName, typeName);
			}
			else if (specialType == 2)
			{
				// 将特殊类型移动到现有模型位置，特殊模型形状不变，特殊类型可以有多个实例
				ReplaceByNewInstance(modelName, typeName);
			}
			else
			{
				WheelModel * tmpWheelModel = new WheelModel(LMSTLs[i]->GetLMupLen(),LMSTLs[i]->GetLMdownLen(),LMSTLs[i]->GetLMr(),m_param.fPipeDiameter/2,m_fhole_r,40,LMSTLs[i]->GetLMtuLen(),20,side,m_bOnlyLeft);
				osg::ref_ptr<osg::Geode> wheelModel = tmpWheelModel->CreateRabbet(LMSTLs[i]->GetModelCopyFatherCenter(),osg::Vec3(0,1,0));
				wheelModel->setName(LMSTLs[i]->GetSTLName().toStdString());
				LMSTLs[i]->replaceCurNode((osg::Node*)wheelModel);
				LMSTLs[i]->SetMaterial(tmpWheelModel->GetMaterial());
				LMSTLs[i]->SetIsNew(true);
				LMSTLs[i]->SetCanChanged(1);

				int ishow = LMSTLs[i]->GetIsShow();
				if (ishow == 1)
				{
					LMSTLs[i]->GetSTLNode()->setNodeMask(-1);
				}
				else if (ishow == 0)
				{
					LMSTLs[i]->GetSTLNode()->setNodeMask(0);
				}

				int ilayer = LMSTLs[i]->GetCurLayer();
				float fRadius = LMSTLs[i]->GetLMr();
				if (LMSTLs[i]->GetIsShow()==1)
				{
					m_MapJT1LayerToR[ilayer] = fRadius;
				}
			}
		}	
	}

	std::vector<STL*> a = m_vSTLs;

	//BuildSemidiameterFile(m_MapJT1LayerToR,1);
	BuildSumOfTwoRadius(m_MapJT1LayerToR,1);
	//BuildLayerToRadiusFile(m_MapJT1LayerToR,1);

}

void MainWindow::ReadNormalUserModels()
{
	/*if (m_pNormalUserWidget!=NULL)
	{
	m_pNormalUserWidget->ReadJT1LeftFileValues();
	}*/
	QString strtmp;
	std::string stmp;
	char sJT1_FILES_PATH[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_FILES_PATH, 3, sJT1_FILES_PATH);
	GetWholePath(sJT1_FILES_PATH);
	stmp = sJT1_FILES_PATH;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	QString JT1Dir;
	JT1Dir = strtmp;

	//QString strFileName = JT1Dir + QString::fromLocal8Bit("/JT1_LEFT.csv");	
	//m_JT1_LFileValues = ReadJT1LeftFileValues(strFileName);//無用

	//strFileName = JT1Dir + QString::fromLocal8Bit("/JT1_RIGHT.csv");	
	//m_JT1_RFileValues = ReadJT1RightFileValues(strFileName);//無用

	//strFileName = JT1Dir + QString::fromLocal8Bit("/JT1_CENTER.csv");	
	//m_JT1_CFileValues = ReadJT1CenterFileValues(strFileName);//無用

	QString strFileName = JT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");
	m_JT1_JMFileValues = ReadJT1LeftFileValues(strFileName);
	RebuildJT1JMModel(m_JT1_JMFileValues);

	strFileName = JT1Dir + QString::fromLocal8Bit("/JT1_LMValues.csv");	
	m_JT1_LMFileValues = ReadJT1CenterFileValues(strFileName);
	if (m_JT1_LMFileValues.empty())
	{
		return;
	}

	RebuildJT1LMModel(m_JT1_LMFileValues);
}

void MainWindow::AddvNeedRebuildLMName(QString str)
{
	if(str =="")
		return;

	size_t i;
	for(i=0;i<m_vSTLs.size();i++)
	{
		if(str == m_vSTLs[i]->GetSTLName())
		{
			break;
		}
	}

	if(i < m_vSTLs.size())
	{
		m_vNeedRebuildLmName.push_back(str);
	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("需要重建的模型不存在!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}

}

void MainWindow::ReBuildLm()
{

#if 0
	m_vNeedRebuildLmName.clear();

	char rebuild1[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName1, 3, rebuild1);
	QString strRebuild1(rebuild1);
	AddvNeedRebuildLMName(strRebuild1);

	char rebuild2[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName2, 3, rebuild2);
	QString strRebuild2(rebuild2);
	AddvNeedRebuildLMName(strRebuild2);

	char rebuild3[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName3, 3, rebuild3);
	QString strRebuild3(rebuild3);
	AddvNeedRebuildLMName(strRebuild3);

	char rebuild4[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName4, 3, rebuild4);
	QString strRebuild4(rebuild4);
	AddvNeedRebuildLMName(strRebuild4);

	char rebuild5[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName5, 3, rebuild5);
	QString strRebuild5(rebuild5);
	AddvNeedRebuildLMName(strRebuild5);

	char rebuild6[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName6, 3, rebuild6);
	QString strRebuild6(rebuild6);
	AddvNeedRebuildLMName(strRebuild6);

	char rebuild7[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName7, 3, rebuild7);
	QString strRebuild7(rebuild7);
	AddvNeedRebuildLMName(strRebuild7);

	char rebuild8[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName8, 3, rebuild8);
	QString strRebuild8(rebuild8);
	AddvNeedRebuildLMName(strRebuild8);

	char rebuild9[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName9, 3, rebuild9);
	QString strRebuild9(rebuild9);
	AddvNeedRebuildLMName(strRebuild9);

	char rebuild10[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ReBuildModelName10, 3, rebuild10);
	QString strRebuild10(rebuild10);
	AddvNeedRebuildLMName(strRebuild10);



	char hole_r[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HOLE_R, 3, hole_r);
	m_fhole_r = atof(hole_r);



	char lm_h[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HOLE_R, 3, lm_h);
	m_flm_h = atof(lm_h);

	/*for (int i=0;i<m_vNeedRebuildLmName.size();i++)
	{
	for(int j=0;j<m_vSTLs.size();j++)
	{
	if(m_vNeedRebuildLmName[i] == m_vSTLs[j]->GetSTLName())
	{

	}
	}
	}*/

#endif

}

void MainWindow::ReBuildLmVector()
{
	m_vLMRFileValues.clear();

	QString strLMrFile = need_filePath + QString::fromLocal8Bit("/LMR.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strLMrFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);
			/*for(vector<string>::iterator iter=Res.begin();iter!=Res.end();iter++)
			{

			}*/
			if(Res.size()!=10)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with LMR.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[90];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}	

			LMR tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.fLmr =  GetPosValueByString(Res[1]);	
			tmp.fupLen =  GetPosValueByString(Res[2]);	
			tmp.fdownLen =  GetPosValueByString(Res[3]);	
			tmp.ftuLen =  GetPosValueByString(Res[4]);	
			tmp.iSide = QString::fromStdString(Res[5]).toInt();
			tmp.isShow = QString::fromStdString(Res[6]).toInt();
			tmp.iJTNth = QString::fromStdString(Res[7]).toInt();
			tmp.iLayer = QString::fromStdString(Res[8]).toInt();

			m_vLMRFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	//m_vLMRFileSTLs
	m_vLMRFileSTLs.clear();
	for(size_t i=0;i<m_vLMRFileValues.size();i++)
	{
		size_t j;
		for (j=0;j<m_vSTLs.size();j++)
		{
			if(m_vLMRFileValues[i].strModelName == m_vSTLs[j]->GetSTLName())
				break;
		}

		if(j<m_vSTLs.size())
		{
			m_vSTLs[j]->SetLMr(m_vLMRFileValues[i].fLmr);
			m_vSTLs[j]->SetLMupLen(m_vLMRFileValues[i].fupLen);
			m_vSTLs[j]->SetLMdownLen(m_vLMRFileValues[i].fdownLen);
			m_vSTLs[j]->SetLMtuLen(m_vLMRFileValues[i].ftuLen);
			m_vSTLs[j]->SetLMShow(m_vLMRFileValues[i].isShow);
			m_vSTLs[j]->SetLMTuSide(m_vLMRFileValues[i].iSide);
			m_vSTLs[j]->SetLMNthJT(m_vLMRFileValues[i].iJTNth);
			m_vSTLs[j]->SetCurLayer(m_vLMRFileValues[i].iLayer);

			m_vLMRFileSTLs.push_back(m_vSTLs[j]);
		}
		else
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("Error with LMR.csv!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[90];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
	}


	ReBuildLMModel();

	//m_pModifyLMTable->InitDlg(m_vLMRFileValues);

}

void MainWindow::OnlyReadLMFile()
{
	m_vLMRFileValues.clear();

	QString strLMrFile = need_filePath + QString::fromLocal8Bit("/LMR.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strLMrFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);
			/*for(vector<string>::iterator iter=Res.begin();iter!=Res.end();iter++)
			{

			}*/
			if(Res.size()!=10)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with LMR.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[90];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}	

			LMR tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.fLmr =  GetPosValueByString(Res[1]);	
			tmp.fupLen =  GetPosValueByString(Res[2]);	
			tmp.fdownLen =  GetPosValueByString(Res[3]);	
			tmp.ftuLen =  GetPosValueByString(Res[4]);	
			tmp.iSide = QString::fromStdString(Res[5]).toInt();
			tmp.isShow = QString::fromStdString(Res[6]).toInt();
			tmp.iJTNth = QString::fromStdString(Res[7]).toInt();



			m_vLMRFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();
}

void MainWindow::ReBuildLmVectorAtLoad()
{
	m_vLMRFileValues.clear();

	QString strLMrFile = need_filePath + QString::fromLocal8Bit("/LMR.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strLMrFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=10)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with LMR.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[90];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}	

			LMR tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.strType = QString::fromStdString(Res[1].c_str()); 
			tmp.fLmr =  GetPosValueByString(Res[2]);	
			tmp.fupLen =  GetPosValueByString(Res[3]);	
			tmp.fdownLen =  GetPosValueByString(Res[4]);	
			tmp.ftuLen =  GetPosValueByString(Res[5]);			
			tmp.iSide = QString::fromStdString(Res[6]).toInt();
			tmp.isShow = QString::fromStdString(Res[7]).toInt();
			tmp.iJTNth = QString::fromStdString(Res[8]).toInt();
			tmp.iLayer = QString::fromStdString(Res[9]).toInt();

			m_vLMRFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}
	fp.close();

	RefreshLMrByParamFile(m_vLMRFileValues);
	RefreshJT1LMValueFile();

	//m_vLMRFileSTLs
	m_vLMRFileSTLs.clear();
	for(size_t i=0;i<m_vLMRFileValues.size();i++)
	{
		size_t j;
		for (j=0;j<m_vSTLs.size();j++)
		{
			if(m_vLMRFileValues[i].strModelName == m_vSTLs[j]->GetSTLName())
				break;
		}

		if(j<m_vSTLs.size())
		{
			m_vSTLs[j]->SetLMr(m_vLMRFileValues[i].fLmr);
			// 设置模型类型
			m_vSTLs[j]->SetType(m_vLMRFileValues[i].strType);
			m_vSTLs[j]->SetLMupLen(m_vLMRFileValues[i].fupLen);
			m_vSTLs[j]->SetLMdownLen(m_vLMRFileValues[i].fdownLen);
			m_vSTLs[j]->SetLMtuLen(m_vLMRFileValues[i].ftuLen);
			m_vSTLs[j]->SetLMShow(m_vLMRFileValues[i].isShow);
			m_vSTLs[j]->SetLMTuSide(m_vLMRFileValues[i].iSide);
			m_vSTLs[j]->SetLMNthJT(m_vLMRFileValues[i].iJTNth);

			m_vLMRFileSTLs.push_back(m_vSTLs[j]);
		}
		else if (j>m_vSTLs.size())
		{ 
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("error with LMR File!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[90];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
	}

	ReBuildLMModelAtLoad();

	//m_pModifyLMTable->InitDlg(m_vLMRFileValues);

}

void MainWindow::ReBuildLMModel()
{
	char hole_r[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HOLE_R, 3, hole_r);
	m_fhole_r = atof(hole_r);

	//char lm_h[64]="";
	//GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HEIGHT, 3, lm_h);
	//m_flm_h = atof(lm_h);

	//osg::Vec3 center(-119.4888507633,650,1088.0019632417);
	for (size_t i=0;i<m_vLMRFileSTLs.size();i++)
	{
		osg::Vec3 center = m_vLMRFileSTLs[i]->GetCurModelCenter();
		bool side;
		if(m_vLMRFileSTLs[i]->GetLMTuSide() == 0)
		{
			side = false;
		}
		else if (m_vLMRFileSTLs[i]->GetLMTuSide() == 1)
		{
			side = true;
		}
		WheelModel * tmpWheelModel = new WheelModel(m_vLMRFileSTLs[i]->GetLMupLen(),m_vLMRFileSTLs[i]->GetLMdownLen(),m_vLMRFileSTLs[i]->GetLMr(),m_param.fPipeDiameter/2,m_fhole_r,40,m_vLMRFileSTLs[i]->GetLMtuLen(),10,side,m_bOnlyLeft);

		osg::ref_ptr<osg::Geode> wheelModel = tmpWheelModel->CreateRabbet(m_vLMRFileSTLs[i]->GetModelCopyFatherCenter(),osg::Vec3(0,1,0));
		wheelModel->setName(/*QString::fromLocal8Bit("NewTest").toStdString()*/m_vLMRFileSTLs[i]->GetSTLName().toStdString());
		m_vLMRFileSTLs[i]->replaceCurNode((osg::Node*)wheelModel);
		m_vLMRFileSTLs[i]->SetMaterial(tmpWheelModel->GetMaterial());
		m_vLMRFileSTLs[i]->SetIsNew(true);
		m_vLMRFileSTLs[i]->SetCanChanged(1);

		//m_vLMRFileSTLs[i]->SetOriginalMaterial(tmpWheelModel->GetMaterial());
		int ishow = m_vLMRFileSTLs[i]->GetIsShow();
		if (ishow == 1)
		{
			m_vLMRFileSTLs[i]->GetSTLNode()->setNodeMask(-1);
		}
		else if (ishow == 0)
		{
			m_vLMRFileSTLs[i]->GetSTLNode()->setNodeMask(0);
		}

		On_modelColor_change(m_vLMRFileSTLs[i]->GetSTLName());
	}

	std::vector<STL*> a = m_vSTLs;
}

void MainWindow::ReBuildLMModelAtLoad()
{
	char hole_r[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HOLE_R, 3, hole_r);
	m_fhole_r = atof(hole_r);

	//char lm_h[64]="";
	//GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HEIGHT, 3, lm_h);
	//m_flm_h = atof(lm_h);

	for (size_t i=0;i<m_vLMRFileSTLs.size();i++)
	{
		bool side;
		if (m_vLMRFileSTLs[i]->GetLMTuSide() == 0)
		{
			side = false;
		}
		else if (m_vLMRFileSTLs[i]->GetLMTuSide() == 1)
		{
			side = true;
		}

		WheelModel * tmpWheelModel = new WheelModel(m_vLMRFileSTLs[i]->GetLMupLen(),m_vLMRFileSTLs[i]->GetLMdownLen(),m_vLMRFileSTLs[i]->GetLMr(),m_param.fPipeDiameter/2,m_fhole_r,40,m_vLMRFileSTLs[i]->GetLMtuLen(),20,side,m_bOnlyLeft);
		osg::ref_ptr<osg::Geode> wheelModel = tmpWheelModel->CreateRabbet(m_vLMRFileSTLs[i]->GetModelCopyFatherCenter(),osg::Vec3(0,1,0));
		wheelModel->setName(m_vLMRFileSTLs[i]->GetSTLName().toStdString());
		m_vLMRFileSTLs[i]->replaceCurNode((osg::Node*)wheelModel);
		m_vLMRFileSTLs[i]->SetMaterial(tmpWheelModel->GetMaterial());
		m_vLMRFileSTLs[i]->SetIsNew(true);
		m_vLMRFileSTLs[i]->SetCanChanged(1);

		int ishow = m_vLMRFileSTLs[i]->GetIsShow();
		if (ishow == 1)
		{
			m_vLMRFileSTLs[i]->GetSTLNode()->setNodeMask(-1);
		}
		else if (ishow == 0)
		{
			m_vLMRFileSTLs[i]->GetSTLNode()->setNodeMask(0);
		}
	}

	ResumeAllModelColor();

	std::vector<STL*> a = m_vSTLs;
}

void MainWindow::UpdateIndexValue(QString& IndexVar,int value)
{
	if (IndexVar=="i")
	{
		m_index = value;
	}
	else if(IndexVar == "i1")
	{
		m_iTwlc1 = value;
	}
	else if (IndexVar == "i2")
	{
		m_iTwlc2 = value;
	}
	else if (IndexVar == "i3")
	{
		//tmpindex = m_iTwlc3;
		m_iTwlc3 = value;
	}
	else if (IndexVar == "i4")
	{
		m_iTwlc4 = value;
	}
	else if (IndexVar == "i5")
	{
		m_iTwlc5 = value;
	}
	else if (IndexVar == "i6")
	{
		m_iTwlc6 = value;
	}
	else if (IndexVar == "i7")
	{
		m_iTwlc7 = value;
	}
	else if (IndexVar == "i8")
	{
		m_iTwlc8 = value;
	}
	else if (IndexVar == "i9")
	{
		m_iTwlc9 = value;
	}
	else if (IndexVar == "i10")
	{
		m_iTwlc10 = value;
	}
	else if (IndexVar == "i11")
	{
		m_iTwlc11 = value;
	}
	else if (IndexVar == "i12")
	{
		m_iTwlc12 = value;
	}
	else if (IndexVar == "i13")
	{
		m_iTwlc13 = value;
	}
	else if (IndexVar == "i14")
	{
		m_iTwlc14 = value;
	}
	else if (IndexVar == "i15")
	{
		m_iTwlc15 = value;
	}
	else if (IndexVar == "i16")
	{
		m_iTwlc16 = value;
	}
	else if (IndexVar == "i17")
	{
		m_iTwlc17 = value;
	}
	else if (IndexVar == "i18")
	{
		m_iTwlc18 = value;
	}
	else if (IndexVar == "i19")
	{
		m_iTwlc19 = value;
	}
	else if (IndexVar == "i20")
	{
		m_iTwlc20 = value;
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("error with Process File!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}
}

void MainWindow::UpdateTWLCINDEX3Value(QString &IndexVar,QString stroperator,QString v0,QString v1)
{
	int value0;
	int value1;

	if (IsNumber(v0.toStdString()))
	{
		value0 = v0.toInt();
	}
	else if (v0 == "i")
	{
		value0 = m_index;
	}
	else if(v0 == "i1")
	{
		value0 = m_iTwlc1;
	}
	else if (v0 == "i2")
	{
		value0 = m_iTwlc2;
	}
	else if (v0 == "i3")
	{
		value0 = m_iTwlc3;
	}
	else if (v0 == "i4")
	{
		value0 = m_iTwlc4;
	}
	else if (v0 == "i5")
	{
		value0 = m_iTwlc5;
	}
	else if (v0 == "i6")
	{
		value0 = m_iTwlc6;
	}
	else if (v0 == "i7")
	{
		value0 = m_iTwlc7;
	}
	else if (v0 == "i8")
	{
		value0 = m_iTwlc8;
	}
	else if (v0 == "i9")
	{
		value0 = m_iTwlc9;
	}
	else if (v0 == "i10")
	{
		value0 = m_iTwlc10;
	}
	else if(v0 == "i11")
	{
		value0 = m_iTwlc11;
	}
	else if (v0 == "i12")
	{
		value0 = m_iTwlc12;
	}
	else if (v0 == "i13")
	{
		value0 = m_iTwlc13;
	}
	else if (v0 == "i14")
	{
		value0 = m_iTwlc14;
	}
	else if (v0 == "i15")
	{
		value0 = m_iTwlc15;
	}
	else if (v0 == "i16")
	{
		value0 = m_iTwlc16;
	}
	else if (v0 == "i17")
	{
		value0 = m_iTwlc17;
	}
	else if (v0 == "i18")
	{
		value0 = m_iTwlc18;
	}
	else if (v0 == "i19")
	{
		value0 = m_iTwlc19;
	}
	else if (v0 == "i20")
	{
		value0 = m_iTwlc20;
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("error with Process File!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}

	if (IsNumber(v1.toStdString()))
	{
		value1 = v1.toInt();
	}
	else if (v1 == "i")
	{
		value1 = m_index;
	}
	else if(v1 == "i1")
	{
		value1 = m_iTwlc1;
	}
	else if (v1 == "i2")
	{
		value1 = m_iTwlc2;
	}
	else if (v1 == "i3")
	{
		value1 = m_iTwlc3;
	}
	else if (v1 == "i4")
	{
		value1 = m_iTwlc4;
	}
	else if (v1 == "i5")
	{
		value1 = m_iTwlc5;
	}
	else if (v1 == "i6")
	{
		value1 = m_iTwlc6;
	}
	else if (v1 == "i7")
	{
		value1 = m_iTwlc7;
	}
	else if (v1 == "i8")
	{
		value1 = m_iTwlc8;
	}
	else if (v1 == "i9")
	{
		value1 = m_iTwlc9;
	}
	else if (v1 == "i10")
	{
		value1 = m_iTwlc10;
	}
	else if(v1 == "i11")
	{
		value1 = m_iTwlc11;
	}
	else if (v1 == "i12")
	{
		value1 = m_iTwlc12;
	}
	else if (v1 == "i13")
	{
		value1 = m_iTwlc13;
	}
	else if (v1 == "i14")
	{
		value1 = m_iTwlc14;
	}
	else if (v1 == "i15")
	{
		value1 = m_iTwlc15;
	}
	else if (v1 == "i16")
	{
		value1 = m_iTwlc16;
	}
	else if (v1 == "i17")
	{
		value1 = m_iTwlc17;
	}
	else if (v1 == "i18")
	{
		value1 = m_iTwlc18;
	}
	else if (v1 == "i19")
	{
		value1 = m_iTwlc19;
	}
	else if (v1 == "i20")
	{
		value1 = m_iTwlc20;
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("error with Process File!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}

	int value = 0;
	//运算符
	if(stroperator == "+")
	{
		value = value0 + value1;
	}
	else if(stroperator == "-")
	{
		value = value0 - value1;
	}
	else if(stroperator == "*")
	{
		value = value0 * value1;
	}
	else if(stroperator == "/")
	{
		value = value0 / value1;
	}
	//else if(stroperator =="=")
	//{
	//	value0 = (int)fvalue;
	//}

	if (IndexVar=="i")
	{
		m_index = value;
	}
	else if(IndexVar == "i1")
	{
		m_iTwlc1 = value;
	}
	else if (IndexVar == "i2")
	{
		m_iTwlc2 = value;
	}
	else if (IndexVar == "i3")
	{
		//tmpindex = m_iTwlc3;
		m_iTwlc3 = value;
	}
	else if (IndexVar == "i4")
	{
		m_iTwlc4 = value;
	}
	else if (IndexVar == "i5")
	{
		m_iTwlc5 = value;
	}
	else if (IndexVar == "i6")
	{
		m_iTwlc6 = value;
	}
	else if (IndexVar == "i7")
	{
		m_iTwlc7 = value;
	}
	else if (IndexVar == "i8")
	{
		m_iTwlc8 = value;
	}
	else if (IndexVar == "i9")
	{
		m_iTwlc9 = value;
	}
	else if (IndexVar == "i10")
	{
		m_iTwlc10 = value;
	}
	else if(IndexVar == "i11")
	{
		m_iTwlc11 = value;
	}
	else if (IndexVar == "i12")
	{
		m_iTwlc12 = value;
	}
	else if (IndexVar == "i13")
	{
		//tmpindex = m_iTwlc3;
		m_iTwlc13 = value;
	}
	else if (IndexVar == "i14")
	{
		m_iTwlc14 = value;
	}
	else if (IndexVar == "i15")
	{
		m_iTwlc15 = value;
	}
	else if (IndexVar == "i16")
	{
		m_iTwlc16 = value;
	}
	else if (IndexVar == "i17")
	{
		m_iTwlc17 = value;
	}
	else if (IndexVar == "i18")
	{
		m_iTwlc18 = value;
	}
	else if (IndexVar == "i19")
	{
		m_iTwlc19 = value;
	}
	else if (IndexVar == "i20")
	{
		m_iTwlc20 = value;
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("error with Process File!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}
}

void MainWindow::UpdateTWLCINDEX(QString &IndexVar,QString stroperator,float fvalue)
{
	int tmpindex;
	if(IndexVar == "i1")
	{
		tmpindex = m_iTwlc1;
	}
	else if (IndexVar == "i2")
	{
		tmpindex = m_iTwlc2;
	}
	else if (IndexVar == "i3")
	{
		tmpindex = m_iTwlc3;
	}
	else if (IndexVar == "i4")
	{
		tmpindex = m_iTwlc4;
	}
	else if (IndexVar == "i5")
	{
		tmpindex = m_iTwlc5;
	}
	else if (IndexVar == "i6")
	{
		tmpindex = m_iTwlc6;
	}
	else if (IndexVar == "i7")
	{
		tmpindex = m_iTwlc7;
	}
	else if (IndexVar == "i8")
	{
		tmpindex = m_iTwlc8;
	}
	else if (IndexVar == "i9")
	{
		tmpindex = m_iTwlc9;
	}
	else if (IndexVar == "i10")
	{
		tmpindex = m_iTwlc10;
	}
	else if(IndexVar == "i11")
	{
		tmpindex = m_iTwlc11;
	}
	else if (IndexVar == "i12")
	{
		tmpindex = m_iTwlc12;
	}
	else if (IndexVar == "i13")
	{
		tmpindex = m_iTwlc13;
	}
	else if (IndexVar == "i14")
	{
		tmpindex = m_iTwlc14;
	}
	else if (IndexVar == "i15")
	{
		tmpindex = m_iTwlc15;
	}
	else if (IndexVar == "i16")
	{
		tmpindex = m_iTwlc16;
	}
	else if (IndexVar == "i17")
	{
		tmpindex = m_iTwlc17;
	}
	else if (IndexVar == "i18")
	{
		tmpindex = m_iTwlc18;
	}
	else if (IndexVar == "i19")
	{
		tmpindex = m_iTwlc19;
	}
	else if (IndexVar == "i20")
	{
		tmpindex = m_iTwlc20;
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("error with Process File!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}


	//运算符
	if(stroperator == "+")
	{
		tmpindex = tmpindex + (int)fvalue;
	}
	else if(stroperator == "-")
	{
		tmpindex = tmpindex - (int)fvalue;
	}
	else if(stroperator == "*")
	{
		tmpindex = tmpindex * (int)fvalue;
	}
	else if(stroperator == "/")
	{
		tmpindex = tmpindex / (int)fvalue;
	}
	else if(stroperator =="=")
	{
		tmpindex = (int)fvalue;
	}

	//更新对应变量索引值
	if(IndexVar == "i1")
	{
		m_iTwlc1 = tmpindex;
	}
	else if (IndexVar == "i2")
	{
		m_iTwlc2 = tmpindex;
	}
	else if (IndexVar == "i3")
	{
		m_iTwlc3 = tmpindex;
	}
	else if (IndexVar == "i4")
	{
		m_iTwlc4 = tmpindex;
	}
	else if (IndexVar == "i5")
	{
		m_iTwlc5 = tmpindex;
	}
	else if (IndexVar == "i6")
	{
		m_iTwlc6 = tmpindex;
	}
	else if (IndexVar == "i7")
	{
		m_iTwlc7 = tmpindex;
	}
	else if (IndexVar == "i8")
	{
		m_iTwlc8 = tmpindex;
	}
	else if (IndexVar == "i9")
	{
		m_iTwlc9 = tmpindex;
	}
	else if (IndexVar == "i10")
	{
		m_iTwlc10 = tmpindex;
	}
	else if(IndexVar == "i11")
	{
		m_iTwlc11 = tmpindex;
	}
	else if (IndexVar == "i12")
	{
		m_iTwlc12 = tmpindex;
	}
	else if (IndexVar == "i13")
	{
		m_iTwlc13 = tmpindex;
	}
	else if (IndexVar == "i14")
	{
		m_iTwlc14 = tmpindex;
	}
	else if (IndexVar == "i15")
	{
		m_iTwlc15 = tmpindex;
	}
	else if (IndexVar == "i16")
	{
		m_iTwlc16 = tmpindex;
	}
	else if (IndexVar == "i17")
	{
		m_iTwlc17 = tmpindex;
	}
	else if (IndexVar == "i18")
	{
		m_iTwlc18 = tmpindex;
	}
	else if (IndexVar == "i19")
	{
		m_iTwlc19 = tmpindex;
	}
	else if (IndexVar == "i20")
	{
		m_iTwlc20 = tmpindex;
	}
}

void MainWindow::ReadOneTWLCFile(QString pathName)
{
	LogInfo("Enter ReadOneTWLCFile !");

	m_vTWLCValues.clear();

	QByteArray ba = pathName.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			//数据
			USES_CONVERSION; //这两句也要加入2
			std::string szBuffer = W2A(buf);//这两句也要加入3
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=4)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = QString::fromLocal8Bit("Error with BendingProcess File!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[89];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}

			TWLC tmp;
			tmp.ftwy = QString::fromStdString(Res[0]).toFloat();
			tmp.ftwbj = QString::fromStdString(Res[1]).toFloat();
			tmp.ftwc = QString::fromStdString(Res[2]).toFloat();
			tmp.ftwb = QString::fromStdString(Res[3]).toFloat();
			m_vTWLCValues.push_back(tmp);
			//m_fallR.push_back(tmp.ftwbj);

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}

		fp.close();
	}
	else
	{
		LogInfo(pathName);
		LogInfo("read this path file failed !");
	}

	LogInfo("Exit ReadOneTWLCFile !");
}

int MainWindow::GetTWLCNUM(int index)
{
	int twlc = (int) m_vproductionvalues[index-2].ftwlc;
	if(twlc == 0)
	{

		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("推弯流程0.csv不存在!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return twlc;


		//QString strWarning;
		//strWarning = strWarning.fromLocal8Bit("推弯流程0.csv不存在!");
		//QMessageBox::warning(this,"",strWarning);
		//return twlc;
	}
	else
		return twlc;
}

void MainWindow::GetValueInGroupSettingFileByGcode(const QString& curCode,float& fvalue)
{
	size_t u = 0;
	for(u=0;u<m_vGcodeFileValues.size();u++)
	{
		if(m_vGcodeFileValues[u].strGCode == curCode)//获取运动属性 和 模型数组
		{

			//if(m_vGcodeFileValues[u].irelative == 0)//相对
			//{
			if(m_vGcodeFileValues[u].iAxis == 0)//X轴
			{
				fvalue = m_vGcodeFileValues[u].vecMoveToPos.x();
			}
			else if(m_vGcodeFileValues[u].iAxis == 1)//Y轴
			{
				fvalue = m_vGcodeFileValues[u].vecMoveToPos.y();

			}
			else if(m_vGcodeFileValues[u].iAxis == 2)//Z轴
			{
				fvalue = m_vGcodeFileValues[u].vecMoveToPos.z();

			}
			//}
			//else if(m_vGcodeFileValues[u].irelative == 1)//绝对，要转成相对（未完成）
			//{

			//	//先转成相对位置
			//	//1模型获得当前位置


			//}

			break;
		}
	}

	if(u == m_vGcodeFileValues.size())
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("流程文件(P1=2)是获取代码定义文件默认数值!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;



		//QString strWarning;
		//strWarning = strWarning.fromLocal8Bit("流程文件(P1=2)，获取代码定义文件默认数值!");
		//QMessageBox::warning(this,"",strWarning);
		//return;
	}
}

std::map<int,std::vector<OnlyReadProcess>>::iterator MainWindow::GetAimReadProcessByAimStep(int istep)
{
	std::map<int,std::vector<OnlyReadProcess>>::iterator  it;
	it = m_MapStepTovOnlyReadProcess.find(istep);
	if(it!=m_MapStepTovOnlyReadProcess.end())
	{
		return it;
	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("跳转索引号错误!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		//return;
	}
}

std::vector<Process> MainWindow::GetAimStepReadProcess(int istep,int index)
{
	std::vector<OnlyReadProcess> tmp;
	std::map<int,std::vector<OnlyReadProcess>>::iterator it;
	it = m_MapStepTovOnlyReadProcess.find(istep);
	if(it!=m_MapStepTovOnlyReadProcess.end())
	{
		tmp=it->second;
	}
	else
	{

		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("选择索引错误!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		//return tmp;
	}

	std::vector<Process> tmpvec;
	for (size_t i=0;i<tmp.size();i++)
	{
		Process tmpProcess;
		std::string strtmp = (tmp[i].strCode).toStdString();
		if(strtmp.find_first_of('M')==0  || strtmp.find_first_of('G')==0 || strtmp.find_first_of('P')==0) //G,M,P
		{
			tmpProcess.strCode = (tmp[i]).strCode;
			tmpProcess.istep = (tmp[i]).istep;
			if((tmp[i]).iDirectly == 0)//间接获取
			{
				if((tmp[i])._valueFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
				{
					if(tmp[i].isInLoop == true)
					{
						(tmp[i])._valueFile.irow = index;
					}
					GetValueByProductionFileRowAndColumn((tmp[i])._valueFile,tmpProcess.fvalue);		
				}
				else if((tmp[i])._valueFile.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
				{
					GetValueByTWLC1FileRowAndColumn((tmp[i])._valueFile,tmpProcess.fvalue);

				}
				else if((tmp[i])._valueFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
				{
					GetValueByTWLC2FileRowAndColumn((tmp[i])._valueFile,tmpProcess.fvalue);

				}
				else if((tmp[i])._valueFile.strFileName == QString::fromLocal8Bit("参数.csv"))
				{
					GetValueByParamFileRowAndColumn((tmp[i])._valueFile,tmpProcess.fvalue);

				}
				else if((tmp[i])._valueFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
				{
					GetValueByGroupSettingFileRowAndColumn((tmp[i])._valueFile,tmpProcess.fvalue);
				}
				else
				{
					QString strWarning;
					strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
					QMessageBox::warning(this,"",strWarning);
					LogInfo(strWarning);
					//return;
				}
			}
			else if((tmp[i]).iDirectly == 1)
			{
				tmpProcess.fvalue = (tmp[i])._fvalue;
			}

			tmpProcess.iPipeRotate = (tmp[i]).iPipeRotate;
			tmpProcess.iCreatPipe = (tmp[i]).iCreatPipe;
			tmpProcess.iCurveOrStraight = (tmp[i]).iCurveOrStraight;
			tmpProcess.iNegative = (tmp[i]).iNegative;

			if ((tmp[i])._curveRFile.strFileName != " ")//间接读半径
			{
				if((tmp[i])._curveRFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
				{
					if(tmp[i].isInLoop == true)
					{
						(tmp[i])._curveRFile.irow = index;
					}

					GetValueByProductionFileRowAndColumn((tmp[i])._curveRFile,tmpProcess.fcurveR);		

				}
				else if((tmp[i])._curveRFile.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
				{
					GetValueByTWLC1FileRowAndColumn((tmp[i])._curveRFile,tmpProcess.fcurveR);

				}
				else if((tmp[i])._curveRFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
				{
					GetValueByTWLC2FileRowAndColumn((tmp[i])._curveRFile,tmpProcess.fcurveR);

				}
				else if((tmp[i])._curveRFile.strFileName == QString::fromLocal8Bit("参数.csv"))
				{
					GetValueByParamFileRowAndColumn((tmp[i])._curveRFile,tmpProcess.fcurveR);

				}
				else if((tmp[i])._curveRFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
				{
					GetValueByGroupSettingFileRowAndColumn((tmp[i])._curveRFile,tmpProcess.fcurveR);
				}
				else
				{
					QString strWarning;
					strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
					QMessageBox::warning(this,"",strWarning);
					LogInfo(strWarning);
					//return;
				}
			}
			else if ((tmp[i])._curveRFile.strFileName == " ")//直接读半径
			{
				tmpProcess.fcurveR = (tmp[i]).fcurveR;
			}		


			tmpvec.push_back(tmpProcess);				
		}
	}

	return tmpvec;


}

void MainWindow::AllSteps()
{
#if 0



	char process_i[32] = "";
	char process_c[32] = "";

	char process_i1[32] = "";
	char process_i2[32] = "";
	char process_i3[32] = "";
	char process_i4[32] = "";
	char process_i5[32] = "";
	char process_i6[32] = "";
	char process_i7[32] = "";
	char process_i8[32] = "";
	char process_i9[32] = "";
	char process_i10[32] = "";

	float fprocess_i;//ini文件的初始加紧值
	float fprocess_c;//
	float fprocess_i1;//ini文件的初始加紧值
	float fprocess_i2;//ini文件的初始加紧值
	float fprocess_i3;//ini文件的初始加紧值
	float fprocess_i4;//ini文件的初始加紧值
	float fprocess_i5;//ini文件的初始加紧值
	float fprocess_i6;//ini文件的初始加紧值
	float fprocess_i7;//ini文件的初始加紧值
	float fprocess_i8;//ini文件的初始加紧值
	float fprocess_i9;//ini文件的初始加紧值
	float fprocess_i10;//ini文件的初始加紧值


	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I, 3, process_i);
	fprocess_i = atof(process_i);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_C, 3, process_c);
	fprocess_c = atof(process_c);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I1, 3, process_i1);
	fprocess_i1 = atof(process_i1);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I2, 3, process_i2);
	fprocess_i2 = atof(process_i2);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I3, 3, process_i3);
	fprocess_i3 = atof(process_i3);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I4, 3, process_i4);
	fprocess_i4 = atof(process_i4);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I5, 3, process_i5);
	fprocess_i5 = atof(process_i5);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I6, 3, process_i6);
	fprocess_i6 = atof(process_i6);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I7, 3, process_i7);
	fprocess_i7 = atof(process_i7);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I8, 3, process_i8);
	fprocess_i8 = atof(process_i8);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I9, 3, process_i9);
	fprocess_i9 = atof(process_i9);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I10, 3, process_i10);
	fprocess_i10 = atof(process_i10);

	//推弯流程索引变量
	m_iTwlc1 = (int)fprocess_i1;
	m_iTwlc2 = (int)fprocess_i2;
	m_iTwlc3 = (int)fprocess_i3;
	m_iTwlc4 = (int)fprocess_i4;
	m_iTwlc5 = (int)fprocess_i5;
	m_iTwlc6 = (int)fprocess_i6;
	m_iTwlc7 = (int)fprocess_i7;
	m_iTwlc8 = (int)fprocess_i8;
	m_iTwlc9 = (int)fprocess_i9;
	m_iTwlc10 = (int)fprocess_i10;
	m_iTwlc11 = (int)fprocess_i11;
	m_iTwlc12 = (int)fprocess_i12;
	m_iTwlc13 = (int)fprocess_i13;
	m_iTwlc14 = (int)fprocess_i14;
	m_iTwlc15 = (int)fprocess_i15;
	m_iTwlc16 = (int)fprocess_i16;
	m_iTwlc17 = (int)fprocess_i17;
	m_iTwlc18 = (int)fprocess_i18;
	m_iTwlc19 = (int)fprocess_i19;
	m_iTwlc20 = (int)fprocess_i20;

	int index = (int)fprocess_i;//当前弯索引
	int icolor = (int)fprocess_c;// 
	int iTimes = 0;//第几次循环
	m_vAllStepvProcess.clear();
	bool skipIncrease = false;

	std::map<int,std::vector<OnlyReadProcess>>::iterator it = m_MapStepTovOnlyReadProcess.begin();
	for(;it!=m_MapStepTovOnlyReadProcess.end();)
	{
		std::vector<Process> tmpvec;
		std::vector<OnlyReadProcess>::iterator it2 = it->second.begin();
		for(;it2!=it->second.end();it2++)
		{
			skipIncrease = false;

			if(it2->strCode == "LOOP_START")
			{
				iTimes++;
				if(iTimes>m_LoopTimes)
				{
					std::vector<std::vector<Process>> a = m_vAllStepvProcess;
					//AddTheEedProcess();
					return;
				}

			}

			if (it2->strCode == "LOOP_END")
			{
				it = GetAimReadProcessByAimStep(m_LoopStartIndex);
				skipIncrease = true;
				break;
			}

			if((*it2).strCode =="INDEX")
			{
				if ((*it2)._strIndexVar == "i")
				{
					if((*it2)._strOperator == "+")
					{
						index = index + (int)(*it2)._fvalue;
					}
					else if((*it2)._strOperator == "-")
					{
						index = index - (int)(*it2)._fvalue;
					}
					else if((*it2)._strOperator == "Mutiply")
					{
						index = index * (int)(*it2)._fvalue;

					}
					else if((*it2)._strOperator == "Divide")
					{
						index = index / (int)(*it2)._fvalue;

					}
					else if((*it2)._strOperator =="equal")
					{
						index = (int)(*it2)._fvalue;

					}
				}
				else 
				{
					UpdateTWLCINDEX((*it2)._strIndexVar,(*it2)._strOperator,(*it2)._fvalue);
				}

			}

			Process tmpProcess;
			std::string strtmp = ((*it2).strCode).toStdString();
			if(strtmp.find_first_of('M')==0  || strtmp.find_first_of('G')==0) //G,M
			{

				tmpProcess.strCode = (*it2).strCode;
				tmpProcess.istep = (*it2).istep;
				tmpProcess.iDirectly = (*it2).iDirectly;
				if((*it2).iDirectly == 0)//间接获取
				{				

					if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
					{
						if((*it2)._valueFile.strIndexVarName == "i")
						{
							(*it2)._valueFile.irow = index;
						}					
						GetValueByProductionFileRowAndColumn((*it2)._valueFile,tmpProcess.fvalue);		

					}
					else if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("推弯流程n.csv"))
					{
						int twlc_num = GetTWLCNUM(index);//当前弯的推弯流程号		
						GetValueByTWLCNUMFileRowAndColumn((*it2)._valueFile,tmpProcess.fvalue,twlc_num);

					}
					//else if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
					//{
					//	GetValueByTWLC2FileRowAndColumn((*it2)._valueFile,tmpProcess.fvalue);

					//}
					else if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("参数.csv"))
					{
						GetValueByParamFileRowAndColumn((*it2)._valueFile,tmpProcess.fvalue);

					}
					else if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
					{
						//GetValueByGroupSettingFileRowAndColumn((*it2)._valueFile,tmpProcess.fvalue);
					}
					else
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
						QMessageBox::warning(this,"",strWarning);
						return;
					}


				}
				else if((*it2).iDirectly == 1)
				{
					tmpProcess.fvalue = (*it2)._fvalue;
				}
				else if((*it2).iDirectly == 2)//取代码定义里对应功能码的默认值
				{
					GetValueInGroupSettingFileByGcode((*it2).strCode,tmpProcess.fvalue);
				}

				tmpProcess.iPipeRotate = (*it2).iPipeRotate;
				tmpProcess.iCreatPipe = (*it2).iCreatPipe;
				tmpProcess.iCurveOrStraight = (*it2).iCurveOrStraight;
				tmpProcess.iNegative = (*it2).iNegative;


				if((*it2)._curveRFile.strFileName != " ")
				{
					if((*it2)._curveRFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
					{
						if((*it2)._curveRFile.strIndexVarName == "i")
						{
							(*it2)._curveRFile.irow = index;
						}
						GetValueByProductionFileRowAndColumn((*it2)._curveRFile,tmpProcess.fcurveR);		

					}
					else if((*it2)._curveRFile.strFileName == QString::fromLocal8Bit("推弯流程n.csv"))
					{
						//GetValueByTWLC1FileRowAndColumn((*it2)._curveRFile,tmpProcess.fcurveR);
						int twlc_num = GetTWLCNUM(index);//当前弯的推弯流程号		
						GetValueByTWLCNUMFileRowAndColumn((*it2)._curveRFile,tmpProcess.fcurveR,twlc_num);
					}
					/*	else if((*it2)._curveRFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
					{
					GetValueByTWLC2FileRowAndColumn((*it2)._curveRFile,tmpProcess.fcurveR);

					}*/
					else if((*it2)._curveRFile.strFileName == QString::fromLocal8Bit("参数.csv"))
					{
						GetValueByParamFileRowAndColumn((*it2)._curveRFile,tmpProcess.fcurveR);

					}
					else if((*it2)._curveRFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
					{
						//GetValueByGroupSettingFileRowAndColumn((*it2)._curveRFile,tmpProcess.fcurveR);
					}
					else
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
						QMessageBox::warning(this,"",strWarning);
						return;
					}
				}
				else if ((*it2)._curveRFile.strFileName == " ")
				{
					tmpProcess.fcurveR = (*it2).fcurveR;
				}		
				//
				tmpProcess.iNthCureve = index;
				tmpvec.push_back(tmpProcess);				
			}
			else if((*it2).strCode =="COLOR")
			{
				tmpProcess.strCode = (*it2).strCode;
				tmpProcess.istep = (*it2).istep;
				if ((*it2)._strIndexVar == "c")
				{
					if((*it2)._strOperator == "+")
					{			
						icolor = icolor + (int)(*it2)._fvalue;
					}
					else if((*it2)._strOperator == "-")
					{
						icolor = icolor - (int)(*it2)._fvalue;
					}
					else if((*it2)._strOperator == "Mutiply")
					{
						icolor = icolor * (int)(*it2)._fvalue;

					}
					else if((*it2)._strOperator == "Divide")
					{
						icolor = icolor / (int)(*it2)._fvalue;

					}
					else if((*it2)._strOperator =="equal")
					{

						if((*it2)._strColorValue == "i")
						{
							icolor = index -1;
						}
						else
						{
							icolor = (int)(*it2)._fvalue;
						}
					}
				}

				tmpProcess.iColor = icolor;
				tmpvec.push_back(tmpProcess);	

			}
			else if ((*it2).strCode =="CJ")
			{
				bool use = false;//条件跳转判断是否成功
				int  iAimStep;
				QString strAimCode;
				if((*it2)._fvalue == 0)//无条件跳转
				{
					iAimStep = (*it2).iDirectly;
					it = GetAimReadProcessByAimStep(iAimStep);
					skipIncrease = true;
					break;
				}
				else if((*it2)._valueFile.strFileName !=" ")//条件跳转
				{
					bool use;
					QString strOperator = (*it2)._valueFile._operator;
					float fConditonValue = (*it2)._valueFile.fcondionvalue;
					float fAimFileValue;
					if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
					{
						if((*it2)._valueFile.strIndexVarName == "i")
						{
							(*it2)._valueFile.irow = index;
						}
						GetValueByProductionFileRowAndColumn((*it2)._valueFile,fAimFileValue);		
					}
					else if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("推弯流程n.csv"))
					{
						//GetValueByTWLC1FileRowAndColumn((*it2)._valueFile,fAimFileValue);

						int twlc_num = GetTWLCNUM(index);//当前弯的推弯流程号		
						GetValueByTWLCNUMFileRowAndColumn((*it2)._valueFile,fAimFileValue,twlc_num);
					}
					/*else if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
					{
					GetValueByTWLC2FileRowAndColumn((*it2)._valueFile,fAimFileValue);
					}*/
					else if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("参数.csv"))
					{
						GetValueByParamFileRowAndColumn((*it2)._valueFile,fAimFileValue);
					}
					else if((*it2)._valueFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
					{
						//GetValueByGroupSettingFileRowAndColumn((*it2)._valueFile,fAimFileValue);
					}
					else
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
						QMessageBox::warning(this,"",strWarning);
						return;
					}

					//判断跳转是否成功
					if(strOperator == "equal")
					{
						if(fConditonValue == fAimFileValue)
						{
							use = true;
						}
						else
						{
							use = false;
						}
					}
					else if(strOperator == ">")
					{
						if(fConditonValue == fAimFileValue)
						{
							use = true;
						}
						else
						{
							use = false;
						}
					}
					else if(strOperator == "<")
					{
						if(fConditonValue == fAimFileValue)
						{
							use = true;
						}
						else
						{
							use = false;
						}
					}


					if(use == true)//条件跳转
					{
						iAimStep = (*it2).iDirectly;
						it = GetAimReadProcessByAimStep(iAimStep);
						skipIncrease = true;
						break;//跳出到目标索引
					}

				}

			}
			else if((*it2).strCode =="SET")
			{
				int  iAimStep = (*it2).iDirectly;
				std::vector<Process> vtmpProcess = GetAimStepReadProcess(iAimStep,index);
				for(int i=0;i<vtmpProcess.size();i++)
				{
					tmpvec.push_back(vtmpProcess[i]);
				}
			}
		}

		if(tmpvec.size()>0)
			m_vAllStepvProcess.push_back(tmpvec);

		if (skipIncrease)
		{
		}
		else
		{
			it++;

		}


		if(it == m_MapStepTovOnlyReadProcess.end() && iTimes<=m_vLoopTimes.size())//到文件尾部，但是循环次数未达到
		{
			it = GetAimReadProcessByAimStep(m_LoopStartIndex);
		}
	}

#endif

}

void MainWindow::AddTheEedProcess()
{

}

bool MainWindow::IsParamInInIFile(std::string str,float& fvalue)
{
	if (str == "i1")
	{
		fvalue = m_iTwlc1;
		return true;
	}
	else if (str == "i2")
	{
		fvalue = m_iTwlc2;
		return true;

	}
	else if (str == "i3")
	{
		fvalue = m_iTwlc3;
		return true;

	}
	else if (str == "i4")
	{
		fvalue = m_iTwlc4;
		return true;

	}
	else if (str == "i5")
	{
		fvalue = m_iTwlc5;
		return true;

	}
	else if (str == "i6")
	{
		fvalue = m_iTwlc6;
		return true;

	}
	else if (str == "i7")
	{
		fvalue = m_iTwlc7;
		return true;

	}
	else if (str == "i8")
	{
		fvalue = m_iTwlc8;
		return true;

	}
	else if (str == "i9")
	{
		fvalue = m_iTwlc9;
		return true;

	}
	else if (str == "i10")
	{
		fvalue = m_iTwlc10;
		return true;

	}
	else if (str == "i11")
	{
		fvalue = m_iTwlc11;
		return true;
	}
	else if (str == "i12")
	{
		fvalue = m_iTwlc12;
		return true;

	}
	else if (str == "i13")
	{
		fvalue = m_iTwlc13;
		return true;

	}
	else if (str == "i14")
	{
		fvalue = m_iTwlc14;
		return true;

	}
	else if (str == "i15")
	{
		fvalue = m_iTwlc15;
		return true;

	}
	else if (str == "i16")
	{
		fvalue = m_iTwlc16;
		return true;

	}
	else if (str == "i17")
	{
		fvalue = m_iTwlc17;
		return true;

	}
	else if (str == "i18")
	{
		fvalue = m_iTwlc18;
		return true;

	}
	else if (str == "i19")
	{
		fvalue = m_iTwlc19;
		return true;

	}
	else if (str == "i20")
	{
		fvalue = m_iTwlc20;
		return true;

	}
	else
	{
		return false;
	}

}

bool MainWindow::isCSVFileName(std::string str)
{
	bool is;
	if (str.find("csv") != std::string::npos)
	{
		is = true;
	}
	else
	{
		is = false;
	}
	return is;
}

bool MainWindow::isHasKuoHao1(std::string str)
{
	bool is;
	if (str.find("{") != std::string::npos || str.find("}") != std::string::npos)
	{
		is = true;
	}
	else
	{
		is = false;
	}
	return is;
}


std::map<int,std::vector<std::vector<FileColumn>>>::iterator MainWindow::GetStartIt(int istep)
{
	std::map<int,std::vector<std::vector<FileColumn>>>::iterator  it;
	it = m_MapStepTovRowString.find(istep);
	if(it!=m_MapStepTovRowString.end())
	{
		return it;
	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("跳转索引号错误!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
	}
}

std::map<int,std::vector<std::vector<FileColumn>>>::iterator MainWindow::GetAimItByAimStep(int istep)
{
	std::map<int,std::vector<std::vector<FileColumn>>>::iterator  it;
	it = m_MapStepTovRowString.find(istep);
	if(it!=m_MapStepTovRowString.end())
	{
		return it;
	}
	else
	{
		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			//strWarning = strWarning.fromLocal8Bit("跳转索引号错误!");
			strWarning = strWarning.fromLocal8Bit("Error with Process with!");

		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		//return;


		/*	QString strWarning;
		strWarning = strWarning.fromLocal8Bit("跳转索引号错误!");
		QMessageBox::warning(this,"",strWarning);*/
		//return;
	}
}

void MainWindow::GetResultValue(NodeNew& nodenew)
{
	if (nodenew.isop)
	{
		return;
	}
	else
	{
		if (IsNumber(nodenew.num))
		{
			nodenew.fvalue = QString::fromLocal8Bit(nodenew.num.c_str()).toFloat();
		}
		else
		{
			// 文件
			std::vector<std::string> Res;
			std::string Delema=",";
			split(nodenew.num,Res,Delema);
			if (QString::fromLocal8Bit(Res[0].c_str())==QString::fromLocal8Bit("参数.csv"))
			{
				OtherFileValue otherFile;
				otherFile.strFileName = QString::fromLocal8Bit(Res[0].c_str());
				otherFile.irow = QString::fromLocal8Bit(Res[1].c_str()).toInt();
				otherFile.icolumn = QString::fromLocal8Bit(Res[2].c_str()).toInt();
				GetValueByParamFileRowAndColumn(otherFile,nodenew.fvalue);
			}
		}
	}
}

void MainWindow::OnlyReadProcessFile()
{
	m_MapStepTovOnlyReadProcess.erase(m_MapStepTovOnlyReadProcess.begin(),m_MapStepTovOnlyReadProcess.end());
	m_vLoopTimes.clear();
	m_LoopTimes = 0;

	bool btmpLoop = false;
	QString strProcessFile = need_filePath + QString::fromLocal8Bit("/流程.csv");
	QByteArray ba = strProcessFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			if (szBuffer.find("//")!= std::string::npos)
			{
				int pos = szBuffer.find("//");
				szBuffer = szBuffer.substr(0,pos);
			}

			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);
			if(Res.size() == 0)
			{
				continue;
			}


			std::map<int,std::vector<OnlyReadProcess>>::iterator it;

			std::string  tmpCode = Res[1];//GCode
			OnlyReadProcess tmpOnlyReadProcess;
			tmpOnlyReadProcess.istep = QString::fromStdString(Res[0]).toInt();
			tmpOnlyReadProcess.strCode = QString::fromLocal8Bit(Res[1].c_str());
			tmpOnlyReadProcess.iDirectly = QString::fromStdString(Res[2]).toInt();

			if(tmpCode == "LOOP_START" ||tmpCode == "LOOP_END" )//判读循环
			{
				if(tmpCode == "LOOP_START")
				{
					m_LoopStartIndex = QString::fromStdString(Res[0]).toInt();
					btmpLoop = true;
					bool isLoopTimes = false;

					if(Res.size()!=7)
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("LOOP_START行数据错误!");
						QMessageBox::warning(this,"",strWarning);
						LogInfo(strWarning);
						return;
					}

					OtherFileValue t;
					t.strFileName = QString::fromLocal8Bit(Res[2].c_str());
					t.irow = QString::fromStdString(Res[3]).toInt();
					t.icolumn = QString::fromStdString(Res[4]).toInt();
					t._operator = QString::fromStdString(Res[5].c_str());
					t.fcondionvalue = QString::fromStdString(Res[6]).toInt();

					if(t.strFileName == QString::fromLocal8Bit("参数.csv"))
					{
						float a; 
						GetValueByParamFileRowAndColumn(t,a);
						if(t._operator == "+")
						{
							a = a + (int)t.fcondionvalue;
						}
						else if(t._operator == "-")
						{
							a = a - (int)t.fcondionvalue;
						}
						else if(t._operator == "Mutiply")
						{
							a = a * (int)t.fcondionvalue;

						}
						else if(t._operator == "Divide")
						{
							a = a / (int)t.fcondionvalue;

						}

						m_LoopTimes = (int)a;


					}

				}
				else
				{
					btmpLoop = false;
				}
			}
			else if(tmpCode.find_first_of('G') == 0
				|| tmpCode.find_first_of('M') == 0
				|| tmpCode.find_first_of('P') == 0)//G,M
			{
				tmpOnlyReadProcess.isInLoop = btmpLoop;
				if(isCSVFileName(Res[3]) == true)//数值为文件
				{
					if(Res.size()!=10 && Res.size()!=12)//弯管半径直接或间接指定
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("G、M、P行的数据格式错误!");
						QMessageBox::warning(this,"",strWarning);
						LogInfo(strWarning);
						return;
					}

					tmpOnlyReadProcess._valueFile.strFileName = QString::fromLocal8Bit(Res[3].c_str());
					tmpOnlyReadProcess._valueFile.irow = QString::fromStdString(Res[4]).toInt();	
					tmpOnlyReadProcess._valueFile.strIndexVarName = QString::fromStdString(Res[4].c_str());	
					tmpOnlyReadProcess._valueFile.icolumn = QString::fromStdString(Res[5]).toInt();	

					//管子
					tmpOnlyReadProcess.iPipeRotate = QString::fromStdString(Res[6]).toInt();
					tmpOnlyReadProcess.iCreatPipe = QString::fromStdString(Res[7]).toInt();
					tmpOnlyReadProcess.iCurveOrStraight = QString::fromStdString(Res[8]).toInt();
					if(isCSVFileName(Res[9]) == true)//管子文件 size=13
					{
						tmpOnlyReadProcess._curveRFile.strFileName =  QString::fromLocal8Bit(Res[9].c_str());
						tmpOnlyReadProcess._curveRFile.irow = QString::fromStdString(Res[10]).toInt();	
						tmpOnlyReadProcess._curveRFile.icolumn = QString::fromStdString(Res[11]).toInt();	
						//tmpOnlyReadProcess.iNegative = -1;
					}
					else if(isCSVFileName(Res[9]) == false)//size=11
					{
						tmpOnlyReadProcess.fcurveR = QString::fromStdString(Res[9]).toInt();
						//tmpOnlyReadProcess.iNegative = QString::fromStdString(Res[10]).toInt();
					}					
				}
				else if(isCSVFileName(Res[3]) == false)
				{
					if(Res.size()!=8 && Res.size()!=10)//弯管半径直接或间接指定
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("G、M行数据格式错误!");
						QMessageBox::warning(this,"",strWarning);
						LogInfo(strWarning);
						return;
					}

					tmpOnlyReadProcess._fvalue = QString::fromStdString(Res[3]).toFloat();
					//管子
					tmpOnlyReadProcess.iPipeRotate = QString::fromStdString(Res[4]).toInt();
					tmpOnlyReadProcess.iCreatPipe = QString::fromStdString(Res[5]).toInt();
					tmpOnlyReadProcess.iCurveOrStraight = QString::fromStdString(Res[6]).toInt();

					if(isCSVFileName(Res[7]) == true)//管子文件 size=11
					{
						tmpOnlyReadProcess._curveRFile.strFileName =  QString::fromLocal8Bit(Res[7].c_str());
						tmpOnlyReadProcess._curveRFile.irow = QString::fromStdString(Res[8]).toInt();
						tmpOnlyReadProcess._curveRFile.strIndexVarName = QString::fromStdString(Res[8].c_str());	

						tmpOnlyReadProcess._curveRFile.icolumn = QString::fromStdString(Res[9]).toInt();	
						//tmpOnlyReadProcess.iNegative = QString::fromStdString(Res[10]).toInt();
					}
					else if(isCSVFileName(Res[7]) == false)//size=9
					{
						tmpOnlyReadProcess.fcurveR = QString::fromStdString(Res[7]).toInt();
						//tmpOnlyReadProcess.iNegative = QString::fromStdString(Res[8]).toInt();
					}				

				}
			}
			else if(tmpCode.find("CJ")!= std::string::npos)
			{
				tmpOnlyReadProcess.isInLoop = btmpLoop;
				if(isCSVFileName(Res[3]) == true)//条件跳转
				{
					if(Res.size()!=14 && Res.size()!=12)//弯管半径直接或间接指定
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("条件跳转行的数据格式错误!");
						QMessageBox::warning(this,"",strWarning);
						LogInfo(strWarning);
						return;
					}


					tmpOnlyReadProcess._valueFile.strFileName =  QString::fromLocal8Bit(Res[3].c_str());
					tmpOnlyReadProcess._valueFile.irow = QString::fromStdString(Res[4]).toInt();	
					tmpOnlyReadProcess._valueFile.strIndexVarName = QString::fromStdString(Res[4].c_str());	

					tmpOnlyReadProcess._valueFile.icolumn = QString::fromStdString(Res[5]).toInt();	
					tmpOnlyReadProcess._valueFile._operator =  QString::fromLocal8Bit(Res[6].c_str());
					tmpOnlyReadProcess._valueFile.fcondionvalue = QString::fromStdString(Res[7]).toFloat();
					//管子
					tmpOnlyReadProcess.iPipeRotate = QString::fromStdString(Res[8]).toInt();
					tmpOnlyReadProcess.iCreatPipe = QString::fromStdString(Res[9]).toInt();
					tmpOnlyReadProcess.iCurveOrStraight = QString::fromStdString(Res[10]).toInt();
					if(isCSVFileName(Res[11]) == true)//管子文件 size=15
					{
						tmpOnlyReadProcess._curveRFile.strFileName =  QString::fromLocal8Bit(Res[11].c_str());
						tmpOnlyReadProcess._curveRFile.irow = QString::fromStdString(Res[12]).toInt();	
						tmpOnlyReadProcess._curveRFile.strIndexVarName = QString::fromStdString(Res[12].c_str());	

						tmpOnlyReadProcess._curveRFile.icolumn = QString::fromStdString(Res[13]).toInt();	
						//tmpOnlyReadProcess.iNegative = QString::fromStdString(Res[14]).toInt();
					}
					else if(isCSVFileName(Res[11]) == false)//size=13
					{
						tmpOnlyReadProcess.fcurveR = QString::fromStdString(Res[11]).toInt();
						//tmpOnlyReadProcess.iNegative = QString::fromStdString(Res[12]).toInt();
					}			


				}
				else if(isCSVFileName(Res[3]) == false)//无条件跳转 9
				{
					if(Res.size()!=8)//弯管半径直接或间接指定
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("无条件跳转行的数据格式错误!");
						QMessageBox::warning(this,"",strWarning);
						LogInfo(strWarning);
						return;
					}


					tmpOnlyReadProcess._fvalue = QString::fromStdString(Res[3]).toFloat();
					//管子
					tmpOnlyReadProcess.iPipeRotate = QString::fromStdString(Res[4]).toInt();
					tmpOnlyReadProcess.iCreatPipe = QString::fromStdString(Res[5]).toInt();
					tmpOnlyReadProcess.iCurveOrStraight = QString::fromStdString(Res[6]).toInt();
					tmpOnlyReadProcess.fcurveR = QString::fromStdString(Res[7]).toFloat();
					//tmpOnlyReadProcess.iNegative = QString::fromStdString(Res[8]).toInt();
				}

			}
			else if(tmpCode.find("SET")!= std::string::npos)
			{
				tmpOnlyReadProcess.isInLoop = btmpLoop;
				tmpOnlyReadProcess._fvalue = QString::fromStdString(Res[3]).toFloat();

				//管子
				tmpOnlyReadProcess.iPipeRotate = QString::fromStdString(Res[4]).toInt();
				tmpOnlyReadProcess.iCreatPipe = QString::fromStdString(Res[5]).toInt();
				tmpOnlyReadProcess.iCurveOrStraight = QString::fromStdString(Res[6]).toInt();
				tmpOnlyReadProcess.fcurveR = QString::fromStdString(Res[7]).toFloat();
				//tmpOnlyReadProcess.iNegative = QString::fromStdString(Res[8]).toInt();
			}
			else if (tmpCode.find("INDEX")!= std::string::npos)
			{
				if(Res.size()!=5)
				{
					QString strWarning;
					strWarning = strWarning.fromLocal8Bit("弯数索引行的数据格式错误!");
					QMessageBox::warning(this,"",strWarning);
					LogInfo(strWarning);
					return;
				}
				tmpOnlyReadProcess._strIndexVar = QString::fromStdString(Res[2].c_str());
				tmpOnlyReadProcess._strOperator = QString::fromStdString(Res[3].c_str());
				tmpOnlyReadProcess._fvalue = QString::fromStdString(Res[4]).toInt();
			}
			else if(tmpCode.find("COLOR")!= std::string::npos)
			{
				if(Res.size()!=5)
				{
					QString strWarning;
					strWarning = strWarning.fromLocal8Bit("变色行的数据格式错误!");
					QMessageBox::warning(this,"",strWarning);
					LogInfo(strWarning);
					return;
				}

				tmpOnlyReadProcess._strIndexVar = QString::fromStdString(Res[2].c_str());
				tmpOnlyReadProcess._strOperator = QString::fromStdString(Res[3].c_str());
				tmpOnlyReadProcess._fvalue = QString::fromStdString(Res[4]).toInt(); 
				tmpOnlyReadProcess._strColorValue = QString::fromStdString(Res[4].c_str());
			}
			else
			{
				QString strWarning;
				strWarning = strWarning.fromLocal8Bit("功能码不存在!");
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}

			//当前读到的行数据加入到数据结构
			it = m_MapStepTovOnlyReadProcess.find(tmpOnlyReadProcess.istep);
			if(it==m_MapStepTovOnlyReadProcess.end())//不存在
			{
				std::vector<OnlyReadProcess> t;
				t.push_back(tmpOnlyReadProcess);
				m_MapStepTovOnlyReadProcess[tmpOnlyReadProcess.istep] = t;
			}
			else if(it!=m_MapStepTovOnlyReadProcess.end())//已存在
			{
				it->second.push_back(tmpOnlyReadProcess);
			}

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	std::map<int,std::vector<OnlyReadProcess>> a = m_MapStepTovOnlyReadProcess;
	std::vector<int> an = m_vLoopTimes;
}


bool MainWindow::isDivideZero(float fvalue)
{

	if(fabs(fvalue) < 0.0000001)
	{
		return true;
	}
	else
	{
		return false;
	}
}

columnType MainWindow::GetStringType(std::string str)
{
	if(str.find_first_of('+')!= std::string::npos ||
		str.find_first_of('-')!= std::string::npos ||
		str.find_first_of('/')!= std::string::npos ||
		str.find_first_of('*')!= std::string::npos ||
		str.find_first_of('%')!= std::string::npos)
	{
		return Formula;
	}
	else if(isCSVFileName(str))
	{
		return File;
	}
	else if(IsNumber(str) || str.find_first_of('=')!= std::string::npos)
	{
		// 变量赋值也当作数据处理
		return Data;
	}
	else
	{
		return Var;
	}
}

bool MainWindow::GetIsDoCJ(std::string symbol,float fFormer,float fLaster)
{
	bool use = false;
	if(QString::fromLocal8Bit(symbol.c_str()) == ">")
	{
		if(fFormer>fLaster)
		{
			use = true;
		}
		else
		{
			use = false;
		}
	}
	else if(QString::fromLocal8Bit(symbol.c_str()) == "<")
	{
		if(fFormer<fLaster)
		{
			use = true;
		}
		else
		{
			use = false;
		}
	}
	else if(QString::fromLocal8Bit(symbol.c_str()) == "=")
	{
		if(fFormer==fLaster)
		{
			use = true;
		}
		else
		{
			use = false;
		}
	}
	else if(QString::fromLocal8Bit(symbol.c_str()) == "!=")
	{
		if(fFormer!=fLaster)
		{
			use = true;
		}
		else
		{
			use = false;
		}
	}

	return use;
}

void MainWindow::GetTwoFormulaBySymbol(std::string& former,std::string& laster,std::string symbol,std::string str)
{

	std::vector<std::string> res;
	std::string Delema = symbol;  //  'a,b;; c d'.split(/[\s\,\;]+/); // ['a', 'b', 'c', 'd']
	split(str,res,Delema);
	former = res[0];
	laster = res[1];
}

void MainWindow::GetCellValue(FileColumn fc,float& fvalue)
{

	if(fc.type == Data)
	{
		fvalue =  QString::fromLocal8Bit(fc.str.c_str()).toFloat(); 
	}
	else if(fc.type == File)
	{
		std::string mid = fc.str;
		float ftmpvalue;
		bool sus = GetFileValue(mid,ftmpvalue);
		if(sus == true)
		{
			fvalue = ftmpvalue;
		}
		else
		{
			return;
		}
	}
	else if(fc.type == Formula)
	{
		String2Formula strToFormu;
		std::vector<NodeNew> back = strToFormu.midToBack(fc.str);

		for(size_t d=0;d<back.size();d++)
		{
			GetResultValueByAllFile(back[d]);
		}

		fvalue = strToFormu.getValue(back);
	}
	else if(fc.type == Var)
	{
		QString IndexVar = QString::fromLocal8Bit(fc.str.c_str());
		int tmpindex;
		if (IndexVar == "i")
		{
			tmpindex = m_index;
		}
		else if(IndexVar == "i1")
		{
			tmpindex = m_iTwlc1;
		}
		else if (IndexVar == "i2")
		{
			tmpindex = m_iTwlc2;
		}
		else if (IndexVar == "i3")
		{
			tmpindex = m_iTwlc3;
		}
		else if (IndexVar == "i4")
		{
			tmpindex = m_iTwlc4;
		}
		else if (IndexVar == "i5")
		{
			tmpindex = m_iTwlc5;
		}
		else if (IndexVar == "i6")
		{
			tmpindex = m_iTwlc6;
		}
		else if (IndexVar == "i7")
		{
			tmpindex = m_iTwlc7;
		}
		else if (IndexVar == "i8")
		{
			tmpindex = m_iTwlc8;
		}
		else if (IndexVar == "i9")
		{
			tmpindex = m_iTwlc9;
		}
		else if (IndexVar == "i10")
		{
			tmpindex = m_iTwlc10;
		}
		else if(IndexVar == "i11")
		{
			tmpindex = m_iTwlc11;
		}
		else if (IndexVar == "i12")
		{
			tmpindex = m_iTwlc12;
		}
		else if (IndexVar == "i13")
		{
			tmpindex = m_iTwlc13;
		}
		else if (IndexVar == "i14")
		{
			tmpindex = m_iTwlc14;
		}
		else if (IndexVar == "i15")
		{
			tmpindex = m_iTwlc15;
		}
		else if (IndexVar == "i16")
		{
			tmpindex = m_iTwlc16;
		}
		else if (IndexVar == "i17")
		{
			tmpindex = m_iTwlc17;
		}
		else if (IndexVar == "i18")
		{
			tmpindex = m_iTwlc18;
		}
		else if (IndexVar == "i19")
		{
			tmpindex = m_iTwlc19;
		}
		else if (IndexVar == "i20")
		{
			tmpindex = m_iTwlc20;
		}
		else
		{

			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("推弯流程INDEX错误!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[86];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;

			/*QString strWarning;
			strWarning = strWarning.fromLocal8Bit("推弯流程INDEX错误!");
			QMessageBox::warning(this,"",strWarning);
			return;*/
		}

		fvalue = (float)tmpindex;
	}
}

void MainWindow::GetValueByTWLCNUMFileRowAndColumnNew(OtherFileValue other,float& fvalue,int twlcIndex)
{
	std::map<int,std::vector<struct TWLC>>::iterator it;
	int num = twlcIndex-1;
	it  = m_MapFileTovTWLCValues.find(num);
	if(it == m_MapFileTovTWLCValues.end())
	{

		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("选择推弯流程文件不存在!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;

		/*QString strWarning;
		strWarning = strWarning.fromLocal8Bit("选择推弯流程文件不存在!");
		QMessageBox::warning(this,"",strWarning);
		return;*/
	}

	if(other.irow >=2 && other.irow <= it->second.size() +1)
	{
		if(other.icolumn == 1)
		{
			fvalue = it->second[other.irow-2].ftwy;
		}
		else if(other.icolumn == 2)
		{
			fvalue = it->second[other.irow-2].ftwbj;
		}
		else if(other.icolumn == 3)
		{
			fvalue = it->second[other.irow-2].ftwc;
		}
		else if(other.icolumn == 4)
		{
			fvalue = it->second[other.irow-2].ftwb;
		}
	}
	else
	{

		QString strWarning;
		if (m_vCurLanguageWords.empty())
		{
			strWarning = strWarning.fromLocal8Bit("选择推弯流程文件数据行错误!");
		}
		else
		{
			strWarning = m_vCurLanguageWords[86];
		}
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}
}



void MainWindow::GetValueBySumOfTwoRadiusFile(OtherFileValue other,float& fvalue,int iNthJT)
{



	if (iNthJT == 1)
	{
		int iPriv = other.irow;
		int iAim = other.icolumn;
		std::map<int,float>::iterator itt;
		itt = m_MapJT1LayerToR.find(iPriv);
		if (itt!=m_MapJT1LayerToR.end())
		{
			float fPR = itt->second;//起始边半径
			itt = m_MapJT1LayerToR.find(iAim);
			float fAR = itt->second;//目标边半径
			//float fbiggestR = GetBiggestLMRadius(iNthJT);
			fvalue = fPR + fAR;
		}

	}
}


void MainWindow::GetValueByMinusOfTwoRadiusFile(OtherFileValue other,float& fvalue,int iNthJT)
{
	if (iNthJT == 1)
	{
		int iPriv = other.irow;
		int iAim = other.icolumn;
		std::map<int,float>::iterator itt;
		itt = m_MapJT1LayerToR.find(iPriv);
		float fPR = itt->second;//起始边半径
		itt = m_MapJT1LayerToR.find(iAim);
		float fAR = itt->second;//目标边半径
		float fbiggestR = GetBiggestLMRadius(iNthJT);
	}
}

void MainWindow::GetResultValueByAllFile(NodeNew& nodenew)
{
	if (nodenew.isop)
	{
		return;
	}
	else
	{
		float tmpValue;
		if (IsNumber(nodenew.num))
		{
			nodenew.fvalue = QString::fromLocal8Bit(nodenew.num.c_str()).toFloat();
		}
		else if (nodenew.num == "Num")
		{
			nodenew.fvalue = m_LayerNum;
		}
		else if(nodenew.num == "MAXR")
		{
			nodenew.fvalue = m_fmax_r;

		}
		else if(IsParamInInIFile(nodenew.num,tmpValue))
		{
			nodenew.fvalue = tmpValue;
		}
		else
		{
			float tmpf;
			bool sus = GetFileValue(nodenew.num,tmpf);

			if(sus == true)
			{
				nodenew.fvalue = tmpf;
			}
			else
			{
				return;
			}
		}
	}
}

void MainWindow::DivideStringByDouHao(std::string str,std::vector<std::string>& res)
{
	res.clear();
	std::string Delema=",";  //  'a,b;; c d'.split(/[\s\,\;]+/); // ['a', 'b', 'c', 'd']
	split(str,res,Delema);
}

bool MainWindow::GetFileValue(std::string mid,float& fvalue)
{
	if(mid.find_first_of("(") != std::string::npos )
	{
		mid = mid.substr(1,mid.size()-2);
	}

	std::vector<std::string> res;
	DivideStringByDouHao(mid,res);

	OtherFileValue t;
	QString fileName = QString::fromLocal8Bit(res[0].c_str());
	QString strRow = QString::fromLocal8Bit(res[1].c_str());
	QString strColumn = QString::fromLocal8Bit(res[2].c_str());
	t.strFileName = fileName;

	//行
	bool brow = IsNumber(strRow.toStdString());
	if(brow == false)
	{		
		int tmpindex;
		QString IndexVar = strRow;
		if (IndexVar == "i")
		{
			tmpindex = m_index;
		}
		else if(IndexVar == "i1")
		{
			tmpindex = m_iTwlc1;
		}
		else if (IndexVar == "i2")
		{
			tmpindex = m_iTwlc2;
		}
		else if (IndexVar == "i3")
		{
			tmpindex = m_iTwlc3;
		}
		else if (IndexVar == "i4")
		{
			tmpindex = m_iTwlc4;
		}
		else if (IndexVar == "i5")
		{
			tmpindex = m_iTwlc5;
		}
		else if (IndexVar == "i6")
		{
			tmpindex = m_iTwlc6;
		}
		else if (IndexVar == "i7")
		{
			tmpindex = m_iTwlc7;
		}
		else if (IndexVar == "i8")
		{
			tmpindex = m_iTwlc8;
		}
		else if (IndexVar == "i9")
		{
			tmpindex = m_iTwlc9;
		}
		else if (IndexVar == "i10")
		{
			tmpindex = m_iTwlc10;
		}
		else if(IndexVar == "i11")
		{
			tmpindex = m_iTwlc11;
		}
		else if (IndexVar == "i12")
		{
			tmpindex = m_iTwlc12;
		}
		else if (IndexVar == "i13")
		{
			tmpindex = m_iTwlc13;
		}
		else if (IndexVar == "i14")
		{
			tmpindex = m_iTwlc14;
		}
		else if (IndexVar == "i15")
		{
			tmpindex = m_iTwlc15;
		}
		else if (IndexVar == "i16")
		{
			tmpindex = m_iTwlc16;
		}
		else if (IndexVar == "i17")
		{
			tmpindex = m_iTwlc17;
		}
		else if (IndexVar == "i18")
		{
			tmpindex = m_iTwlc18;
		}
		else if (IndexVar == "i19")
		{
			tmpindex = m_iTwlc19;
		}
		else if (IndexVar == "i20")
		{
			tmpindex = m_iTwlc20;
		}
		else
		{

			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				//strWarning = strWarning.fromLocal8Bit("推弯流程INDEX错误!");
				strWarning = strWarning.fromLocal8Bit("error with Process File!");

			}
			else
			{
				strWarning = m_vCurLanguageWords[86];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return false;
		}

		t.irow = tmpindex;
	}
	else
	{
		t.irow = strRow.toInt();
	}

	//列
	bool bcolumn = IsNumber(strColumn.toStdString());
	if(bcolumn == false)
	{		
		int tmpindexC;
		QString IndexCVar = strColumn;
		if (IndexCVar == "i")
		{
			tmpindexC = m_index;
		}
		else if(IndexCVar == "i1")
		{
			tmpindexC = m_iTwlc1;
		}
		else if (IndexCVar == "i2")
		{
			tmpindexC = m_iTwlc2;
		}
		else if (IndexCVar == "i3")
		{
			tmpindexC = m_iTwlc3;
		}
		else if (IndexCVar == "i4")
		{
			tmpindexC = m_iTwlc4;
		}
		else if (IndexCVar == "i5")
		{
			tmpindexC = m_iTwlc5;
		}
		else if (IndexCVar == "i6")
		{
			tmpindexC = m_iTwlc6;
		}
		else if (IndexCVar == "i7")
		{
			tmpindexC = m_iTwlc7;
		}
		else if (IndexCVar == "i8")
		{
			tmpindexC = m_iTwlc8;
		}
		else if (IndexCVar == "i9")
		{
			tmpindexC = m_iTwlc9;
		}
		else if (IndexCVar == "i10")
		{
			tmpindexC = m_iTwlc10;
		}
		else if(IndexCVar == "i11")
		{
			tmpindexC = m_iTwlc11;
		}
		else if (IndexCVar == "i12")
		{
			tmpindexC = m_iTwlc12;
		}
		else if (IndexCVar == "i13")
		{
			tmpindexC = m_iTwlc13;
		}
		else if (IndexCVar == "i14")
		{
			tmpindexC = m_iTwlc14;
		}
		else if (IndexCVar == "i15")
		{
			tmpindexC = m_iTwlc15;
		}
		else if (IndexCVar == "i16")
		{
			tmpindexC = m_iTwlc16;
		}
		else if (IndexCVar == "i17")
		{
			tmpindexC = m_iTwlc17;
		}
		else if (IndexCVar == "i18")
		{
			tmpindexC = m_iTwlc18;
		}
		else if (IndexCVar == "i19")
		{
			tmpindexC = m_iTwlc19;
		}
		else if (IndexCVar == "i20")
		{
			tmpindexC = m_iTwlc20;
		}
		else
		{

			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				//strWarning = strWarning.fromLocal8Bit("推弯流程INDEX错误!");
				strWarning = strWarning.fromLocal8Bit("error with Process File!");

			}
			else
			{
				strWarning = m_vCurLanguageWords[86];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return false;

		}

		t.icolumn = tmpindexC;
	}
	else
	{
		t.icolumn = strColumn.toInt();
	}

	if(fileName == QString::fromLocal8Bit("参数.csv"))
	{
		GetValueByParamFileRowAndColumn(t,fvalue);
	}
	else if(fileName == QString::fromLocal8Bit("加工值.csv"))
	{					
		GetValueByProductionFileRowAndColumn(t,fvalue);	
	}
	else if(fileName == QString::fromLocal8Bit("推弯流程n.csv"))
	{
		int twlc_num = m_iN/*GetTWLCNUM(m_index)*/;//当前弯的推弯流程号		
		GetValueByTWLCNUMFileRowAndColumnNew(t,fvalue,twlc_num);
	}
	else if (fileName == QString::fromLocal8Bit("JT1_SRadius.csv"))
	{
		GetValueBySumOfTwoRadiusFile(t,fvalue,1);
	}
	else if (fileName == QString::fromLocal8Bit("JT1_DRadius.csv"))
	{
		//GetValueByMinusOfTwoRadiusFile(t,fvalue,1);
	}

	return true;
}

std::string MainWindow::GetSymbol(std::string str)
{
	if(str.find_first_of('>')!= std::string::npos)
	{
		return std::string(">");
	}
	else if (str.find_first_of('<')!= std::string::npos)
	{
		return std::string("<");
	}
	//else if (str.find_first_of('!')!= std::string::npos)
	//{
	//	return std::string("!=");
	//}
	else if (str.find_first_of('=')!= std::string::npos)
	{
		return std::string("=");
	}

}

std::string MainWindow::GetOperator(std::string str)
{
	if(str.find_first_of('+')!= std::string::npos)
	{
		return std::string("+");
	}
	else if (str.find_first_of('=')!= std::string::npos)
	{
		// 防止公式中出现"twcolor=-1"这种先识别出负数，没有识别出等号
		return std::string("=");
	}
	else if (str.find_first_of('-')!= std::string::npos)
	{
		return std::string("-");
	}
	else if (str.find_first_of('*')!= std::string::npos)
	{
		return std::string("*");
	}
	else if (str.find_first_of('/')!= std::string::npos)
	{
		return std::string("/");
	}
	else if (str.find_first_of('%')!= std::string::npos)
	{
		return std::string("%");
	}

}

void MainWindow::DivideString(std::string str,std::vector<std::string>& res)
{
	res.clear();
	std::string Delema="/[\+\-\=\*\/]+/";  //  'a,b;; c d'.split(/[\s\,\;]+/); // ['a', 'b', 'c', 'd']
	split(str,res,Delema);
}

bool MainWindow::IsNumber(std::string num)
{
	/*if (num.empty())
	{
	return false;
	}

	for(int i=0;i<num.length();i++)
	{
	if (num[i]=='.')
	{
	continue;
	}

	if (num[i]<'0' || num[i] > '9')
	{
	return false;
	}
	}

	return true;*/

	if (num=="i7")
	{
		int m = 0;
	}

	istringstream sin(num);
	double test;
	return sin >> test && sin.eof();
}

void MainWindow::OnlyReadProcessFileNew()
{
	if (!m_MapStepTovRowString.empty())
	{
		return;
	}

	m_MapStepTovRowString.erase(m_MapStepTovRowString.begin(),m_MapStepTovRowString.end());
	m_vLoopTimes.clear();
	m_LoopTimes = 0;
	int iColumnNum = 0;
	bool btmpLoop = false;//当前行是否在循环体里面

	QString strProcessFile = need_filePath + QString::fromLocal8Bit("/流程.csv");
	QByteArray ba = strProcessFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);
		USES_CONVERSION; 
		std::string szBuffer = W2A(buf);
		if (szBuffer.find("//")!= std::string::npos)
		{
			int pos = szBuffer.find("//");
			szBuffer = szBuffer.substr(0,pos);
		}

		std::vector<string> Res;
		std::string Delema=","/*",(?=([^\"]*\"[^\"]*\")*[^\"]*$)"*/;
		split(szBuffer,Res,Delema);
		iColumnNum = Res.size();


		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			szBuffer = W2A(buf);

			if (szBuffer.find("//")!= std::string::npos)
			{
				int pos = szBuffer.find("//");
				szBuffer = szBuffer.substr(0,pos);
			}

			Res.clear();
			split(szBuffer,Res,Delema);

			/*		for (int si=0;si<Res.size();si++)
			{
			Trim(Res[si]);
			}
			*/

			if(Res.size() <  2)
			{
				continue;
			}
			/*		if (Res.size()== 1)
			{

			int re;
			re=0;
			}*/
			if ("1800001"==Res[0])
			{
				int a = 0;
			}

			std::string  tmpCode = Res[1];//GCode

			//**********************循环开始行************************
			if(tmpCode == "LOOP_START" ||tmpCode == "LOOP_END" )//判读循环
			{
				if(tmpCode == "LOOP_START")
				{
					m_LoopStartIndex = QString::fromStdString(Res[0]).toInt();
					btmpLoop = true;
					//bool isLoopTimes = false;
					//计算循环次数
					String2Formula strToFormu;
					std::string mid;
					for(size_t i=2;i<Res.size();i++)
					{
						if(isCSVFileName(Res[i]))
						{
							mid = Res[i] + ','+Res[i+1]+','+Res[i+2];

							// 去掉两段的冒号
							mid = mid.substr(1,mid.size()-2);
							i = i+2;
						}
					}
					std::vector<NodeNew> back = strToFormu.midToBack(mid);
					for(size_t d=0;d<back.size();d++)
					{
						GetResultValue(back[d]);
					}

					double res = strToFormu.getValue(back);
					m_LoopTimes = (int)res;

				}
				else
				{
					btmpLoop = false;
				}

				AnalyzeCurRow(Res);
			}
			else if (tmpCode.find("INDEX")!= std::string::npos)
			{
				// INDEX格式此处不用处理
				AnalyzeCurRow(Res);
			}
			else if(tmpCode.find("COLOR")!= std::string::npos)
			{
				AnalyzeCurRow(Res);
			}
			else if(tmpCode.find("TWCOLOR")!= std::string::npos)
			{
				AnalyzeCurRow(Res);
			}
			else if(tmpCode.find_first_of('G') == 0 
				|| tmpCode.find_first_of('M') == 0
				|| tmpCode.find_first_of('P') == 0)//G,M
			{			
				AnalyzeCurRow(Res);

				//if(tmpCode=="G4" || tmpCode=="M1030")
				std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.find(QString(tmpCode.c_str()));
				if (iter != m_mapGcodeSpecialMove.end() && iter->second==3) // 3表明是沿着机头弯管运动
				{
					m_dRDistance = atoi(Res[3].c_str());
					m_dRDistance = fabs(m_dRDistance);
				}
				else if (iter != m_mapGcodeSpecialMove.end() && iter->second==4)  // 沿着机头弯管垂直运动
				{
					m_dTDistance= atoi(Res[3].c_str());
					//m_dTDistance = fabs(m_dTDistance);
				}
			}
			else if (tmpCode.find("CJ")!= std::string::npos)
			{
				AnalyzeCurRow(Res);
			}
			else if (tmpCode.find("CREATE")!= std::string::npos)
			{
				AnalyzeCurRow(Res);
			}
			else
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("功能码格式错误!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[86];
				}

				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	std::map<int,std::vector<std::vector<FileColumn>>> a = m_MapStepTovRowString;
}

void MainWindow::AllStepsNew()
{
	char process_i[32] = "";
	char process_c[32] = "";
	char process_n[32] = "";
	char process_twc[32] = "";

	char process_i1[32] = "";
	char process_i2[32] = "";
	char process_i3[32] = "";
	char process_i4[32] = "";
	char process_i5[32] = "";
	char process_i6[32] = "";
	char process_i7[32] = "";
	char process_i8[32] = "";
	char process_i9[32] = "";
	char process_i10[32] = "";
	char process_i11[32] = "";
	char process_i12[32] = "";
	char process_i13[32] = "";
	char process_i14[32] = "";
	char process_i15[32] = "";
	char process_i16[32] = "";
	char process_i17[32] = "";
	char process_i18[32] = "";
	char process_i19[32] = "";
	char process_i20[32] = "";

	float fprocess_i;//ini文件的初始加紧值
	float fprocess_c;//
	float fprocess_n;
	float fprocess_twc;

	float fprocess_i1;//ini文件的初始加紧值
	float fprocess_i2;//ini文件的初始加紧值
	float fprocess_i3;//ini文件的初始加紧值
	float fprocess_i4;//ini文件的初始加紧值
	float fprocess_i5;//ini文件的初始加紧值
	float fprocess_i6;//ini文件的初始加紧值
	float fprocess_i7;//ini文件的初始加紧值
	float fprocess_i8;//ini文件的初始加紧值
	float fprocess_i9;//ini文件的初始加紧值
	float fprocess_i10;//ini文件的初始加紧值
	float fprocess_i11;//ini文件的初始加紧值
	float fprocess_i12;//ini文件的初始加紧值
	float fprocess_i13;//ini文件的初始加紧值
	float fprocess_i14;//ini文件的初始加紧值
	float fprocess_i15;//ini文件的初始加紧值
	float fprocess_i16;//ini文件的初始加紧值
	float fprocess_i17;//ini文件的初始加紧值
	float fprocess_i18;//ini文件的初始加紧值
	float fprocess_i19;//ini文件的初始加紧值
	float fprocess_i20;//ini文件的初始加紧值

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I, 3, process_i);
	fprocess_i = atof(process_i);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_C, 3, process_c);
	fprocess_c = atof(process_c);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_N, 3, process_n);
	fprocess_n = atof(process_n);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_TWC, 3, process_twc);
	fprocess_twc = atof(process_twc);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I1, 3, process_i1);
	fprocess_i1 = atof(process_i1);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I2, 3, process_i2);
	fprocess_i2 = atof(process_i2);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I3, 3, process_i3);
	fprocess_i3 = atof(process_i3);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I4, 3, process_i4);
	fprocess_i4 = atof(process_i4);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I5, 3, process_i5);
	fprocess_i5 = atof(process_i5);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I6, 3, process_i6);
	fprocess_i6 = atof(process_i6);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I7, 3, process_i7);
	fprocess_i7 = atof(process_i7);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I8, 3, process_i8);
	fprocess_i8 = atof(process_i8);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I9, 3, process_i9);
	fprocess_i9 = atof(process_i9);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I10, 3, process_i10);
	fprocess_i10 = atof(process_i10);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I11, 3, process_i11);
	fprocess_i11 = atof(process_i11);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I12, 3, process_i12);
	fprocess_i12 = atof(process_i12);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I13, 3, process_i13);
	fprocess_i13 = atof(process_i13);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I14, 3, process_i14);
	fprocess_i14 = atof(process_i14);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I15, 3, process_i15);
	fprocess_i15 = atof(process_i15);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I16, 3, process_i16);
	fprocess_i16 = atof(process_i16);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I17, 3, process_i17);
	fprocess_i17 = atof(process_i17);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I18, 3, process_i18);
	fprocess_i18 = atof(process_i18);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I19, 3, process_i19);
	fprocess_i19 = atof(process_i19);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PROCESS_I20, 3, process_i20);
	fprocess_i20 = atof(process_i20);

	//推弯流程索引变量
	m_iTwlc1 = (int)fprocess_i1;
	m_iTwlc2 = (int)fprocess_i2;
	m_iTwlc3 = (int)fprocess_i3;
	m_iTwlc4 = (int)fprocess_i4;
	m_iTwlc5 = (int)fprocess_i5;
	m_iTwlc6 = (int)fprocess_i6;
	m_iTwlc7 = (int)fprocess_i7;
	m_iTwlc8 = (int)fprocess_i8;
	m_iTwlc9 = (int)fprocess_i9;
	m_iTwlc10 = (int)fprocess_i10;
	m_iTwlc11 = (int)fprocess_i11;
	m_iTwlc12 = (int)fprocess_i12;
	m_iTwlc13 = (int)fprocess_i13;
	m_iTwlc14 = (int)fprocess_i14;
	m_iTwlc15 = (int)fprocess_i15;
	m_iTwlc16 = (int)fprocess_i16;
	m_iTwlc17 = (int)fprocess_i17;
	m_iTwlc18 = (int)fprocess_i18;
	m_iTwlc19 = (int)fprocess_i19;
	m_iTwlc20 = (int)fprocess_i20;

	m_index = (int)fprocess_i;//当前弯索引
	int icolor = (int)fprocess_c;// 
	int itwcolor = (int)fprocess_twc;
	m_iN = (int)fprocess_n;

	int iTimes = 0;//第几次循环
	bool skipIncrease = false;

	m_vAllStepvProcess.clear();

	std::map<int,std::vector<std::vector<FileColumn>>>::iterator it = m_MapStepTovRowString.begin();
	for(;it!=m_MapStepTovRowString.end();)
	{
		std::vector<Process> tmpvec;
		std::vector<std::vector<FileColumn>>::iterator it2 = it->second.begin();
		for(;it2!=it->second.end();it2++)
		{
			skipIncrease = false;
			Process tmpProcess;

			std::string process;
			for (size_t m = 0; m < (*it2).size();++m)
			{
				process.append((*it2)[m].str);
				process.append("_");
			}
			tmpProcess.strProcess = process.c_str();

			QString info("process_step "+ QString::fromLocal8Bit(tmpProcess.strProcess.c_str()));
			LogInfo(info);

			// 要与流程文件搭配使用，判断下一个动作是左边还是右边加工
			if ((m_index-1)<m_vproductionvalues.size())
			{
				tmpProcess.fDir = m_vproductionvalues[m_index-1].flr;
			}

			QString strcode =  QString::fromLocal8Bit ((*it2)[1].str.c_str());
			if(QString::fromLocal8Bit ((*it2)[1].str.c_str()) == "LOOP_START")
			{
				iTimes++;
				if(iTimes>m_LoopTimes)
				{
					std::vector<std::vector<Process>> a = m_vAllStepvProcess;
					return;
				}
			}
			else if (QString::fromLocal8Bit ((*it2)[1].str.c_str()) == "LOOP_END")
			{
				it = GetStartIt(m_LoopStartIndex);
				skipIncrease = true;
				break;
			}
			else if(QString::fromLocal8Bit ((*it2)[1].str.c_str()) =="INDEX")
			{
				std::string formula = (*it2)[2].str;
				if (formula.find("{")!= std::string::npos)
				{
					// INDEX中存在算式的情况
					int pos0 = formula.find("{");
					int pos1 = formula.find("}");

					//std::string head = formula.substr(0,pos0);
					std::string formula0 = formula.substr(pos0+1,pos1-pos0-1);

					int pos2 = formula.find("=");
					QString strIndex = QString::fromLocal8Bit(formula.substr(1,pos2-1).c_str());

					//std::vector<std::string> vresult;	
					//DivideString(formula0,vresult);

					float value;
					FileColumn fc;
					fc.str = formula0;
					fc.type = Formula;
					GetCellValue(fc,value);
					UpdateIndexValue(strIndex,value);
				}
				else
				{
					std::vector<std::string> vresult;
					QString tmpOpe = QString::fromLocal8Bit(GetOperator(formula).c_str());		
					DivideString(formula,vresult);

					QString strIndexVar = QString::fromLocal8Bit(vresult[0].c_str());
					QString strOperator = tmpOpe;
					QString value = QString::fromLocal8Bit(vresult[1].c_str());
					if (vresult.size()==2) // 如果是两个操作数
					{
						float fVal;
						if(value == QString::fromLocal8Bit("i"))
						{
							fVal = (float)m_index;
						}
						else
						{
							if((*it2)[2].type == Data)
							{
								fVal = value.toFloat();
							}
							else if((*it2)[2].type == File)
							{
								float ftmpvalue;
								bool sus = GetFileValue(vresult[1],ftmpvalue);
								if(sus == true)
								{
									fVal = ftmpvalue;
								}
							}
						}

						if (strIndexVar == "i")
						{
							if(strOperator == "+")
							{
								m_index = m_index + (int)fVal;
							}
							else if(strOperator == "-")
							{
								m_index = m_index - (int)fVal;
							}
							else if(strOperator == "*")
							{
								m_index = m_index * (int)fVal;
							}
							else if(strOperator == "/")
							{
								bool iszero = isDivideZero(fVal);
								if (iszero == true)
								{
									QString strWarning;
									if (m_vCurLanguageWords.empty())
									{
										//strWarning = strWarning.fromLocal8Bit("存在分母为0!");
										strWarning = strWarning.fromLocal8Bit("error with Process File!");
									}
									else
									{
										strWarning = m_vCurLanguageWords[86];
									}
									QMessageBox::warning(this,"",strWarning);
									LogInfo(strWarning);
									return;
								}
								m_index = m_index / (int)fVal;

							}
							else if(strOperator =="=")
							{
								m_index = (int)fVal;
							}
						}
						else if (strIndexVar == "n")
						{
							if(strOperator == "+")
							{
								m_iN = m_iN + (int)fVal;
							}
							else if(strOperator == "-")
							{
								m_iN = m_iN - (int)fVal;
							}
							else if(strOperator == "*")
							{
								m_iN = m_iN * (int)fVal;

							}
							else if(strOperator == "/")
							{
								bool iszero = isDivideZero(fVal);
								if (iszero == true)
								{
									QString strWarning;
									if (m_vCurLanguageWords.empty())
									{
										//strWarning = strWarning.fromLocal8Bit("存在分母为0!");
										strWarning = strWarning.fromLocal8Bit("error with Process File!");
									}
									else
									{
										strWarning = m_vCurLanguageWords[86];
									}
									QMessageBox::warning(this,"",strWarning);
									LogInfo(strWarning);
									return;
								}
								m_iN = m_iN / (int)fVal;
							}
							else if(strOperator =="=")
							{
								m_iN = (int)fVal;
							}
						}
						else 
						{
							UpdateTWLCINDEX(strIndexVar,strOperator,fVal);
						}
					}
					else if (vresult.size()==3)
					{
						QString value1 = QString::fromLocal8Bit(vresult[2].c_str());
						UpdateTWLCINDEX3Value(strIndexVar,strOperator,value,value1);
					}
				}
				//else
				//{
				//	float value;
				//	FileColumn fc;
				//	fc.str = formula;
				//	fc.type = Formula;
				//	GetCellValue(fc,value);
				//	UpdateIndexValue(strIndexVar,value);
				//}
			}
			else if(QString::fromLocal8Bit ((*it2)[1].str.c_str()) =="COLOR")
			{
				tmpProcess.istep = QString::fromLocal8Bit ((*it2)[0].str.c_str()).toInt();
				tmpProcess.strCode = QString::fromLocal8Bit ((*it2)[1].str.c_str());

				std::string formula = (*it2)[2].str;
				QString tmpOpe = QString::fromLocal8Bit(GetOperator(formula).c_str());		
				std::vector<std::string> vresult;
				DivideString(formula,vresult);

				QString strIndexVar = QString::fromLocal8Bit(vresult[0].c_str());
				QString strOperator = tmpOpe;
				QString value = QString::fromLocal8Bit(vresult[1].c_str());
				float fVal;
				if(value == QString::fromLocal8Bit("i"))
				{
					fVal = (float)m_index;
				}
				else
				{
					fVal = value.toFloat();
				}			
				if (strIndexVar == "c")
				{
					if(strOperator == "+")
					{			
						icolor = icolor + (int)fVal;
					}
					else if(strOperator == "-")
					{
						icolor = icolor - (int)fVal;
					}
					else if(strOperator == "*")
					{
						icolor = icolor * (int)fVal;

					}
					else if(strOperator == "/")
					{

						bool iszero = isDivideZero(fVal);
						if (iszero == true)
						{
							QString strWarning;
							if (m_vCurLanguageWords.empty())
							{
								strWarning = strWarning.fromLocal8Bit("error with ProcessFile!");
							}
							else
							{
								strWarning = m_vCurLanguageWords[86];
							}
							QMessageBox::warning(this,"",strWarning);
							LogInfo(strWarning);
							return;
						}
						icolor = icolor / (int)fVal;

					}
					else if(strOperator =="=")
					{

						if(value == "i")
						{
							icolor = m_index -1;
						}
						else
						{
							icolor = (int)fVal;
						}
					}
				}

				tmpProcess.iColor = icolor;

				tmpvec.push_back(tmpProcess);
			}
			else if(QString::fromLocal8Bit ((*it2)[1].str.c_str()) =="TWCOLOR")
			{
				tmpProcess.istep = QString::fromLocal8Bit ((*it2)[0].str.c_str()).toInt();
				tmpProcess.strCode = QString::fromLocal8Bit ((*it2)[1].str.c_str());

				std::string formula = (*it2)[2].str;
				QString tmpOpe = QString::fromLocal8Bit(GetOperator(formula).c_str());		
				std::vector<std::string> vresult;
				DivideString(formula,vresult);

				QString strIndexVar = QString::fromLocal8Bit(vresult[0].c_str());
				QString strOperator = tmpOpe;
				QString value = QString::fromLocal8Bit(vresult[1].c_str());
				float fVal;
				if(value == QString::fromLocal8Bit("i"))
				{
					fVal = (float)m_index;
				}
				else
				{
					fVal = value.toFloat();
				}			
				if (strIndexVar == "twcolor")
				{
					if(strOperator == "+")
					{			
						itwcolor = itwcolor + (int)fVal;
					}
					else if(strOperator == "-")
					{
						itwcolor = itwcolor - (int)fVal;
					}
					else if(strOperator == "*")
					{
						itwcolor = itwcolor * (int)fVal;

					}
					else if(strOperator == "/")
					{

						bool iszero = isDivideZero(fVal);
						if (iszero == true)
						{
							QString strWarning;
							if (m_vCurLanguageWords.empty())
							{
								strWarning = strWarning.fromLocal8Bit("error with ProcessFile!");
							}
							else
							{
								strWarning = m_vCurLanguageWords[86];
							}
							QMessageBox::warning(this,"",strWarning);
							LogInfo(strWarning);
							return;
						}
						itwcolor = itwcolor / (int)fVal;

					}
					else if(strOperator =="=")
					{

						if(value == "i")
						{
							itwcolor = m_index -1;
						}
						else
						{
							itwcolor = (int)fVal;
							if (formula.find("-")!=string::npos)
							{
								itwcolor *= -1;
							}
						}
					}
				}

				tmpProcess.iTwColor = itwcolor;
				tmpvec.push_back(tmpProcess);
			}
			else if(QString::fromLocal8Bit ((*it2)[1].str.c_str()) =="CJ")
			{
				bool use = false;//条件跳转判断是否成功
				int  iAimStep;
				QString strAimCode;
				if(QString::fromLocal8Bit ((*it2)[3].str.c_str()) == "0")//无条件跳转
				{
					iAimStep = QString::fromLocal8Bit ((*it2)[2].str.c_str()).toInt();
					it = GetAimItByAimStep(iAimStep);
					skipIncrease = true;
					break;
				}
				else if(QString::fromLocal8Bit ((*it2)[3].str.c_str()) != "0")//条件跳转
				{
					std::string mid = (*it2)[3].str;
					std::string symbol = GetSymbol(mid);
					std::string former;
					std::string laster;
					GetTwoFormulaBySymbol(former,laster,symbol,mid);

					FileColumn TypeFormer;
					FileColumn TypeLaster;
					TypeFormer.type = GetStringType(former);
					TypeFormer.str = former;
					TypeLaster.type = GetStringType(laster);
					TypeLaster.str = laster;

					float fFormer,fLaster;
					GetCellValue(TypeFormer,fFormer);
					GetCellValue(TypeLaster,fLaster);

					use = GetIsDoCJ(symbol,fFormer,fLaster);			
					if(use == true)
					{
						iAimStep = QString::fromLocal8Bit ((*it2)[2].str.c_str()).toInt();
						it = GetAimItByAimStep(iAimStep);
						skipIncrease = true;
						break;
					}
				}
			}
			else if(QString::fromLocal8Bit ((*it2)[1].str.c_str()) =="CREATE")
			{
				tmpProcess.istep = QString::fromLocal8Bit ((*it2)[0].str.c_str()).toInt();
				tmpProcess.strCode = QString::fromLocal8Bit ((*it2)[1].str.c_str());

				tmpvec.push_back(tmpProcess);
			}
			else if((*it2)[1].str.find_first_of('M')==0  
				|| (*it2)[1].str.find_first_of('G')==0
				|| (*it2)[1].str.find_first_of('P')==0)
			{		
				tmpProcess.istep = QString::fromLocal8Bit((*it2)[0].str.c_str()).toInt();
				//if (tmpProcess.istep == 360 || tmpProcess.istep == 546)
				//{
				//	int a = 0;
				//}

				// 运动代码
				tmpProcess.strCode = QString::fromLocal8Bit((*it2)[1].str.c_str());
				tmpProcess.iDirectly = QString::fromLocal8Bit((*it2)[2].str.c_str()).toInt();
				if(tmpProcess.iDirectly == 1)
				{
					tmpProcess.fvalue = QString::fromLocal8Bit((*it2)[3].str.c_str()).toFloat();
					tmpProcess.iPipeRotate = QString::fromLocal8Bit((*it2)[4].str.c_str()).toInt();
					tmpProcess.iCreatPipe = QString::fromLocal8Bit((*it2)[5].str.c_str()).toInt();
					tmpProcess.iCurveOrStraight = QString::fromLocal8Bit((*it2)[6].str.c_str()).toInt();
					if ((*it2).size()<8)
					{
						std::string info("Step ");
						info.append((*it2)[0].str);
						info.append(" CurveR no value !");
						QMessageBox::warning(this,"Error",info.c_str());

						return;
					}

					GetCellValue((*it2)[7],tmpProcess.fcurveR);

                    // 增加机头编号和长管加工端
					if ((*it2).size()<9)
					{
						std::string info("Step ");
						info.append((*it2)[0].str);
						info.append(" Header no value !");
						QMessageBox::warning(this,"Error",info.c_str());

						return;
					}

					tmpProcess.iHeader = QString::fromLocal8Bit((*it2)[8].str.c_str()).toInt();

					if ((*it2).size()<10)
					{
						std::string info("Step ");
						info.append((*it2)[0].str);
						info.append(" Tip no value !");
						QMessageBox::warning(this,"Error",info.c_str());

						return;
					}

					tmpProcess.iTip = QString::fromLocal8Bit((*it2)[9].str.c_str()).toInt();
				}
				else if(tmpProcess.iDirectly == 2)
				{
					GetValueInGroupSettingFileByGcode(QString::fromLocal8Bit((*it2)[1].str.c_str()),tmpProcess.fvalue);
					tmpProcess.iPipeRotate = QString::fromLocal8Bit((*it2)[4].str.c_str()).toInt();
					tmpProcess.iCreatPipe = QString::fromLocal8Bit((*it2)[5].str.c_str()).toInt();
					tmpProcess.iCurveOrStraight = QString::fromLocal8Bit((*it2)[6].str.c_str()).toInt();
					GetCellValue((*it2)[7],tmpProcess.fcurveR);
				}
				else if(tmpProcess.iDirectly == 0)
				{
					GetCellValue((*it2)[3],tmpProcess.fvalue);
					tmpProcess.iPipeRotate = QString::fromLocal8Bit((*it2)[4].str.c_str()).toInt();
					tmpProcess.iCreatPipe = QString::fromLocal8Bit((*it2)[5].str.c_str()).toInt();
					tmpProcess.iCurveOrStraight = QString::fromLocal8Bit((*it2)[6].str.c_str()).toInt();
					GetCellValue((*it2)[7],tmpProcess.fcurveR);

                    // 增加机头编号和长管加工端
                    if ((*it2).size()<9)
					{
						std::string info("Step ");
						info.append((*it2)[0].str);
						info.append(" Header no value !");
						QMessageBox::warning(this,"Error",info.c_str());

						return;
					}

					tmpProcess.iHeader = QString::fromLocal8Bit((*it2)[8].str.c_str()).toInt();

					if ((*it2).size()<10)
					{
						std::string info("Step ");
						info.append((*it2)[0].str);
						info.append(" Tip no value !");
						QMessageBox::warning(this,"Error",info.c_str());

						return;
					}

					tmpProcess.iTip = QString::fromLocal8Bit((*it2)[9].str.c_str()).toInt();
				}
				else if (tmpProcess.iDirectly == 11)//小车往复推弯
				{
					GetCellValue((*it2)[3],tmpProcess.fvalue);
					tmpProcess.iPipeRotate = QString::fromLocal8Bit((*it2)[4].str.c_str()).toInt();
					tmpProcess.iCreatPipe = QString::fromLocal8Bit((*it2)[5].str.c_str()).toInt();
					tmpProcess.iCurveOrStraight = QString::fromLocal8Bit((*it2)[6].str.c_str()).toInt();
					GetCellValue((*it2)[7],tmpProcess.fcurveR);
				}
				else if (tmpProcess.iDirectly == 12)//小车夹送
				{
					GetCellValue((*it2)[3],tmpProcess.fvalue);
					tmpProcess.iPipeRotate = QString::fromLocal8Bit((*it2)[4].str.c_str()).toInt();
					tmpProcess.iCreatPipe = QString::fromLocal8Bit((*it2)[5].str.c_str()).toInt();
					tmpProcess.iCurveOrStraight = QString::fromLocal8Bit((*it2)[6].str.c_str()).toInt();
					GetCellValue((*it2)[7],tmpProcess.fcurveR);
				}

				tmpProcess.iNthCureve = m_index;
				tmpvec.push_back(tmpProcess);				
			}
		}

		if (tmpvec.size()>0)
		{
			m_vAllStepvProcess.push_back(tmpvec);
		}
		else
		{
			int a = 0;
		}

		if (skipIncrease)
		{
		}
		else
		{
			it++;
		}
	}
}

std::vector<std::pair<int,std::string> > MainWindow::GetFormulaString(std::vector<pair<bool,std::string> >& vres)
{
	std::vector<int> itmp;
	std::string strtmp;
	for(size_t i=0;i<vres.size();i++)
	{	
		if(isHasKuoHao1(vres[i].second))
		{
			itmp.push_back(i);
		}
	}

	vector<std::pair<int ,string> > pairs;

	for (size_t i=0;i<itmp.size()/2;i++)
	{
		for(int j=itmp[2*i];j<=itmp[2*i+1];j++)
		{
			strtmp = strtmp + vres[j].second;
			vres[j].first = false;
		}

		pairs.push_back(make_pair(itmp[2*i],strtmp));
	}

	return pairs;
}

std::vector<std::pair<int,std::string> > MainWindow::GetVarString(std::vector<pair<bool,std::string> >& vres)
{
	vector<std::pair<int ,string> > pairs;
	std::string mid;
	for(size_t i=0;i<vres.size();i++)
	{
		if (!vres[i].first)
		{
			continue;
		}

		if(GetStringType(vres[i].second)==Var)
		{
			pairs.push_back(make_pair(i,vres[i].second));
		}
	}

	return pairs;
}

std::vector<std::pair<int,std::string> > MainWindow::GetFileNameString(std::vector<pair<bool,std::string> >& vres)
{
	vector<std::pair<int ,string> > pairs;
	std::string mid;
	for(size_t i=0;i<vres.size();i++)
	{
		if (!vres[i].first)
		{
			continue;
		}

		if(isCSVFileName(vres[i].second))
		{
			mid = vres[i].second  + vres[i+1].second + vres[i+2].second;
			vres[i].first = false;
			vres[i+1].first = false;
			vres[i+2].first = false;
			i = i+2;
			mid = mid.substr(1,mid.size()-2);
			pairs.push_back(make_pair(i,mid));
		}
	}

	return pairs;
}

void MainWindow::AnalyzeGMCode(std::vector<string> res)
{
	vector<pair<bool,string>> rawString;
	for (size_t i = 0; i < res.size();++i)
	{
		rawString.push_back(make_pair(true,res[i]));
	}

	vector<pair<int, string> > vFumulas;
	vFumulas = GetFormulaString(rawString);

	vector<pair<int,string>> vFiles;
	vFiles = GetFileNameString(rawString);

	map<int, FileColumn> strs;
	FileColumn column;
	for (size_t i = 0; i < rawString.size();++i)
	{
		if (rawString[i].first)
		{
			column.str = rawString[i].second;
			column.type= Data;
			strs[i] = column;
		}
	}

	for (size_t i = 0; i < vFiles.size();++i)
	{
		column.str= vFiles[i].second;
		column.type = File; 
		strs[vFiles[i].first] = column;
	}

	for (size_t i = 0; i < vFumulas.size();++i)
	{
		column.str = vFumulas[i].second;
		column.type = Formula;
		strs[vFumulas[i].first] = column;
	}

	std::map<int,std::vector<std::map<int,FileColumn>>>::iterator it;
	int index = QString::fromLocal8Bit(strs[0].str.c_str()).toInt();
	it = m_MapStepTovColumnToString.find(index);
	if(it == m_MapStepTovColumnToString.end())//不存在
	{
		std::vector<std::map<int,FileColumn>> vtmp;
		vtmp.push_back(strs);
		m_MapStepTovColumnToString[index] = vtmp;
	}
	else if(it != m_MapStepTovColumnToString.end())
	{
		it->second.push_back(strs);
	}
}

void MainWindow::AnalyzeCJCode(std::vector<string> res)
{
	if (res[0]=="360" || res[0]== "546")
	{
		int m = 0;   
	}

	vector<pair<bool,string>> rawString;
	for (size_t i = 0; i < res.size();++i)
	{
		rawString.push_back(make_pair(true,res[i]));
	}

	vector<pair<int, string> > vFumulas;
	vFumulas = GetFormulaString(rawString);

	vector<pair<int,string>> vFiles;
	vFiles = GetFileNameString(rawString);

	vector<pair<int,string>> vVars;
	vVars = GetVarString(rawString);

	map<int, FileColumn> strs;
	FileColumn column;
	for (size_t i = 0; i < rawString.size();++i)
	{
		if (rawString[i].first)
		{
			column.str = rawString[i].second;
			column.type= Data;
			strs[i] = column;
		}
	}

	for (size_t i = 0; i < vFiles.size();++i)
	{
		column.str= vFiles[i].second;
		column.type = File; 
		strs[vFiles[i].first] = column;
	}

	for (size_t i = 0; i < vFumulas.size();++i)
	{
		column.str = vFumulas[i].second;
		column.type = Formula;
		strs[vFumulas[i].first] = column;
	}

	for (size_t i = 0; i < vVars.size();++i)
	{
		column.str = vVars[i].second;
		column.type = Var;
		strs[vFumulas[i].first] = column;
	}

	std::map<int,std::vector<std::map<int,FileColumn>>>::iterator it;
	int index = QString::fromLocal8Bit(strs[0].str.c_str()).toInt();
	it = m_MapStepTovColumnToString.find(index);
	if(it == m_MapStepTovColumnToString.end())//步号不存在
	{
		std::vector<std::map<int,FileColumn>> vtmp;
		vtmp.push_back(strs);
		m_MapStepTovColumnToString[index] = vtmp;
	}
	else if(it != m_MapStepTovColumnToString.end())//步号存在
	{
		it->second.push_back(strs);
	}
}

void MainWindow::AnalyzeLOOP_SorE(std::vector<string> res)
{
	vector<pair<bool,string>> rawString;
	for (size_t i = 0; i < res.size();++i)
	{
		rawString.push_back(make_pair(true,res[i]));
	}

	vector<pair<int, string> > vFumulas;
	vFumulas = GetFormulaString(rawString);

	vector<pair<int,string>> vFiles;
	vFiles = GetFileNameString(rawString);

	map<int, FileColumn> strs;
	FileColumn column;
	for (size_t i = 0; i < rawString.size();++i)
	{
		if (rawString[i].first)
		{
			column.str = rawString[i].second;
			column.type= Data;
			strs[i] = column;
		}
	}

	for (size_t i = 0; i < vFiles.size();++i)
	{
		column.str= vFiles[i].second;
		column.type = File; 
		strs[vFiles[i].first] = column;
	}

	for (size_t i = 0; i < vFumulas.size();++i)
	{
		column.str = vFumulas[i].second;
		column.type = Formula;
		strs[vFumulas[i].first] = column;
	}

	std::map<int,std::vector<std::map<int,FileColumn>>>::iterator it;
	int index = QString::fromLocal8Bit(strs[0].str.c_str()).toInt();
	it = m_MapStepTovColumnToString.find(index);
	if(it == m_MapStepTovColumnToString.end())//步号不存在
	{
		std::vector<std::map<int,FileColumn>> vtmp;
		vtmp.push_back(strs);
		m_MapStepTovColumnToString[index] = vtmp;
	}
	else if(it != m_MapStepTovColumnToString.end())//步号存在
	{
		it->second.push_back(strs);
	}
}

void MainWindow::BuildNormalUserFile(std::map<int,std::map<QString,std::vector<QString>>> values)
{
	std::map<int,std::map<QString,std::vector<QString>>>::iterator it = values.begin();
	for (;it!=values.end();it++)
	{
		int iNum = it->first;
		std::map<QString,std::vector<QString>> MapSideToModels = it->second;
		BuildOneJTFile(iNum,MapSideToModels);
	}
}

void MainWindow::BuildOneJTFile(int num,std::map<QString,std::vector<QString>> SideToModels)
{
	// 将用户分组写进配置文件
	BuildSideConfigFile(num,SideToModels);

	std::map<QString,std::vector<QString>>::iterator it = SideToModels.begin();
	for(;it!=SideToModels.end();it++)
	{
		QString strFileName = QString::fromLocal8Bit("/JT")+ QString::number(num)+ QString::fromLocal8Bit("_") + QString::fromLocal8Bit("%1").arg(it->first) +QString::fromLocal8Bit(".csv");
		std::vector<QString> vValues = it->second;

		QString strDir;
		if (num == 1)
		{
			QString strtmp;
			std::string stmp;
			char sJT1_FILES_PATH[256] = "";
			GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_FILES_PATH, 3, sJT1_FILES_PATH);
			GetWholePath(sJT1_FILES_PATH);
			stmp = sJT1_FILES_PATH;
			strtmp = QString::fromLocal8Bit(stmp.c_str());
			strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
			strDir = strtmp;
			strFileName = strDir + strFileName;
			BuildOneSideFile(strFileName,vValues,it->first,num);

		}
		else if (num == 2)
		{
			QString strtmp;
			std::string stmp;
			char sJT2_FILES_PATH[256] = "";
			GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT2_FILES_PATH, 3, sJT2_FILES_PATH);
			GetWholePath(sJT2_FILES_PATH);
			stmp = sJT2_FILES_PATH;
			strtmp = QString::fromLocal8Bit(stmp.c_str());
			strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
			strDir = strtmp;
			strFileName = strDir + strFileName;
			BuildOneSideFile(strFileName,vValues,it->first,num);
		}

	}


	if (num == 1)
	{
		QString strDir1;
		QString strtmp1;
		std::string stmp1;
		char sJT1_FILES_PATH1[256] = "";
		GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_FILES_PATH, 3, sJT1_FILES_PATH1);
		GetWholePath(sJT1_FILES_PATH1);
		stmp1 = sJT1_FILES_PATH1;
		strtmp1 = QString::fromLocal8Bit(stmp1.c_str());
		strtmp1 = QDir::fromNativeSeparators(strtmp1);//"\"转"/"
		strDir1 = strtmp1;

		QString strJmPath = strDir1 + QString::fromLocal8Bit("/JT1_JMValues.csv");
		QString strLmPath = strDir1 + QString::fromLocal8Bit("/JT1_LMValues.csv");
		BuildNormalUserJMFile(strJmPath,num);
		BuildNormalUserLMFile(strLmPath,num);
	}

}

void MainWindow::ReadSideConfigFile(int num,std::map<QString,std::vector<QString>>& SideToModels)
{
	LogInfo("Enter ReadSideConfigFile !");
	QString fileName = QString::fromLocal8Bit("/SideConfig")+ QString::fromLocal8Bit(".csv");

	QString strtmp;
	std::string stmp;
	char sJT_FILES_PATH[256] = "";
	if (num==1)
	{
		GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_FILES_PATH, 3, sJT_FILES_PATH);

	}
	else if (num==2)
	{
		GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT2_FILES_PATH, 3, sJT_FILES_PATH);
	}
	GetWholePath(sJT_FILES_PATH);
	stmp = sJT_FILES_PATH;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	QString strFileName = strtmp + fileName;

	QByteArray ba = strFileName.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);

	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			QString type = QString::fromLocal8Bit(Res[0].c_str());

			std::vector<QString> models;
			for(size_t i=1;i<Res.size();i++)//获取组成员
			{
				models.push_back(QString::fromLocal8Bit(Res[i].c_str()));
			}
			SideToModels[type] = models;

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	LogInfo("Exit Read Side ConfigFile !");

	/*std::vector<QString> vJmNames;
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
	}*/

	//sidetomodels.clear();
	//qstring type1 = "jm";
	//qstring type2 = "lm";

	//sidetomodels[type1] = vjmnames;
	//sidetomodels[type2] = vlmnames;


}

void MainWindow::BuildSideConfigFile(int num,const std::map<QString,std::vector<QString>>& SideToModels)
{
	QString fileName = QString::fromLocal8Bit("/SideConfig")+ QString::fromLocal8Bit(".csv");

	QString strtmp;
	std::string stmp;
	char sJT_FILES_PATH[256] = "";
	if (num==1)
	{
		GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_FILES_PATH, 3, sJT_FILES_PATH);

	}
	else if (num==2)
	{
		GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT2_FILES_PATH, 3, sJT_FILES_PATH);
	}
	GetWholePath(sJT_FILES_PATH);
	stmp = sJT_FILES_PATH;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	QString strFileName = strtmp + fileName;

	std::ofstream outFile;
	std::string str = std::string((const char *)strFileName.toLocal8Bit().constData());
	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"Side"<<','<<"Models"<<std::endl;
	//写入数据-字符串
	std::map<QString,std::vector<QString>>::const_iterator it = SideToModels.begin();
	for(;it!=SideToModels.end();it++)
	{	
		std::string Side = it->first.toStdString();
		outFile<< Side <<',';
		outFile<<"\"(";
		std::vector<QString>::const_iterator itr = it->second.begin();
		for (;itr!=it->second.end();itr++)
		{
			std::string name = string((*itr).toLocal8Bit());

			if((itr+1)==it->second.end())
			{
				outFile<<name;
			}
			else
			{
				outFile<<name<<',';
			}
		}
		outFile<<")\""<<std::endl;
	}
	outFile.close();





}

void MainWindow::BuildOneSideFile(QString strFileName,std::vector<QString> vValues,QString strType,int num)
{
	std::vector<STL*> vSTLs;
	vSTLs = GetSideSTLsBySideName(vValues);//边模型


	QString file =  strFileName;
	std::ofstream outFile;
	std::string str = std::string((const char *)file.toLocal8Bit().constData());

	if (strType == "LEFT" )//夹模
	{	
		m_JT1_LJMValues.clear();
		outFile.open(str,std::ios::out | std::ios::trunc);
		//写标题
		outFile<<"Layer"<<','<<"ModelName"<<','<<"Type"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"Length"<<','<<"Wide"<<','<<"AoSide"<<','<<"IsShow"<<','<<"Modifiable"<<std::endl;
		//写入数据-字符串
		std::vector<STL*>::iterator it = vSTLs.begin();
		for(;it!=vSTLs.end();it++)
		{	
			std::string sL = QString::number((*it)->GetCurLayer()).toStdString();
			std::string sName = string((*it)->GetSTLName().toLocal8Bit());     /* (*it)->GetSTLName().toStdString();*/
			std::string sType = string((*it)->GetSTLName().toLocal8Bit());  /*(*it)->GetSTLName().toStdString();*/
			std::string sUp = QString::number((*it)->GetLMupLen()).toStdString();
			std::string sDown = QString::number((*it)->GetLMdownLen()).toStdString();
			std::string sLen = QString::number((*it)->GetJMLen()).toStdString();
			std::string sWide = QString::number((*it)->GetJMWide()).toStdString();
			std::string sAo = QString::number((*it)->GetJMAoSide()).toStdString();
			std::string sShow = QString::number((*it)->GetIsShow()).toStdString();
			std::string sCanChange = QString::number((*it)->GetCanChanged()).toStdString();
			outFile<<sL<<','<<sName<<','<<sType<<','<<sUp<<','<<sDown<<','<<sLen<<','<<sWide<<','<<sAo<<','<<sShow<<','<<sCanChange<<std::endl;

			JM tmp;
			tmp.iLayer = (*it)->GetCurLayer();
			tmp.strModelName = (*it)->GetSTLName();
			tmp.strType = (*it)->GetSTLName();
			tmp.fUpHeight = (*it)->GetLMupLen();
			tmp.fDownHeight = (*it)->GetLMdownLen();
			tmp.fLength = (*it)->GetJMLen();
			tmp.fWide = (*it)->GetJMWide();
			tmp.iSide = (*it)->GetJMAoSide();
			tmp.iShow = (*it)->GetIsShow();
			tmp.iModifiable = (*it)->GetCanChanged();
			m_JT1_LJMValues.push_back(tmp);
		}
		outFile.close();
	}
	else if (strType == "RIGHT")
	{
		m_JT1_RJMValues.clear();
		outFile.open(str,std::ios::out | std::ios::trunc);
		//写标题
		outFile<<"Layer"<<','<<"ModelName"<<','<<"Type"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"Length"<<','<<"Wide"<<','<<"AoSide"<<','<<"IsShow"<<','<<"Modifiable"<<std::endl;
		//写入数据-字符串
		std::vector<STL*>::iterator it = vSTLs.begin();
		for(;it!=vSTLs.end();it++)
		{	
			std::string sL = QString::number((*it)->GetCurLayer()).toStdString();
			std::string sName = string((*it)->GetSTLName().toLocal8Bit());     /* (*it)->GetSTLName().toStdString();*/
			std::string sType = string((*it)->GetSTLName().toLocal8Bit());  /*(*it)->GetSTLName().toStdString();*/
			std::string sUp = QString::number((*it)->GetLMupLen()).toStdString();
			std::string sDown = QString::number((*it)->GetLMdownLen()).toStdString();
			std::string sLen = QString::number((*it)->GetJMLen()).toStdString();
			std::string sWide = QString::number((*it)->GetJMWide()).toStdString();
			std::string sAo = QString::number((*it)->GetJMAoSide()).toStdString();
			std::string sShow = QString::number((*it)->GetIsShow()).toStdString();
			std::string sCanChange = QString::number((*it)->GetCanChanged()).toStdString();
			outFile<<sL<<','<<sName<<','<<sType<<','<<sUp<<','<<sDown<<','<<sLen<<','<<sWide<<','<<sAo<<','<<sShow<<','<<sCanChange<<std::endl;


			JM tmp;
			tmp.iLayer = (*it)->GetCurLayer();
			tmp.strModelName = (*it)->GetSTLName();
			tmp.strType = (*it)->GetSTLName();
			tmp.fUpHeight = (*it)->GetLMupLen();
			tmp.fDownHeight = (*it)->GetLMdownLen();
			tmp.fLength = (*it)->GetJMLen();
			tmp.fWide = (*it)->GetJMWide();
			tmp.iSide = (*it)->GetJMAoSide();
			tmp.iShow = (*it)->GetIsShow();
			tmp.iModifiable = (*it)->GetCanChanged();
			m_JT1_RJMValues.push_back(tmp);


		}
		outFile.close();
	}
	else if(strType == "CENTER")
	{
		m_JT1_CLMValues.clear();
		outFile.open(str,std::ios::out | std::ios::trunc);
		//写标题
		outFile<<"Layer"<<','<<"ModelName"<<','<<"Type"<<','<<"LM_R"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"TuLen"<<','<<"TuSide"<<','<<"IsShow"<<','<<"Modifiable"<<std::endl;
		//写入数据-字符串
		std::vector<STL*>::iterator it = vSTLs.begin();
		for(;it!=vSTLs.end();it++)
		{	
			std::string sL = QString::number((*it)->GetCurLayer()).toStdString();
			std::string sName = string((*it)->GetSTLName().toLocal8Bit()); /*(*it)->GetSTLName().toStdString();*/
			std::string sType = string((*it)->GetSTLName().toLocal8Bit()); /*(*it)->GetSTLName().toStdString();*/
			std::string sR =  QString::number((*it)->GetLMr()).toStdString();
			std::string sUp = QString::number((*it)->GetLMupLen()).toStdString();
			std::string sDown = QString::number((*it)->GetLMdownLen()).toStdString();
			std::string sTuLen = QString::number((*it)->GetLMtuLen()).toStdString();
			std::string sTuSide = QString::number((*it)->GetLMTuSide()).toStdString();
			std::string sShow = QString::number((*it)->GetIsShow()).toStdString();
			std::string sCanChange = QString::number((*it)->GetCanChanged()).toStdString();

			outFile<<sL<<','<<sName<<','<<sType<<','<<sR<<','<<sUp<<','<<sDown<<','<<sTuLen<<','<<sTuSide<<','<<sShow<<','<<sCanChange<<std::endl;


			LMR tmp;
			tmp.iLayer = (*it)->GetCurLayer();
			tmp.strModelName = (*it)->GetSTLName();
			tmp.strType = (*it)->GetSTLName();
			tmp.fLmr =  (*it)->GetLMr();
			tmp.fupLen = (*it)->GetLMupLen();
			tmp.fdownLen = (*it)->GetLMdownLen();
			tmp.ftuLen = (*it)->GetLMtuLen();
			tmp.iSide = (*it)->GetLMTuSide();
			tmp.isShow = (*it)->GetIsShow();
			tmp.iModifiable = (*it)->GetCanChanged();
			m_JT1_CLMValues.push_back(tmp);

		}
		outFile.close();


		/*if (num == 1)
		{
		SetJT1LayerValues(vSTLs);
		}
		else if (num == 2)
		{
		SetJT1LayerValues(vSTLs);
		}*/

	}
}

void MainWindow::SetMapToMapTypeToSTLs(std::map<int,std::map<QString,std::vector<QString>>> values)
{

	m_MapNumToMapTypeToSTLs.clear();
	std::map<int,std::map<QString,std::vector<QString>>>::iterator it = values.begin();
	for (;it!=values.end();it++)
	{
		int iNum = it->first;
		std::map<QString,std::vector<QString>> MapSideToModels = it->second;
		//BuildOneJTFile(iNum,MapSideToModels);
		std::map<QString,std::vector<STL*>> tmp;
		std::map<QString,std::vector<QString>>::iterator _it = MapSideToModels.begin();
		for (;_it!=MapSideToModels.end();_it++)
		{
			std::vector<STL*> vSTLs = GetSideSTLsBySideName(_it->second);
			tmp[_it->first] = vSTLs;
		}

		m_MapNumToMapTypeToSTLs[it->first] = tmp;
	}
}

void MainWindow::BuildNormalUserJMFile(QString strFileName, int num)
{
	if (num == 1)
	{
		m_JT1_JMValues = m_JT1_LJMValues;
		for (size_t i=0;i<m_JT1_RJMValues.size();i++)
		{
			m_JT1_JMValues.push_back(m_JT1_RJMValues[i]);
		}

		std::vector<QString> vJMNames;
		for (size_t j=0;j<m_JT1_JMValues.size();j++)
		{
			vJMNames.push_back(m_JT1_JMValues[j].strModelName);
		}

		std::vector<STL*> vSTLs;
		vSTLs = GetSideSTLsBySideName(vJMNames);//边模型


		std::ofstream outFile;
		std::string str = std::string((const char *)strFileName.toLocal8Bit().constData());


		outFile.open(str,std::ios::out | std::ios::trunc);
		//写标题
		outFile<<"Layer"<<','<<"ModelName"<<','<<"Type"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"Length"<<','<<"Wide"<<','<<"AoSide"<<','<<"IsAid"<<','<<"IsShow"<<','<<"Modifiable"<<std::endl;
		//写入数据-字符串
		std::vector<STL*>::iterator it = vSTLs.begin();
		for(;it!=vSTLs.end();it++)
		{	
			std::string sL = QString::number((*it)->GetCurLayer()).toStdString();
			std::string sName = string((*it)->GetSTLName().toLocal8Bit());     /* (*it)->GetSTLName().toStdString();*/
			std::string sType = string((*it)->GetSTLName().toLocal8Bit());  /*(*it)->GetSTLName().toStdString();*/
			std::string sUp = QString::number((*it)->GetLMupLen()).toStdString();
			std::string sDown = QString::number((*it)->GetLMdownLen()).toStdString();
			std::string sLen = QString::number((*it)->GetJMLen()).toStdString();
			std::string sWide = QString::number((*it)->GetJMWide()).toStdString();
			std::string sAo = QString::number((*it)->GetJMAoSide()).toStdString();
			std::string sAid = QString::number((*it)->GetJMAid()).toStdString();
			std::string sShow = QString::number((*it)->GetIsShow()).toStdString();
			std::string sCanChange = QString::number((*it)->GetCanChanged()).toStdString();
			outFile<<sL<<','<<sName<<','<<sType<<','<<sUp<<','<<sDown<<','<<sLen<<','<<sWide<<','<<sAo<<','<<sAid<<','<<sShow<<','<<sCanChange<<std::endl;
		}

		outFile.close();
	}
}

void MainWindow::BuildNormalUserLMFile(QString strFileName, int num)
{
	if (num == 1)
	{
		std::vector<QString> vLMNames;
		for (size_t j=0;j<m_JT1_CLMValues.size();j++)
		{
			vLMNames.push_back(m_JT1_CLMValues[j].strModelName);
		}

		std::vector<STL*> vSTLs;
		vSTLs = GetSideSTLsBySideName(vLMNames);//边模型


		std::ofstream outFile;
		std::string str = std::string((const char *)strFileName.toLocal8Bit().constData());


		outFile.open(str,std::ios::out | std::ios::trunc);
		//写标题
		outFile<<"Layer"<<','<<"ModelName"<<','<<"Type"<<','<<"LM_R"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"TuLen"<<','<<"TuSide"<<','<<"IsShow"<<','<<"Modifiable"<<std::endl;
		//写入数据-字符串
		std::vector<STL*>::iterator it = vSTLs.begin();
		for(;it!=vSTLs.end();it++)
		{	
			std::string sL = QString::number((*it)->GetCurLayer()).toStdString();
			std::string sName = string((*it)->GetSTLName().toLocal8Bit()); /*(*it)->GetSTLName().toStdString();*/
			std::string sType = string((*it)->GetSTLName().toLocal8Bit()); /*(*it)->GetSTLName().toStdString();*/
			std::string sR =  QString::number((*it)->GetLMr()).toStdString();
			std::string sUp = QString::number((*it)->GetLMupLen()).toStdString();
			std::string sDown = QString::number((*it)->GetLMdownLen()).toStdString();
			std::string sTuLen = QString::number((*it)->GetLMtuLen()).toStdString();
			std::string sTuSide = QString::number((*it)->GetLMTuSide()).toStdString();
			std::string sShow = QString::number((*it)->GetIsShow()).toStdString();
			std::string sCanChange = QString::number((*it)->GetCanChanged()).toStdString();

			outFile<<sL<<','<<sName<<','<<sType<<','<<sR<<','<<sUp<<','<<sDown<<','<<sTuLen<<','<<sTuSide<<','<<sShow<<','<<sCanChange<<std::endl;
		}

		outFile.close();
	}
}

std::vector<pair<QString,QString>> MainWindow::ReadTypeNameMap(QString filepath)
{
	m_MapModelNameToType.clear();
	std::vector<pair<QString,QString>> vTypeName;
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			//if(Res.size()!=9)
			//{
			//	QString strWarning;
			//	if (m_vCurLanguageWords.empty())
			//	{
			//		strWarning = strWarning.fromLocal8Bit("Error with read type picture name map file !");
			//	}
			//	else
			//	{
			//		strWarning = m_vCurLanguageWords[92];//xbc
			//	}

			//	QMessageBox::warning(this,"",strWarning);
			//	//return;

			//}	

			//JM tmp;
			//tmp.iLayer = QString::fromStdString(Res[0]).toInt();
			//tmp.strModelName = QString::fromStdString(Res[1].c_str()); 
			//tmp.strType = QString::fromStdString(Res[2].c_str()); 
			//tmp.fUpHeight =  GetPosValueByString(Res[3]);	
			//tmp.fDownHeight =  GetPosValueByString(Res[4]);	
			//tmp.fLength =  GetPosValueByString(Res[5]);	
			//tmp.fWide =  GetPosValueByString(Res[6]);	
			//tmp.iSide = QString::fromStdString(Res[7]).toInt(); 
			//tmp.iShow = QString::fromStdString(Res[8]).toInt();
			QString model = QString::fromStdString(Res[0].c_str());
			//str_1 = QString(QString::fromLocal8Bit(str_2.data()))
			QString type = QString(QString::fromLocal8Bit(Res[1].data()));
			QString name = QString(QString::fromLocal8Bit(Res[2].data()));

			//tmp.id = m_id++;

			vTypeName.push_back(make_pair(model,name));
			m_MapModelNameToType[model] = type;

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	return  vTypeName;
}

std::vector<JM> MainWindow::ReadJT1LeftFileValues(QString filepath)
{
	std::vector<JM> vJT1LeftFileValus;
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=11)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with JT1Left.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[92];//xbc
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				//return;

			}	

			JM tmp;
			tmp.iLayer = QString::fromStdString(Res[0]).toInt();
			tmp.strModelName = QString::fromStdString(Res[1].c_str()); 
			tmp.strType = QString::fromStdString(Res[2].c_str()); 
			tmp.fUpHeight =  GetPosValueByString(Res[3]);	
			tmp.fDownHeight =  GetPosValueByString(Res[4]);	
			tmp.fLength =  GetPosValueByString(Res[5]);	
			tmp.fWide =  GetPosValueByString(Res[6]);	
			tmp.iSide = QString::fromStdString(Res[7]).toInt();
			tmp.iAid = QString::fromStdString(Res[8]).toInt(); 
			tmp.iShow = QString::fromStdString(Res[9]).toInt();
			tmp.iModifiable = QString::fromStdString(Res[10]).toInt();

			//tmp.id = m_id++;

			vJT1LeftFileValus.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	//按照分类顺序重新排列

	//std::vector<JM> vTmp = vJT1LeftFileValus;
	//std::vector<JM> vNew;

	//std::map<int,std::vector<QString>>::iterator _it = m_MapTovJT1JMSort.begin();
	//for (;_it!=m_MapTovJT1JMSort.end();_it++)
	//{

	//	for (int j=0;j<_it->second.size();j++)
	//	{
	//		std::vector<JM>::iterator _itt = vTmp.begin();
	//		for (;_itt!=vTmp.end();_itt++)
	//		{
	//			if (_it->second[j] == (*_itt).strModelName)
	//			{
	//				vNew.push_back(*_itt);
	//			}
	//		}
	//	}


	//	
	//}



	return  vJT1LeftFileValus;
}

std::vector<JM> MainWindow::ReadJT1RightFileValues(QString filepath)
{
	std::vector<JM> vJT1LeftFileValus;
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=11)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with JT1Left.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[92];//xbc
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				//return;

			}	


			JM tmp;
			tmp.iLayer = QString::fromStdString(Res[0]).toInt(); 
			tmp.strModelName = QString::fromStdString(Res[1].c_str()); 
			tmp.strType = QString::fromStdString(Res[2].c_str());
			tmp.fUpHeight =  GetPosValueByString(Res[3]);	
			tmp.fDownHeight =  GetPosValueByString(Res[4]);	
			tmp.fLength =  GetPosValueByString(Res[5]);	
			tmp.fWide =  GetPosValueByString(Res[6]);	
			tmp.iSide = QString::fromStdString(Res[7]).toInt(); 
			tmp.iShow = QString::fromStdString(Res[8]).toInt();
			tmp.iModifiable = QString::fromStdString(Res[9]).toInt();
			//tmp.id = m_id++;

			vJT1LeftFileValus.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	return  vJT1LeftFileValus;
}

std::vector<LMR> MainWindow::ReadJT1CenterFileValues(QString filepath)
{
	std::vector<LMR> vJT1CenterFileValues;

	//获取当前文件里的所有模型及初始位置
	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=10)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with JT1Center.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[92];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);

				return vJT1CenterFileValues;
			}	

			LMR tmp;
			tmp.iLayer = QString::fromStdString(Res[0]).toInt();
			tmp.strModelName = QString::fromStdString(Res[1].c_str());
			tmp.strType = QString::fromStdString(Res[2].c_str()); 
			tmp.fLmr =  GetPosValueByString(Res[3]);	
			tmp.fupLen =  GetPosValueByString(Res[4]);	
			tmp.fdownLen =  GetPosValueByString(Res[5]);	
			tmp.ftuLen =  GetPosValueByString(Res[6]);	
			tmp.iSide = QString::fromStdString(Res[7]).toInt();
			tmp.isShow = QString::fromStdString(Res[8]).toInt();
			tmp.iModifiable = QString::fromStdString(Res[9]).toInt();

			//tmp.id = m_id++;

			vJT1CenterFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	return vJT1CenterFileValues;
}

void MainWindow::BuildModifyLMTableFile(std::vector<AccumulateModel> vAccumulateNewModelFileValues)
{
	for(size_t i=0;i<vAccumulateNewModelFileValues.size();i++)
	{
		//vAccumulateNewModelFileValues[i]
	}
}

void MainWindow::ReplaceModelByLmTable(LMR lmValue,STL* replaceSTL)
{
	std::vector<LMR> vtmpLMRFileValues;

	QString strLMrFile = need_filePath + QString::fromLocal8Bit("/LMR.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strLMrFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=10)
			{


				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with LMR.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[90];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}	

			LMR tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.fLmr =  GetPosValueByString(Res[1]);	
			tmp.fupLen =  GetPosValueByString(Res[2]);	
			tmp.fdownLen =  GetPosValueByString(Res[3]);	
			tmp.ftuLen =  GetPosValueByString(Res[4]);	
			tmp.iSide = QString::fromStdString(Res[5]).toInt();
			tmp.isShow = QString::fromStdString(Res[6]).toInt();

			vtmpLMRFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();
	//判断文件里是否有这个模型	
	std::vector<LMR>::iterator it = vtmpLMRFileValues.begin();
	for(;it!=vtmpLMRFileValues.end();it++)
	{
		if((*it).strModelName == lmValue.strModelName)
		{
			break;
		}
	}
	if(it!=vtmpLMRFileValues.end())//已存在，再次修改
	{
		((*it).fLmr = lmValue.fLmr);
		((*it).fupLen = lmValue.fupLen);
		((*it).fdownLen = lmValue.fdownLen);
		((*it).ftuLen = lmValue.ftuLen);
		((*it).isShow = lmValue.isShow);
		((*it).iSide = lmValue.iSide);

	}
	else//新增
	{
		LMR newTmp;	
		newTmp.strModelName = lmValue.strModelName;
		newTmp.fLmr = lmValue.fLmr;
		newTmp.fupLen = lmValue.fupLen;
		newTmp.fdownLen = lmValue.fdownLen;
		newTmp.ftuLen = lmValue.ftuLen;
		newTmp.iSide = lmValue.iSide;
		newTmp.isShow = 1;


		vtmpLMRFileValues.push_back(newTmp);

	}

	//重新生成文件
	ReBuildLMRFile(vtmpLMRFileValues);
}

void MainWindow::ReplaceModelByJmTable(JM lmValue,STL* replaceSTL)//当前未用，分左中右表格的时候写的
{
	std::vector<JM> vtmpJMFileValues;

	QString strJMFile = need_filePath + QString::fromLocal8Bit("/JM.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strJMFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=10)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with JM.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[91];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}	

			JM tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.fUpHeight =  GetPosValueByString(Res[1]);	
			tmp.fDownHeight =  GetPosValueByString(Res[2]);	
			tmp.fLength =  GetPosValueByString(Res[3]);	
			tmp.fWide =  GetPosValueByString(Res[4]);	
			tmp.iShow = QString::fromStdString(Res[5]).toInt();

			vtmpJMFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();
	//判断文件里是否有这个模型	
	std::vector<JM>::iterator it = vtmpJMFileValues.begin();
	for(;it!=vtmpJMFileValues.end();it++)
	{
		if((*it).strModelName == lmValue.strModelName)
		{
			break;
		}
	}
	if(it!=vtmpJMFileValues.end())//已存在，再次修改
	{
		((*it).fUpHeight = lmValue.fUpHeight);
		((*it).fDownHeight = lmValue.fDownHeight);
		((*it).fLength = lmValue.fLength);
		((*it).fWide = lmValue.fWide);
		((*it).iShow = lmValue.iShow);
	}
	else//新增
	{
		JM newTmp;	
		newTmp.strModelName = lmValue.strModelName;
		newTmp.fUpHeight = lmValue.fUpHeight;
		newTmp.fDownHeight = lmValue.fDownHeight;
		newTmp.fLength = lmValue.fLength;
		newTmp.fWide = lmValue.fWide;
		newTmp.iShow = 1;

		vtmpJMFileValues.push_back(newTmp);
	}

	//重新生成文件
	ReBuildJMFile(vtmpJMFileValues);
}

void MainWindow::AnalyzeCurRow(std::vector<std::string> res)
{
	if (QString::fromLocal8Bit(res[1].c_str()) == "LOOP_START" ||QString::fromLocal8Bit(res[1].c_str()) == "LOOP_END")
	{

	}
	else
	{
		// 添加间接寻址中的逗号
		for (size_t i = 0; i < res.size();)
		{
			if (isCSVFileName(res[i]))
			{
				res[i] += ',';
				res[i+1] += ',';
				i = i+2;
			}
			else
			{
				i++;
			}
		}
	}

	vector<pair<bool,string>> rawString;
	for (size_t i = 0; i < res.size();++i)
	{
		rawString.push_back(make_pair(true,res[i]));
	}

	vector<pair<int, string> > vFumulas;
	vFumulas = GetFormulaString(rawString);

	vector<pair<int,string>> vFiles;
	vFiles = GetFileNameString(rawString);

	vector<pair<int,string>> vVars;
	vVars = GetVarString(rawString);

	map<int, FileColumn> strs;
	FileColumn column;
	for (size_t i = 0; i < rawString.size();++i)
	{
		if (rawString[i].first)
		{
			column.str = rawString[i].second;
			column.type= Data;
			strs[i] = column;
		}
	}

	for (size_t i = 0; i < vFiles.size();++i)
	{
		column.str= vFiles[i].second;
		column.type = File; 
		strs[vFiles[i].first] = column;
	}

	// 如果是INDEX，后面解析
	for (size_t i = 0; i < vFumulas.size();++i)
	{
		std::string formula1 = vFumulas[i].second;
		std::string formula = formula1;
		int pos1 = formula.find("{");
		int pos2 = formula.find("}");
		int len = pos2-pos1-1;
		if (QString::fromLocal8Bit(res[1].c_str()) == "INDEX")
		{
			// 如果是INDEX，后面处理
			column.str = formula;
		}
		else
		{
			column.str = formula.substr(pos1+1,len);
		}
		column.type = Formula;
		strs[vFumulas[i].first] = column;
	}

	for (size_t i = 0; i < vVars.size();++i)
	{
		column.str= vVars[i].second;
		column.type = Var; 
		strs[vVars[i].first] = column;
	}

	vector<FileColumn> rowData;
	map<int, FileColumn>::iterator iter = strs.begin();
	for (;iter != strs.end();++iter)
	{
		rowData.push_back(iter->second);
	}

	std::map<int,std::vector<std::vector<FileColumn>>> ::iterator it;
	int index = QString::fromLocal8Bit(rowData[0].str.c_str()).toInt();

	it = m_MapStepTovRowString.find(index);
	if(it == m_MapStepTovRowString.end())
	{
		std::vector<std::vector<FileColumn>> vtmp;
		vtmp.push_back(rowData);
		m_MapStepTovRowString[index] =vtmp;
	}
	else
	{
		it->second.push_back(rowData);
	}
}

void MainWindow::GetLoopStartAndEnd()//
{
	//
	m_vLoopStartIndexs.clear();
	//	m_vLoopEndIndexs.clear();
	bool isUp = true;
	bool isDown = false;

	std::map<int,std::vector<OnlyReadProcess>>::iterator iter = m_MapStepTovOnlyReadProcess.begin();
	for (;iter!=m_MapStepTovOnlyReadProcess.end();iter++)
	{
		for (auto it = iter->second.begin(); it != iter->second.end(); ++it) 
		{
			if ((*it).isInLoop)
			{
				isUp = false;
				isDown = true;
			}
			else if((*it).isInLoop==false && isUp==true)
			{
				m_vLoopStartIndexs.push_back(iter->first);
			}
			else if ((*it).isInLoop==false && isDown==true)
			{
				m_vLoopEndIndexs.push_back(iter->first);
			}
		}
	}

	std::vector<int> a = m_vLoopStartIndexs;
	a = m_vLoopEndIndexs;
}

void MainWindow::ResumeAllModelColor()
{
	for (size_t i = 0; i < m_vSTLs.size();++i)
	{
		m_vSTLs[i]->ResumeColor();
	}
}

void MainWindow::SetColor(QString name,osg::Vec4f color)
{
	for (size_t j=0;j<m_vSTLs.size();j++)
	{
		if(name == m_vSTLs[j]->GetSTLName())
		{
			m_vSTLs[j]->SetColor(color);
			break;
		}
	}
}

void MainWindow::ChangeMutiModelColor(std::vector<QString> vmodelname)
{
	ResumeAllModelColor();

	for(size_t i=0;i<vmodelname.size();i++)
	{
		for (size_t j=0;j<m_vSTLs.size();j++)
		{
			if(vmodelname[i] == m_vSTLs[j]->GetSTLName())
			{
				m_vSTLs[j]->SetSelected();
				//break;
			}
		}
	}
}

void MainWindow::InitProductionTable()
{
	QStringList header;
	header<<QString::fromLocal8Bit("弯数")
		<<QString::fromLocal8Bit("Y")
		<<QString::fromLocal8Bit("B")
		<<QString::fromLocal8Bit("C")
		<<QString::fromLocal8Bit("弯管模层");
	m_productionTable->setColumnValue(5, header);

	m_productionTable->setHeaderWidth(0, 40);
	m_productionTable->setHeaderWidth(1, 65);
	m_productionTable->setHeaderWidth(2, 65);
	m_productionTable->setHeaderWidth(3, 65);
	m_productionTable->setHeaderWidth(4, 40);
	m_productionTable->setHeaderHeight(20);

	m_productionTable->setSortingEnabled(false);
	m_productionTable->setSelectionMode(QAbstractItemView::NoSelection);
	//m_productionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_productionTable->setFont(QFont("Microsoft YaHe", 10));

	for(int i=0;i<m_param.imn+1;i++)
	{
		m_productionTable->addRowValue(18,QStringList()<<QString::number(i+1)<<QString::number(m_vproductionvalues[i].fmy)<<QString::number(m_vproductionvalues[i].fmb)<<QString::number(m_vproductionvalues[i].fmc)<<QString::number(m_vproductionvalues[i].fmm));
	}

	for(int j=0;j<m_productionTable->rowCount();j++)
	{
		for(int k=0;k<5;k++)
		{
			//m_productionTable->item(j,k)->setForeground(Qt::black);
		}
	}
}

void MainWindow::InitTwTable()
{
	QStringList header;
	header<<QString::fromLocal8Bit("twy")
		<<QString::fromLocal8Bit("twbj")
		<<QString::fromLocal8Bit("twc")
		<<QString::fromLocal8Bit("twb");
	m_twTable->setColumnValue(4, header);

	m_twTable->setHeaderWidth(0, 55);
	m_twTable->setHeaderWidth(1, 55);
	m_twTable->setHeaderWidth(2, 60);
	m_twTable->setHeaderWidth(3, 60);
	m_twTable->setHeaderHeight(20);

	m_twTable->setSortingEnabled(false);
	m_twTable->setSelectionMode(QAbstractItemView::NoSelection);
	m_twTable->setFont(QFont("Microsoft YaHe", 10));

	SetTwValue();
}

void MainWindow::SetTwValue()
{
	//根据加工文件获取推弯数据
	std::vector<std::vector<TWLC>> allvTovTwlcValues;
	for (size_t i=0;i<m_vproductionvalues.size();i++)
	{

		int twlcNum =  (int)m_vproductionvalues[i].ftwlc;
		if (twlcNum == 0)
		{
			if (m_vproductionvalues[i].ftwy > 0)
			{
				std::vector<TWLC> vTmp;
				TWLC tmp;
				tmp.ftwy = m_vproductionvalues[i].ftwy;
				tmp.ftwbj = m_vproductionvalues[i].ftwbj;
				tmp.ftwc = m_vproductionvalues[i].ftwc;
				tmp.ftwb = m_vproductionvalues[i].ftwb;
				vTmp.push_back(tmp);
				allvTovTwlcValues.push_back(vTmp);
			}
		}
		else if (twlcNum > 0)
		{
			std::map<int,std::vector<struct TWLC>>::iterator it;
			it = m_MapFileTovTWLCValues.find(twlcNum - 1);
			if (it!=m_MapFileTovTWLCValues.end())
			{
				std::vector<struct TWLC> vtmp = it->second;
				allvTovTwlcValues.push_back(vtmp);

			}
		}
	}
	m_allvTWLCValues = allvTovTwlcValues;

	//数据加入表格中
	m_twTable->DeleteAllValueRow();
	//m_twTable->setRowCount(0);
	//m_twTable->clearContents();
	for (size_t i=0;i<m_allvTWLCValues.size();i++)
	{
		for (size_t j=0;j<m_allvTWLCValues[i].size();j++)
		{
			m_twTable->addRowValue(18,QStringList()<<QString::number(m_allvTWLCValues[i][j].ftwy)<<QString::number(m_allvTWLCValues[i][j].ftwbj)<<QString::number(m_allvTWLCValues[i][j].ftwc)<<QString::number(m_allvTWLCValues[i][j].ftwb));		
		}
	}

}

void MainWindow::UpdateModelColor(osg::Vec4 color,STL* curSTL)
{


	//osg::ref_ptr<osg::Geode> geode = dynamic_cast<osg::Geode*>(curSTL->GetSTLNode());
	//if(NULL == geode)
	//	return;

	//int numDrawable = geode->getNumDrawables();
	//for(int i = 0; i < numDrawable; i++)
	//{
	//	osg::Drawable * drawable = geode->getDrawable(i);
	//	if(drawable != NULL)
	//	{
	//		osg::Geometry *geometry = drawable->asGeometry();
	//		if(geometry != NULL)
	//		{
	//			osg::Array* ay = geometry->getColorArray();
	//			osg::Vec4Array * vec4 = dynamic_cast<osg::Vec4Array*>(ay);

	//			//高亮状态下，显示的颜色是高亮颜色
	//			(*vec4)[0].set(color[0], color[1], color[2], color[3]);
	//			geometry->setColorArray(vec4);
	//			geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	//		}
	//	}
	//}




	//osg::Node* node = curSTL->GetSTLNode();

	//osg::ref_ptr<osg::Group> parent = new osg::Group;
	//parent = m_sceneModel->GetNodeScene();
	//osgFX::Outline *ot = dynamic_cast<osgFX::Outline*>(parent.get());
	//if (!ot) //若ot不存在（未高亮） (node->parent)=>(node->outline->parent)
	//{
	//	osg::ref_ptr<osgFX::Outline> outline = new osgFX::Outline();
	//	outline->setColor(osg::Vec4(1, 1, 0, 1));
	//	outline->setWidth(5);
	//	outline->addChild(node);
	//	parent->replaceChild(node, outline);
	//}
	//else
	//{
	//	osg::Node::ParentList parentList = ot->getParents();
	//	osg::Node::ParentList::iterator itr = parentList.begin();
	//	(*itr)->replaceChild(ot, node);
	//}
}

void MainWindow::UpdataTableRow()
{
	std::vector<QString> vstrName;
	for(size_t i=0;i<m_vSTLs.size();i++)
	{
		vstrName.push_back(m_vSTLs[i]->GetSTLName());
	}

	m_ShowTableDlg->UpdateTable(vstrName);
}

void MainWindow::CheTouFun(int i,double STime,double ETime,float frotate_z)
{
	InfoClear(CheTouInfo);
	CheTouInfo.fStartTime = STime;
	CheTouInfo.fEndTime = ETime;
	CheTouInfo.rotateZ = frotate_z;
	CheTouInfo.center3d = m_OriginalCheTouCenter;
	m_vCheTouAnimation.push_back(CheTouInfo);
}

//void MainWindow::ZhuaLiaoMoFun(int i,double STime,double ETime,float frotate_z)
//{
//	InfoClear(ZhuaLiaoMoinfo);
//	ZhuaLiaoMoinfo.fStartTime = STime;
//	ZhuaLiaoMoinfo.fEndTime = ETime;
//	
//
//}

void MainWindow::JiTouMoZuoAnimationFun(int i,double STime,double ETime,float fmove_x)
{
	InfoClear(JiTouMoZuoinfo);
	JiTouMoZuoinfo.fStartTime = STime;
	JiTouMoZuoinfo.fEndTime = ETime;
	JiTouMoZuoinfo.translate.x() = fmove_x;
	m_vJiTouMoZuoAnimation.push_back(JiTouMoZuoinfo);
}

void MainWindow::JiTouWanGuan2AnimationFun(int i,double STime,double ETime,float rotate_y,float dir)
{
	if(rotate_y>=180 || rotate_y<=-180)
	{
		InfoClear(JiTouWanGuan2info);
		JiTouWanGuan2info.fStartTime = STime;
		JiTouWanGuan2info.fEndTime = STime+(ETime - STime)/2 ;
		JiTouWanGuan2info.rotateY = rotate_y/2;
		JiTouWanGuan2info.center3d = m_CurZhuLunCenter;
		m_vJiTouWanGuan2Animation.push_back(JiTouWanGuan2info);

		InfoClear(JiTouWanGuan2info);
		JiTouWanGuan2info.fStartTime = STime+(ETime - STime)/2;
		JiTouWanGuan2info.fEndTime =  ETime;
		JiTouWanGuan2info.rotateY = rotate_y/2;
		JiTouWanGuan2info.center3d = m_CurZhuLunCenter;
		m_vJiTouWanGuan2Animation.push_back(JiTouWanGuan2info);
	}
	else
	{
		InfoClear(JiTouWanGuan2info);
		JiTouWanGuan2info.fStartTime = STime;
		JiTouWanGuan2info.fEndTime = ETime;
		JiTouWanGuan2info.rotateY = rotate_y;
		JiTouWanGuan2info.center3d = m_CurZhuLunCenter;
		m_vJiTouWanGuan2Animation.push_back(JiTouWanGuan2info);
	}

}

void MainWindow::JiTouWanGuan1AnimationFun(int i,double STime,double ETime,float rotate_y)
{
	InfoClear(JiTouWanGuan1info);
	JiTouWanGuan1info.fStartTime = STime;
	JiTouWanGuan1info.fEndTime = ETime;
	JiTouWanGuan1info.rotateY = rotate_y;
	JiTouWanGuan1info.center3d = m_CurZhuLunCenter;
	m_vJiTouWanGuan1Animation.push_back(JiTouWanGuan1info);
}

void MainWindow::JiTouDiZuoAnimationFun(int i,double STime,double ETime,float move_y)
{
	InfoClear(JiTouDiZuoinfo);
	JiTouDiZuoinfo.fStartTime = STime;
	JiTouDiZuoinfo.fEndTime = ETime;
	JiTouDiZuoinfo.translate.y() = move_y;
	m_vJiTouDiZuoAnimation.push_back(JiTouDiZuoinfo);
}

void MainWindow::XiaoCheAnimation(int i,double STime,double ETime,float move_z)
{
	InfoClear(XiaoCheinfo);
	XiaoCheinfo.fStartTime = STime;
	XiaoCheinfo.fEndTime = ETime;
	XiaoCheinfo.translate.z() = move_z;
	m_vXiaoCheAnimation.push_back(XiaoCheinfo);
	m_zlmTozlDis = m_zlmTozlDis - move_z; 
}

void MainWindow::PrePare(AnimationInfo& ZhuaLiaoMoInfo,float fzlm,AnimationInfo& CheTouInfo,float fct,AnimationInfo& JiTouMoZuoInfo,float fjtmz,AnimationInfo& JiTouDiZuoInfo,float fjtdz,float fjtwg1Zore,float fjtwg2Zore)
{
	//抓料模、车头、机头底座、模座、机头弯管初始化
	InfoClear(ZhuaLiaoMoInfo);
	ZhuaLiaoMoInfo.fStartTime = dtime;
	ZhuaLiaoMoInfo.fEndTime = ZhuaLiaoMoInfo.fStartTime + 0.5;
	ZhuaLiaoMoInfo.rotateZ = fzlm;
	osg::Vec3 tmp(-172.050585,766.828229,323.7074500);//支撑圆心
	m_CurZhuaLiaoMoCenter = tmp;
	ZhuaLiaoMoInfo.center3d = m_CurZhuaLiaoMoCenter;
	m_vZhuaLiaoMoAnimation.push_back(ZhuaLiaoMoInfo);
	InfoClear(CheTouInfo);
	CheTouInfo.fStartTime = ZhuaLiaoMoInfo.fStartTime;
	CheTouInfo.fEndTime = ZhuaLiaoMoInfo.fEndTime;
	CheTouInfo.rotateZ = fct;
	CheTouInfo.center3d = m_OriginalCheTouCenter;
	m_vCheTouAnimation.push_back(CheTouInfo);
	InfoClear(JiTouMoZuoInfo);
	JiTouMoZuoInfo.fStartTime = CheTouInfo.fStartTime;
	JiTouMoZuoInfo.fEndTime = CheTouInfo.fEndTime;
	JiTouMoZuoInfo.translate.x() = fjtmz;
	m_vJiTouMoZuoAnimation.push_back(JiTouMoZuoInfo);
	InfoClear(JiTouDiZuoInfo);
	JiTouDiZuoInfo.fStartTime = CheTouInfo.fStartTime;
	JiTouDiZuoInfo.fEndTime = CheTouInfo.fEndTime;
	JiTouDiZuoInfo.translate.y() = fjtdz;
	m_vJiTouDiZuoAnimation.push_back(JiTouDiZuoInfo);
	InfoClear(JiTouWanGuan1info);
	JiTouWanGuan1info.fStartTime = JiTouMoZuoInfo.fStartTime;
	JiTouWanGuan1info.fEndTime = JiTouMoZuoInfo.fEndTime;
	JiTouWanGuan1info.rotateY = fjtwg1Zore;
	JiTouWanGuan1info.center3d = m_CurZhuLunCenter;
	m_vJiTouWanGuan1Animation.push_back(JiTouWanGuan1info);
	InfoClear(JiTouWanGuan2info);
	JiTouWanGuan2info.fStartTime = JiTouMoZuoInfo.fStartTime;
	JiTouWanGuan2info.fEndTime = JiTouMoZuoInfo.fEndTime;
	JiTouWanGuan2info.rotateY = fjtwg2Zore;
	JiTouWanGuan2info.center3d = m_CurZhuLunCenter;
	m_vJiTouWanGuan2Animation.push_back(JiTouWanGuan2info);
}



osg::Vec3 MainWindow::GetCurZhuaLiaoMoCenter(float angle,int dir)
{
	osg::Vec3 center;
	if(dir == 0)
	{
		//需要记录
		//center.x = m_OriginalCheTouCenter.x - fzhichengDis*sin(90-angle);
		//center.y() = m_OriginalCheTouCenter.y - fzhichengDis*

	}
	else if(dir == 1)
	{

	}
	return center;
}

void MainWindow::SetStringLength(std::string& str,size_t len)
{
	if(str.length()<len)
	{
		int k = len - str.length();
		for(int i =0;i<k;i++)
		{
			str = str + ' ';
		}
	}
}

void MainWindow::LoadAllModel()
{
	QString strpath;
	std::string s;
	std::string s2 = "\\";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, STL_PATH, 3, stlpath);//获取文件路径
	GetWholePath(stlpath);
	s = stlpath + s2;
	strpath = QString::fromLocal8Bit(s.c_str());//string转Qstring
	strpath = QDir::fromNativeSeparators(strpath);//"\"转"/"   

	char machineType[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MCAH_TYPE, 3, machineType);// 获取是否纯左侧加工
	if (machineType==QString("0"))
	{
		m_bOnlyLeft = false;
	}
	else
	{
		m_bOnlyLeft = true;
	}

	QDir dir(strpath);
	if(!dir.exists())
	{
		LogInfo(strpath);
		LogInfo("open this path failed !");
		return;
	}

	QStringList filters;
	filters<<"*.stl";//可以多种格式或者单个格式
	dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
	dir.setNameFilters(filters);  //设置文件名称过滤器

	//统计文件个数
	int count = dir.count();
	//if(count <= 0)
	//	return;
	//导入所有模型
	QStringList name_list;
	for(int i = 0; i < count; i++)
	{
		QString file_name= dir[i];  //文件名称
		LoadOneModel(strpath,file_name);

		//m_sceneModel->GetNodeScene()->addChild(m_vSTLs[i]->GetModelMatrix());
	}

	//for(int i=0;i<count;i++)
	//{
	//	m_sceneModel->setData(m_vSTLs[i]->GetSTLNode());
	//	m_sceneModel->GetNodeScene()->addChild(m_vSTLs[i]->GetModelMatrix());
	//	//m_sceneModel->GetNodeScene()->addChild(m_vSTLs[i]->GetModelMatrix());
	//}

	//保存所有模型名
	for(size_t i=0;i<m_vSTLs.size();i++)
	{
		m_vAllNodeName.push_back(m_vSTLs[i]->GetSTLName());
		//if (m_vSTLs[i]->GetSTLName()=="zj3.stl")
		//{
		//	//m_vSTLs[i]->GetSTLNode()->setNodeMask(-1);
		//}
		//else
		//{
		//	m_vSTLs[i]->GetSTLNode()->setNodeMask(0);
		//}
		//m_sceneModel->GetNodeScene()->addChild(m_vSTLs[i]->GetModelMatrix());
	}

	// 为方便调试，隐藏所有模型
	//for (int i = 0; i < m_vSTLs.size();++i)
	//{
	//	m_vSTLs[i]->GetSTLNode()->setNodeMask(0);
	//}

	//坐标轴
	QString fileName = QCoreApplication::applicationDirPath();
	std::string strAxis =  fileName.toStdString() +"/axes.osgt";
	osg::ref_ptr<osg::Node> _axes = osgDB::readNodeFile(strAxis);
	m_HudAxis = new HUDAxis;
	m_HudAxis->addChild(_axes);
	osg::Camera* camera = m_sceneView->getOsgView()->getCamera();
	m_HudAxis->setMainCamera(camera);
	m_HudAxis->setRenderOrder(osg::Camera::POST_RENDER);
	m_HudAxis->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_HudAxis->setAllowEventFocus(false);
	m_HudAxis->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	osg::ref_ptr<osg::Group> scene = m_sceneModel->GetNodeScene();
	scene->addChild(m_HudAxis);

	std::string stmp;
	QString strtmp;
	QString strtmpName;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ANIMATION_FILES_PATH, 3, animation_files_path);
	GetWholePath(animation_files_path);
	stmp = animation_files_path;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmpName = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	need_filePath = strtmpName;

	char num[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LAYER_NUM, 3, num);
	m_LayerNum = atoi(num);
}

void MainWindow::LoadOneModel(QString str,std::string s)
{
	QString strName;
	strName = QString::fromLocal8Bit(s.c_str());
	strName = strName.trimmed();
	strName = str + strName;
	loadFile(strName);
}

void MainWindow::LoadOneModel(QString strPath,QString stlName)
{
	QString strName;
	strName = strPath + stlName;
	loadFile(strName);
}

void MainWindow::ReBuildInitPosFile(std::vector<InitModelCenter> vtmp)
{
	//std::locale loc = std::locale::global(std::locale(""));
	std::ofstream outFile;
	//QString file = need_filePath + QString::fromLocal8Bit("/chushiweizhi.csv");
	QString file = need_filePath + QString::fromLocal8Bit("/InitPosition.csv");

	std::string str = std::string((const char *)file.toLocal8Bit().constData());

	//ifstream fp(str, ios::in);    //打开文件
	//string line;
	//if (fp.fail()) 
	//{
	//	QString strWarning;
	//	strWarning = strWarning.fromLocal8Bit("无法打开初始位置.csv!");
	//	QMessageBox::warning(this,"",strWarning);
	//	return;
	//}
	//while(getline(fp,line)) 
	//{

	//}

	//file.trunc;       //这里划重点， truc函数是将file文件长度设为0，也就达到了清空原文件的效果


	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"ModelName"<<','<<"Layer"<<','<<"Initial Position"<<std::endl;
	//写入数据-字符串
	std::vector<InitModelCenter>::iterator it = vtmp.begin();
	for(;it!=vtmp.end();it++)
	{	

		std::string layer = QString::number((*it).ModelLayer).toStdString();
		std::string sx = QString::number((*it).ModelCenter.x()).toStdString();
		std::string sy = QString::number((*it).ModelCenter.y()).toStdString();
		std::string sz = QString::number((*it).ModelCenter.z()).toStdString();
		std::string sname = string((*it).ModelName.toLocal8Bit());
		outFile<<sname<<','<<layer<<','<<"\"("<<sx<<','
			<<sy<<','
			<<sz<<")\""
			<<std::endl;
	}
	outFile.close();
	//std::locale::global(std::locale(loc ));//注意这里的locale( "", locale::all ^ locale::numeric)！表示表示使用本地语言，这里当然相当于“chs”或者“chinese”。
}

void MainWindow::ReBuildLMRFile(std::vector<LMR> vtmp)
{
	std::ofstream outFile;
	QString file = need_filePath + QString::fromLocal8Bit("/LMR.csv");

	std::string str = std::string((const char *)file.toLocal8Bit().constData());

	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"ModelName"<<','<<"LM_R"<<','<<"UpHeight"<<','<<"DownHeight"<<','<<"TuLen"<<','<<"TuSide"<<','<<"IsShow"<<','<<"JTNth"<<','<<"Layer"<<std::endl;
	//写入数据-字符串
	std::vector<LMR>::iterator it = vtmp.begin();
	for(;it!=vtmp.end();it++)
	{	
		std::string sr = QString::number((*it).fLmr).toStdString();
		std::string sup = QString::number((*it).fupLen).toStdString();
		std::string sdown = QString::number((*it).fdownLen).toStdString();
		std::string stu = QString::number((*it).ftuLen).toStdString();
		std::string sshow =  QString::number((*it).isShow).toStdString();
		std::string sside =  QString::number((*it).iSide).toStdString();
		std::string sJTnth =  QString::number((*it).iJTNth).toStdString();
		std::string sname = string((*it).strModelName.toLocal8Bit());
		std::string sLayer = QString::number((*it).iLayer).toStdString();

		outFile<<sname<<','<<sr<<','<<sup<<','<<sdown<<','<<stu<<','<<sside<<','<<sshow<<','<<sJTnth<<','<<sLayer<<std::endl;
	}
	outFile.close();

	ReBuildLmVector();
}

void MainWindow::ReBuildAccumulateNewModelFile(std::vector<AccumulateModel> vtmp)
{

	std::ofstream outFile;
	QString file = need_filePath + QString::fromLocal8Bit("/AccumulateNewModel.csv");

	std::string str = std::string((const char *)file.toLocal8Bit().constData());

	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"NewModelName"<<','<<"CopyModeName"<<','<<"UpUnits"<<','<<"NthJT"<<std::endl;
	//写入数据-字符串
	std::vector<AccumulateModel>::iterator it = vtmp.begin();
	for(;it!=vtmp.end();it++)
	{	
		std::string sCopyName = string((*it).strCopyModelName.toLocal8Bit()); /*(*it).strCopyModelName.toStdString();*/
		std::string sunits = QString::number((*it).iUpUints).toStdString();
		std::string snth = QString::number((*it).iJTNth).toStdString();

		std::string sname = string((*it).strNewModelName.toLocal8Bit());
		outFile<<sname<<','<<sCopyName<<','<<sunits<<','<<snth<<std::endl;
	}
	outFile.close();

	//ReBuildAccumulateNewModelVector();
	//ReBuildAccumulateNewModelVectorNew();
	LoadAccumulateModelsFromTheFile();
}

void MainWindow::ReBuildAccumulateNewModelVector()
{
	m_vAccumulateNewModelFileValues.clear();
	//char lm_h[64]="";
	//GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HEIGHT, 3, lm_h);
	//m_flm_h = atof(lm_h);


	QString strAccumulateFile = need_filePath + QString::fromLocal8Bit("/AccumulateNewModel.csv");
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = strAccumulateFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);
			/*for(vector<string>::iterator iter=Res.begin();iter!=Res.end();iter++)
			{

			}*/
			if(Res.size()!=4)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("error with AccumulateNewModel.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[93];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;


				/*		QMessageBox::warning(NULL, "warning", "error with AccumulateNewModel.csv", QMessageBox::Yes);
				return;*/
			}	

			AccumulateModel tmp;
			tmp.strNewModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.strCopyModelName = QString::fromStdString(Res[1].c_str()); 
			//float f = GetPosValueByString(Res[2]);	
			tmp.iUpUints = QString::fromStdString(Res[2]).toInt(); 
			tmp.iJTNth = QString::fromStdString(Res[3]).toInt(); 

			m_vAccumulateNewModelFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	//m_vAccumulateFileSTLs
	m_vAccumulateFileSTLs.clear();
	for(size_t i=0;i<m_vAccumulateNewModelFileValues.size();i++)
	{
		size_t j;
		STL* pExistNewSTL;
		for (j=0;j<m_vSTLs.size();j++)
		{
			if(m_vAccumulateNewModelFileValues[i].strNewModelName == m_vSTLs[j]->GetSTLName())
			{
				pExistNewSTL = m_vSTLs[j];
				break;
			}

		}

		if(j<m_vSTLs.size())//newmodel存在,直接上移存在的
		{
			////获取目标模型
			//STL *CopySTL;
			//int k;
			//for (k=0;k<m_vSTLs.size();k++)
			//{
			//	if(m_vAccumulateNewModelFileValues[i].strCopyModelName == m_vSTLs[k]->GetSTLName())
			//		break;
			//}

			//if(k<m_vSTLs.size())//目标存在
			//{
			//	CopySTL = m_vSTLs[k];
			//}
			//else
			//{
			//	QMessageBox::warning(NULL, "warning", QString::fromLocal8Bit("error with AccumulateNewModelFile's %1").arg(m_vAccumulateNewModelFileValues[i].strCopyModelName), QMessageBox::Yes);
			//	return;
			//}

			////生成一个STLNode
			//osg::Vec3 newcenter;
			//newcenter = CopySTL->GetCurModelCenter();
			//float Aunit = CopySTL->GetLMupLen()+CopySTL->GetLMdownLen();
			//newcenter.y()+=Aunit*m_vAccumulateNewModelFileValues[i].iUpUints;

			//osg::ref_ptr<osg::MatrixTransform> tmpMatrix = pExistNewSTL->GetPosMatrix();
			//int needUnit = m_vAccumulateNewModelFileValues[i].iUpUints - pExistNewSTL->GetCurUpUnit();
			//tmpMatrix->setMatrix(osg::Matrix::translate(osg::Vec3(0,Aunit*needUnit,0)));
			//pExistNewSTL->SetCurUpUnit(m_vAccumulateNewModelFileValues[i].iUpUints);
			//pExistNewSTL->SetModelCenter(newcenter);//随模型运动的中心
			//pExistNewSTL->SetModelOriginalCenter(newcenter);//原始中心
			//On_modelColor_change(pExistNewSTL->GetSTLName());

		}
		else//不存在  重新生成，再上移
		{
			//获取目标模型
			STL *CopySTL;
			size_t k;
			for (k=0;k<m_vSTLs.size();k++)
			{
				if(m_vAccumulateNewModelFileValues[i].strCopyModelName == m_vSTLs[k]->GetSTLName())
					break;
			}

			if(k<m_vSTLs.size())//目标存在
			{
				CopySTL = m_vSTLs[k];
			}
			else
			{
				QMessageBox::warning(NULL, "warning", QString::fromLocal8Bit("error with AccumulateNewModelFile's %1").arg(m_vAccumulateNewModelFileValues[i].strCopyModelName), QMessageBox::Yes);
				LogInfo(QString("error with AccumulateNewModelFile's %1").arg(m_vAccumulateNewModelFileValues[i].strCopyModelName));
				return;
			}

			//生成一个STLNode
			float fuplen,fdownlen;

			if(CopySTL->GetIsNew() == true)
			{
				fuplen = CopySTL->GetLMupLen();
				fdownlen = CopySTL->GetLMdownLen();
			}
			else
			{
				char lm_up[64]="";
				char lm_down[64]="";


				GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_UPLEN, 3, lm_up);
				GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_DOWNLEN, 3, lm_down);

				fuplen = atof(lm_up);
				fdownlen = atof(lm_down);

			}
#if 1

			osg::Vec3 newcenter;
			newcenter = CopySTL->GetCurModelCenter();
			float Aunit = fuplen + fdownlen;
			newcenter.y()+=Aunit*m_vAccumulateNewModelFileValues[i].iUpUints;

			osg::ref_ptr<osg::Geode> aGeode;
			osg::ref_ptr<osg::Material> mat;
			CreateSameModel(CopySTL->GetSTLNode(),aGeode,mat);
			STL *newModelSTL = new STL(aGeode.get(),mat,m_vAccumulateNewModelFileValues[i].strNewModelName.toStdString());
			newModelSTL->SetSTLColor(CopySTL->GetSTLColor());
			m_sceneModel->GetNodeScene()->addChild(newModelSTL->GetModelMatrix());

			osg::ref_ptr<osg::MatrixTransform> tmpMatrix = newModelSTL->GetPosMatrix();
			float ft = Aunit*m_vAccumulateNewModelFileValues[i].iUpUints; 
			tmpMatrix->setMatrix(osg::Matrix::translate(osg::Vec3(0,Aunit*m_vAccumulateNewModelFileValues[i].iUpUints,0)));
			newModelSTL->SetModelCenter(newcenter);//随模型运动的中心
			newModelSTL->SetModelOriginalCenter(newcenter);//原始中心
			newModelSTL->SetModelCopyFatherCenter(CopySTL->GetCurModelCenter());//

			newModelSTL->SetCurUpUnit(m_vAccumulateNewModelFileValues[i].iUpUints);
			newModelSTL->SetLMupLen(CopySTL->GetLMupLen());
			newModelSTL->SetLMdownLen(CopySTL->GetLMdownLen());
			newModelSTL->SetLMr(CopySTL->GetLMr());
			newModelSTL->SetCanChanged(1);

			m_vSTLs.push_back(newModelSTL);
			m_vAllNodeName.push_back(newModelSTL->GetSTLName());
			m_vAccumulateFileSTLs.push_back(newModelSTL);

			On_modelColor_change(newModelSTL->GetSTLName());

#endif
			/*loadFile("E:/Osg/osg3DViewer-master/ModelFiles/zl1.stl");
			for(int i=0;i<m_vSTLs.size();i++)
			{
			if(m_vSTLs[i]->GetSTLName() == "zl1.stl")
			{
			osg::ref_ptr<osg::MatrixTransform> tmpMatrix = m_vSTLs[i]->GetPosMatrix();
			tmpMatrix->setMatrix(osg::Matrix::translate(osg::Vec3(0,32*1,0)));
			m_vAllNodeName.push_back(m_vSTLs[i]->GetSTLName());
			m_vAccumulateFileSTLs.push_back(m_vSTLs[i]);
			}
			}*/


		}
	}


	//ReBuildLMModel();
}

void MainWindow::UpdateInitPosFile(QString strModelName,osg::Vec3 center)
{
	//QString strPosFile = need_filePath + QString::fromLocal8Bit("/chushiweizhi.csv");
	QString strPosFile = need_filePath + QString::fromLocal8Bit("/InitPosition.csv");
	//获取当前文件里的所有模型及初始位置
	std::vector<InitModelCenter> vtmpPosFileValues;
	QByteArray ba = strPosFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);
			/*for(vector<string>::iterator iter=Res.begin();iter!=Res.end();iter++)
			{

			}*/
			if(Res.size()!=5)
			{
				QMessageBox::warning(NULL, "warning", "error with Init position File", QMessageBox::Yes);
				return;
			}	

			InitModelCenter tmp;
			tmp.ModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.ModelLayer = QString::fromStdString(Res[1].c_str()).toInt();
			float f_x =  GetPosValueByString(Res[2]);
			float f_y =  GetPosValueByString(Res[3]);
			float f_z =  GetPosValueByString(Res[4]);
			osg::Vec3 vectmp = osg::Vec3(f_x,f_y,f_z);
			tmp.ModelCenter = vectmp;
			vtmpPosFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();
	//判断文件里是否有这个模型	
	std::vector<InitModelCenter>::iterator it = vtmpPosFileValues.begin();
	for(;it!=vtmpPosFileValues.end();it++)
	{
		if((*it).ModelName == strModelName)
		{
			break;
		}
	}
	if(it!=vtmpPosFileValues.end())
	{
		(*it).ModelCenter = center;
		//(*it).ModelCenter += move;
	}
	else//新增
	{
		InitModelCenter newTmp;
		newTmp.ModelCenter = center;
		// 模层默认为第一层
		newTmp.ModelLayer = 0;
		//newTmp.ModelCenter += move;
		newTmp.ModelName = strModelName;
		vtmpPosFileValues.push_back(newTmp);
	}

	//重新生成文件
	ReBuildInitPosFile(vtmpPosFileValues);
}

void MainWindow::UpdateLmrFile(QString ModelName,double dLMr)
{
	QString strLMrFile = need_filePath + QString::fromLocal8Bit("/LMR.csv");
	//获取当前文件里的所有模型及初始位置
	std::vector<LMR> vtmpLMRFileValues;
	QByteArray ba = strLMrFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=10)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with LMR.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[90];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

			}	

			LMR tmp;
			tmp.strModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.fLmr =  GetPosValueByString(Res[1]);	
			tmp.fupLen =  GetPosValueByString(Res[2]);	
			tmp.fdownLen =  GetPosValueByString(Res[3]);	
			tmp.ftuLen =  GetPosValueByString(Res[4]);	
			tmp.iSide = QString::fromStdString(Res[5]).toInt();
			tmp.isShow = QString::fromStdString(Res[6]).toInt();
			tmp.iJTNth = QString::fromStdString(Res[7]).toInt();
			tmp.iLayer = QString::fromStdString(Res[8]).toInt();

			vtmpLMRFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	//判断文件里是否有这个模型	
	std::vector<LMR>::iterator it = vtmpLMRFileValues.begin();
	for(;it!=vtmpLMRFileValues.end();it++)
	{
		if((*it).strModelName == ModelName)
		{
			break;
		}
	}
	if(it!=vtmpLMRFileValues.end())//已存在，再次修改
	{
		((*it).fLmr = (float)dLMr);
		((*it).fupLen = (float)m_LMupLen);
		((*it).fdownLen = (float)m_LMdownLen);
		((*it).ftuLen = (float)m_LMtuLen);
		((*it).iSide = (int)m_LMTuSide);
		((*it).iJTNth = (int)m_LMJTNth);
		((*it).iLayer = (int)m_LMLayer);
	}
	else//新增
	{
		LMR newTmp;	
		newTmp.strModelName = ModelName;
		newTmp.fLmr = (float)dLMr;
		newTmp.fupLen = (float)m_LMupLen;
		newTmp.fdownLen = (float)m_LMdownLen;
		newTmp.ftuLen = (float)m_LMtuLen;
		newTmp.iSide = (int)m_LMTuSide;
		newTmp.isShow = 1;
		newTmp.iJTNth = (int)m_LMJTNth;
		newTmp.iLayer = (int)m_LMLayer;

		vtmpLMRFileValues.push_back(newTmp);
	}

	//重新生成文件
	ReBuildLMRFile(vtmpLMRFileValues);
}

void MainWindow::UpdateAccumulateNewModelFile(QString copyName,QString newName,int upUnits)
{

	/*LoadAccumulateModelsFromTheFile();
	return;*/

	QString strLMrFile = need_filePath + QString::fromLocal8Bit("/AccumulateNewModel.csv");
	//获取当前文件里的所有模型及初始位置
	std::vector<AccumulateModel> vtmpAccumulateFileValues;
	QByteArray ba = strLMrFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);
			/*for(vector<string>::iterator iter=Res.begin();iter!=Res.end();iter++)
			{

			}*/
			if(Res.size()!=4)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("error with AccumulateNewModel.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[93];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

				//QMessageBox::warning(NULL, "warning", "error with AccumulateNewModel.csv", QMessageBox::Yes);
				//return;
			}	

			AccumulateModel tmp;
			tmp.strNewModelName = QString::fromStdString(Res[0].c_str()); 
			tmp.strCopyModelName = QString::fromStdString(Res[1].c_str()); 
			tmp.iUpUints = QString::fromStdString(Res[2]).toInt(); 
			tmp.iJTNth = QString::fromStdString(Res[3]).toInt(); 

			vtmpAccumulateFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}
	fp.close();

	//判断文件里是否有这个模型	
	std::vector<AccumulateModel>::iterator it = vtmpAccumulateFileValues.begin();
	for(;it!=vtmpAccumulateFileValues.end();it++)
	{
		if((*it).strNewModelName == newName)
		{
			break;
		}
	}
	if(it!=vtmpAccumulateFileValues.end())//已存在，再次修改
	{
		((*it).strCopyModelName = copyName);
		((*it).iUpUints = upUnits);
		((*it).iJTNth = m_iJTNthInAccumulate);
	}
	else//新增
	{
		AccumulateModel newTmp;	
		newTmp.strNewModelName = newName;
		newTmp.strCopyModelName = copyName;
		newTmp.iUpUints = upUnits;
		newTmp.iJTNth = m_iJTNthInAccumulate;

		vtmpAccumulateFileValues.push_back(newTmp);
	}
	//重新生成文件
	ReBuildAccumulateNewModelFile(vtmpAccumulateFileValues);
}

void MainWindow::ReadGroupSettingFile(QString filepath)
{
	m_vGcodeFileValues.clear();

	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			// 去掉注释
			if (szBuffer.find("//")!= std::string::npos)
			{
				int pos = szBuffer.find("//");
				szBuffer = szBuffer.substr(0,pos);
			}

			if (szBuffer.empty())
			{
				continue;
			}

			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			if(Res.size()!=12)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("error with Gcode File!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[88];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}

			Gcode tmp;
			tmp.strGroupName = QString::fromStdString(Res[0].c_str()); 
			tmp.strGCode = QString::fromStdString(Res[1].c_str()); 
			tmp.icode = QString::fromStdString(Res[2]).toInt(); 
			int all = QString::fromStdString(Res[3]).toInt();
			tmp.iSpecialMove = all/1000;  //是否是特殊运动，1 机头弯管相对旋转，2 机头弯管绝对旋转，3 相对机头弯管运动
			tmp.iMoveType = all/100%10;//平移，旋转，置管（不是因为运动而生成的管子）
			tmp.iAxis = all/10%10;
			tmp.irelative = all%100%10;
			float f_rotatex =  GetPosValueByString(Res[4]);
			float f_rotatey =  GetPosValueByString(Res[5]);
			float f_rotatez =  GetPosValueByString(Res[6]);
			osg::Vec3 vecRotatetmp = osg::Vec3(f_rotatex,f_rotatey,f_rotatez);
			tmp.vecRotateCenter = vecRotatetmp;
			tmp.iDirectly = QString::fromStdString(Res[7]).toInt(); 
			if(tmp.iDirectly == 1)
			{
				float f_movetoposx =  GetPosValueByString(Res[8]);
				float f_movetoposy =  GetPosValueByString(Res[9]);
				float f_movetoposz =  GetPosValueByString(Res[10]);
				osg::Vec3 vecMoveToPostmp = osg::Vec3(f_movetoposx,f_movetoposy,f_movetoposz);
				tmp.vecMoveToPos = vecMoveToPostmp;
			}
			else if(tmp.iDirectly == 0)//间接
			{
				OtherFileValue tmpother;
				float value;
				tmpother.strFileName = GetStrPosValueByString(Res[8]);
				tmpother.irow = GetStrPosValueByString(Res[9]).toInt();
				tmpother.icolumn = GetStrPosValueByString(Res[10]).toInt();
				if(tmpother.strFileName == "运动位置.csv")
				{
					if(m_vMovePosFileValues.size()>0)
					{
						if(tmpother.irow > 1)
						{
							if(tmpother.icolumn == 1)//第一列
							{
								tmp.vecMoveToPos = m_vMovePosFileValues[tmpother.irow-2].vecMovePos;							
							}
						}
						else
						{
							QString strWarning;
							strWarning = strWarning.fromLocal8Bit("读运动位置文件行位置错误,起始位置为第二行!");//xbc
							QMessageBox::warning(this,"",strWarning);
							LogInfo(strWarning);
							return;
						}
					}
				}
			}

			tmp.iDir = QString::fromStdString(Res[11]).toInt();;
			m_vGcodeFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	m_mapGcodeSpecialMove.clear();
	for (size_t i = 0; i < m_vGcodeFileValues.size();++i)
	{
		Gcode& code = m_vGcodeFileValues[i];
		m_mapGcodeSpecialMove[code.strGCode] = code.iSpecialMove;

		if (code.iSpecialMove==8)
		{
			std::map<QString,GroupChildren>::iterator iter = m_MapGroupToGroupChildren.find(code.strGroupName);
			if (iter!=m_MapGroupToGroupChildren.end())
			{
				// 轮模组第一个模型名字，计算出当前旋转中心
				m_strLunmoName = iter->second.vChildrenNames[0];
			}
		}
	}

	fp.close();
}

void MainWindow::ReadAnimationStepFile(QString filepath,int iCurmn)
{
#if 0


	m_MapStepTovProcessFileValues.erase(m_MapStepTovProcessFileValues.begin(),m_MapStepTovProcessFileValues.end());

	bool btmpLoop = false;
	std::map<int,std::vector<int>> tmpMapSetStepToAimSteps;//SET的時候，unknowToNeedSteps // 当前步号对应目标步号
	std::map<int,std::vector<int>> tmpMapCJUnKnowToNeedSteps;//CJ的時候，unknowToNeedSteps

	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);
			if(Res.size() == 0)
			{
				continue;
			}

			if(Res.size()!=4&&Res.size()!=7&&Res.size()!=9 && Res.size()!=15 &&Res.size()!=11 &&Res.size()!=12&&Res.size()!=13 &&Res.size()!=14)
			{
				QMessageBox::warning(NULL, "warning", "error with Process File", QMessageBox::Yes);
				return;
			}	


			std::map<int,std::vector<Process>> tmpMapStepTovProcess = m_MapStepTovProcessFileValues;
			std::map<int,std::vector<Process>>::iterator it;
			std::map<int,std::vector<Process>>::iterator itt;
			std::map<int,std::vector<int>>::iterator iter;

			std::string tmpCode = Res[1];

			Process tmp;
			tmp.strCode = QString::fromLocal8Bit(Res[1].c_str());
			if(tmp.strCode == "LOOP_START" ||tmp.strCode == "LOOP_END" )//判读循环，此行不存储
			{
				if(tmp.strCode == "LOOP_START")
				{
					btmpLoop = true;
				}
				else
				{
					btmpLoop = false;
				}
			}
			else//G、M代码和CJ、SET
			{

				tmp.bIsLoop = btmpLoop;
				tmp.istep = QString::fromStdString(Res[0]).toInt();
				tmp.strCode = QString::fromLocal8Bit(Res[1].c_str());
				tmp.iDirectly = QString::fromStdString(Res[2]).toInt();

				if( (tmpCode.find_first_of('G') == 0 || tmpCode.find_first_of('M') == 0)/*tmp.strCode != "CJ"*/ && tmp.iDirectly == 1)//直接指定(G、M)
				{

					tmp.fvalue = GetPosValueByString(Res[3]);
					tmp.iPipeRotate = QString::fromStdString(Res[4]).toInt();
					tmp.iCreatPipe = QString::fromStdString(Res[5]).toInt();
					tmp.iCurveOrStraight = QString::fromStdString(Res[6]).toInt();
					if(Res.size()==11)//间接指定 弯管半径
					{
						OtherFileValue ROtherFile;
						ROtherFile.strFileName = QString(QString::fromLocal8Bit(Res[7].c_str()));  
						ROtherFile.irow = QString(QString::fromLocal8Bit(Res[8].c_str())).toInt();  
						ROtherFile.icolumn = QString(QString::fromLocal8Bit(Res[9].c_str())).toInt();  

						if(ROtherFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
						{
							ROtherFile.irow = iCurmn + 2;
							GetValueByProductionFileRowAndColumn(ROtherFile,tmp.fcurveR);		

						}
						else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
						{
							GetValueByTWLC1FileRowAndColumn(ROtherFile,tmp.fcurveR);

						}
						else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
						{
							GetValueByTWLC2FileRowAndColumn(ROtherFile,tmp.fcurveR);

						}
						else if(ROtherFile.strFileName == QString::fromLocal8Bit("参数.csv"))
						{
							GetValueByParamFileRowAndColumn(ROtherFile,tmp.fcurveR);

						}
						else if(ROtherFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
						{
							GetValueByGroupSettingFileRowAndColumn(ROtherFile,tmp.fcurveR);
						}
						else
						{
							QString strWarning;
							strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
							QMessageBox::warning(this,"",strWarning);
							return;
						}

						tmp.iNegative = QString::fromStdString(Res[10]).toInt();
					}
					else if(Res.size()==9)//直接指定弯管半径
					{

						tmp.fcurveR = QString::fromStdString(Res[7]).toFloat();
					}

					//添加
					it = tmpMapStepTovProcess.find(tmp.istep);
					if(it==tmpMapStepTovProcess.end())//不存在
					{
						std::vector<Process> t;
						t.push_back(tmp);
						tmpMapStepTovProcess[tmp.istep] = t;
					}
					else if(it!=tmpMapStepTovProcess.end())//已存在
					{
						it->second.push_back(tmp);
					}

					//判断当前索引是否也在前面的SET同步目标索引steps中
					iter = tmpMapSetStepToAimSteps.find(tmp.istep);
					if(iter != tmpMapSetStepToAimSteps.end())//是的,就再加入到前面需要同步的索引组里
					{
						std::vector<int> vNeedSteps = iter->second;//前面需要当前索引的对应的索引组号
						for(int i=0;i<vNeedSteps.size();i++)
						{
							itt = tmpMapStepTovProcess.find(vNeedSteps[i]);
							if(itt!=tmpMapStepTovProcess.end())//存在
							{
								//判断跳转的是循环内部还是外部

								std::vector<Process> vtempProcess = itt->second;
								int k=0;
								for(;k<vtempProcess.size();k++)
								{
									if(vtempProcess[k].bIsLoop == false)
									{
										break;
									}
								}

								if(k<vtempProcess.size())//不是循环
								{
									tmp.bIsLoop = false;
									itt->second.push_back(tmp);
								}
								else
								{
									tmp.bIsLoop = true;
									itt->second.push_back(tmp);
								}

							}
						}
					}



					//判断当前索引是否也在前面的CJ未知目标索引steps中
					iter = tmpMapCJUnKnowToNeedSteps.find(tmp.istep);
					if(iter != tmpMapCJUnKnowToNeedSteps.end())//是的,就再加入到前面CJ需要的索引组里
					{
						std::vector<int> vNeedSteps = iter->second;//前面需要当前索引的对应的索引组号
						for(int i=0;i<vNeedSteps.size();i++)
						{
							itt = tmpMapStepTovProcess.find(vNeedSteps[i]);
							if(itt!=tmpMapStepTovProcess.end())//存在，获取填入未知的地方指针，并存入当前数据
							{
								//判断跳转的目标是不是循环

								std::vector<Process> vtempProcess = itt->second;
								int k=0;
								for(;k<vtempProcess.size();k++)
								{
									if(vtempProcess[k].bIsLoop == false)
									{
										break;
									}
								}

								if(k<vtempProcess.size())//不是循环
								{
									tmp.bIsLoop = false;
									itt->second.push_back(tmp);
								}
								else
								{
									tmp.bIsLoop = true;
									itt->second.push_back(tmp);
								}

							}
						}
					}


				}
				else // 间接指定和条件跳转
				{
					if( (tmpCode.find_first_of('G') == 0 || tmpCode.find_first_of('M') == 0) /*tmp.strCode != "CJ"*/ && tmp.iDirectly == 0)//间接指定(G/M)
					{
						if(Res.size()!=11 && Res.size()!=13)//弯管半径直接或间接指定
						{
							QString strWarning;
							strWarning = strWarning.fromLocal8Bit("间接指定行的数据格式错误!");
							QMessageBox::warning(this,"",strWarning);
							return;
						}

						//加工值
						tmp._other.strFileName = QString(QString::fromLocal8Bit(Res[3].c_str()));  
						tmp._other.irow = QString::fromStdString(Res[4]).toInt();
						tmp._other.icolumn = QString::fromStdString(Res[5]).toInt();
						if(tmp._other.strFileName == QString::fromLocal8Bit("加工值.csv"))
						{
							tmp._other.irow = iCurmn + 2;
							GetValueByProductionFileRowAndColumn(tmp._other,tmp.fvalue);		
						}
						else if(tmp._other.strFileName == QString::fromLocal8Bit("参数.csv"))
						{
							GetValueByParamFileRowAndColumn(tmp._other,tmp.fvalue);
						}
						else if(tmp._other.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
						{
							GetValueByTWLC1FileRowAndColumn(tmp._other,tmp.fvalue);
						}
						else if(tmp._other.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
						{
							GetValueByTWLC2FileRowAndColumn(tmp._other,tmp.fvalue);
						}
						else if(tmp._other.strFileName == QString::fromLocal8Bit("代码定义.csv"))
						{
							GetValueByGroupSettingFileRowAndColumn(tmp._other,tmp.fvalue);
						}
						else
						{
							QString strWarning;
							strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
							QMessageBox::warning(this,"",strWarning);
							return;
						}

						//弯管值
						tmp.iPipeRotate = QString::fromStdString(Res[6]).toInt();
						tmp.iCreatPipe = QString::fromStdString(Res[7]).toInt();
						tmp.iCurveOrStraight = QString::fromStdString(Res[8]).toInt();
						if(Res.size() == 11)//直接指定半径
						{
							tmp.fcurveR = QString::fromStdString(Res[9]).toFloat();
							tmp.iNegative = QString::fromStdString(Res[10]).toInt();
						}
						else if(Res.size() == 13)//间接指定
						{
							OtherFileValue ROtherFile;
							ROtherFile.strFileName = QString(QString::fromLocal8Bit(Res[9].c_str()));  
							ROtherFile.irow = QString(QString::fromLocal8Bit(Res[10].c_str())).toInt();  
							ROtherFile.icolumn = QString(QString::fromLocal8Bit(Res[11].c_str())).toInt();  

							if(ROtherFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
							{
								ROtherFile.irow = iCurmn + 2;
								GetValueByProductionFileRowAndColumn(ROtherFile,tmp.fcurveR);		

							}
							else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
							{
								GetValueByTWLC1FileRowAndColumn(ROtherFile,tmp.fcurveR);

							}
							else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
							{
								GetValueByTWLC2FileRowAndColumn(ROtherFile,tmp.fcurveR);

							}
							else if(ROtherFile.strFileName == QString::fromLocal8Bit("参数.csv"))
							{
								GetValueByParamFileRowAndColumn(ROtherFile,tmp.fcurveR);

							}
							else if(ROtherFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
							{
								GetValueByGroupSettingFileRowAndColumn(ROtherFile,tmp.fcurveR);
							}
							else
							{
								QString strWarning;
								strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
								QMessageBox::warning(this,"",strWarning);
								return;
							}
							tmp.iNegative = QString::fromStdString(Res[12]).toInt();

						}



						//添加有用的value到map
						it = tmpMapStepTovProcess.find(tmp.istep);
						if(it==tmpMapStepTovProcess.end())//不存在
						{
							std::vector<Process> t;
							t.push_back(tmp);
							tmpMapStepTovProcess[tmp.istep] = t;
						}
						else if(it!=tmpMapStepTovProcess.end())//已存在
						{
							it->second.push_back(tmp);
						}


						//判断当前索引是否也在前面的同步目标索引steps中
						iter = tmpMapSetStepToAimSteps.find(tmp.istep);
						if(iter != tmpMapSetStepToAimSteps.end())//是的,就再加入到前面需要同步的索引组里
						{
							std::vector<int> vNeedSteps = iter->second;//前面需要当前索引的对应的索引组号
							for(int i=0;i<vNeedSteps.size();i++)
							{
								itt = tmpMapStepTovProcess.find(vNeedSteps[i]);
								if(itt!=tmpMapStepTovProcess.end())//存在
								{
									//判断跳转的是循环内部还是外部

									std::vector<Process> vtempProcess = itt->second;
									int k=0;
									for(;k<vtempProcess.size();k++)
									{
										if(vtempProcess[k].bIsLoop == false)
										{
											break;
										}
									}

									if(k<vtempProcess.size())//不是循环
									{
										tmp.bIsLoop = false;
										itt->second.push_back(tmp);
									}
									else
									{
										tmp.bIsLoop = true;
										itt->second.push_back(tmp);
									}
								}
							}
						}

					}
					else if(tmp.strCode == "CJ" && tmp.iDirectly != 0)//(CJ)条件跳转，条件跳转的时候，iDectly是填的是索引，所以不可为0
					{
						bool use = false;//条件跳转判断是否成功

						if(QString(QString::fromLocal8Bit(Res[3].c_str())) == "0")//无条件跳转
						{
							use = true;

							tmp.iPipeRotate = QString::fromStdString(Res[4]).toInt();
							tmp.iCreatPipe = QString::fromStdString(Res[5]).toInt();
							tmp.iCurveOrStraight = QString::fromStdString(Res[6]).toInt();
							if(Res.size() == 9)//直接获取半径
							{
								tmp.fcurveR =  QString::fromStdString(Res[7]).toFloat();
								tmp.iNegative = QString::fromStdString(Res[8]).toInt();
							}
							else if(Res.size() == 11)
							{
								OtherFileValue ROtherFile;
								ROtherFile.strFileName = QString(QString::fromLocal8Bit(Res[7].c_str()));  
								ROtherFile.irow = QString(QString::fromLocal8Bit(Res[8].c_str())).toInt();  
								ROtherFile.icolumn = QString(QString::fromLocal8Bit(Res[9].c_str())).toInt();  

								if(ROtherFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
								{
									ROtherFile.irow = iCurmn + 2;
									GetValueByProductionFileRowAndColumn(ROtherFile,tmp.fcurveR);		

								}
								else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
								{
									GetValueByTWLC1FileRowAndColumn(ROtherFile,tmp.fcurveR);

								}
								else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
								{
									GetValueByTWLC2FileRowAndColumn(ROtherFile,tmp.fcurveR);

								}
								else if(ROtherFile.strFileName == QString::fromLocal8Bit("参数.csv"))
								{
									GetValueByParamFileRowAndColumn(ROtherFile,tmp.fcurveR);

								}
								else if(ROtherFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
								{
									GetValueByGroupSettingFileRowAndColumn(ROtherFile,tmp.fcurveR);
								}
								else
								{
									QString strWarning;
									strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
									QMessageBox::warning(this,"",strWarning);
									return;
								}
								tmp.iNegative = QString::fromStdString(Res[10]).toInt();
							}
						}
						else//条件跳转(等号读euqal)
						{
							if(Res.size()!=15 && Res.size()!=12 && Res.size()!=13 && Res.size()!=14)
							{
								QString strWarning;
								strWarning = strWarning.fromLocal8Bit("跳转行的数据格式错误!");
								QMessageBox::warning(this,"",strWarning);
								return;
							}

							//bool use = false;
							tmp._other.strFileName = QString(QString::fromLocal8Bit(Res[3].c_str()));  
							tmp._other.irow = QString::fromStdString(Res[4]).toInt();
							tmp._other.icolumn = QString::fromStdString(Res[5]).toInt();



							QString _operator;
							float fAim;
							//if(Res.size()==12 || Res.size() == 14)
							//{
							//	_operator = "=";
							//	fAim = QString::fromStdString(Res[6]).toFloat();

							//	tmp.iPipeRotate = QString::fromStdString(Res[7]).toInt();
							//	tmp.iCreatPipe = QString::fromStdString(Res[8]).toInt();
							//	tmp.iCurveOrStraight = QString::fromStdString(Res[9]).toInt();
							//	if(Res.size() == 12)//直接获取半径
							//	{
							//		tmp.fcurveR = QString::fromStdString(Res[10]).toFloat();
							//		tmp.iNegative = QString::fromStdString(Res[11]).toInt();
							//	}
							//	else if(Res.size() == 14)//间接获取半径
							//	{
							//		OtherFileValue ROtherFile;
							//		ROtherFile.strFileName = QString(QString::fromLocal8Bit(Res[10].c_str()));  
							//		ROtherFile.irow = QString(QString::fromLocal8Bit(Res[11].c_str())).toInt();  
							//		ROtherFile.icolumn = QString(QString::fromLocal8Bit(Res[12].c_str())).toInt();  

							//		if(ROtherFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
							//		{
							//			ROtherFile.irow = iCurmn + 2;
							//			GetValueByProductionFileRowAndColumn(ROtherFile,tmp.fcurveR);		

							//		}
							//		else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
							//		{
							//			GetValueByTWLC1FileRowAndColumn(ROtherFile,tmp.fcurveR);

							//		}
							//		else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
							//		{
							//			GetValueByTWLC2FileRowAndColumn(ROtherFile,tmp.fcurveR);

							//		}
							//		else if(ROtherFile.strFileName == QString::fromLocal8Bit("参数.csv"))
							//		{
							//			GetValueByParamFileRowAndColumn(ROtherFile,tmp.fcurveR);

							//		}
							//		else if(ROtherFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
							//		{
							//			GetValueByGroupSettingFileRowAndColumn(ROtherFile,tmp.fcurveR);
							//		}
							//		else
							//		{
							//			QString strWarning;
							//			strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
							//			QMessageBox::warning(this,"",strWarning);
							//			return;
							//		}

							//		tmp.iNegative = QString::fromStdString(Res[13]).toInt();
							//	}

							//}
							//else 
							if(Res.size()==13 || Res.size() == 15)
							{
								_operator = QString(QString::fromLocal8Bit(Res[6].c_str()));
								fAim = QString::fromStdString(Res[7]).toFloat();

								tmp.iPipeRotate = QString::fromStdString(Res[8]).toInt();
								tmp.iCreatPipe = QString::fromStdString(Res[9]).toInt();
								tmp.iCurveOrStraight = QString::fromStdString(Res[10]).toInt();
								if(Res.size() == 13)
								{
									tmp.fcurveR = QString::fromStdString(Res[11]).toFloat();
									tmp.iNegative = QString::fromStdString(Res[12]).toInt();
								}
								else if (Res.size() == 15)
								{
									OtherFileValue ROtherFile;
									ROtherFile.strFileName = QString(QString::fromLocal8Bit(Res[11].c_str()));  
									ROtherFile.irow = QString(QString::fromLocal8Bit(Res[12].c_str())).toInt();  
									ROtherFile.icolumn = QString(QString::fromLocal8Bit(Res[13].c_str())).toInt();  

									if(ROtherFile.strFileName == QString::fromLocal8Bit("加工值.csv"))
									{
										ROtherFile.irow = iCurmn + 2;
										GetValueByProductionFileRowAndColumn(ROtherFile,tmp.fcurveR);		

									}
									else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
									{
										GetValueByTWLC1FileRowAndColumn(ROtherFile,tmp.fcurveR);

									}
									else if(ROtherFile.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
									{
										GetValueByTWLC2FileRowAndColumn(ROtherFile,tmp.fcurveR);

									}
									else if(ROtherFile.strFileName == QString::fromLocal8Bit("参数.csv"))
									{
										GetValueByParamFileRowAndColumn(ROtherFile,tmp.fcurveR);

									}
									else if(ROtherFile.strFileName == QString::fromLocal8Bit("代码定义.csv"))
									{
										GetValueByGroupSettingFileRowAndColumn(ROtherFile,tmp.fcurveR);
									}
									else
									{
										QString strWarning;
										strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
										QMessageBox::warning(this,"",strWarning);
										return;
									}

									tmp.iNegative = QString::fromStdString(Res[14]).toInt();
								}

							}




							//判断跳转是否成功
							if(_operator == "equal")
							{
								if(tmp._other.strFileName == QString::fromLocal8Bit("加工值.csv"))
								{
									tmp._other.irow = iCurmn + 2;

									GetValueByProductionFileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName ==  QString::fromLocal8Bit("参数.csv"))
								{
									GetValueByParamFileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
								{
									GetValueByTWLC1FileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
								{
									GetValueByTWLC2FileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else
								{
									QString strWarning;
									strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
									QMessageBox::warning(this,"",strWarning);
									return;
								}
								//判断是否需要
								if(tmp.fvalue == fAim)
								{
									use = true;
								}
								else
								{
									use = false;
								}
							}
							else if (_operator == ">")
							{

								if(tmp._other.strFileName == QString::fromLocal8Bit("加工值.csv"))
								{
									tmp._other.irow = iCurmn + 2;

									GetValueByProductionFileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName == QString::fromLocal8Bit("参数.csv"))
								{
									GetValueByParamFileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
								{
									GetValueByTWLC1FileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
								{
									GetValueByTWLC2FileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else
								{
									QString strWarning;
									strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
									QMessageBox::warning(this,"",strWarning);
									return;
								}
								//判断是否需要
								if(tmp.fvalue > fAim)
								{
									use = true;
								}
								else
								{
									use = false;
								}
							}
							else if (_operator == "<")
							{
								if(tmp._other.strFileName == QString::fromLocal8Bit("加工值.csv"))
								{
									tmp._other.irow = iCurmn + 2;

									GetValueByProductionFileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName == QString::fromLocal8Bit("参数.csv"))
								{
									GetValueByParamFileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName == QString::fromLocal8Bit("推弯流程1.csv"))
								{
									GetValueByTWLC1FileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else if(tmp._other.strFileName == QString::fromLocal8Bit("推弯流程2.csv"))
								{
									GetValueByTWLC2FileRowAndColumn(tmp._other,tmp.fvalue);
								}
								else
								{
									QString strWarning;
									strWarning = strWarning.fromLocal8Bit("指定文件不存在!");
									QMessageBox::warning(this,"",strWarning);
									return;
								}
								//判断是否需要
								if(tmp.fvalue < fAim)
								{
									use = true;
								}
								else
								{
									use = false;
								}
							}
						}
						if(use == true)//条件跳转为true,
						{
							//跳转索引的数据
							/*if(QString::fromStdString(Res[0]).toInt()==19)
							{
							int a =0;
							}*/
							bool isAimStepinLoop;
							int _step = QString::fromStdString(Res[2]).toInt();//目标索引
							std::map<int,std::vector<OnlyReadProcess>>::iterator iitt = m_MapStepTovOnlyReadProcess.begin();
							for(;iitt!=m_MapStepTovOnlyReadProcess.end();iitt++)
							{
								std::vector<OnlyReadProcess>::iterator iitt_v = iitt->second.begin();
								for(;iitt_v!=iitt->second.end();iitt_v++)
								{
									if(_step == iitt_v->istep)
									{
										isAimStepinLoop = iitt_v->isInLoop;
									}
								}
							}



							std::map<int,std::vector<int>>::iterator its = tmpMapSetStepToAimSteps.begin(); 
							for(;its!=tmpMapSetStepToAimSteps.end();its++)
							{
								std::vector<int>::iterator its2 = its->second.begin();
								for(;its2!=its->second.end();its2++)
								{
									if(_step == (its->first))//目标索引含有当前未读到的数据
									{
										its->second.push_back(QString::fromStdString(Res[0]).toInt());
										break;
									}
								}
							}

							it = tmpMapStepTovProcess.find(_step);
							std::vector<Process> vtmpCjStepProcess;
							if(it != tmpMapStepTovProcess.end())//跳转的索引存在
							{
								vtmpCjStepProcess = it->second;
								if(tmp.bIsLoop == true)
								{
									for(int j=0;j<vtmpCjStepProcess.size();j++)
									{
										vtmpCjStepProcess[j].bIsLoop = true;
									}
								}
								else if(tmp.bIsLoop == false)
								{
									for(int j=0;j<vtmpCjStepProcess.size();j++)
									{
										vtmpCjStepProcess[j].bIsLoop = false;
									}
								}
							}
							else if(it == tmpMapStepTovProcess.end())//跳转到后面，即当前还没有读到的数据
							{
								std::map<int,std::vector<int>>::iterator itr;
								itr = tmpMapCJUnKnowToNeedSteps.find(_step);
								if(itr == tmpMapCJUnKnowToNeedSteps.end())//不存在
								{
									std::vector<int> tt;
									tt.push_back(tmp.istep);
									tmpMapCJUnKnowToNeedSteps[_step] = tt;
								}
								else if(itr != tmpMapCJUnKnowToNeedSteps.end())//存在
								{
									itr->second.push_back(tmp.istep);
								}

								//QString strWarning;
								//strWarning = strWarning.fromLocal8Bit("条件跳转索引错误!");//只能跳转到前面的索引，不能往后面跳
								//QMessageBox::warning(this,"",strWarning);
								//return;
							}

							//添加有用的value到map
							it = tmpMapStepTovProcess.find(tmp.istep);
							if(it==tmpMapStepTovProcess.end())//不存在
							{

								tmpMapStepTovProcess[tmp.istep] = vtmpCjStepProcess;
							}
							else if(it!=tmpMapStepTovProcess.end())//已存在
							{
								for(int i=0;i<vtmpCjStepProcess.size();i++)
								{
									it->second.push_back(vtmpCjStepProcess[i]);

								}
							}
						}
					}
					else if(tmp.strCode == "SET")//设置同步行
					{
						if(Res.size()== 9 && GetPosValueByString(Res[3])==0)//SET行p2需为0
						{
							tmp.iPipeRotate = QString::fromStdString(Res[4]).toInt();
							tmp.iCreatPipe = QString::fromStdString(Res[5]).toInt();
							tmp.iCurveOrStraight = QString::fromStdString(Res[6]).toInt();
							tmp.fcurveR = QString::fromStdString(Res[7]).toFloat();
							tmp.iNegative = QString::fromStdString(Res[8]).toInt();

							it = tmpMapStepTovProcess.find(tmp.iDirectly);//目标索引 

							if(it==tmpMapStepTovProcess.end())//不存在，即跳转的索引在当前索引之后
							{
								//存储目标索引和当前索引的map
								iter = tmpMapSetStepToAimSteps.find(tmp.iDirectly);
								if(iter==tmpMapSetStepToAimSteps.end())//不存在
								{
									std::vector<int> vtmpSetSteps;//set行的索引
									vtmpSetSteps.push_back(tmp.istep);
									tmpMapSetStepToAimSteps[tmp.iDirectly] = vtmpSetSteps;
								}
								else//已存在
								{
									iter->second.push_back(tmp.istep);
								}
							}
							else//目标索引已存在，即跳转的索引在当前索引之前
							{
								std::vector<Process> vProcess;
								vProcess = it->second;
								if(tmp.bIsLoop == true)
								{
									for(int i=0;i<vProcess.size();i++)
									{
										vProcess[i].bIsLoop = true;
									}
								}
								else if(tmp.bIsLoop == false)
								{
									for(int i=0;i<vProcess.size();i++)
									{
										vProcess[i].bIsLoop = false;
									}

								}
								//判断当前的动作索引是否已经存储
								it = tmpMapStepTovProcess.find(tmp.istep);
								if(it==tmpMapStepTovProcess.end())//不存在，新建当前索引
								{
									tmpMapStepTovProcess[tmp.istep] = vProcess;								
								}
								else if(it!=tmpMapStepTovProcess.end())//已存在，目标数据放到当前索引
								{
									for(int i=0;i<vProcess.size();i++)
									{
										it->second.push_back(vProcess[i]);

									}
								}
							}					
						}
						else
						{
							QString strWarning;
							strWarning = strWarning.fromLocal8Bit("SET数据行格式错误!");
							QMessageBox::warning(this,"",strWarning);
							return;
						}
					}
					else
					{
						QString strWarning;
						strWarning = strWarning.fromLocal8Bit("功能码或参数1错误!");//只能跳转到前面的索引，不能往后面跳
						QMessageBox::warning(this,"",strWarning);
						return;
					}
				}

			}


			m_MapStepTovProcessFileValues = tmpMapStepTovProcess;

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}
	std::map<int,std::vector<Process>> a = m_MapStepTovProcessFileValues;
#endif
}

void MainWindow::ReadInitModelCenterFile(QString strPosFile)
{
	m_vInitModelCenterFileValues.clear();

	QByteArray ba = strPosFile.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			if(Res.size()!=5)
			{
				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("Error with InitPosition.csv!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[87];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;

				//QMessageBox::warning(NULL, "warning", "error with InitModelPos File", QMessageBox::Yes);
				//return;
			}	

			InitModelCenter tmp;
			tmp.ModelName = QString::fromStdString(Res[0]); 
			tmp.ModelLayer = QString::fromStdString(Res[1]).toInt(); 
			float f_x =  GetPosValueByString(Res[2]);
			float f_y =  GetPosValueByString(Res[3]);
			float f_z =  GetPosValueByString(Res[4]);
			osg::Vec3 vectmp = osg::Vec3(f_x,f_y,f_z);
			tmp.ModelCenter = vectmp;

			m_vInitModelCenterFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();
}

// 获取模型因轮模半径不同而导致的x方向偏移量
double MainWindow::GetJMShift(QString modelName)
{
	double shift = 0.0;
	std::vector<double> shifts;
	std::map<int,double> mapLayerRadius;
	if (m_JT1_LMFileValues.empty())
	{
		return 0.0;
	}

	for (size_t i = 0; i < m_JT1_LMFileValues.size();++i)
	{
		LMR& lm = m_JT1_LMFileValues[i];
		mapLayerRadius[lm.iLayer] = lm.fLmr;
	}

	std::map<int,double>::iterator iter = mapLayerRadius.begin();
	double min = iter->second;
	for (;iter!= mapLayerRadius.end();++iter)
	{
		if (min>iter->second)
		{
			min = iter->second;
		}
	}

	// 不要统一移动，那层变化就只移动哪层
	//iter = mapLayerRadius.begin();
	//for (;iter!= mapLayerRadius.end();++iter)
	//{
	//	iter->second -= min;
	//}

	for (size_t i = 0; i < m_JT1_JMFileValues.size();++i)
	{
		JM& model = m_JT1_JMFileValues[i];
		if (modelName==model.strModelName)
		{
			if (/*model.iAid==1 &&*/ model.iLayer <= mapLayerRadius.size())
			{
				if (model.iSide==1)
				{
					// 凹槽朝右
					return -1.0*mapLayerRadius.find(model.iLayer)->second;
				}
				else
				{
					return mapLayerRadius.find(model.iLayer)->second;;
				}
			}
			else
			{
				return 0.0;
			}
		}
	}

	return 0.0;
}

void MainWindow::ResetModelPosByInitPosFile()
{
	//QString strPosFile = need_filePath + QString::fromLocal8Bit("/chushiweizhi.csv");
	QString strPosFile = need_filePath + QString::fromLocal8Bit("/InitPosition.csv");
	ReadInitModelCenterFile(strPosFile);

	QString strtmp;
	std::string stmp;
	char sJT1_FILES_PATH[256] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JT1_FILES_PATH, 3, sJT1_FILES_PATH);
	GetWholePath(sJT1_FILES_PATH);
	stmp = sJT1_FILES_PATH;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmp = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	QString JT1Dir;
	JT1Dir = strtmp;

	QString strFileName = JT1Dir + QString::fromLocal8Bit("/JT1_JMValues.csv");
	m_JT1_JMFileValues = ReadJT1LeftFileValues(strFileName);

	if(m_vInitModelCenterFileValues.size() > 0)//文件里位置有数据
	{
		std::vector<InitModelCenter>::iterator iter = m_vInitModelCenterFileValues.begin();
		for(;iter!=m_vInitModelCenterFileValues.end();iter++)
		{
			STL* curSTL = GetCurSTLByName((*iter).ModelName);
			if(curSTL!=NULL)
			{
				//当前位置,累加和修改模型后的位置
				//osg::Vec3 curCenter;
				//curCenter = (curSTL->GetSTLNode())->getBound().center()*osg::computeLocalToWorld((curSTL->GetSTLNode())->getParentalNodePaths()[0]);/*curSTL->GetModelOriginalCenter()*/;
				//计算变换量
				osg::Vec3 center = (*iter).ModelCenter;
				// 根据轮模半径微调模型位置
				double shift = GetJMShift((*iter).ModelName);
				QString name = (*iter).ModelName;
				center[0] += shift;

				// 获取模型当前位置
				osg::Vec3 curPos = (curSTL->GetSTLNode())->getBound().center()*osg::computeLocalToWorld((curSTL->GetSTLNode())->getParentalNodePaths()[0]);
				// 得到模型要移动的向量
				osg::Vec3 moveValue = center - curPos;

				osg::ref_ptr<osg::MatrixTransform> tmpMatrix = curSTL->GetPosMatrix();
				// osg::Matrixd mat0 = tmpMatrix->getMatrix();
				osg::Matrixd mat1 = osg::Matrix::translate(moveValue);
				tmpMatrix->setMatrix(/*mat0**/mat1);

				//Vec3d pos = tmpMatrix->getMatrix().getTrans();

				//更新模型中心
				curSTL->SetModelCenter(moveValue);
			}
		}
	}

	m_JT1_JMFileValues.clear();
}

void MainWindow::GetValueByProductionFileRowAndColumn(OtherFileValue other,float& fvalue)
{
	if(m_vproductionvalues.size()>0)//打开软件就已经读取加工值文件
	{
		/*	if(other.icolumn <= m_param.imn)
		{*/
		if(other.irow>=2)
		{
			if(other.icolumn == 1)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmy;
			}
			else if(other.icolumn == 2)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmyb;
			}
			else if(other.icolumn == 3)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmb;
			}
			else if(other.icolumn == 4)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmbb;
			}
			else if(other.icolumn == 5)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmc;
			}
			else if(other.icolumn == 6)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmcb;
			}
			else if(other.icolumn == 7)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmm;
			}
			else if(other.icolumn == 8)
			{
				fvalue = m_vproductionvalues[other.irow-2].flr;
			}
			else if(other.icolumn == 9)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmyt;
			}
			else if(other.icolumn == 10)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmz;
			}
			else if(other.icolumn == 11)
			{
				fvalue = m_vproductionvalues[other.irow-2].fbtm;
			}
			else if(other.icolumn == 12)
			{
				fvalue = m_vproductionvalues[other.irow-2].fctm;
			}
			else if(other.icolumn == 13)
			{
				fvalue = m_vproductionvalues[other.irow-2].fdz;
			}
			else if(other.icolumn == 14)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmyth;
			}
			else if(other.icolumn == 15)
			{
				fvalue = m_vproductionvalues[other.irow-2].flc;
			}
			else if(other.icolumn == 16)
			{
				fvalue = m_vproductionvalues[other.irow-2].fmmd;
			}
			else if(other.icolumn == 17)
			{
				fvalue = m_vproductionvalues[other.irow-2].fjm;
			}
			else if(other.icolumn == 18)
			{
				fvalue = m_vproductionvalues[other.irow-2].fxb;
			}
			else if(other.icolumn == 19)
			{
				fvalue = m_vproductionvalues[other.irow-2].fjy;
			}
			else if(other.icolumn == 20)
			{
				fvalue = m_vproductionvalues[other.irow-2].ftwy;
			}
			else if(other.icolumn == 21)
			{
				fvalue = m_vproductionvalues[other.irow-2].ftwbj;
			}
			else if(other.icolumn == 22)
			{
				fvalue = m_vproductionvalues[other.irow-2].ftwc;
			}
			else if(other.icolumn == 23)
			{
				fvalue = m_vproductionvalues[other.irow-2].ftwb;
			}
			else if(other.icolumn == 24)
			{
				fvalue = m_vproductionvalues[other.irow-2].ftwlc;
			}
		}
		else
		{

			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("选择加工值文件数据行错误，起始行为2!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[86];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;



			/*QString strWarning;
			strWarning = strWarning.fromLocal8Bit("选择加工值文件数据行错误，起始行为2!");
			QMessageBox::warning(this,"",strWarning);
			return;*/
		}

		/*}
		else
		{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("选择加工值文件数据行错误，起始行为2!");
		QMessageBox::warning(this,"",strWarning);
		return;
		}*/

	}			
}

void MainWindow::GetValueByParamFileRowAndColumn(OtherFileValue other,float& fvalue)
{
	if(m_param.imn != -1)
	{
		if(other.icolumn ==2)
		{

			if(other.irow >=1)
			{
				if(other.irow == 1)
				{
					fvalue = m_param.LmRadius[0];

				}
				else if(other.irow == 2)
				{
					fvalue = m_param.LmRadius[1];

				}
				else if(other.irow == 3)
				{
					fvalue = m_param.LmRadius[2];

				}
				else if(other.irow == 4)
				{
					fvalue = m_param.LmRadius[3];

				}
				else if(other.irow == 5)
				{
					fvalue = m_param.LmRadius[4];

				}
				else if(other.irow == 6)
				{
					fvalue = m_param.fPipeDiameter;

				}
				else if(other.irow == 7)
				{
					fvalue = m_param.fPipeLength;

				}
				else if(other.irow == 8)
				{
					fvalue = (float)m_param.imn;

				}
				else if(other.irow == 9)
				{
					// 文件里读出来的都是正值
					//fvalue = m_param.fYdPos;
					fvalue = m_param.fYdPos;
				}
				else if(other.irow == 10)
				{
					// 文件里读出来的都是正值
					fvalue = m_param.fZgPos;
				}
				else if(other.irow == 11)
				{
					// 文件里读出来的都是正值
					fvalue = m_param.fDywPos;
				}
				else if(other.irow == 12)
				{
					fvalue = m_param.fWqmRadius;

				}
				else if(other.irow == 13)
				{
					fvalue = m_param.fMjDistance;
				}
				else if(other.irow == 14)
				{
					fvalue = m_param.fZxmRadius;

				}
				else if(other.irow == 15)
				{
					fvalue = m_param.fDmaqDistance;
				}
				else if(other.irow == 16)
				{
					// 文件里读出来的都是正值
					fvalue = m_param.fWftwPos;
				}
				else if(other.irow == 17)
				{
					fvalue = m_param.Zmlength[0];
				}
				else if(other.irow == 18)
				{
					fvalue = m_param.Zmlength[1];
				}
				else if(other.irow == 19)
				{
					fvalue = m_param.Zmlength[2];
				}
				else if(other.irow == 20)
				{
					fvalue = m_param.Zmlength[3];
				}
				else if(other.irow == 21)
				{
					fvalue = m_param.Zmlength[4];
				}
				else if(other.irow == 22)
				{
					fvalue = m_param.RDmlength[0];

				}
				else if(other.irow == 23)
				{
					fvalue = m_param.RDmlength[1];
				}
				else if(other.irow == 24)
				{
					fvalue = m_param.RDmlength[2];
				}
				else if(other.irow == 25)
				{
					fvalue = m_param.RDmlength[3];
				}
				else if(other.irow == 26)
				{
					fvalue = m_param.RDmlength[4];
				}
				else if(other.irow == 27)
				{
					fvalue = m_param.LDmlength[0];
				}
				else if(other.irow == 28)
				{
					fvalue = m_param.LDmlength[1];
				}
				else if(other.irow == 29)
				{
					fvalue = m_param.LDmlength[2];
				}
				else if(other.irow == 30)
				{
					fvalue = m_param.LDmlength[3];
				}
				else if(other.irow == 31)
				{
					fvalue = m_param.LDmlength[4];
				}
			}
			else
			{

				QString strWarning;
				if (m_vCurLanguageWords.empty())
				{
					strWarning = strWarning.fromLocal8Bit("选择参数文件数据列错误，起始行为1!");
				}
				else
				{
					strWarning = m_vCurLanguageWords[86];
				}
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;


				/*		QString strWarning;
				strWarning = strWarning.fromLocal8Bit("选择参数文件数据列错误，起始行为1!");
				QMessageBox::warning(this,"",strWarning);
				return;*/
			}
		}
		else
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("选择参数文件数据列错误，列为2!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[86];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;

			//QString strWarning;
			//strWarning = strWarning.fromLocal8Bit("选择参数文件数据列错误，列为2!");
			//QMessageBox::warning(this,"",strWarning);
			//return;
		}

	}
}

void MainWindow::GetValueByTWLC1FileRowAndColumn(OtherFileValue other,float& fvalue)
{
	if(m_vTwlc1values.size()>0)
	{
		if(other.irow >=2)
		{
			if(other.icolumn == 1)
			{
				fvalue = m_vTwlc1values[other.irow-2].ftwy;
			}
			else if(other.icolumn == 2)
			{
				fvalue = m_vTwlc1values[other.irow-2].ftwbj;
			}
			else if(other.icolumn == 3)
			{
				fvalue = m_vTwlc1values[other.irow-2].ftwc;
			}
			else if(other.icolumn == 4)
			{
				fvalue = m_vTwlc1values[other.irow-2].ftwb;
			}
		}
		else
		{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
				strWarning = strWarning.fromLocal8Bit("选择推弯流程1文件数据行错误，起始行为2!");
			}
			else
			{
				strWarning = m_vCurLanguageWords[86];
			}
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;


			//QString strWarning;
			//strWarning = strWarning.fromLocal8Bit("选择推弯流程1文件数据行错误，起始行为2!");
			//QMessageBox::warning(this,"",strWarning);
			//return;
		}
	}
}

void MainWindow::GetValueByTWLC2FileRowAndColumn(OtherFileValue other,float& fvalue)
{
	if(m_vTwlc2values.size()>0)
	{
		if(other.irow >=2)
		{
			if(other.icolumn == 1)
			{
				fvalue = m_vTwlc2values[other.irow-2].ftwy;
			}
			else if(other.icolumn == 2)
			{
				fvalue = m_vTwlc2values[other.irow-2].ftwbj;
			}
			else if(other.icolumn == 3)
			{
				fvalue = m_vTwlc2values[other.irow-2].ftwc;
			}
			else if(other.icolumn == 4)
			{
				fvalue = m_vTwlc2values[other.irow-2].ftwb;
			}
		}
		else
		{
			QString strWarning;
			strWarning = strWarning.fromLocal8Bit("选择推弯流程2文件数据行错误，起始行为2!");
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
	}
}

void MainWindow::GetValueByGroupSettingFileRowAndColumn(OtherFileValue other,float& fvalue)
{
	//std::vector<Gcode> m_vGcodeFileValues;//代码定义文件

	if(m_vGcodeFileValues.size()>0)
	{
		if(other.irow>2)
		{
			if(other.icolumn == 7)
			{
				if(m_vGcodeFileValues[other.irow-2].irelative == 0)//相对
				{
					if(m_vGcodeFileValues[other.irow-2].iAxis == 0)//X轴
					{
						fvalue = m_vGcodeFileValues[other.irow-2].vecMoveToPos.x();
					}
					else if(m_vGcodeFileValues[other.irow-2].iAxis == 1)//Y轴
					{
						fvalue = m_vGcodeFileValues[other.irow-2].vecMoveToPos.y();

					}
					else if(m_vGcodeFileValues[other.irow-2].iAxis == 2)//Z轴
					{
						fvalue = m_vGcodeFileValues[other.irow-2].vecMoveToPos.z();

					}
				}
				else if(m_vGcodeFileValues[other.irow-2].irelative == 1)//绝对，要转成相对
				{

					//先转成相对位置
					//1模型获得当前位置


				}
			}
			else
			{
				QString strWarning;
				strWarning = strWarning.fromLocal8Bit("选择代码定义文件数据列错误，列为7!");
				QMessageBox::warning(this,"",strWarning);
				LogInfo(strWarning);
				return;
			}
		}
		else
		{
			QString strWarning;
			strWarning = strWarning.fromLocal8Bit("选择代码定义文件数据行错误，起始行为2!");
			QMessageBox::warning(this,"",strWarning);
			LogInfo(strWarning);
			return;
		}
	}
}

void MainWindow::GetValueByTWLCNUMFileRowAndColumn(OtherFileValue other,float& fvalue,int twlcIndex)
{
	std::map<int,std::vector<struct TWLC>>::iterator it;
	int num = twlcIndex-1;
	it  = m_MapFileTovTWLCValues.find(num);
	if(it == m_MapFileTovTWLCValues.end())
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("选择推弯流程文件不存在!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}


	if(other.strIndexVarName == "i1")
	{
		other.irow = m_iTwlc1;
	}
	else if(other.strIndexVarName == "i2")
	{
		other.irow = m_iTwlc2;
	}
	else if(other.strIndexVarName == "i3")
	{
		other.irow = m_iTwlc3;
	}
	else if(other.strIndexVarName == "i4")
	{
		other.irow = m_iTwlc4;
	}
	else if(other.strIndexVarName == "i5")
	{
		other.irow = m_iTwlc5;
	}
	else if(other.strIndexVarName == "i6")
	{
		other.irow = m_iTwlc6;
	}
	else if(other.strIndexVarName == "i7")
	{
		other.irow = m_iTwlc7;
	}
	else if(other.strIndexVarName == "i8")
	{
		other.irow = m_iTwlc8;
	}
	else if(other.strIndexVarName == "i9")
	{
		other.irow = m_iTwlc9;
	}
	else if(other.strIndexVarName == "i10")
	{
		other.irow = m_iTwlc10;
	}
	else if(other.strIndexVarName == "i11")
	{
		other.irow = m_iTwlc11;
	}
	else if(other.strIndexVarName == "i12")
	{
		other.irow = m_iTwlc12;
	}
	else if(other.strIndexVarName == "i13")
	{
		other.irow = m_iTwlc13;
	}
	else if(other.strIndexVarName == "i14")
	{
		other.irow = m_iTwlc14;
	}
	else if(other.strIndexVarName == "i15")
	{
		other.irow = m_iTwlc15;
	}
	else if(other.strIndexVarName == "i16")
	{
		other.irow = m_iTwlc16;
	}
	else if(other.strIndexVarName == "i17")
	{
		other.irow = m_iTwlc17;
	}
	else if(other.strIndexVarName == "i18")
	{
		other.irow = m_iTwlc18;
	}
	else if(other.strIndexVarName == "i19")
	{
		other.irow = m_iTwlc19;
	}
	else if(other.strIndexVarName == "i20")
	{
		other.irow = m_iTwlc20;
	}

	if(other.irow >=2 && other.irow < it->second.size()+1)
	{
		if(other.icolumn == 1)
		{
			fvalue = it->second[other.irow-2].ftwy;
		}
		else if(other.icolumn == 2)
		{
			fvalue = it->second[other.irow-2].ftwbj;
		}
		else if(other.icolumn == 3)
		{
			fvalue = it->second[other.irow-2].ftwc;
		}
		else if(other.icolumn == 4)
		{
			fvalue = it->second[other.irow-2].ftwb;
		}
	}
	else
	{
		QString strWarning;
		strWarning = strWarning.fromLocal8Bit("选择推弯流程文件数据行错误，起始行为2!");
		QMessageBox::warning(this,"",strWarning);
		LogInfo(strWarning);
		return;
	}
}

void MainWindow::UpdataSettingNodeGroupDlg()
{
	LogInfo("Enter UpdataSettingNodeGroupDlg !");
	//if(m_MapGroupToGroupChildren.empty() == false)
	//{
	if(m_pSettingNodeGroupDlg!=NULL)
	{
		m_pSettingNodeGroupDlg->SetGroupToChildrenMap(m_MapGroupToGroupChildren);
		m_pSettingNodeGroupDlg->SetJTSideModelsMap(m_MapJTSideModels);
		//SetAllJTLayerToRadiusAtLoad(m_MapJTSideModels);

	}
	//}

	LogInfo("Exit UpdataSettingNodeGroupDlg !");
}

void MainWindow::AddNodeToTree(QString father,QString son)
{
	if (son.right(4)==".stl")
	{
		m_pTree->Insert(father,son);
	}
	else
	{
		// 子节点还有子节点
		m_pTree->Insert(father,son);

		std::map<QString,GroupChildren>::iterator iter = m_MapGroupToGroupChildren.find(son);
		if (iter == m_MapGroupToGroupChildren.end())
		{
			QString info("Find node ");
			info.append(son);
			info.append(" failed !");
			QMessageBox::warning(NULL, "find node failed !", info, QMessageBox::Ok);

			return;
		}
		for (size_t i = 0; i < iter->second.vChildrenNames.size();++i)
		{
			AddNodeToTree(son,iter->second.vChildrenNames[i]);
		}
	}
}

void MainWindow::AddNodeToTree(osg::ref_ptr<osg::MatrixTransform> matrix,QString nodeName)
{
	std::map<QString,GroupChildren>::iterator iter = m_MapGroupToGroupChildren.find(nodeName);
	if (iter!= m_MapGroupToGroupChildren.end())
	{
		STL* node0 = new STL(nodeName.toStdString());
		node0->GetCurAbsolutePos() = iter->second.currentPos;
		m_vNodes.push_back(node0);
		matrix->addChild(node0->GetModelMatrix());

		matrix = node0->GetPosMatrix();
		for (size_t m = 0; m < iter->second.vChildrenNames.size();++m)
		{
			QString name = iter->second.vChildrenNames[m];
			if (name.right(4)==".stl")
			{
				for (size_t n = 0; n < m_vSTLs.size();++n)
				{
					if (m_vSTLs[n]->GetSTLName()==name)
					{
						m_sceneModel->setData(m_vSTLs[n]->GetSTLNode());
						matrix->addChild(m_vSTLs[n]->GetModelMatrix());
					}
				}
			}
			else
			{
				STL* node = new STL(name.toStdString());
				m_vNodes.push_back(node);
				matrix->addChild(node->GetModelMatrix());

				std::map<QString,GroupChildren>::iterator iter0 = m_MapGroupToGroupChildren.find(name);
				iter0->second.node = node;
				node->GetCurAbsolutePos() = iter0->second.currentPos;
				for (size_t i = 0; i < iter0->second.vChildrenNames.size();++i)
				{
					AddNodeToTree(node->GetPosMatrix(),iter0->second.vChildrenNames[i]);
				}
			}
		}
	}
	else if (nodeName.right(4)==".stl")
	{
		for (size_t n = 0; n < m_vSTLs.size();++n)
		{
			if (m_vSTLs[n]->GetSTLName()==nodeName)
			{
				m_sceneModel->setData(m_vSTLs[n]->GetSTLNode());
				matrix->addChild(m_vSTLs[n]->GetModelMatrix());
			}
		}
	}
}

void MainWindow::BuildNodeTree()
{
	if (!m_vNodes.empty())
	{
		for (size_t i = 0; i < m_vNodes.size(); ++i)
		{
			std::map<QString,GroupChildren>::iterator iter1 = m_MapGroupToGroupChildren.find(m_vNodes[i]->GetSTLName());
			if (iter1!= m_MapGroupToGroupChildren.end())
			{
				for (size_t j = 0; j < iter1->second.vChildrenNames.size();++j)
				{
					QString name = iter1->second.vChildrenNames[j];
					if (name.right(4)==".stl")
					{
						for (size_t k = 0; k < m_vSTLs.size();++k)
						{
							if (m_vSTLs[k]->GetSTLName()==name)
							{
								m_vNodes[i]->GetPosMatrix()->removeChild(m_vSTLs[k]->GetModelMatrix());
							}
						}
					}
				}
			}
		}

		m_vNodes.clear();
	}

	if (m_pTree==NULL)
	{
		m_pTree = new CTree("Root");
	}
	else
	{
		delete m_pTree;
		m_pTree = NULL;
		m_pTree = new CTree("Root");
	}

	std::map<QString,GroupChildren>::iterator iter = m_MapGroupToGroupChildren.begin();
	iter = m_MapGroupToGroupChildren.find("Root");
	if (iter == m_MapGroupToGroupChildren.end())
	{
		QString info("Find node Root failed !");
		QMessageBox::warning(NULL, "find node failed !", info, QMessageBox::Ok);
	}
	else
	{
		for (size_t i = 0; i < iter->second.vChildrenNames.size();++i)
		{
			AddNodeToTree("Root",iter->second.vChildrenNames[i]);
		}
	}

	//vector<QString> vNodes;
	//m_pTree->GetSubNode(m_pTree->pRoot,"YouJiaJin",vNodes);

	//m_sceneModel->GetNodeScene()->addChild
	// 创建模型节点树


	iter = m_MapGroupToGroupChildren.find("Root");
	STL* root = new STL("Root");
	m_vNodes.push_back(root);
	m_sceneModel->GetNodeScene()->addChild(root->GetModelMatrix());

	if (iter!= m_MapGroupToGroupChildren.end())
	{
		for (size_t m = 0; m < iter->second.vChildrenNames.size();++m)
		{
			QString name = iter->second.vChildrenNames[m];
			if (name.right(4)==".stl")
			{
				for (size_t n = 0; n < m_vSTLs.size();++n)
				{
					if (m_vSTLs[n]->GetSTLName()==name)
					{
						m_sceneModel->setData(m_vSTLs[n]->GetSTLNode());
						//m_sceneModel->GetNodeScene()->addChild(m_vSTLs[n]->GetModelMatrix());
						root->GetPosMatrix()->addChild(m_vSTLs[n]->GetModelMatrix());
					}
				}
			}
			else
			{
				STL* node = new STL(name.toStdString());
				m_vNodes.push_back(node);
				//m_sceneModel->GetNodeScene()->addChild(node->GetModelMatrix());
				root->GetPosMatrix()->addChild(node->GetModelMatrix());

				std::map<QString,GroupChildren>::iterator iter0 = m_MapGroupToGroupChildren.find(name);
				iter0->second.node = node;
				node->GetCurAbsolutePos() = iter0->second.currentPos;
				for (size_t i = 0; i < iter0->second.vChildrenNames.size();++i)
				{
					AddNodeToTree(node->GetPosMatrix(),iter0->second.vChildrenNames[i]);
				}
			}
		}
	}

#if 0

	iter = m_MapGroupToGroupChildren.begin();
	for (; iter != m_MapGroupToGroupChildren.end();++iter)
	{
		vector<QString> tempModels = iter->second.vChildrenNames;
		iter->second.vChildrenNames.clear();
		iter->second.vNodes.clear();
		for (int j = 0;j < tempModels.size();++j)
		{
			QString model = tempModels[j];
			if (model.right(4)==".stl")
			{
				iter->second.vChildrenNames.push_back(tempModels[j]);
			}
			else
			{
				vector<QString> vNodes;
				m_pTree->GetSubNode(m_pTree->pRoot,model,vNodes);

				iter->second.vNodes.push_back(model);

				for (int k = 0; k < vNodes.size();++k)
				{
					if (vNodes[k].right(4)==".stl")
					{
						iter->second.vChildrenNames.push_back(vNodes[k]);
					}
					else
					{
						iter->second.vNodes.push_back(vNodes[k]);
					}
				}
			}
		}
	}
#endif 
}

void MainWindow::ReadGroupToSTLsFile(QString filepath)
{
	LogInfo(filepath);
	LogInfo("Enter Read GroupToSTLs File !");

	if (!m_MapGroupToGroupChildren.empty())
	{
		return;
	}

	//先清空
	m_MapGroupToGroupChildren.erase(m_MapGroupToGroupChildren.begin(),m_MapGroupToGroupChildren.end());

	std::map<QString,GroupChildren>::iterator iter;
	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);

	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			QString strGN = QString::fromLocal8Bit(Res[0].c_str());
			iter = m_MapGroupToGroupChildren.find(strGN);

			if(iter == m_MapGroupToGroupChildren.end())//不存在
			{
				GroupChildren tmpGC;
				tmpGC.fx = QString::fromLocal8Bit(Res[1].c_str()).toFloat();
				tmpGC.fy = QString::fromLocal8Bit(Res[2].c_str()).toFloat();
				tmpGC.fz = QString::fromLocal8Bit(Res[3].c_str()).toFloat();
				tmpGC.currentPos = osg::Vec3d(tmpGC.fx,tmpGC.fy,tmpGC.fz);

				for(size_t i=4;i<Res.size();i++)//获取组成员
				{
					tmpGC.vChildrenNames.push_back(QString::fromLocal8Bit(Res[i].c_str()));
				}

				m_MapGroupToGroupChildren[strGN] = tmpGC;
			}
			else if (iter == m_MapGroupToGroupChildren.end())//存在
			{			
				iter->second.fx = QString::fromLocal8Bit(Res[1].c_str()).toFloat();
				iter->second.fy = QString::fromLocal8Bit(Res[2].c_str()).toFloat();
				iter->second.fz = QString::fromLocal8Bit(Res[3].c_str()).toFloat();
				iter->second.currentPos = osg::Vec3d(iter->second.fx,iter->second.fy,iter->second.fz);

				for(size_t i=4;i<Res.size();i++)//获取组成员
				{
					iter->second.vChildrenNames.push_back(QString::fromLocal8Bit(Res[i].c_str()));
				}
			}

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}
	else
	{
		LogInfo(filepath);
		LogInfo("Read this file failed !");
	}

	fp.close();

	BuildNodeTree();

	UpdataSettingNodeGroupDlg();

	LogInfo("Exit Read GroupToSTLs File !");
}

void MainWindow::ReadMovePosFile(QString filepath)
{
	m_vMovePosFileValues.clear();

	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",(?=([^\"]*\"[^\"]*\")*[^\"]*$)";
			split(szBuffer,Res,Delema);

			/*if(Res.size()!=3)
			{
			QString strWarning;
			if (m_vCurLanguageWords.empty())
			{
			strWarning = strWarning.fromLocal8Bit("error with InitModelPos File!");
			}
			else
			{
			strWarning = m_vCurLanguageWords[55];
			}
			QMessageBox::warning(this,"",strWarning);
			return;
			}	*/

			MovePos tmp;
			float f_x =  GetPosValueByString(Res[0]);
			float f_y =  GetPosValueByString(Res[1]);
			float f_z =  GetPosValueByString(Res[2]);
			osg::Vec3 vectmp = osg::Vec3(f_x,f_y,f_z);
			tmp.vecMovePos = vectmp;

			m_vMovePosFileValues.push_back(tmp);

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();
}

void MainWindow::ReadFilesAtLoad()
{
	LogInfo("Enter Read FilesAt Load values !");

	//执行读取加工值文件
	QString strtmpName;
	QString strtmp;
	std::string stmp;

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PRODUCTION_VALUES_FILES_PATH, 3, production_values_files_path);
	GetWholePath(production_values_files_path);
	stmp = production_values_files_path;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmpName = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	strtmpName = strtmpName + QString::fromLocal8Bit("/加工值.csv");	
	ReadProductionValues(strtmpName);

	// 读入所有的机头边模型配置
	ReadAllJTSideConfigFile();

	//组和模型
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ANIMATION_FILES_PATH, 3, animation_files_path);
	GetWholePath(animation_files_path);
	stmp = animation_files_path;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmpName = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	need_filePath = strtmpName;
	QString groupSetting = need_filePath + QString::fromLocal8Bit("/GroupToModels.csv");
	ReadGroupToSTLsFile(groupSetting);

	//fengtest 20240302
	osgDragger::ShowBound::Instence()->reset();
	osgVirtualAssembly::CustomDraggerManager::Instence()->reset();
	osgCollisionDetection::CollisionInterface::Instence()->reset(true);

	/*
	StringList nameList;
	for(size_t i=0;i<m_vAllNodeName.size();i++)
	{
		nameList.push_back(m_vAllNodeName[i].toStdString());
	}
	//_pickHandler->setNameList(nameList);

	for(StringList::iterator itr = nameList.begin(); itr != nameList.end(); itr++)
	{
		std::string name = *itr;
		QString name1 = QString::fromStdString(name);
		for(size_t i=0;i<m_vAllNodeName.size();i++)
		{
			if(m_vSTLs[i]->GetSTLName()==name1
				// 添加需要做碰撞检测的模型
				&& m_MapCollisionModel.find(name1)!=m_MapCollisionModel.end())
			{
				osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(name, (MatrixTransform*)m_vSTLs[i]->GetPosMatrix());
				LogInfo(QString("add collision model object %1 ").arg(name1));
			}
		}
	}

	osgCollisionDetection::CollisionInterface::Instence()->setDoCollisionTest(true);
	//fengtest end
	*/

	LogInfo("Exit Read FilesAt Load values !");
}

// 根据碰撞对规划碰撞检测group和masker,要保证碰撞检测设置文件里各列各模型排列顺序一致，数量一致
void MainWindow::PlanCollisionGroupAndMasker(std::map<QString,std::pair<int,int>>& mapGroupMask)
{
	mapGroupMask.clear();

	std::map<QString,int> mapNameGroup;
	std::map<QString,std::vector<std::pair<QString,int>>>::iterator iter = m_MapDisplayNameToPairModelNames.begin();
	for (; iter != m_MapDisplayNameToPairModelNames.end();++iter)
	{
		mapNameGroup[iter->first] = 1;
		std::vector<std::pair<QString,int>>::iterator it = iter->second.begin();
		for (; it != iter->second.end(); ++it)
		{
			mapNameGroup[it->first] = 1;
		}
	}

	std::map<QString,int>::iterator itr = mapNameGroup.begin();
	int index = 0;
	for (; itr != mapNameGroup.end();++itr,++index)
	{
		itr->second = 1<<index;
	}

    iter = m_MapDisplayNameToPairModelNames.begin();
	for (; iter != m_MapDisplayNameToPairModelNames.end();++iter)
	{
		QString key = iter->first;

		int headerMask = 0;
		int headerGroup = mapNameGroup.find(key)->second;
		int indexMask = 0;
		std::vector<std::pair<QString,int>>::iterator it = iter->second.begin();
		for (; it != iter->second.end(); ++it,++indexMask)
		{
			int iGroup = mapNameGroup.find(it->first)->second;

			int iMask = 0;
			if (it->second==1)
			{
				iMask = mapNameGroup.find(key)->second;

				headerMask |= iGroup;
			}
			else
			{
				iMask = 0;
			}

			if (mapGroupMask.find(it->first)!=mapGroupMask.end())
			{
				std::map<QString,std::pair<int,int>>::iterator iter0 = mapGroupMask.find(it->first);
				iter0->second.second |= iMask;
			}
			else
			{
				mapGroupMask[it->first]= std::make_pair(iGroup,iMask);
			}
		}

		mapGroupMask[key] = std::make_pair(headerGroup,headerMask);
	}
}

void MainWindow::AddPipesToCollision()
{
	for (int i = 0; i < m_vPipes.size(); ++i)
	{
		osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
		temp->addChild(m_vPipes[i]->GetPipeNode());
		std::string name = m_vPipes[i]->GetName().toStdString();
		osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(name, temp, m_pairPipeGroupMask.first, m_pairPipeGroupMask.second);

		LogInfo(QString("add collision pipe %1 group %2 mask %3").arg(name.c_str()).arg(m_pairPipeGroupMask.first).arg(m_pairPipeGroupMask.second));
	}

	//osgCollisionDetection::CollisionInterface::Instence()->setDoCollisionTest(true);
}

// 将要做碰撞的模型添加进检测列表
void MainWindow::AddModelsToCollision()
{
	//先删除原来添加的碰撞检测对象
	if (!m_CollisionNameList.empty())
	{
		for (StringList::iterator itr = m_CollisionNameList.begin(); itr != m_CollisionNameList.end(); itr++)
		{
			osgCollisionDetection::CollisionInterface::Instence()->deleteCollisionObject(*itr);
		}
	}
	m_CollisionNameList.clear();

	std::map<QString,std::pair<int,int>> mapGroupMask;
	PlanCollisionGroupAndMasker(mapGroupMask);

	std::map<QString,std::pair<int,int>>::iterator iter = mapGroupMask.begin();
	for (;iter != mapGroupMask.end();++iter)
	{
		if (iter->second.first!=0 && iter->second.second!=0)
		{
			m_CollisionNameList.push_back(iter->first.toStdString());
		}
	}

	m_pairPipeGroupMask = mapGroupMask.find("pipe")->second;

	//AddPipesToCollision();
	for (int i = 0; i < m_vPipes.size(); ++i)
	{
		osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
		temp->addChild(m_vPipes[i]->GetPipeNode());
		std::string name = m_vPipes[i]->GetName().toStdString();
		osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(name, temp, m_pairPipeGroupMask.first, m_pairPipeGroupMask.second);

		LogInfo(QString("add collision pipe %1 group %2 mask %3").arg(name.c_str()).arg(m_pairPipeGroupMask.first).arg(m_pairPipeGroupMask.second));
	}

	for(std::map<QString,std::pair<int,int>>::iterator itr = mapGroupMask.begin(); itr != mapGroupMask.end(); itr++)
	{
		std::string name = itr->first.toStdString();
		QString name1 = QString::fromStdString(name);
		for(size_t i=0;i<m_vAllNodeName.size();i++)
		{
			if(m_vSTLs[i]->GetSTLName()==name1 && itr->second.first!= 0 && itr->second.second!= 0)
			{
				osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(name, (MatrixTransform*)m_vSTLs[i]->GetPosMatrix(),
					itr->second.first,itr->second.second);

				LogInfo(QString("add collision model object %1 group %2 mask %3").arg(name1).arg(itr->second.first).arg(itr->second.second));
			}
		}
	}

	osgCollisionDetection::CollisionInterface::Instence()->setDoCollisionTest(true);
	//fengtest end
}

void MainWindow::ReadAllJTSideConfigFile()
{
	LogInfo("Enter ReadAllJTSideConfigFile !");

	int i = 1;
	std::map<QString,std::vector<QString>> models;
	ReadSideConfigFile(i,models);
	m_MapJTSideModels[i] = models;

	i = 2;
	models.clear();
	ReadSideConfigFile(i,models);
	m_MapJTSideModels[i] = models;

	LogInfo("Exit ReadAllJTSideConfigFile !");
}

std::map<QString,int> MainWindow::ReadModelToLayerFileValues(QString filepath)
{
	std::map<QString,int> ModelToLayer;
	//获取当前文件里的所有模型及初始位置
	QByteArray ba = filepath.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()==2)
			{
				QString str = QString::fromStdString(Res[0].c_str());
				int ilayer = QString::fromStdString(Res[1]).toInt();
				ModelToLayer[str] = ilayer;

			}	

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}
	}

	fp.close();

	return  ModelToLayer;
}

void MainWindow::GetWholePath(char* file)
{
	QString path = QCoreApplication::applicationDirPath();
	path.append(file);

	std::string newpath = path.toStdString();
	const char* wholefilename = newpath.c_str();
	memcpy(file,wholefilename,newpath.size());
}

void MainWindow::ReadAllTWLCFiles()
{
	LogInfo("Enter ReadAllTWLCFiles !");

	QString strpath;
	char twlc[256]="";
	std::string s;
	std::string s2 = "\\";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, TWLC_FILES_PATH, 3, twlc);//获取文件路径
	GetWholePath(twlc);

	s = twlc + s2;
	strpath = QString::fromLocal8Bit(s.c_str());//string转Qstring
	strpath = QDir::fromNativeSeparators(strpath);//"\"转"/"   

	LogInfo(s.c_str());
	QDir dir(strpath);
	if(!dir.exists())
	{
		LogInfo(strpath);
		LogInfo("read this path file failed !");
		return;
	}

	QStringList filters;
	filters<<"*.csv";//可以多种格式或者单个格式
	dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
	dir.setNameFilters(filters);  //设置文件名称过滤器

	//统计文件个数
	int count = dir.count();
	if(count <= 0)
	{
		LogInfo("count is less than zero !");
		return;
	}

	QStringList name_list;
	for(int i = 0; i < count; i++)
	{
		QString file_name= dir[i];  //文件名称
		QString pathName = strpath + file_name;
		ReadOneTWLCFile(pathName);
		m_MapFileTovTWLCValues[i] = m_vTWLCValues;
	}

	//std::map<int,std::vector<struct TWLC>> a = m_MapFileTovTWLCValues;

	LogInfo("Exit ReadAllTWLCFiles !");
}

void MainWindow::HighLightHUDText(int i)
{
	for(size_t j=0;j<m_vOsgText.size();j++)
	{
		if(j == i)
			m_vOsgText[j]->setColor(osg::Vec4(1, 0, 0, 1));
		else
			m_vOsgText[j]->setColor(osg::Vec4(0, 255, 0, 255));


	}
}

bool MainWindow::sma(double a, double b, double tol /*= TOL*/)
{
	if(b - a > tol) return true;
	return false;
}

void MainWindow::updateUi()
{
	// setup app name, release, ...
	updateApplicationIdentity();

	// prepare recent files menu ...
	connect(ui.menuFile, SIGNAL(aboutToShow()), this, SLOT(setupRecentFilesMenu()));
	connect(ui.menuRecent_Files, SIGNAL(triggered(QAction*)), this, SLOT(recentFileActivated(QAction*)));

	// create the log handler
	connect(xLogHandler::getInstance(),SIGNAL(sigNewMessage(const QString &)),this,SLOT(slotPrintToLog(const QString &)));
	connect(xLogHandler::getInstance(),SIGNAL(sigNewMessages(const QStringList &)),this,SLOT(slotPrintToLog(const QStringList &)));
	xLogHandler::getInstance()->startEmission(true); // start log emission

	// limit log display
	ui.textBrowserLog->document()->setMaximumBlockCount(1000);

	// create tree model
	m_treeModel = new xTreeModel(this);
	m_treeView = new xTreeView(ui.dockWidget_Model);
	m_treeView->setModel(m_treeModel);
	m_sceneView->installEventFilter(this);
	connect(m_treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotTreeNodeSelected(const QModelIndex &)), Qt::UniqueConnection);
	connect(m_treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotTreeViewCustomContextMenuRequested(const QPoint &)), Qt::UniqueConnection);
	//QVBoxLayout *pTreeWidgetLayout = new QVBoxLayout(this);

	//属性界面不显示
	QWidget *pStructureWidget = new QWidget(ui.dockWidget_Model);
	//ui.dockWidget_Model->setWidget(pStructureWidget);
	QVBoxLayout *pTreeLayout = new QVBoxLayout(pStructureWidget);
	pStructureWidget->setLayout(pTreeLayout);
	pStructureWidget->hide();

	m_lineEditSearch = new xSearchLineEdit(ui.dockWidget_Model);
	m_lineEditSearch->setObjectName(QString::fromUtf8("lineEditSearch"));
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(m_lineEditSearch->sizePolicy().hasHeightForWidth());
	m_lineEditSearch->setSizePolicy(sizePolicy);
	connect(m_lineEditSearch, SIGNAL(sigCaseSensitiveToggled(bool)), this, SLOT(slotSearchCaseSensitiveToggled(bool)), Qt::UniqueConnection);
	connect(m_lineEditSearch, SIGNAL(returnPressed()), this, SLOT(slotEditSearchReturnPressed()), Qt::UniqueConnection);

	pTreeLayout->addWidget(m_lineEditSearch);
	pTreeLayout->addWidget(m_treeView);

	enableActions(false);

	// create the scene manager (scene model)
	m_sceneModel = new xSceneModel(this);//
	float _z;	
	char _ch_z[32];
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_1, 3, _ch_z);
	_z = atof(_ch_z);

	ReadHeaderPara();

	m_sceneModel->setMyClipPlane(m_vPlaneCenters,m_vPlaneNormals,_z);
	m_pipe_z = _z;
	//m_setNodeDlg = new xSettingNodeDlg(this);
	m_pSettingAnimationDlg = new xSettingAnimation(this);//动画配置界面
	NodeDlg = new xSettingNodeDlg(this);//模型链接界面
	m_pCollisionDlg = new xCollisionDetectionDlg(this);
	m_ShowTableDlg = new ShowTableDlg;
	m_pSettingNodeGroupDlg = new xSettingNodeGroupDlg(this);
	m_pSlightAdjustDlg = new xSlightAdjust(this);
	m_pReplaceWheelModelDlg = new xReplaceWheelModel(this);
	m_pRotateTestDlg = new xRotateTest(this);
	m_pAddModelDlg = new xAddModel(this);
	m_pLanguageDlg = new xLanguageDlg();
	m_pSpeedDlg = new xSpeedDlg();

	m_pOptimizeDlg = new xOptimizeDlg();

	m_pModifyLMTable = new xModifyLMTable(this);
	m_pNormalUserWidget = new xNormalUserWidget(this);
	m_pAdvanceUserDlg = new xAdvanceUserDlg(this);
	m_pSettingColor = new xSettingColor(this);
	m_sceneView->setModel(m_sceneModel);
	m_treeModel->index(0, 0, m_treeView->rootIndex());

	connect(m_sceneModel, SIGNAL(sigLoadFinished()), m_sceneView, SLOT(slotUpdateModel()), Qt::UniqueConnection);

	//connect(ui->widgetSceneView,SIGNAL(newScreenshotAvailable(osg::Image *)),this,SLOT(takeIntoAccountScreenshot(osg::Image*)),Qt::QueuedConnection);
	connect(m_sceneView, SIGNAL(sigPicked(osg::Drawable*)), this, SLOT(slotSelectTreeItem(osg::Drawable*)));
	connect(m_sceneView, SIGNAL(sigNewAspectRatio(const QSize &)), this, SLOT(slotChangeAspectRatio(const QSize &)));

	// add a label to display the current aspect ratio of the  display (for bookmark)
	m_aspectRatioLabel = new QLabel(this);
	QMainWindow::statusBar()->addPermanentWidget(m_aspectRatioLabel);

	// add a label to display the LOD factor
	m_LODFactorLabel = new QLabel(this);
	m_LODFactorLabel->setText("LOD x1"); // set default value
	QMainWindow::statusBar()->addPermanentWidget(m_LODFactorLabel);

	// create a background loader
	m_objectLoader = new xObjectLoader();
	m_objectLoader->moveToThread(xThreadPool::getInstance()->getThread());
	connect(this,SIGNAL(sigNewFileToLoad(const QString &)),m_objectLoader,SLOT(slotNewObjectToLoad(const QString &)));
	connect(m_objectLoader,SIGNAL(sigNewObjectToView(osg::ref_ptr<osg::Node> node,
		osg::ref_ptr<osg::Material> material, const QString& name)),this,
		SLOT(slotNewLoadedFile(osg::ref_ptr<osg::Node> node,osg::ref_ptr<osg::Material> material, const QString& name)));
	connect(NodeDlg,SIGNAL(sigAddChildToFather(QString,QString)),this,SLOT(AddChildToFatherMatrix(QString,QString)));
	connect(NodeDlg,SIGNAL(sigDelChildToFather(QString,QString)),this,SLOT(DelChildToFatherMatrix(QString,QString)));
	connect(m_pSettingAnimationDlg,SIGNAL(sigSettingAnimationOK()),this,SLOT(SlotSetAnimation()));
	connect(m_ShowTableDlg,SIGNAL(SigCloseThisWindow()),this,SLOT(SlotChangeTableBtnState()));
	connect(m_pSettingNodeGroupDlg,SIGNAL(SigSettingGroupFinished()),this,SLOT(SlotSetGroupToChildren()));
	connect(m_pSettingNodeGroupDlg,SIGNAL(SigSettingJTSideFinished()),this,SLOT(SlotSetJTSideModels()));

	connect(m_pSlightAdjustDlg,SIGNAL(SigSettingSlightAdjustFinished()),this,SLOT(SlotSlightAdjust()));
	connect(m_pReplaceWheelModelDlg,SIGNAL(SigSettingLMrFinished()),this,SLOT(SlotReplaceWheelModel()));//轮模
	connect(m_pReplaceWheelModelDlg,SIGNAL(SigSettingJMFinished()),this,SLOT(SlotReplaceJMModel()));//夹模

	connect(m_pAddModelDlg,SIGNAL(SigSettingAddModelFinished()),this,SLOT(SlotAddModels()));
	//connect(m_pModifyLMTable,SIGNAL(SigLMTableCurValues()),this,SLOT(SlotReplaceByModifyLMTableModel()));
	//connect(m_pModifyLMTable,SIGNAL(SigLMTableModelIsShow()),this,SLOT(SlotShowModifyLMTableModel()));
	connect(m_pModifyLMTable,SIGNAL(SigSetLMTableFinished()),this,SLOT(SlotModifyLMTableFinished()));
	connect(m_pCollisionDlg,SIGNAL(SigSetDetectedFinished()),this,SLOT(SlotSetDetectedFinished()));


	//connect(m_pMoveRangeDlg,SIGNAL(SigSettingMoveRangeFinished()),this,SLOT(SlotSetMoveRange()));
	//connect(m_pMoveRangeDlg,SIGNAL(SigSettingMoveRangeModelHighLight()),this,SLOT(SlotSetMoveModelHighLight()));
	connect(m_pRotateTestDlg,SIGNAL(SigSettingRotateTestFinished()),this,SLOT(SlotPlayRotateTest()));
	connect(m_pLanguageDlg,SIGNAL(SigSettingLanguageFinished()),this,SLOT(SlotSetLanguage()));
	connect(m_pAdvanceUserDlg,SIGNAL(SigInputAdvancePasswordFinished()),this,SLOT(SlotSignInAdvanceUser()));

	//// bookmarks
	//connect(ui->widgetBookmarkManager,SIGNAL(newBookmarkRequest()),this,SLOT(on_actionNewBookmark_triggered()));
	//connect(ui->widgetBookmarkManager,SIGNAL(useBookmarkItem(BookmarkItem *)),this,SLOT(useBookmarkItem(BookmarkItem *)));
	//connect(ui->widgetBookmarkManager,SIGNAL(updateBookmarkItemRequest(BookmarkItem *)),this,SLOT(updateBookmarkItem(BookmarkItem *)));

	//connect(m_setNodeDlg, SIGNAL(sigSetiingNodeFinished(osg::Node *,const QString& )), this, SLOT(slotSettingNodeFinished(osg::Node *,const QString&))/*, Qt::UniqueConnection*/);
}

void MainWindow::on_actionQuit_triggered()
{
	if(m_pNormalUserWidget != NULL)
	{
		//m_pNormalUserWidget->InitTab1(m_vLMRFileValues);
		//m_pNormalUserWidget->SetNormalUserWidgetLanguage(m_vCurLanguageWords);
		if (m_pNormalUserWidget->isMinimized())
		{
			m_pNormalUserWidget->showNormal();

		}
		else
		{
			m_pNormalUserWidget->InitTable();
			m_pNormalUserWidget->show();
		}
	}

	// close();


	/*double value;

	char position[20];

	std::ifstream fp;

	QString fileName = QFileDialog::getOpenFileName(this, "Select one file to build", "", "CSV files (*.*)");
	if(!fileName.isEmpty())
	{
	QByteArray cdata1 = fileName.toLocal8Bit();	
	std::string stdstrFileName1 = std::string(cdata1);
	ReadProductionValues(QString::fromLocal8Bit(stdstrFileName1.data()));
	}
	ui.actionUnload->setEnabled(true);*/

	//QFile file(fileName);  
	//if (!file.open(QIODevice::ReadOnly | QIODevice::Text))  
	//	return;  

	//QTextStream in(&file);  
	//QString line = in.readLine();  
	//while (!line.isNull()) 
	//{  
	//	BuidPipe(line);  
	//	line = in.readLine();  
	//}  


	//fp.open(file,std::ios::in); //ios::in表示读流的方式，表示打开模式。
	//if(!fp.is_open())
	//{
	////	std::cout<<"打开文件失败！！\n";
	//	return ; 
	//}
	//while(!fp.eof()) 
	//{
	//	fp>>position>>value; 
	//	DoProduction(position,value);
	//}
	//fp.close();

	// enableActions(false);
}

void MainWindow::on_actionOpen_triggered()
{
	//ReadLanguageFile();

	if (m_pSettingNodeGroupDlg!=NULL)
	{

		if (m_pSettingNodeGroupDlg->isMinimized())
		{
			m_pSettingNodeGroupDlg->showNormal();

		}
		else
		{
			char num[64]="";
			int iNum;
			GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JITOU_NUM, 3, num);
			iNum = atoi(num);
			//m_pSettingNodeGroupDlg->setLanguage(m_vLaguages);
			m_pSettingNodeGroupDlg->InitJTNum(iNum);
			m_pSettingNodeGroupDlg->InitAllChildrenCombox(m_vAllNodeName);
			m_pSettingNodeGroupDlg->show();
			m_isFirstOpenGroupDlg = false;
		}



	}


}
void MainWindow::on_actionSave_As_triggered()
{
	//QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), m_lastDirectory, tr("OSG files (*.*)"));

	//m_sceneModel->saveSceneData(fileName.toStdString());
	////osgDB::writeNodeFile(*m_rootNode.get(), fileName.toStdString());

	m_pCollisionDlg->close();
	if(m_pLanguageDlg!=NULL)
	{

		if (m_pLanguageDlg->isMinimized())
		{
			m_pLanguageDlg->showNormal();
		}
		else
		{
			m_pLanguageDlg->InitLanguageCombox(m_vLanguagevWords,m_strDefaultLanguageType);
			m_pLanguageDlg->show();
		}

	}


}
void MainWindow::on_actionLoad_Shader_triggered()
{
	if (m_pSettingColor!=NULL)
	{
		if (m_pSettingColor->isMinimized())
		{
			m_pSettingColor->showNormal();
		}
		else
		{
			m_pSettingColor->InitModelComBox(m_vAllNodeName);
			m_pSettingColor->show();
		}
	}




#if 0
	xShaderSettingDialog *shaderSetting = new xShaderSettingDialog(this);
	if (shaderSetting != NULL)
	{
		if (shaderSetting->exec() != QDialog::Accepted)
			return;
		QString vertShaderFile = shaderSetting->getVertexShaderPath();
		QString fragmentShaderFile = shaderSetting->getFragmentShaderPath();

		osg::Shader *vertShader = new osg::Shader(osg::Shader::VERTEX);
		std::string vertShaderSource = osgDB::findDataFile(vertShaderFile.toStdString());

		bool success = vertShader->loadShaderSourceFromFile(vertShaderSource);
		if (!success)
		{
			xLogHandler::getInstance()->slotReportError(tr("Couldn't load file: %1 ...").arg(vertShaderFile));
			return;
		}

		osg::Shader *fragShader = new osg::Shader(osg::Shader::FRAGMENT);
		std::string fragShaderSource = osgDB::findDataFile(fragmentShaderFile.toStdString());
		success = fragShader->loadShaderSourceFromFile(fragShaderSource);
		if (!success)
		{
			xLogHandler::getInstance()->slotReportError(tr("Couldn't load file: %1 ...").arg(fragmentShaderFile));
			return;
		}

		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->setName("userShading");

		program->addShader(vertShader);
		program->addShader(fragShader);

		osg::ref_ptr<osg::StateSet> stateSet = m_sceneModel->getScene()->getOrCreateStateSet();
		if (stateSet.valid())
			stateSet->setAttributeAndModes(program.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	}

#endif
}

void MainWindow::on_actionRemove_Shader_triggered()
{
	if (m_sceneModel->getScene() != NULL)
	{
		osg::ref_ptr<osg::StateSet> stateSet = m_sceneModel->getScene()->getOrCreateStateSet();
		if (stateSet.valid())
			stateSet->removeAttribute(osg::StateAttribute::PROGRAM);
	}

}
//bool MainWindow::loadFile(const QString &file)
//{
//	if (file.isEmpty() || !QFileInfo(file).exists())
//	{
//		QMessageBox::warning(NULL, "warning", tr("%1 does not exist").arg(file), QMessageBox::Ok);
//		return false;
//
//	}
//
//	//saveIfNeeded();
//
//	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//
//	//m_currFile = file;
//	m_lastDirectory = QFileInfo(file).absolutePath();
//
//	// enable actions
//	enableActions(true);
//	
//	QString  file_name;  
//	QFileInfo fi;  
//	fi = QFileInfo(file);  
//	file_name = fi.fileName();  
//	BeforeSettingNodeDlg(file_name);
//	if(m_bIsFirstSTL == false)
//	{
//		if (NodeDlg->exec()==0)
//		{	
//			if(NodeDlg->GetIsDel() == true)//删除节点的话，节点不添加进视图
//			{
//				DelChildToFatherMatrix(NodeDlg->GetCurChildName(),NodeDlg->GetCurFatherName());
//				return false;
//			}
//			//添加
//			if(NodeDlg->GetIsAdd() == true)
//			{
//				m_objectLoader->slotNewObjectToLoad(file);
//				slotNewLoadedFileNew(m_objectLoader->GetLoadedModel(),m_objectLoader->GetFileName(),m_bIsFirstSTL);
//				m_bIsFirstSTL = false;
//			}
//			else
//			{
//				return false;
//			}			
//					
//		}
//	}
//	else if(m_bIsFirstSTL == true)
//	{
//		m_objectLoader->slotNewObjectToLoad(file);
//		if (NodeDlg->exec()==0)
//		{
//			slotNewLoadedFileNew(m_objectLoader->GetLoadedModel(),m_objectLoader->GetFileName(),m_bIsFirstSTL);
//			m_bIsFirstSTL = false;
//		}
//	}
//
//		
//
//	//m_objectLoader->slotNewObjectToLoad(file);
//	//if (NodeDlg->exec()==0)
//	//{
//	//	slotNewLoadedFileNew(m_objectLoader->GetLoadedModel(),m_objectLoader->GetFileName(),m_bIsFirstSTL);
//	//	m_bIsFirstSTL = false;
//	//}
//	//m_curNode = m_objectLoader->GetLoadedModel();
//	//if(m_bIsFirstSTL == true)
//	//{
//	//	STL *firststl = new STL(m_curNode,file_name.toStdString());
//	//	m_vSTLs.push_back(firststl);
//
//	//	m_sceneModel->GetNodeScene()->addChild(m_curNode);
//	//}
//	//else
//	//{
//	//	if(NodeDlg->GetIsAdd() == true)
//	//	{
//	//		AddChildToFatherMatrix(NodeDlg->GetCurChildName(),NodeDlg->GetCurFatherName());
//	//	}
//	//}
//	//emit sigNewFileToLoad(file);
//
//	return true;
//}





bool MainWindow::loadFile(const QString &file)
{
	if (file.isEmpty() || !QFileInfo(file).exists())
	{
		//QMessageBox::warning(NULL, "warning", tr("%1 does not exist").arg(file), QMessageBox::Ok);
		LogInfo(QString("%1 does not exist").arg(file));
		return false;

	}

	//saveIfNeeded();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	//m_currFile = file;
	m_lastDirectory = QFileInfo(file).absolutePath();

	// enable actions
	enableActions(true);
	//m_setNodeDlg->SetVecGNodes(m_sceneModel->getVecGNodes());

	m_objectLoader->slotNewObjectToLoad(file);

	slotNewLoadedFile(m_objectLoader->GetLoadedModel(),
		m_objectLoader->GetMaterial(),
		m_objectLoader->GetFileName());

	//emit sigNewFileToLoad(file);

	return true;
}



void MainWindow::InitSTLSide(STL *stl)
{
	float fvalue;

	char sLM_HOLE_R[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_HOLE_R, 3, sLM_HOLE_R);
	fvalue = atof(sLM_HOLE_R);
	stl->SetLMHOLE_R(fvalue);

	char sLM_R[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_R, 3, sLM_R);
	fvalue = atof(sLM_R);
	stl->SetLMr(fvalue);


	char sLM_UPLEN[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_UPLEN, 3, sLM_UPLEN);
	fvalue = atof(sLM_UPLEN);
	stl->SetLMupLen(fvalue);

	char sLM_DOWNLEN[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_DOWNLEN, 3, sLM_DOWNLEN);
	fvalue = atof(sLM_DOWNLEN);
	stl->SetLMdownLen(fvalue);


	char sLM_TULEN[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_TULEN, 3, sLM_TULEN);
	fvalue = atof(sLM_TULEN);
	stl->SetLMtuLen(fvalue);

	char sLM_TUSIDE[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LM_TUSIDE, 3, sLM_TUSIDE);
	fvalue = atof(sLM_TUSIDE);
	stl->SetLMTuSide(fvalue);

	char sJM_LEN[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JM_LEN, 3, sJM_LEN);
	fvalue = atof(sJM_LEN);
	stl->SetJMLen(fvalue);

	char sJM_WIDE[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JM_WIDE, 3, sJM_WIDE);
	fvalue = atof(sJM_WIDE);
	stl->SetJMWide(fvalue);

	char sJM_AOSIDE[64]="";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JM_AOSIDE, 3, sJM_AOSIDE);
	fvalue = atof(sJM_AOSIDE);
	stl->SetJMAoSide(fvalue);
}

void MainWindow::ReBuildGroupToSTLsFile(std::map<QString,GroupChildren> MapGroupToGC)
{
	//std::locale loc = std::locale::global(std::locale(""));
	std::ofstream outFile;
	QString file = need_filePath + QString::fromLocal8Bit("/GroupToModels.csv");
	std::string str = std::string((const char *)file.toLocal8Bit().constData());
	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"GroupName"<<','<<"Position"<<','<<"Models"<<std::endl;
	//写入数据-字符串
	std::map<QString,GroupChildren>::iterator it = MapGroupToGC.begin();
	for(;it!=MapGroupToGC.end();it++)
	{	
		std::string GN =  string(it->first.toLocal8Bit()); /*it->first.toStdString();*/
		outFile<<GN<<','<<"\"(";//组名
		std::string sx = toString(it->second.fx);
		std::string sy = toString(it->second.fy);
		std::string sz = toString(it->second.fz);
		outFile<<sx<<','<<sy<<','<<sz<<")\""<<',';//坐标

		outFile<<"\"(";
		std::vector<QString>::iterator itr = it->second.vChildrenNames.begin();
		for (;itr!=it->second.vChildrenNames.end();itr++)
		{
			std::string sname = string((*itr).toLocal8Bit());
			if((itr+1)==it->second.vChildrenNames.end())
			{
				outFile<<sname;
			}
			else
			{
				outFile<<sname<<',';
			}
		}
		outFile<<")\""<<std::endl;
	}
	outFile.close();
	//std::locale::global(std::locale(loc ));//注意这里的locale( "", locale::all ^ locale::numeric)！表示表示使用本地语言，这里当然相当于“chs”或者“chinese”。
}

bool MainWindow::isFileExist(QString fullFileName)
{
	QFileInfo file(fullFileName);
	if(!file.exists())
	{
		return false;
	}
	else
	{
		return true;
	}	
}

void MainWindow::ReadBendProcess1(QString pathName)
{
	//if(isFileExist(pathName) == false)
	//{
	//	QMessageBox::warning(NULL, "warning", "推弯流程1文件不存在", QMessageBox::Yes);
	//	return;
	//}

	m_vTwlc1values.clear();

	QByteArray ba = pathName.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			//数据
			USES_CONVERSION; //这两句也要加入2
			std::string szBuffer = W2A(buf);//这两句也要加入3
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=4)
			{
				QMessageBox::warning(NULL, "warning", "error with productionvalues in BendProcess1", QMessageBox::Yes);
				LogInfo("error with productionvalues in BendProcess1");
				return;
			}

			TWLC1 tmp;
			tmp.ftwy = QString::fromStdString(Res[0]).toFloat();
			tmp.ftwbj = QString::fromStdString(Res[1]).toFloat();
			tmp.ftwc = QString::fromStdString(Res[2]).toFloat();
			tmp.ftwb = QString::fromStdString(Res[3]).toFloat();
			m_vTwlc1values.push_back(tmp);
			m_fallR.push_back(tmp.ftwbj);

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();
}

void MainWindow::ReadBendProcess2(QString pathName)
{
	//if(isFileExist(pathName) == false)
	//{
	//	QMessageBox::warning(NULL, "warning", "推弯流程2文件不存在", QMessageBox::Yes);
	//	return;
	//}


	m_vTwlc2values.clear();

	QByteArray ba = pathName.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; //这两句也要加入2
			std::string szBuffer = W2A(buf);//这两句也要加入3
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);

			if(Res.size()!=4)
			{
				QMessageBox::warning(NULL, "warning", "error with productionvalues in BendProcess2", QMessageBox::Yes);
				LogInfo("error with productionvalues in BendProcess2");
				return;
			}

			TWLC2 tmp;
			tmp.ftwy = QString::fromStdString(Res[0]).toFloat();
			tmp.ftwbj = QString::fromStdString(Res[1]).toFloat();
			tmp.ftwc = QString::fromStdString(Res[2]).toFloat();
			tmp.ftwb = QString::fromStdString(Res[3]).toFloat();
			m_vTwlc2values.push_back(tmp);
			m_fallR.push_back(tmp.ftwbj);

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();
}

void MainWindow::ReadParams(QString pathName)
{
	LogInfo("Enter Read Params values !");

	if(isFileExist(pathName) == false)
	{
		//QMessageBox::warning(NULL, "warning", "ParamFile does not exist !", QMessageBox::Yes);
		LogInfo(pathName);
		LogInfo("ParamFile does not exist !");
		return;
	}

	memset(&m_param,0,sizeof(m_param));
	m_param.LmValueNum = 0;
	m_param.RDmValueNum = 0;
	m_param.ZmValueNum = 0;
	m_param.LDmValueNum = 0;

	int count = 0;
	QByteArray ba = pathName.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		memset(buf, 0, maxChar*sizeof(wchar_t));

		QString strtmp = "TMP";
		wchar_t* t  = (WCHAR*)strtmp.utf16();

		while (fp.getline(buf, maxChar))
		{
			std::wstring str(buf);
			std::wstring header;   // jitou bianhao
			QString qstr = stdWToQString(str);

			if (qstr.contains("R",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				header = str.substr(str.find(',')+1,str.size());
				str = str.substr(0, str.find(',')); 
				float lmr = stdWToQString(str).toFloat();
				int num = stdWToQString(header).toInt();
				m_param.LmRadius.push_back(lmr);
				m_param.LmAcRadius.push_back(0);//初始化轮模凹槽0
				m_param.LmHeaderNum.push_back(num);
				if(lmr>0)
				{
					m_param.LmValueNum++;
				}
			}
			else if (qstr.contains("D",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.fPipeDiameter = pd;
			}
			else if (qstr.contains("L",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.fPipeLength = pd;
			}

			else if (qstr.contains("mn",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toInt();
				m_param.imn = pd;
			}
			else if (qstr.contains("ydp",Qt::CaseSensitive)) //原点位置
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				//if (pd>0)
				//{
				//	pd = m_pipe_z-pd;
				//}
				m_param.fYdPos = pd;
			}
			else if (qstr.contains("zgp",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				//if (pd>0)
				//{

				//	pd = m_pipe_z-pd;
				//}
				// 参数文件里写的是多少就是多少，不反，流程解析的时候根据设置设定
				m_param.fZgPos = pd;
			}

			else if (qstr.contains("dywp",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				//if (pd>0)
				//{
				//	pd = m_pipe_z-pd;
				//}
				m_param.fDywPos = pd;
			}

			else if (qstr.contains("wqmr",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.fWqmRadius = pd;
			}

			else if (qstr.contains("mjl",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.fMjDistance = pd;
			}

			else if (qstr.contains("zxmr",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.fZxmRadius = pd;
			}

			else if (qstr.contains("dmal",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.fDmaqDistance = pd;
			}

			else if (qstr.contains("wtwp",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				//if (pd>0)
				//{
				//	pd = m_pipe_z-pd;
				//}
				m_param.fWftwPos = pd;
			}
			else if (qstr.contains("zm",Qt::CaseSensitive))
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.Zmlength.push_back(pd);
				if(pd>0)
				{
					m_param.ZmValueNum++;
				}
			}

			else if (qstr.contains(QString::fromLocal8Bit("ydm"),Qt::CaseSensitive))  //右导模
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.RDmlength.push_back(pd);
				if(pd>0)
				{
					m_param.RDmValueNum++;
				}

			}
			else if (qstr.contains(QString::fromLocal8Bit("zdm"),Qt::CaseSensitive))   //左导模
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(0, str.find(',')); 
				float pd = stdWToQString(str).toFloat();
				m_param.LDmlength.push_back(pd);
				if(pd>0)
				{
					m_param.LDmValueNum++;
				}

			}

			memset(buf, 0, maxChar*sizeof(wchar_t));

		}


	}
	else
	{
		LogInfo(pathName);
		LogInfo("read this file failed !");
	}

	fp.close();

	LogInfo("Exit Read Params values !");

	//if(m_param.LmValueNum != m_param.LDmValueNum || m_param.LmValueNum != m_param.RDmValueNum)
	//{
	//	QMessageBox::warning(NULL, "warning", "missing data in ParamFile !", QMessageBox::Yes);
	//	return;
	//}

	//InitModel();
}

void MainWindow::split(string& s,vector<string>&res,string& delema1)
{
	string::size_type start=s.find_first_not_of(delema1,0); //找到第一个不为逗号的下标
	string::size_type pose=s.find_first_of(delema1,start);  //找到第一个逗号的下标
	while(string::npos!=start || string::npos!=pose)
	{//当即没有逗号也没有字符的时候结束
		res.push_back(s.substr(start,pose-start));
		start=s.find_first_not_of(delema1,pose);//更新start 从pose开始

		pose=s.find_first_of(delema1,start);//更新pos,从start开始
		//if(pose==string::npos)
		//{
		//	pose = s.size();
		//}
	}
}

void MainWindow::ReadProductionValues(QString pathName)
{
	LogInfo("Enger read production values !");

	QString strName;
	QString str;
	std::string s;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PRODUCTION_VALUES_FILES_PATH, 3, param_path);
	GetWholePath(param_path);
	s = param_path;
	str = QString::fromLocal8Bit(s.c_str());
	strName = QDir::fromNativeSeparators(str);//"\"转"/"
	strName = strName + QString::fromLocal8Bit("/参数.csv");	
	//参数文件
	ReadParams(strName);

	//推弯流程文件
	ReadAllTWLCFiles();

	LogInfo("Exit read production values !");
#if 0

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PRODUCTION_VALUES_FILES_PATH, 3, twlc_path);
	GetWholePath(twlc_path);
	s = twlc_path;
	str = QString::fromLocal8Bit(s.c_str());
	strName = QDir::fromNativeSeparators(str);//"\"转"/"
	strName = strName + QString::fromLocal8Bit("/推弯流程1.csv");	
	ReadBendProcess1(strName);


	strName = QDir::fromNativeSeparators(str);//"\"转"/"
	strName = strName + QString::fromLocal8Bit("/推弯流程2.csv");
	ReadBendProcess2(strName);

#endif

	int count = 0;
	m_vproductionvalues.clear();
	QByteArray ba = pathName.toLocal8Bit();
	char* ch=ba.data();
	std::wifstream fp(ch);
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		//跳过第一行
		memset(buf, 0, maxChar*sizeof(wchar_t));
		fp.getline(buf, maxChar);

		memset(buf, 0, maxChar*sizeof(wchar_t));
		while (fp.getline(buf, maxChar))
		{
			USES_CONVERSION; 
			std::string szBuffer = W2A(buf);
			std::vector<string> Res;
			std::string Delema=",";
			split(szBuffer,Res,Delema);
			/*for(vector<string>::iterator iter=Res.begin();iter!=Res.end();iter++)
			{

			}*/
			if(Res.size()!=24)
			{
				QMessageBox::warning(NULL, "warning", "error with ProductionValue File", QMessageBox::Yes);
				LogInfo("error with ProductionValue File !");
				return;
			}		

			ProductionValue tmp;
			tmp.fmy = QString::fromStdString(Res[0]).toFloat();
			tmp.fmyb =QString::fromStdString(Res[1]).toFloat();
			tmp.fmb =QString::fromStdString(Res[2]).toFloat();
			tmp.fmbb =QString::fromStdString(Res[3]).toFloat();
			tmp.fmc =QString::fromStdString(Res[4]).toFloat();
			tmp.fmcb =QString::fromStdString(Res[5]).toFloat();
			tmp.fmm =QString::fromStdString(Res[6]).toFloat();
			tmp.flr =QString::fromStdString(Res[7]).toFloat();
			tmp.fmyt =QString::fromStdString(Res[8]).toFloat();
			tmp.fmz =QString::fromStdString(Res[9]).toFloat();
			tmp.fbtm =QString::fromStdString(Res[10]).toFloat();
			tmp.fctm =QString::fromStdString(Res[11]).toFloat();
			tmp.fdz =QString::fromStdString(Res[12]).toFloat();
			tmp.fmyth =QString::fromStdString(Res[13]).toFloat();
			tmp.flc =QString::fromStdString(Res[14]).toFloat();
			tmp.fmmd =QString::fromStdString(Res[15]).toFloat();
			tmp.fjm =QString::fromStdString(Res[16]).toFloat();
			tmp.fxb =QString::fromStdString(Res[17]).toFloat();
			tmp.fjy =QString::fromStdString(Res[18]).toFloat();
			tmp.ftwy =QString::fromStdString(Res[19]).toFloat();
			tmp.ftwbj =QString::fromStdString(Res[20]).toFloat();
			tmp.ftwc =QString::fromStdString(Res[21]).toFloat();
			tmp.ftwb =QString::fromStdString(Res[22]).toFloat();
			tmp.ftwlc =QString::fromStdString(Res[23]).toFloat();
			m_vproductionvalues.push_back(tmp);	

			m_fallR.push_back(tmp.ftwbj);


			Point3D tmp3d;
			tmp3d.x() = tmp.fmy;
			tmp3d.y() = tmp.fmb;
			tmp3d.z() = tmp.fmc;
			InputData tmpInput;
			tmpInput.point = tmp3d;
			//tmpInput.radius = m_param.LmRadius[count];
			if(tmp.flr == 0)
			{
				tmpInput.isRight = false;
			}
			else if(tmp.flr == 1)
			{
				tmpInput.isRight = true;

			}
			m_InputDataArray.push_back(tmpInput);
			if(count< m_param.LmValueNum -1)
				count++;

			memset(buf, 0, maxChar*sizeof(wchar_t));
		}
	}

	fp.close();

	if(m_param.imn >= (int)m_vproductionvalues.size())
	{
		QMessageBox::warning(NULL, "warning", "missing data in ProductionValue!", QMessageBox::Yes);
		LogInfo("missing data in ProductionValue !");
		return;
	}

	////显示所有加工值
	////m_vOsgText.clear();
	//m_vMachineInfo.clear();
	//osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
	//// 显示加工值为弯数加1
	//for(int i=0;i<m_param.imn+1;i++)
	//{
	//	std::string s_n = toString(i+1);
	//	SetStringLength(s_n,2);
	//	std::string s_y ="Y:" + toString(m_vproductionvalues[i].fmy);	
	//	SetStringLength(s_y,10);
	//	std::string s_b ="B:" + toString(m_vproductionvalues[i].fmb);
	//	SetStringLength(s_b,10);
	//	std::string s_c ="C:" + toString(m_vproductionvalues[i].fmc);
	//	SetStringLength(s_c,10);
	//	std::string s =s_n + s_y + s_b + s_c;
	//	m_vMachineInfo.push_back(s);

	//}

	//m_sceneModel->GetHudtext()->setHUDText(m_vMachineInfo);

}

float MainWindow::GetPosValueByString(std::string str)
{
	float fresult;
	//std::string str1;
	//QString s1;
	//std::string s11;
	//s1 = s1.fromLocal8Bit("（");
	//s11 = s1.toStdString();
	//QString s2;
	//std::string s12;
	//s2 = s2.fromLocal8Bit("）");
	//s12 = s2.toStdString();
	//if (str.find(s11) == std::string::npos || str.find(s12) == std::string::npos || str.find(" ") == std::string::npos)
	//{
	//	QString strWarning;
	//	strWarning = strWarning.fromLocal8Bit("初始位置含有中文括号或空格!");
	//	QMessageBox::warning(this,"",strWarning);
	//	//return ;
	//}
	//else
	//{
	if (str.find("(") != std::string::npos)
	{
		str = str.replace(str.find("("),1,"");

	}

	if(str.find("\"") != std::string::npos)
	{
		str = str.replace(str.find("\""),1,"");
	}

	if(str.find(")") != std::string::npos)
	{
		str = str.replace(str.find(")"),1,"");
	}


	fresult = QString::fromStdString(str).toFloat();

	//}

	return fresult;

	/*char ch[20];
	char result[20];
	bool style = false;
	strcpy(ch,str.data());

	while(*ch)
	{
	if((*ch)=='(' || (*ch)=='"' || (*ch)==')')
	{
	ch++;
	style = true;
	}
	else
	{
	result = result + ch;
	}
	}*/
}

QString MainWindow::GetStrPosValueByString(std::string str)
{
	QString strresult;
	if (str.find("(") != std::string::npos)
	{
		str = str.replace(str.find("("),1,"");

	}

	if(str.find("\"") != std::string::npos)
	{
		str = str.replace(str.find("\""),1,"");
	}

	if(str.find(")") != std::string::npos)
	{
		str = str.replace(str.find(")"),1,"");
	}


	strresult = QString::fromStdString(str);
	return strresult;
}

//void MainWindow::CreateAnimation()
//{
//	//刷新颜色
//	m_sceneModel->GetHudtext()->ResetAllColor();
//
//	//使能暂停键
//	ui.actionLight->setEnabled(true);
//	//取消暂停选中状态
//	emit sigPartsPause(true);
//	ui.actionLight->setChecked(false);
//
//	//clear vPipe
//	osg::ref_ptr<osg::Group> pipefather = m_sceneModel->getPipeFather();//管子父节点
//	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrix = m_sceneModel->getPipeFatherMatrix();//管子父节点下的变化矩阵
//	if(m_vPipes.size() >= 2)
//	{
//		for (int i = m_vPipes.size()-1;i>=1;--i)
//		{
//			m_vPipes[i].GetMoveMatrix()->removeChild(m_vPipes[i-1].GetRotateMatrix());
//		}
//		osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
//		pipefatherMatrix->removeChild(m_vPipes[m_vPipes.size() -1].GetRotateMatrix());//绑定管子最后一届到管子矩阵根节点上
//		((osg::Group*)nodestl)->removeChild(pipefather);
//
//		m_vPipes.clear();
//		m_sceneModel->ResetPipeIndex();
//	}
//
//	if (!m_vMatrixAnimation.empty())
//	{
//		m_vMatrixAnimation.clear();
//	}
//
//	osg::ref_ptr<osg::ClipNode> clip = m_sceneModel->getClipNode();
//
//	osg::ref_ptr<osg::MatrixTransform> pipematrix;
//
//	dtime = 0.5; // 时间轴
//
//	double jitouwanguan3move = 106;
//	//1.系统复归
//	m_vWanQuMo2Animation.clear();
//	m_vJiTouWanGuan3Animation.clear();
//	m_vPipeAnimation.clear();
//	m_vXiaoCheAnimation.clear();
//	m_vJiTouWanGuan1Animation.clear();
//	m_vJiTouWanGuan2Animation.clear();
//	m_vZhuaLiaoMoAnimation.clear();
//	m_vZhuJia1Animation.clear();
//	m_vZhuJia2Animation.clear();
//	m_vDaoJia2Animation.clear();
//	m_vDaoJia1Animation.clear();
//	m_vDaoJia1_1Animation.clear();
//	m_vZuoFuTuiAnimation.clear();
//	m_vYouFuTuiAnimation.clear();
//	m_vJiTouMoZuoAnimation.clear();
//	m_vJiTouDiZuoAnimation.clear();
//	m_vDaoLun1Animation.clear();
//	m_vDaoLun2Animation.clear();
//	m_vZhuLunAnimation.clear();
//	m_vCheTouAnimation.clear();
//	m_vMatrixAnimation.clear();
//	//辅推导夹
//	m_vDaoJiaTmpAnimation.clear();
//	m_vLFuiTuiDaoJia0Animation.clear();
//	m_vLFuiTuiDaoJia1Animation.clear();
//	m_vLFuiTuiDaoJia2Animation.clear();
//	m_vLFuiTuiDaoJia3Animation.clear();
//	m_vRFuiTuiDaoJia0Animation.clear();
//	m_vRFuiTuiDaoJia1Animation.clear();
//	m_vRFuiTuiDaoJia2Animation.clear();
//	m_vRFuiTuiDaoJia3Animation.clear();
//
//
//	//ini文件里的加紧时导夹移到距离
//	Point3D fJiaMoDis(0, 80.0f, 80.0f);
//	char zjoffsetvalue[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_ZHUJIA_Z_OFFSET, 3, zjoffsetvalue);
//	fJiaMoDis.z() = atof(zjoffsetvalue);
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_ZHUJIA_Y_OFFSET, 3, zjoffsetvalue);
//	fJiaMoDis.y() = atof(zjoffsetvalue);
//
//	char zoremove[32] = "";
//	float fzoremove;//ini文件的初始加紧值
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ZORE_MOVE, 3, zoremove);
//	fzoremove = atof(zoremove);
//
//	//换边 主夹2移动
//	char ZhuJia2ChangeMove[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ZHUJIA2_CHANGEMOVE, 3, ZhuJia2ChangeMove);
//	m_fZhuJia2ChangeMove = atof(ZhuJia2ChangeMove);
//
//	//抓料模支撑到车头中心距离
//	char cdis[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ZHUALIAOMO_ZhICHENGDIS, 3, cdis);
//	float fzhichengDis = atof(cdis);
//
//	//车头开始归零
//	char CheTouZero[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ZORE_B_ANGLE, 3, CheTouZero);
//	float fCheTouZeroRatate = atof(CheTouZero);
//
//	//抓料模开始归零
//	char ZhuaLiaoMoZero[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ZORE_ZHUA_ANGLE, 3, ZhuaLiaoMoZero);
//	float fZhuaLiaoMoZeroRatate = atof(ZhuaLiaoMoZero);
//
//	//机头模座
//	char cjitoumozuodis[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JITOUMOZUO_ZORE_MOVE, 3, cjitoumozuodis);
//	float fjitoumozuoDis = atof(cjitoumozuodis);
//
//	//机头底座
//	char cjitoudizuodis[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JITOUDIZUO_ZORE_MOVE, 3, cjitoudizuodis);
//	float fjitoudizuoDis = atof(cjitoudizuodis);
//
//	//主轮凹槽半径
//	char czhulunAcR[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ZHULUN_AOCAO_R, 3, czhulunAcR);
//	float fOriginalzlAcR = atof(czhulunAcR);
//
//	char cjtdzLower[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JITOUDIZUO_LOWER_LEN, 3, cjtdzLower);
//	float fjtdzLowerLen = atof(cjtdzLower);
//
//	char cjtwg1Zore[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JITOUWANGUAN1_ZORE_ANGLE, 3, cjtwg1Zore);
//	float fjtwg1Zore = atof(cjtwg1Zore);
//
//	char cjtwg2Zore[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, JITOUWANGUAN2_ZORE_ANGLE, 3, cjtwg2Zore);
//	float fjtwg2Zore = atof(cjtwg2Zore);
//
//	char cdl1JiaJin[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, DAOLUN1_JIAJIN_ANGLE, 3, cdl1JiaJin);
//	float fdl1JiaJin = atof(cdl1JiaJin);
//
//	char cdl2JiaJin[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, DAOLUN2_JIAJIN_ANGLE, 3, cdl2JiaJin);
//	float fdl2JiaJin = atof(cdl2JiaJin);
//
//	char czlR[32] = "";
//	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ZHULUN_R, 3, czlR);
//	float fzlR = atof(czlR);
//
//	//当前轮模
//	m_fcurModelLayer = 1.0;
//	//机头弯管上一次的弯角度
//	m_IsLeftFuTuiFlat = true;
//	m_IsRightFuTuiFlat = true;
//	//中心模左移的距离
//	m_fNeedRightmove = 0;
//	m_fNeedRightmove2 = 0;
//	ilastmmdR = 0;
//	flastmz = 0;
//	m_linkedNum = 0;
//	m_changesidedown = 2;
//
//
//	m_bIsLeft = false;
//	m_lastangle = 0;
//	m_Rlastangle = 0;
//	//m_v = m_pCollisionDlg->GetAnimatiuonSpeed();
//	m_v = m_pSpeedDlg->GetCurSpeed();
//	m_fAnimationTime = 0.5/m_v;//动画时间 
//	m_fAnimationStopTime = 0.5/m_v;//间隔时间
//	int icount;//多次推弯判断
//	osg::Vec3 vec3Center = m_CurZhuLunCenter;
//	float fmove_z;
//	float fmove_x;
//	float fmove_y;
//	float frotate_y;
//	float frotate_z;
//	float fFuiZhuiRotate;//辅助轮旋转值
//	//m_fFuiZhuiRotateBack = 0;//辅助轮旋转值归零
//	m_fRdlastangle = 0;
//	m_fLdlastangle = 0;
//	float fSafeDis;//小车可以前进的安全距离
//	m_zlmTozlDis = m_CurZhuLunCenter.z() - m_CurZhuaLiaoMoCenter.z();
//
//
//
//	//车头、抓料模,机头模座归零
//	PrePare(ZhuaLiaoMoinfo,fZhuaLiaoMoZeroRatate,CheTouInfo,fCheTouZeroRatate,JiTouMoZuoinfo,fjitoumozuoDis,JiTouDiZuoinfo,fjitoudizuoDis,fjtwg1Zore,fjtwg2Zore);
//	//小车至待料区
//	fmove_z = m_zlmTozlDis + m_param.fZgPos;
//	XiaoCheAnimation(-1,dtime,dtime+0.5,fmove_z);
//	dtime = dtime + 0.5 + 1.0;
//
//	for(int i=0;i<m_param.imn;i++)
//	{
//		icount = 1;
//		//*********************************************弯管前小车位置
//		if(i == 0)
//		{
//			//*********************************************弯管前小车位置
//			fmove_z = -m_param.fZgPos - m_param.fDywPos;
//			XiaoCheAnimation(0,dtime,dtime+m_fAnimationTime,fmove_z);
//			dtime  = dtime + m_fAnimationTime + m_fAnimationStopTime;
//			//同时生成直管
//			float flength = m_vproductionvalues[i].fmy;
//			ShowStraightPipe(0,m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,flength,i+1);
//		}
//		else{}
//
//		//*********************************************换边
//		float fcurrentAcR;
//		float fcurrentR;
//		float fproviousAcR;
//		float fproviousR;
//
//		if(m_vproductionvalues[i].flr == 1 && m_bIsLeft == false)//当前是右边，需要换到左边
//		{
//
//			if(i == 0)
//			{
//				//
//			}
//			else if(i> 0)
//			{
//				//机头弯管归零，1：辅助轮是固定压制值
//				JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,m_fRdlastangle);
//				frotate_y = -m_lastangle;
//				JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//机头底座下降
//				fmove_y = fjtdzLowerLen;
//				JiTouDiZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//机头弯管换边
//				frotate_y = 180;
//				JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				m_lastangle = 180;
//				m_Rlastangle = 0;
//
//				//模座右移
//				fmove_x =  36.08 + m_fNeedRightmove2 + m_vproductionvalues[i].fmz +m_param.fPipeDiameter;//主轮凹槽直径+已经脱模+下一次脱模+管直径
//				JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_x);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//机头底座上升
//				fmove_y = -fjtdzLowerLen;
//				JiTouDiZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//			}
//
//			m_bIsLeft = true;	 
//
//
//		}
//		else if(m_vproductionvalues[i].flr == 1 && m_bIsLeft == true)//当前在左边，不需要换边
//		{
//			m_bIsLeft = true;
//		}
//		else if(m_vproductionvalues[i].flr == 0 && m_bIsLeft == true)//当前为左边，需要换到右边
//		{
//
//			if(i == 0){}
//			else if(i>0)
//			{
//				//机头弯管归零，1：辅助轮是固定压制值
//				JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,-m_fLdlastangle);
//				frotate_y = m_Rlastangle;
//				JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//机头底座下降
//				fmove_y = fjtdzLowerLen;
//				JiTouDiZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_y);
//				dtime = dtime+m_fAnimationTime + m_fAnimationStopTime;
//
//				//机头弯管换边
//				frotate_y = -180;
//				JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				m_lastangle = 0;
//				m_Rlastangle = 180;
//
//				//模座左移
//				fmove_x =  -(36.08 + m_fNeedRightmove2 + m_vproductionvalues[i].fmz +m_param.fPipeDiameter);//主轮凹槽直径+已经脱模+下一次脱模+管直径
//				JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_x);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//机头底座上升
//				fmove_y = -fjtdzLowerLen;
//				JiTouDiZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//			}
//			m_bIsLeft = false;
//
//		}
//		else if(m_vproductionvalues[i].flr == 0 && m_bIsLeft == false)//当前为右边，不需要换边
//		{
//			m_bIsLeft = false;
//		}
//
//		//*********************************************夹紧管子
//		if(i == 0)
//		{
//			if(m_bIsLeft == false)
//			{
//				//主轮夹紧
//				fmove_x = 5.96 + (6 - m_param.fPipeDiameter/2);
//				JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_x);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//导轮2加紧
//				frotate_y = fdl2JiaJin;
//				JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				m_lastangle = fdl2JiaJin;
//				//导论1加紧 輔助轮
//				m_fRdlastangle = fdl1JiaJin;
//				frotate_y = -fdl1JiaJin;
//				JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//			}
//			else if(m_bIsLeft == true)
//			{
//				//主轮夹紧
//				fmove_x = -(5.96 + (6 - m_param.fPipeDiameter/2));
//				JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_x);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//导轮2加紧
//				frotate_y = -fdl2JiaJin;
//				JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				m_Rlastangle = fdl2JiaJin;
//				//导论1加紧
//				m_fLdlastangle = fdl1JiaJin;
//				frotate_y = fdl1JiaJin;
//				JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//			}
//		}
//		else if(i>0)
//		{
//			if(m_bIsLeft == false)
//			{
//				if(m_vproductionvalues[i].flr != m_vproductionvalues[i-1].flr)
//				{
//					//主轮夹紧
//					fmove_x = m_vproductionvalues[i].fmz;
//					JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_x);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				}
//				else
//				{
//					//主轮夹紧
//					fmove_x = m_fNeedRightmove2;
//					JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_x);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				}
//
//				//导轮2加紧
//				frotate_y = fdl2JiaJin;
//				JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				m_lastangle = fdl2JiaJin;
//				//导论1加紧 辅助轮
//				frotate_y = -fdl1JiaJin;
//				JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				m_fRdlastangle = m_fRdlastangle + fdl1JiaJin;
//			}
//			else if(m_bIsLeft == true)
//			{
//				if(m_vproductionvalues[i].flr != m_vproductionvalues[i-1].flr)
//				{
//					//主轮夹紧
//					fmove_x = m_vproductionvalues[i].fmz;
//					JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,-fmove_x);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				}
//				else
//				{
//					//主轮夹紧
//					fmove_x = m_fNeedRightmove2;
//					JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,-fmove_x);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				}
//				//导轮2加紧
//				frotate_y = -fdl2JiaJin;
//				JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				m_Rlastangle = fdl2JiaJin;
//				//导论1加紧//辅助轮
//				frotate_y = fdl1JiaJin;
//				JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				m_fLdlastangle = m_fLdlastangle + fdl1JiaJin;
//			}
//		}
//
//
//		//*********************************************弯管
//		if(m_vproductionvalues[i].ftwlc == 0)//推弯流程0
//		{
//			//判断小车是否要后退到“夹送位置+下弯送料长度或送料脱模+2次送料(送料脱模+2次送料>下弯送料长度时）
//			float fback;//夹送位置
//			float flength = m_vproductionvalues[i].fmyt+m_vproductionvalues[i].fmyth;
//			if(flength > m_vproductionvalues[i+1].fmy )
//			{
//				fback = flength + m_vproductionvalues[i].fjy;
//			}
//			else
//			{
//				fback = m_vproductionvalues[i].fjy + m_vproductionvalues[i+1].fmy;
//			}
//
//			//*********************************************绕弯、推弯判断
//			if(m_vproductionvalues[i].ftwy > 0)//进行推弯
//			{		
//				if(m_vproductionvalues[i].fjy == 0)//直接推弯，不到加送位置，不判断往复推弯位置
//				{
//					//*****************先进弯
//					if(m_bIsLeft == false)
//					{
//						//进弯
//						frotate_y = m_vproductionvalues[i].ftwc;
//						JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//						m_lastangle = m_lastangle + m_vproductionvalues[i].ftwc;			
//						//生成弯管 
//						//if(m_vproductionvalues[i].ftwc!=0)
//						ShowCurvePipe(i,0,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),JiTouWanGuan2info,PipeInfo,fzlR,m_vproductionvalues[i].ftwc,true);
//					}
//					else if(m_bIsLeft == true)
//					{
//						//进弯
//						frotate_y = -m_vproductionvalues[i].ftwc;
//						JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//						m_Rlastangle = m_Rlastangle + m_vproductionvalues[i].ftwc;
//						//生成弯管 
//						//if(m_vproductionvalues[i].ftwc!=0)
//						ShowCurvePipe(i,0,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),JiTouWanGuan2info,PipeInfo,fzlR,m_vproductionvalues[i].ftwc,true);
//					}
//					//弯的同时小车跟着往前
//					fmove_z = m_vproductionvalues[i].ftwc*fzlR*MYDEG_TO_RAD;
//					XiaoCheAnimation(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,fmove_z);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//					//*****************直接推弯
//					//小车向前推
//					fmove_z = m_vproductionvalues[i].ftwy - fmove_z;
//					XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					//车头同时转角
//					float frotate_z = m_vproductionvalues[i].ftwb;
//					CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//					//生成弯管
//					if(m_bIsLeft == true)//机头弯管在左边，要往右弯
//					{					
//						//生成弯管
//						ShowCurvePipe(i,0,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,m_vproductionvalues[i].ftwbj,RadiansToDegrees((fmove_z)/m_vproductionvalues[i].ftwbj),true);
//					}
//					else if(m_bIsLeft == false)
//					{
//						//生成弯管;
//						ShowCurvePipe(i,0,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,m_vproductionvalues[i].ftwbj,RadiansToDegrees((fmove_z)/m_vproductionvalues[i].ftwbj),true);
//					}
//					//转动管子
//					PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].ftwb,i);
//
//
//				}
//				else if(m_vproductionvalues[i].fjy > 0)//往复推，最后到夹送
//				{
//					//小车先到往复推弯位置
//					fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//					XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					//小车可推的安全距离
//					fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//
//					//判断进弯是否安全
//					float fjinwanLength = m_vproductionvalues[i].ftwc*fzlR*MYDEG_TO_RAD;
//					if(fSafeDis < fjinwanLength)//不安全就先回到往复推弯位置
//					{
//						//退回往复推弯位置
//						fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//						XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//						fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//					}
//
//					//*****************先进弯
//					if(m_bIsLeft == false)
//					{
//						//进弯
//						frotate_y = m_vproductionvalues[i].ftwc;
//						JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//						m_lastangle = m_lastangle + m_vproductionvalues[i].ftwc;			
//						//生成弯管 
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),JiTouWanGuan2info,PipeInfo,fzlR,m_vproductionvalues[i].ftwc,true);
//					}
//					else if(m_bIsLeft == true)
//					{
//						//进弯
//						frotate_y = -m_vproductionvalues[i].ftwc;
//						JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//						m_Rlastangle = m_Rlastangle + m_vproductionvalues[i].ftwc;
//						//生成弯管 
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),JiTouWanGuan2info,PipeInfo,fzlR,m_vproductionvalues[i].ftwc,true);
//					}
//					//弯的同时小车跟着往前
//					fmove_z = fjinwanLength;
//					XiaoCheAnimation(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,fmove_z);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//
//
//					//*****************往复推弯
//					float ftuiwanmove = m_vproductionvalues[i].ftwy - fjinwanLength;
//					if(fSafeDis > ftuiwanmove)//推弯安全,直接推
//					{
//						//推弯
//						fmove_z = ftuiwanmove;
//						XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//						fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//						//转角
//						frotate_z = m_vproductionvalues[i].ftwb;
//						CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//
//						//生成弯管
//						if(m_bIsLeft == true)//机头弯管在左边，要往右弯
//						{					
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,m_vproductionvalues[i].ftwbj,RadiansToDegrees(ftuiwanmove/m_vproductionvalues[i].ftwbj),true);
//						}
//						else if(m_bIsLeft == false)
//						{
//							//生成弯管;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,m_vproductionvalues[i].ftwbj,RadiansToDegrees(ftuiwanmove/m_vproductionvalues[i].ftwbj),true);
//						}
//						//转动管子
//						PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].ftwb,i);
//					}
//					else//不安全，往复推
//					{
//						while( ftuiwanmove > fSafeDis)//大于安全距离 就分几次推
//						{
//							//小车推安全距离
//							fmove_z = fSafeDis;
//							XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							//生成弯管
//							float fSafeAngle = osg::RadiansToDegrees(fSafeDis/m_vproductionvalues[i].ftwbj);
//							if(m_bIsLeft == false)
//							{
//								//生成弯管
//								ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,m_vproductionvalues[i].ftwbj,fSafeAngle,true);
//							}
//							else if(m_bIsLeft == true)
//							{
//								//生成弯管
//								ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,m_vproductionvalues[i].ftwbj,fSafeAngle,true);
//							}
//
//							//小车后退到往复推弯位置
//							fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//							XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							//剩余需要移动的距离
//							ftuiwanmove = ftuiwanmove -fSafeDis;
//							fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//							if(ftuiwanmove <= fSafeDis)//小车不需要再次后退
//							{
//								//小车移动剩余距离
//								fmove_z = ftuiwanmove;
//								XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//								dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//								fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//								//同时转角
//								frotate_z = m_vproductionvalues[i].ftwb;
//								CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//								//生成弯管
//								float fneedAngle = osg::RadiansToDegrees(ftuiwanmove/m_vproductionvalues[i].ftwbj);
//								if(m_bIsLeft == false)
//								{								
//									//生成弯管
//									ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,m_vproductionvalues[i].ftwbj,fneedAngle,true);
//								}
//								else if(m_bIsLeft == true)
//								{
//									//生成弯管
//									ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,m_vproductionvalues[i].ftwbj,fneedAngle,true);
//								}
//								//转动管子
//								PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].ftwb,i);
//								break;
//							}
//						}
//					}
//					//小车到夹送位置
//					fmove_z = m_zlmTozlDis - fback;
//					XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//					fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//
//				}
//
//			}
//			else if(m_vproductionvalues[i].ftwy == 0)
//			{
//				if(m_vproductionvalues[i].fjy == 0)//无小车夹送动作
//				{
//					//转动机头弯管	 绕弯	
//					if(m_bIsLeft == false)
//					{
//						//绕弯
//						frotate_y = m_vproductionvalues[i].fmc;
//						JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//						m_lastangle = m_lastangle + frotate_y;
//						//生成弯管
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),JiTouWanGuan2info,PipeInfo,fzlR,m_vproductionvalues[i].fmc,true);
//					}
//					else if(m_bIsLeft == true)
//					{
//						//绕弯
//						frotate_y = m_vproductionvalues[i].fmc;
//						JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//						m_Rlastangle = m_Rlastangle + frotate_y;
//						//生成弯管
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),JiTouWanGuan2info,PipeInfo,fzlR,m_vproductionvalues[i].fmc,true);
//					}
//					//弯的同时小车跟着往前
//					fmove_z = m_vproductionvalues[i].fmc*fzlR*MYDEG_TO_RAD;
//					XiaoCheAnimation(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,fmove_z);
//				}
//				else if(m_vproductionvalues[i].fjy > 0)//绕弯的同时小车夹送
//				{
//					//转动机头弯管	 绕弯	
//					if(m_bIsLeft == false)
//					{
//						//绕弯
//						frotate_y = m_vproductionvalues[i].fmc;
//						JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//						m_lastangle = m_lastangle + frotate_y;
//						//生成弯管
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),JiTouWanGuan2info,PipeInfo,fzlR,m_vproductionvalues[i].fmc,true);
//					}
//					else if(m_bIsLeft == true)
//					{
//						//绕弯
//						frotate_y = m_vproductionvalues[i].fmc;
//						JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//						m_Rlastangle = m_Rlastangle + frotate_y;
//						//生成弯管
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),JiTouWanGuan2info,PipeInfo,fzlR,m_vproductionvalues[i].fmc,true);
//					}
//					//弯的同时小车到夹送位置
//					fmove_z = m_zlmTozlDis - fback;
//					XiaoCheAnimation(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,fmove_z);
//				}
//			}
//
//		}
//		else if(m_vproductionvalues[i].ftwlc == 1)//推弯流程1
//		{
//			//判断小车是否要后退到“夹送位置+下弯送料长度或送料脱模+2次送料(送料脱模+2次送料>下弯送料长度时）
//			float fback;//夹送位置
//			float flength = m_vproductionvalues[i].fmyt+m_vproductionvalues[i].fmyth;
//			if(flength > m_vproductionvalues[i+1].fmy )
//			{
//				fback = flength + m_vproductionvalues[i].fjy;
//			}
//			else
//			{
//				fback = m_vproductionvalues[i].fjy + m_vproductionvalues[i+1].fmy;
//			}
//
//			float fSafeDis;
//			std::vector<struct TWLC1>::iterator iter= m_vTwlc1values.begin();
//			for(;iter!=m_vTwlc1values.end();iter++)
//			{
//				//判断是否夹送
//				if(m_vproductionvalues[i].fjy == 0)//直接推弯，不到加送位置，不判断往复推弯位置
//				{
//					//先进弯
//					float fjinwanLength;
//					if(m_bIsLeft == false)
//					{
//						if(icount == 1)
//						{
//							frotate_y = (*iter).ftwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							m_lastangle = m_lastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),JiTouWanGuan1info,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//						else//在上一个弯的基础上继续进弯
//						{
//							frotate_y = (*iter).ftwc - m_fLastTwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							m_lastangle = m_lastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//
//							AnimationInfo tmpInfo;
//							tmpInfo = JiTouWanGuan2info;
//							tmpInfo.fStartTime = dtime;
//							tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
//							dtime = tmpInfo.fEndTime +m_fAnimationStopTime;
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),tmpInfo,PipeInfo,fzlR,(*iter).ftwc,true);
//						}				
//					}
//					else if(m_bIsLeft == true)
//					{
//						if(icount == 1)
//						{
//							frotate_y = (*iter).ftwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							m_Rlastangle = m_Rlastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),JiTouWanGuan1info,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//						else//在上一个弯的基础上继续进弯
//						{
//							frotate_y = (*iter).ftwc - m_fLastTwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							m_Rlastangle = m_Rlastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//
//							AnimationInfo tmpInfo;
//							tmpInfo = JiTouWanGuan2info;
//							tmpInfo.fStartTime = dtime;
//							tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
//							dtime = tmpInfo.fEndTime +m_fAnimationStopTime;
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),tmpInfo,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//					}
//					//小车同时移动：进弯弧长
//					fjinwanLength = (*iter).ftwc*fzlR*MYDEG_TO_RAD;
//					fmove_z = fjinwanLength;
//					XiaoCheAnimation(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,fmove_z);
//
//					//再直接推弯
//					//小车向前推
//					fmove_z = (*iter).ftwy - fjinwanLength;
//					XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					//车头同时转角
//					frotate_z = (*iter).ftwb;
//					CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//					//生成弯管
//					if(m_bIsLeft == true)//机头弯管在左边，要往右弯
//					{					
//						//生成弯管
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,RadiansToDegrees(((*iter).ftwy - fjinwanLength)/(*iter).ftwbj),true);
//					}
//					else if(m_bIsLeft == false)
//					{
//						//生成弯管;
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,RadiansToDegrees(((*iter).ftwy - fjinwanLength)/(*iter).ftwbj),true);
//					}
//					//转动管子
//					PipeRotateX(CheTouInfo,PipeInfo,(*iter).ftwb,i);
//				}
//				else if(m_vproductionvalues[i].fjy > 0)//小车需要往复推弯，回到夹送位置
//				{
//					//小车先到往复推弯位置
//					fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//					XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//
//					float fjinwanLength = (*iter).ftwc*fzlR*MYDEG_TO_RAD;				
//					if(fjinwanLength < fSafeDis)
//					{
//						//回到往复推弯位置
//						fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//						XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//						fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//					}
//
//					//进弯
//					if(m_bIsLeft == false)
//					{
//						if(icount == 1)
//						{
//							frotate_y = (*iter).ftwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							m_lastangle = m_lastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),JiTouWanGuan1info,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//						else//在上一个弯的基础上继续进弯
//						{
//							frotate_y = (*iter).ftwc - m_fLastTwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							m_lastangle = m_lastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//
//							AnimationInfo tmpInfo;
//							tmpInfo = JiTouWanGuan2info;
//							tmpInfo.fStartTime = dtime;
//							tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
//							dtime = tmpInfo.fEndTime +m_fAnimationStopTime;
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),tmpInfo,PipeInfo,fzlR,(*iter).ftwc,true);
//						}				
//					}
//					else if(m_bIsLeft == true)
//					{
//						if(icount == 1)
//						{
//							frotate_y = (*iter).ftwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							m_Rlastangle = m_Rlastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),JiTouWanGuan1info,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//						else//在上一个弯的基础上继续进弯
//						{
//							frotate_y = (*iter).ftwc - m_fLastTwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							m_Rlastangle = m_Rlastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//
//							AnimationInfo tmpInfo;
//							tmpInfo = JiTouWanGuan2info;
//							tmpInfo.fStartTime = dtime;
//							tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
//							dtime = tmpInfo.fEndTime +m_fAnimationStopTime;
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),tmpInfo,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//					}
//					//小车同时移动：进弯弧长
//					fjinwanLength = (*iter).ftwc*fzlR*MYDEG_TO_RAD;
//					fmove_z = fjinwanLength;
//					XiaoCheAnimation(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,fmove_z);
//					fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//
//					//*****************往复推弯
//					float ftuiwanmove = (*iter).ftwy - fjinwanLength;
//					if(fSafeDis > ftuiwanmove)//推弯安全,直接推
//					{
//						//推弯
//						fmove_z = ftuiwanmove;
//						XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//						fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//						//转角
//						frotate_z = (*iter).ftwb;
//						CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//
//						//生成弯管
//						if(m_bIsLeft == true)//机头弯管在左边，要往右弯
//						{					
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,RadiansToDegrees(ftuiwanmove/(*iter).ftwbj),true);
//						}
//						else if(m_bIsLeft == false)
//						{
//							//生成弯管;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,RadiansToDegrees(ftuiwanmove/(*iter).ftwbj),true);
//						}
//						//转动管子
//						PipeRotateX(CheTouInfo,PipeInfo,(*iter).ftwb,i);
//					}
//					else//不安全，往复推
//					{
//						while( ftuiwanmove > fSafeDis)//大于安全距离 就分几次推
//						{
//							//小车推安全距离
//							fmove_z = fSafeDis;
//							XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							//生成弯管
//							float fSafeAngle = osg::RadiansToDegrees(fSafeDis/(*iter).ftwbj);
//							if(m_bIsLeft == false)
//							{
//								//生成弯管
//								ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,fSafeAngle,true);
//							}
//							else if(m_bIsLeft == true)
//							{
//								//生成弯管
//								ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,fSafeAngle,true);
//							}
//
//							//小车后退到往复推弯位置
//							fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//							XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							//剩余需要移动的距离
//							ftuiwanmove = ftuiwanmove -fSafeDis;
//							fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//							if(ftuiwanmove <= fSafeDis)//小车不需要再次后退
//							{
//								//小车移动剩余距离
//								fmove_z = ftuiwanmove;
//								XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//								dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//								fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//								//同时转角
//								frotate_z = (*iter).ftwb;
//								CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//								//生成弯管
//								float fneedAngle = osg::RadiansToDegrees(ftuiwanmove/(*iter).ftwbj);
//								if(m_bIsLeft == false)
//								{								
//									//生成弯管
//									ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,fneedAngle,true);
//								}
//								else if(m_bIsLeft == true)
//								{
//									//生成弯管
//									ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,fneedAngle,true);
//								}
//								//转动管子
//								PipeRotateX(CheTouInfo,PipeInfo,(*iter).ftwb,i);
//								break;
//							}
//						}
//					}
//				}
//			}
//			//小车到夹送位置
//			fmove_z = m_zlmTozlDis - fback;
//			XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//			dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//			fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//
//		}
//		else if(m_vproductionvalues[i].ftwlc == 2)//推弯流程1
//		{
//			//判断小车是否要后退到“夹送位置+下弯送料长度或送料脱模+2次送料(送料脱模+2次送料>下弯送料长度时）
//			float fback;//夹送位置
//			float flength = m_vproductionvalues[i].fmyt+m_vproductionvalues[i].fmyth;
//			if(flength > m_vproductionvalues[i+1].fmy )
//			{
//				fback = flength + m_vproductionvalues[i].fjy;
//			}
//			else
//			{
//				fback = m_vproductionvalues[i].fjy + m_vproductionvalues[i+1].fmy;
//			}
//
//			float fSafeDis;
//			std::vector<struct TWLC2>::iterator iter= m_vTwlc2values.begin();
//			for(;iter!=m_vTwlc2values.end();iter++)
//			{
//				//判断是否夹送
//				if(m_vproductionvalues[i].fjy == 0)//直接推弯，不到加送位置，不判断往复推弯位置
//				{
//					//先进弯
//					float fjinwanLength;
//					if(m_bIsLeft == false)
//					{
//						if(icount == 1)
//						{
//							frotate_y = (*iter).ftwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							m_lastangle = m_lastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),JiTouWanGuan1info,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//						else//在上一个弯的基础上继续进弯
//						{
//							frotate_y = (*iter).ftwc - m_fLastTwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							m_lastangle = m_lastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//
//							AnimationInfo tmpInfo;
//							tmpInfo = JiTouWanGuan2info;
//							tmpInfo.fStartTime = dtime;
//							tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
//							dtime = tmpInfo.fEndTime +m_fAnimationStopTime;
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),tmpInfo,PipeInfo,fzlR,(*iter).ftwc,true);
//						}				
//					}
//					else if(m_bIsLeft == true)
//					{
//						if(icount == 1)
//						{
//							frotate_y = (*iter).ftwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							m_Rlastangle = m_Rlastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),JiTouWanGuan1info,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//						else//在上一个弯的基础上继续进弯
//						{
//							frotate_y = (*iter).ftwc - m_fLastTwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							m_Rlastangle = m_Rlastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//
//							AnimationInfo tmpInfo;
//							tmpInfo = JiTouWanGuan2info;
//							tmpInfo.fStartTime = dtime;
//							tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
//							dtime = tmpInfo.fEndTime +m_fAnimationStopTime;
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),tmpInfo,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//					}
//					//小车同时移动：进弯弧长
//					fjinwanLength = (*iter).ftwc*fzlR*MYDEG_TO_RAD;
//					fmove_z = fjinwanLength;
//					XiaoCheAnimation(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,fmove_z);
//
//					//再直接推弯
//					//小车向前推
//					fmove_z = (*iter).ftwy - fjinwanLength;
//					XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					//车头同时转角
//					frotate_z = (*iter).ftwb;
//					CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//					//生成弯管
//					if(m_bIsLeft == true)//机头弯管在左边，要往右弯
//					{					
//						//生成弯管
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,RadiansToDegrees(((*iter).ftwy - fjinwanLength)/(*iter).ftwbj),true);
//					}
//					else if(m_bIsLeft == false)
//					{
//						//生成弯管;
//						ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,RadiansToDegrees(((*iter).ftwy - fjinwanLength)/(*iter).ftwbj),true);
//					}
//					//转动管子
//					PipeRotateX(CheTouInfo,PipeInfo,(*iter).ftwb,i);
//				}
//				else if(m_vproductionvalues[i].fjy > 0)//小车需要往复推弯，回到夹送位置
//				{
//					//小车先到往复推弯位置
//					fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//					XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//
//					float fjinwanLength = (*iter).ftwc*fzlR*MYDEG_TO_RAD;				
//					if(fjinwanLength < fSafeDis)
//					{
//						//回到往复推弯位置
//						fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//						XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//						fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//					}
//
//					//进弯
//					if(m_bIsLeft == false)
//					{
//						if(icount == 1)
//						{
//							frotate_y = (*iter).ftwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							m_lastangle = m_lastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),JiTouWanGuan1info,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//						else//在上一个弯的基础上继续进弯
//						{
//							frotate_y = (*iter).ftwc - m_fLastTwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							m_lastangle = m_lastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//
//							AnimationInfo tmpInfo;
//							tmpInfo = JiTouWanGuan2info;
//							tmpInfo.fStartTime = dtime;
//							tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
//							dtime = tmpInfo.fEndTime +m_fAnimationStopTime;
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),tmpInfo,PipeInfo,fzlR,(*iter).ftwc,true);
//						}				
//					}
//					else if(m_bIsLeft == true)
//					{
//						if(icount == 1)
//						{
//							frotate_y = (*iter).ftwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							m_Rlastangle = m_Rlastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),JiTouWanGuan1info,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//						else//在上一个弯的基础上继续进弯
//						{
//							frotate_y = (*iter).ftwc - m_fLastTwc;
//							JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							m_Rlastangle = m_Rlastangle + frotate_y;
//							m_fLastTwc = (*iter).ftwc;
//							icount++;
//
//							AnimationInfo tmpInfo;
//							tmpInfo = JiTouWanGuan2info;
//							tmpInfo.fStartTime = dtime;
//							tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
//							dtime = tmpInfo.fEndTime +m_fAnimationStopTime;
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),tmpInfo,PipeInfo,fzlR,(*iter).ftwc,true);
//						}
//					}
//					//小车同时移动：进弯弧长
//					fjinwanLength = (*iter).ftwc*fzlR*MYDEG_TO_RAD;
//					fmove_z = fjinwanLength;
//					XiaoCheAnimation(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,fmove_z);
//					fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//
//					//*****************往复推弯
//					float ftuiwanmove = (*iter).ftwy - fjinwanLength;
//					if(fSafeDis > ftuiwanmove)//推弯安全,直接推
//					{
//						//推弯
//						fmove_z = ftuiwanmove;
//						XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//						dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//						fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//						//转角
//						frotate_z = (*iter).ftwb;
//						CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//
//						//生成弯管
//						if(m_bIsLeft == true)//机头弯管在左边，要往右弯
//						{					
//							//生成弯管
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,RadiansToDegrees(ftuiwanmove/(*iter).ftwbj),true);
//						}
//						else if(m_bIsLeft == false)
//						{
//							//生成弯管;
//							ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,RadiansToDegrees(ftuiwanmove/(*iter).ftwbj),true);
//						}
//						//转动管子
//						PipeRotateX(CheTouInfo,PipeInfo,(*iter).ftwb,i);
//					}
//					else//不安全，往复推
//					{
//						while( ftuiwanmove > fSafeDis)//大于安全距离 就分几次推
//						{
//							//小车推安全距离
//							fmove_z = fSafeDis;
//							XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//							//生成弯管
//							float fSafeAngle = osg::RadiansToDegrees(fSafeDis/(*iter).ftwbj);
//							if(m_bIsLeft == false)
//							{
//								//生成弯管
//								ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,fSafeAngle,true);
//							}
//							else if(m_bIsLeft == true)
//							{
//								//生成弯管
//								ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,fSafeAngle,true);
//							}
//
//							//小车后退到往复推弯位置
//							fmove_z = m_zlmTozlDis - m_param.fWftwPos;
//							XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//							dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//							//剩余需要移动的距离
//							ftuiwanmove = ftuiwanmove -fSafeDis;
//							fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//							if(ftuiwanmove <= fSafeDis)//小车不需要再次后退
//							{
//								//小车移动剩余距离
//								fmove_z = ftuiwanmove;
//								XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//								dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//								fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//								//同时转角
//								frotate_z = (*iter).ftwb;
//								CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//								//生成弯管
//								float fneedAngle = osg::RadiansToDegrees(ftuiwanmove/(*iter).ftwbj);
//								if(m_bIsLeft == false)
//								{								
//									//生成弯管
//									ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,fneedAngle,true);
//								}
//								else if(m_bIsLeft == true)
//								{
//									//生成弯管
//									ShowCurvePipe(i,m_vPipeCenter[0],osg::Vec3(0.0,-1.0,0.0),XiaoCheinfo,PipeInfo,(*iter).ftwbj,fneedAngle,true);
//								}
//								//转动管子
//								PipeRotateX(CheTouInfo,PipeInfo,(*iter).ftwb,i);
//								break;
//							}
//						}
//					}
//				}
//			}
//			//小车到夹送位置 
//			fmove_z = m_zlmTozlDis - fback;
//			XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//			dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//			fSafeDis = m_zlmTozlDis - m_param.fDmaqDistance;
//		}
//
//		//*********************************************Y轴脱模
//		fmove_z = m_vproductionvalues[i].fmyt;
//		XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//		dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//		//生成直管
//		//2次Y轴为0&&需要回退，Y轴脱模的直管分段生成
//		if(m_vproductionvalues[i].fmyth == 0 )
//		{
//			float fshortpipe = m_vproductionvalues[i+1].fmy + m_vproductionvalues[i+1].fmyb - m_vproductionvalues[i].fmyt;//需要退回的直管长
//			if((m_vproductionvalues[i+1].fmy + m_vproductionvalues[i+1].fmyb - m_vproductionvalues[i].fmyt)<0)//退回多的直管
//			{
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,m_vproductionvalues[i].fmyt/*+fshortpipe*/,i+1);
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,-fshortpipe,i+1);
//
//			}
//			else
//			{
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,m_vproductionvalues[i].fmyt,i+1);
//
//			}
//		}
//		else if(m_vproductionvalues[i].fmyth > 0 )
//		{
//			ShowStraightPipe(vec3Center,osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,m_vproductionvalues[i].fmyt,i+1);
//
//		}
//
//
//		//*********************************************动作顺序（为下一弯做准备）
//		m_fzhuanjiao = 0;
//		int fdz_ge = ((int)m_vproductionvalues[i].fdz)%10;
//		int fds_shi = ((int)m_vproductionvalues[i].fdz)/10;
//		m_fNeedRightmove = 0;
//		m_fNeedRightmove2 = 0;
//		m_fAdvanceTuiWan = 0;
//
//		if(fds_shi == 0)
//		{
//			if(fdz_ge == 0)
//			{
//				//提前转角
//				m_fzhuanjiao = m_vproductionvalues[i].fbtm;
//				frotate_z = m_vproductionvalues[i].fbtm;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].fbtm,i);
//
//				//提前退弯
//				if(m_bIsLeft == false)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = m_lastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fRdlastangle = m_fRdlastangle - m_vproductionvalues[i].fctm;
//				}
//				else if(m_bIsLeft == true)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = m_Rlastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fLdlastangle = m_fLdlastangle - m_vproductionvalues[i].fctm;
//				}
//			}
//			else if(fdz_ge == 1)
//			{
//				//提前退弯
//				if(m_bIsLeft == false)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = m_lastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fRdlastangle = m_fRdlastangle - m_vproductionvalues[i].fctm;
//				}
//				else if(m_bIsLeft == true)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = m_Rlastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fLdlastangle = m_fLdlastangle - m_vproductionvalues[i].fctm;
//				}
//
//				//提前转角
//				m_fzhuanjiao = m_vproductionvalues[i].fbtm;
//				frotate_z = m_vproductionvalues[i].fbtm;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].fbtm,i);
//			}
//		}
//		else if(fds_shi == 1)
//		{
//			//X轴脱模 横向
//			if(m_bIsLeft == false)
//			{
//				fmove_x = m_vproductionvalues[i].fmz;
//				JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,-fmove_x);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				m_fNeedRightmove = m_vproductionvalues[i].fmz;
//			}
//			else if(m_bIsLeft == true)
//			{
//				fmove_x = m_vproductionvalues[i].fmz;
//				JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_x);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				m_fNeedRightmove = m_vproductionvalues[i].fmz;
//			}
//
//			if(fdz_ge == 0)
//			{
//				//提前转角
//				m_fzhuanjiao = m_vproductionvalues[i].fbtm;
//				frotate_z = m_vproductionvalues[i].fbtm;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].fbtm,i);
//
//				//提前退弯
//				if(m_bIsLeft == false)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = m_lastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fRdlastangle = m_fRdlastangle - m_vproductionvalues[i].fctm;
//				}
//				else if(m_bIsLeft == true)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = m_Rlastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fLdlastangle = m_fLdlastangle - m_vproductionvalues[i].fctm;
//				}
//			}
//			else if(fdz_ge == 1)
//			{
//				//提前退弯
//				if(m_bIsLeft == false)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = m_lastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fRdlastangle = m_fRdlastangle - m_vproductionvalues[i].fctm;
//				}
//				else if(m_bIsLeft == true)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = m_Rlastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fLdlastangle = m_fLdlastangle - m_vproductionvalues[i].fctm;
//				}
//
//				//提前转角
//				m_fzhuanjiao = m_vproductionvalues[i].fbtm;
//				frotate_z = m_vproductionvalues[i].fbtm;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].fbtm,i);
//			}
//		}
//		else if(fds_shi == 2)
//		{
//			if(fdz_ge == 0)
//			{
//				//提前转角
//				m_fzhuanjiao = m_vproductionvalues[i].fbtm;
//				frotate_z = m_vproductionvalues[i].fbtm;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].fbtm,i);
//
//				//提前退弯
//				if(m_bIsLeft == false)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = m_lastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fRdlastangle = m_fRdlastangle - m_vproductionvalues[i].fctm;
//				}
//				else if(m_bIsLeft == true)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = m_Rlastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fLdlastangle = m_fLdlastangle - m_vproductionvalues[i].fctm;
//				}
//			}
//			else if(fdz_ge == 1)
//			{
//				//提前退弯
//				if(m_bIsLeft == false)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = m_lastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fRdlastangle = m_fRdlastangle - m_vproductionvalues[i].fctm;
//				}
//				else if(m_bIsLeft == true)
//				{
//					//导轮2
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = m_Rlastangle - frotate_y;
//					//导论1 辅助轮
//					frotate_y = m_vproductionvalues[i].fctm;
//					JiTouWanGuan1AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//					m_fLdlastangle = m_fLdlastangle - m_vproductionvalues[i].fctm;
//				}
//
//				//提前转角
//				m_fzhuanjiao = m_vproductionvalues[i].fbtm;
//				frotate_z = m_vproductionvalues[i].fbtm;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,m_vproductionvalues[i].fbtm,i);
//			}
//
//			//X轴脱模 横向
//			if(m_bIsLeft == false)
//			{
//				fmove_x = m_vproductionvalues[i].fmz;
//				JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,-fmove_x);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				m_fNeedRightmove = m_vproductionvalues[i].fmz;
//			}
//			else if(m_bIsLeft == true)
//			{
//				fmove_x = m_vproductionvalues[i].fmz;
//				JiTouMoZuoAnimationFun(i,dtime,dtime+m_fAnimationTime,fmove_x);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				m_fNeedRightmove = m_vproductionvalues[i].fmz;
//			}
//
//		}
//
//		//************************************************************************动作流程的动作
//		int flc_ge = ((int)m_vproductionvalues[i].flc)%10;
//		int flc_shi = ((int)m_vproductionvalues[i].flc)/10;	
//		float songliao;
//		if(m_vproductionvalues[i].fmyth == 0)
//		{
//			songliao = m_vproductionvalues[i+1].fmy + m_vproductionvalues[i+1].fmyb - m_vproductionvalues[i].fmyt;
//		}
//		else if(m_vproductionvalues[i].fmyth > 0)
//		{
//			songliao = m_vproductionvalues[i].fmyth;
//		}
//
//		if(flc_shi == 0)
//		{
//			if((int)m_vproductionvalues[i].flc == 0)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;					
//
//			}
//			else if((int)m_vproductionvalues[i].flc == 1)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;		
//			}
//			else if((int)m_vproductionvalues[i].flc == 2)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//			}
//			else if((int)m_vproductionvalues[i].flc == 3)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//
//			}
//			else if((int)m_vproductionvalues[i].flc == 4)
//			{
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;					
//			}
//			else if((int)m_vproductionvalues[i].flc == 5)
//			{
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//
//
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//
//			}
//			else if((int)m_vproductionvalues[i].flc == 6)
//			{
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//			}
//			else if((int)m_vproductionvalues[i].flc == 7)
//			{
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);				
//			}
//			else if((int)m_vproductionvalues[i].flc == 8)
//			{
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);	
//
//				//退弯 机头弯管归0
//				if(m_bIsLeft == false)
//				{
//					frotate_y = m_lastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,-frotate_y);
//					m_lastangle = 0;
//
//					frotate_y = m_fRdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,frotate_y);
//					m_fRdlastangle = 0;
//				}
//				else if(m_bIsLeft == true)
//				{
//					frotate_y = m_Rlastangle;
//					JiTouWanGuan2AnimationFun(i,dtime,dtime+m_fAnimationTime,frotate_y);
//					m_Rlastangle = 0;
//
//					frotate_y = m_fLdlastangle;
//					JiTouWanGuan1AnimationFun(i,JiTouWanGuan2info.fStartTime,JiTouWanGuan2info.fEndTime,-frotate_y);
//					m_fLdlastangle = 0;
//				}
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//			}
//		}
//		else if(flc_shi == 1)
//		{
//			if((int)m_vproductionvalues[i].flc == 0)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);	
//
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//			}
//			else if((int)m_vproductionvalues[i].flc == 1)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);	
//
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;	
//			}
//			else if((int)m_vproductionvalues[i].flc == 2)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);	
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//			}
//			else if((int)m_vproductionvalues[i].flc == 3)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);	
//
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//			}
//			else if((int)m_vproductionvalues[i].flc == 4)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);	
//
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//			}
//			else if((int)m_vproductionvalues[i].flc == 5)
//			{
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);	
//			}
//			else if((int)m_vproductionvalues[i].flc == 6)
//			{
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,XiaoCheinfo.fStartTime,XiaoCheinfo.fEndTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//			}
//			else if((int)m_vproductionvalues[i].flc == 7)
//			{
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//			}
//			else if((int)m_vproductionvalues[i].flc == 8)
//			{
//				//转角
//				frotate_z = m_vproductionvalues[i+1].fmb - m_fzhuanjiao;
//				CheTouFun(i,dtime,dtime+m_fAnimationTime,frotate_z);
//				//转动管子
//				PipeRotateX(CheTouInfo,PipeInfo,(m_vproductionvalues[i+1].fmb - m_fzhuanjiao),i);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//
//
//				//送料
//				fmove_z =  songliao;
//				XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//				dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//				//生成直管
//				ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,songliao,i+1);
//			}
//		}
//		else if(flc_shi >= 2)
//		{
//
//		}
//
//
//		if(m_vproductionvalues[i].fmyth > 0)//动作流程完成后还需送料一次
//		{
//			fmove_z = m_vproductionvalues[i+1].fmy + m_vproductionvalues[i+1].fmyb - m_vproductionvalues[i].fmyt - m_vproductionvalues[i].fmyth;
//			XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//			dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//			//生成直管
//			ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,fmove_z,i+1);
//		}
//
//
//		//最后送尾段料
//		if(i == m_param.imn - 1)
//		{
//			//送料
//			fmove_z = m_vproductionvalues[i+1].fmy;
//			XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//			dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//			//生成直管
//			ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),XiaoCheinfo,PipeInfo,i,m_vproductionvalues[i+1].fmy,i+1);
//
//			//小车回到原点位置
//			fmove_z = m_zlmTozlDis + m_param.fYdPos;
//			XiaoCheAnimation(i,dtime,dtime+m_fAnimationTime,fmove_z);
//			dtime = dtime + m_fAnimationTime + m_fAnimationStopTime;
//		}
//	}
//
//	float ftmpmax = 0;
//	for(int k=0;k<m_fallR.size();k++)
//	{
//		if(m_fallR[k]>ftmpmax)
//			ftmpmax = m_fallR[k];
//	}
//
//	m_sceneModel->setZ(ftmpmax);
//
//	//连接管子矩阵
//	if(m_vPipes.size() >= 2)
//	{
//		for (int i = m_vPipes.size()-1;i>=1;--i)
//		{
//			m_vPipes[i].GetMoveMatrix()->addChild(m_vPipes[i-1].GetRotateMatrix());
//		}
//		osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
//		pipefatherMatrix->addChild(m_vPipes[m_vPipes.size() -1].GetRotateMatrix());//绑定管子最后一截到管子矩阵根节点上
//		((osg::Group*)nodestl)->addChild(pipefather);
//	}
//
//	//设置管子动画
//	for (int i= 0; i < m_vMatrixAnimation.size();++i)
//	{
//		std::vector<AnimationInfo> tempVector;
//		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vMatrixAnimation[i].first;
//
//		tempVector.push_back(m_vMatrixAnimation[i].second);
//		osg::ref_ptr<osg::Geode> group;
//		for (int j= 0; j < m_vPipeBackMatrixAnimation.size();++j)
//		{
//			if(matrix == m_vPipeBackMatrixAnimation[j].first)
//			{
//				tempVector.push_back(m_vPipeBackMatrixAnimation[j].second);
//				group = m_vPipeBackMatrixAnimation[j].second.group;
//			}
//		}
//
//		int pipeSeg = m_vMatrixAnimation[i].second.pipeSeg;
//		int pipeSetTw = m_vMatrixAnimation[i].second.pipeSegTw;
//		double s = m_vMatrixAnimation[i].second.fStartTime;
//		double e = m_vMatrixAnimation[i].second.fEndTime;
//		bool istw = m_vMatrixAnimation[i].second.isTW;
//
//		if(tempVector.size()>1)
//		{
//			matrix->setUpdateCallback(getBackPipePathCallBack(tempVector));
//			PipeHide* hide = new PipeHide(tempVector.crbegin()->fEndTime,group);
//			matrix->addUpdateCallback(hide);
//		}
//		else
//		{
//			matrix->setUpdateCallback(getPipePathCallBack(tempVector));
//		}
//
//		PipeInfoCallback* tmp = new PipeInfoCallback(s,e,pipeSeg,pipeSetTw,istw,this,m_bAutoPlaning);
//		matrix->addUpdateCallback(tmp);
//	}
//
//	osg::ref_ptr<osg::MatrixTransform> jitouwangaun1 = m_sceneModel->getJiTouWanGuan1FatherMatrix();
//	jitouwangaun1->setUpdateCallback(getJiTouWanGuan1PathCallback(m_vJiTouWanGuan1Animation));
//
//
//	osg::ref_ptr<osg::MatrixTransform> jitouwangaun2 = m_sceneModel->getJiTouWanGuan2FatherMatrix();
//	jitouwangaun2->setUpdateCallback(getJiTouWanGuan2PathCallback(m_vJiTouWanGuan2Animation));
//
//	//osg::ref_ptr<osg::MatrixTransform> jitouwangaun2 = m_sceneModel->getJiTouWanGuan2FatherMatrix();
//	//std::vector<osg::ref_ptr<osg::AnimationPathCallback>> tmp2 = getJiTouWanGuan2PathCallback(m_vJiTouWanGuan2Animation);
//	//for(int i=0;i<tmp2.size();i++)
//	//{
//	//	if(i == 0)
//	//	{
//	//		jitouwangaun2->setUpdateCallback(tmp2[0]);
//	//	}
//	//	else
//	//	{
//	//		jitouwangaun2->addUpdateCallback(tmp2[i]);
//	//	}
//	//}
//
//
//	osg::ref_ptr<osg::MatrixTransform> xiaoche = m_sceneModel->getXiaoCheFatherMatrix();
//	xiaoche->setUpdateCallback(getCarPathCallback(m_vXiaoCheAnimation));
//
//	osg::ref_ptr<osg::MatrixTransform> chetou = m_sceneModel->getCheTouFatherMatrix();
//	chetou->setUpdateCallback(getCheTouCallback(m_vCheTouAnimation));
//
//	osg::ref_ptr<osg::MatrixTransform> zhualiaomo = m_sceneModel->getZhuLiaoMoFatherMatrix();
//	zhualiaomo->setUpdateCallback(getZhuaLiaoMoPathCallback(m_vZhuaLiaoMoAnimation));
//
//	osg::ref_ptr<osg::MatrixTransform> jitoumozuo = m_sceneModel->getJiTouMoZuoFatherMatrix();
//	jitoumozuo->setUpdateCallback(getJiTouMoZuoPathCallBack(m_vJiTouMoZuoAnimation));
//
//	osg::ref_ptr<osg::MatrixTransform> jitoudizuo = m_sceneModel->getJiTouDiZuoFatherMatrix();
//	jitoudizuo->setUpdateCallback(getJiTouDiZuoPathCallBack(m_vJiTouDiZuoAnimation));
//
//	startTimer(dtime);
//	SetCollisionDetectParts();
//
//	m_sceneView->UpdateView();
//}

//void MainWindow::PlayAnimation()
//{
//	//刷新颜色
//	m_sceneModel->GetHudtext()->ResetAllColor();
//	//使能暂停键
//	ui.actionLight->setEnabled(true);
//	//取消暂停选中状态
//	emit sigPartsPause(true);
//	ui.actionLight->setChecked(false);
//
//	//生成长直管
//	if(m_showLongStraightPipe == 0)
//	{
//		CreatLongStraightPipe();
//	}
//	else
//	{
//		m_showLongStraightPipe++;
//	}
//
//
//	osg::ref_ptr<osg::Group> pipefather = m_sceneModel->getPipeFather();//管子父节点
//	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrix = m_sceneModel->getPipeFatherMatrix();//管子父节点下的变化矩阵
//	//管子动画
//	//连接管子矩阵
//	if(m_vPipes.size() >= 2)
//	{
//		for (int i = m_vPipes.size()-1;i>=1;--i)
//		{
//			m_vPipes[i].GetMoveMatrix()->addChild(m_vPipes[i-1].GetRotateMatrix());
//
//		}
//		osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
//		pipefatherMatrix->addChild(m_vPipes[m_vPipes.size() -1].GetRotateMatrix());//绑定管子最后一届到管子矩阵根节点上
//		((osg::Group*)nodestl)->addChild(pipefather);
//	}
//
//
//	//设置管子动画
//	for (int i= 0; i < m_vMatrixAnimation.size();++i)
//	{
//		std::vector<AnimationInfo> tempVector;
//		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vMatrixAnimation[i].first;
//
//		tempVector.push_back(m_vMatrixAnimation[i].second);
//		osg::ref_ptr<osg::Geode> group;
//		for (int j= 0; j < m_vPipeBackMatrixAnimation.size();++j)
//		{
//			if(matrix == m_vPipeBackMatrixAnimation[j].first)
//			{
//				tempVector.push_back(m_vPipeBackMatrixAnimation[j].second);
//				group = m_vPipeBackMatrixAnimation[j].second.group;
//			}
//		}
//
//		int pipeSeg = m_vMatrixAnimation[i].second.pipeSeg;
//		double s = m_vMatrixAnimation[i].second.fStartTime;
//		double e = m_vMatrixAnimation[i].second.fEndTime;
//		if(tempVector.size()>1)
//		{
//			matrix->setUpdateCallback(getBackPipePathCallBack(tempVector));
//			PipeHide* hide = new PipeHide(tempVector.crbegin()->fEndTime,group);
//			matrix->addUpdateCallback(hide);
//		}
//		else
//		{
//			matrix->setUpdateCallback(getPipePathCallBack(tempVector));
//
//		}
//
//		PipeInfoCallback* tmp = new PipeInfoCallback(s,e,pipeSeg);
//		matrix->addUpdateCallback(tmp);
//	}
//
//
//
//
//
//	//模型动画
//	for(int i=1;i<m_vSTLs.size();i++)//0为根节点scene
//	{
//		osg::ref_ptr<osg::MatrixTransform> matrix = m_vSTLs[i]->GetModelMatrix();
//		std::vector<AnimationInfo> Vmove = m_vSTLs[i]->GetVMoveAnimation();
//		std::vector<AnimationInfo> Vrotate = m_vSTLs[i]->GetVRotateAnimation();
//		if(Vmove.size()!=0)
//		{
//			matrix->setUpdateCallback(getModelMoveCallBack(Vmove));
//
//		}
//		if(Vrotate.size()!=0)
//		{
//			matrix->addUpdateCallback(getModelRotateCallBack(Vrotate));
//
//		}
//
//	}
//
//	startTimer(dtime);
//	SetCollisionDetectParts();
//
//	m_sceneView->UpdateView();
//}

std::vector<STL*> MainWindow::GetSTLsByGroupName(QString strGName)
{
	std::map<QString,std::vector<STL*>>::iterator iter = m_MapGroupToSTL.find(strGName);
	if (iter != m_MapGroupToSTL.end())
	{
		if (!iter->second.empty())
		{
			return iter->second;
		}
	}
}

std::vector<STL*> MainWindow::GetGroupNodeByGroupName(QString strGName)
{
	std::vector<STL*> stls;
	//for(int i=0;i<m_Groups.size();i++)
	//{
	//	if(m_Groups[i]->GetGroupName() == strGName)
	//	{
	//		stls = m_Groups[i]->GetCurGroupSTLs();
	//	}
	//}

	for (size_t i = 0; i < m_vNodes.size();++i)
	{
		if (m_vNodes[i]->GetSTLName()==strGName)
		{
			stls.push_back(m_vNodes[i]);
			return stls;
		}
	}

	return stls;
}

std::vector<GroupChildren*> MainWindow::GetGroupNodesByGroupName(QString name)
{
	std::vector<GroupChildren*> nodes;
	std::map<QString,GroupChildren>::iterator iter = m_MapGroupToGroupChildren.find(name);
	if (iter!=m_MapGroupToGroupChildren.end())
	{
		nodes.push_back(&(iter->second));

		//for (int i = 0; i < iter->second.vNodes.size();++i)
		//{
		//	std::map<QString,GroupChildren>::iterator iter0 = m_MapGroupToGroupChildren.find(iter->second.vNodes[i]);
		//	if (iter0 != m_MapGroupToGroupChildren.end())
		//	{
		//		nodes.push_back(&(iter0->second));
		//	}
		//}

		return nodes;

	}
	else
	{
		return nodes;
	}
}

std::vector<STL*> MainWindow::GetSideSTLsBySideName(std::vector<QString> strSName)
{
	std::vector<STL*> stls;
	for(size_t i=0;i<m_vSTLs.size();i++)
	{
		for (size_t j=0;j<strSName.size();j++)
		{
			if(m_vSTLs[i]->GetSTLName() == strSName[j])
			{
				stls.push_back(m_vSTLs[i]);
			}
		}

	}
	return stls;
}
#if 0

void MainWindow::SetPlayNewNew()
{
	char safedis[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, SAFE_DIS, 3, safedis);
	m_safeDis = atof(safedis);

	//清零
	for(int i=0;i<m_vSTLs.size();i++)//
	{
		m_vSTLs[i]->ClearMoveAnimation();
		m_vSTLs[i]->ClearRotateAnimation();
		m_vSTLs[i]->ResetAbsoluteAngle();

		// 恢复主夹相关变换矩阵
		std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = m_vSTLs[i]->GetTransMatrix();
		if (!trans.empty())
		{
			osg::ref_ptr<osg::MatrixTransform> moveMatrix = m_vSTLs[i]->GetModelMoveMatrix();
			osg::ref_ptr<osg::MatrixTransform> rotateMatrix = m_vSTLs[i]->GetModelSecondRotateMatrix();

			moveMatrix->removeChild(trans[trans.size()-1]);
			trans[0]->removeChild(rotateMatrix);
			moveMatrix->addChild(rotateMatrix);

			trans.clear();
		}
	}

	// 机头弯管初始角度为90度
	m_dJitouAngle = 0.0;

	ResetAllModelAbsPos();

	//clear vPipe
	osg::ref_ptr<osg::Group> pipefather = m_sceneModel->getPipeFather();//管子父节点
	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrix = m_sceneModel->getPipeFatherMatrix();//管子父节点下的变化矩阵
	if(m_vPipes.size() >= 2)
	{
		for (int i = m_vPipes.size()-1;i>=1;--i)
		{
			m_vPipes[i].GetMoveMatrix()->removeChild(m_vPipes[i-1].GetRotateMatrix());
		}
		osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
		pipefatherMatrix->removeChild(m_vPipes[m_vPipes.size() -1].GetRotateMatrix());//绑定管子最后一届到管子矩阵根节点上
		((osg::Group*)nodestl)->removeChild(pipefather);

		m_vPipes.clear();
		m_sceneModel->ResetPipeIndex();
	}

	if (!m_vMatrixAnimation.empty())
	{
		m_vMatrixAnimation.clear();
	}

	if (!m_vMatrixAnimations.empty())
	{
		m_vMatrixAnimations.clear();
	}

	if (!m_vColorInfo.empty())
	{
		m_vColorInfo.clear();
	}

	if (!m_vLongStraightPipeMaxtrixAnimation.empty())
	{
		m_vLongStraightPipeMaxtrixAnimation.clear();
	}

	if (!m_vLongStraightPipAnimation.empty())
	{
		m_vLongStraightPipAnimation.clear();
	}

	if (!m_mapMatrixInstead.empty())
	{
		m_mapMatrixInstead.clear();
	}

	//osg::ref_ptr<osg::Group> LongStraightpipefather = m_sceneModel->getLongStraightPipeFatherNode();//stl->Pipenode->trans
	osg::ref_ptr<osg::PositionAttitudeTransform> LongStraightpipefatherTrans = m_sceneModel->getLongStraightPipeTrans();

	//osg::Node* nodestl2 = m_sceneModel->getNode("NodeSTL");
	//if (m_pstraiPipe!=NULL)
	//{
	//	LongStraightpipefatherTrans->removeChild(m_pstraiPipe);
	//}
	//((osg::Group*)nodestl2)->removeChild(LongStraightpipefather);
	if (m_showLongStraightPipe ==0 )
	{
		CreatLongStraightPipe();
		//m_pstraiPipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getLongClipPlane(), osg::StateAttribute::ON);
		m_showLongStraightPipe++;
	}
	else
	{
		m_showLongStraightPipe++;
	}

	m_pstraiPipe->setNodeMask(0);

	//生成一次长直管
	float _x,_y,_z;	
	char ch_x[32],ch_y[32],ch_z[32];
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_1, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_1, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_1, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenter.push_back(osg::Vec3(_x,_y,_z));

	dtime = 1;//整个动画开始时间
	//m_v = m_pCollisionDlg->GetAnimatiuonSpeed();
	m_v = m_pSpeedDlg->GetCurSpeed();
	m_fAnimationTime = 1/m_v;//动画时间 
	m_fAnimationStopTime = 1/m_v;//间隔时间

	AnimationInfo tmpcolorInfo;
	AnimationInfo tmpTWcolorInfo;

	std::vector<std::vector<Process>>::iterator iters = m_vAllStepvProcess.begin();
	for(;iters!=m_vAllStepvProcess.end();iters++)
	{

		//同一个索引组成员 用同一个时间
		AnimationInfo tmpInfo;
		tmpInfo.fStartTime = dtime;
		tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
		dtime = tmpInfo.fEndTime + m_fAnimationStopTime;

		if (fabs(tmpInfo.fStartTime-10.2)<0.001)
		{
			int m = 0;
		}

		for(int i=0;i<(*iters).size();i++)//遍历每组成员，执行每一个动作
		{
			Process curProcess = (*iters)[i];
			QString curCode = curProcess.strCode;//功能码

			QString tmpGroupName;//组名
			int imovetype = -1;//平移 旋转
			int iAxis = -1;//轴
			int iMoShi = -1;//相对或绝对位置
			int dir;//为0，正值为负半轴方向
			osg::Vec3 vectmpRotateCenter;
			std::vector<STL*> tmpSTLs;//组模型				
			float fcurvalue = curProcess.fvalue;//动画值
			int iRotatePipe = curProcess.iPipeRotate;
			int iCreatPipe = curProcess.iCreatPipe;
			int iShape = curProcess.iCurveOrStraight;
			float fCurveR = curProcess.fcurveR;

			tmpInfo.strProcess = curProcess.strProcess;
			std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==4)  // 沿着机头弯管垂直运动
			{
				tmpInfo.isZhujiaZ = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaZ = false;
			}

			iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==5)  // 主夹沿着机头弯管垂直运动
			{
				tmpInfo.isZhujiaX = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaX = false;
			}

			iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==3)  // 主夹沿着机头弯管运动
			{
				tmpInfo.isZhujiaR = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaR = false;
			}

			tmpInfo.fdir = curProcess.fDir;

			if(curCode == "COLOR")
			{
				tmpcolorInfo.fStartTime = tmpInfo.fStartTime;
				tmpcolorInfo.fEndTime = tmpInfo.fEndTime;
				tmpcolorInfo.pipeSeg = curProcess.iColor-1;
				tmpcolorInfo.isTW = false;
				m_vColorInfo.push_back(tmpcolorInfo);
			}
			else if (curCode == "TWCOLOR")
			{
				tmpcolorInfo.fStartTime = tmpInfo.fStartTime;
				tmpcolorInfo.fEndTime = tmpInfo.fEndTime;
				tmpcolorInfo.pipeSeg = curProcess.iTwColor-1;
				tmpcolorInfo.isTW = true;
			}
			else if (curCode == "CREATE")
			{
				//m_vMatrixAnimation.
				AnimationInfo tmpStraightInfo;
				tmpStraightInfo.fStartTime = tmpInfo.fStartTime;
				osg::Vec3 center3d = m_vPipeCenter[0];
				//m_pstraiPipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,m_param.fPipeLength,center3d,osg::Vec3(0,0,-1));
				std::pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo> tmppair = make_pair(LongStraightpipefatherTrans,tmpStraightInfo);
				m_vLongStraightPipeMaxtrixAnimation.push_back(tmppair);
				//m_vMatrixAnimation.push_back(tmppair);
			}
			else
			{
				//****************获取运动属性 和 模型数组
				for(int u=0;u<m_vGcodeFileValues.size();u++)
				{
					if(m_vGcodeFileValues[u].strGCode == curCode)
					{
						// 更新机头弯管角度
						std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.find(curCode);
						//if (curCode==/*"G30"*/"G20")
						if (iter!= m_mapGcodeSpecialMove.end() && iter->second==1)
						{
							//m_dJitouAngle = fcurvalue;
							m_dJitouAngle += fcurvalue;
						}
						//else if (curCode==/*"G3"*/"G21")
						else if (iter!= m_mapGcodeSpecialMove.end() && iter->second==2)
						{
							// m_dJitouAngle += fcurvalue;
							m_dJitouAngle = fcurvalue;
						}

						//if(curCode == "G9" || curCode == "G0")
						//{
						//	int aa;
						//	aa =1;
						//}

						tmpGroupName = m_vGcodeFileValues[u].strGroupName;
						tmpSTLs = GetGroupNodeByGroupName(tmpGroupName);
						if(tmpSTLs.empty())
						{
							/*QString strWarning;
							strWarning = strWarning.fromLocal8Bit("存在组成员为空!");
							QMessageBox::warning(this,"",strWarning);
							return;*/
						}
						imovetype = m_vGcodeFileValues[u].iMoveType;
						iAxis = m_vGcodeFileValues[u].iAxis;//轴
						iMoShi = m_vGcodeFileValues[u].irelative;
						dir = m_vGcodeFileValues[u].iDir;//为0，正值为负半轴方向
						if(iMoShi == 0)//相对
						{
							if(dir == 0)
							{
								fcurvalue = -fcurvalue;
							}
							else if (dir == 1)
							{

							}
						}
						else if (iMoShi==1) // 绝对
						{
							if(dir == 0)
							{
								fcurvalue = -fcurvalue;
							}
							else if (dir == 1)
							{

							}
						}

						if(imovetype == 1)
						{
							vectmpRotateCenter = m_vGcodeFileValues[u].vecRotateCenter;
						}
						break;
					}
				}

				//****************添加模型动画
				// if(tmpSTLs.size()>0)
				float value = fcurvalue;
				for (int m = 0; m < tmpSTLs.size();++m)
				{			
					fcurvalue = value;
					if(imovetype == 0)//平移
					{
						if(iAxis == 0)//X轴
						{						
							if(iMoShi == 1) // 绝对
							{
								if(dir == 1)//顺时针转
								{
									float aimx = fcurvalue;
									osg::Vec3 curPos = tmpSTLs[m]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									fcurvalue = aimx - curPos.x();
									//for(int k=0;k<tmpSTLs.size();k++)
									//{
									tmpSTLs[m]->SetSTLAbsolutePosX(aimx);
									//}
								}
							}
							else if(iMoShi == 0)
							{
								float cur_x = tmpSTLs[m]->GetSTLAbsolutePosX();
								float aimx = cur_x + fcurvalue;
								//for(int k=0;k<tmpSTLs.size();k++)
								//{
								tmpSTLs[m]->SetSTLAbsolutePosX(aimx);
								//}
							}

							tmpInfo.translate.x() = fcurvalue;
							tmpInfo.translate.y() = 0;
							tmpInfo.translate.z() = 0;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;

						}
						else if(iAxis == 1)//Y轴
						{
							if(iMoShi == 1) // 绝对位置
							{
								if(dir == 1) // 与当前坐标轴方向相同
								{
									float aimy = fcurvalue;
									osg::Vec3 curPos = tmpSTLs[m]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									fcurvalue = aimy - curPos.y();
									//for(int k=0;k<tmpSTLs.size();k++)
									//{
									tmpSTLs[m]->SetSTLAbsolutePosY(aimy);
									//}
								}
								else if (dir==0) //与当前坐标轴方向相反
								{
									float aimy = -fcurvalue; // 坐标位置先按照流程文件设置，前面反过一次
									osg::Vec3 curPos = tmpSTLs[m]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									fcurvalue = aimy - curPos.y();
									//for(int k=0;k<tmpSTLs.size();k++)
									//{
									tmpSTLs[m]->SetSTLAbsolutePosY(aimy);
									//}

									fcurvalue *= -1.0;
								}

							}
							else if(iMoShi == 0)
							{
								float cur_y = tmpSTLs[m]->GetSTLAbsolutePosY();
								float aimy = cur_y + fcurvalue;
								//for(int k=0;k<tmpSTLs.size();k++)
								//{
								tmpSTLs[m]->SetSTLAbsolutePosY(aimy);
								//}
							}

							tmpInfo.translate.x() = 0;
							tmpInfo.translate.z() = 0;
							tmpInfo.translate.y() = fcurvalue;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;
						}
						else if(iAxis == 2)//Z轴
						{
							if(iMoShi == 1)
							{
								if(dir == 1)
								{
									if (curProcess.iDirectly == 11)//判读小车是否往复
									{
										osg::Vec3 curPos = tmpSTLs[m]->GetSTLAbsolutePos();//此组模型的当前绝对位置
										float pipe_z = m_vPipeCenter[0].z();
										float curdis = fabs(curPos.z()) - fabs(pipe_z);
										if (curdis <= m_safeDis)
										{
											float aimz = fcurvalue;
											fcurvalue = aimz - curPos.z();
											//for(int k=0;k<tmpSTLs.size();k++)
											//{
											tmpSTLs[m]->SetSTLAbsolutePosZ(aimz);
											//}
										}
										else
										{
											fcurvalue = 0;
										}
									}
									else
									{
										if (curProcess.iDirectly == 12)
										{
											float aimz = -fcurvalue;
											osg::Vec3 curPos = tmpSTLs[m]->GetSTLAbsolutePos();//此组模型的当前绝对位置
											//float z = curPos.z();
											//LogInfo(QString(("%d is cur pos")).arg(z));
											fcurvalue = aimz - curPos.z();
											//for(int k=0;k<tmpSTLs.size();k++)
											//{
											tmpSTLs[m]->SetSTLAbsolutePosZ(aimz);
											//}
										}
										else
										{
											float aimz = fcurvalue;
											osg::Vec3 curPos = tmpSTLs[m]->GetSTLAbsolutePos();//此组模型的当前绝对位置
											//float z = curPos.z();
											//LogInfo(QString(("%d is cur pos")).arg(z));
											fcurvalue = aimz - curPos.z();
											//for(int k=0;k<tmpSTLs.size();k++)
											//{
											tmpSTLs[m]->SetSTLAbsolutePosZ(aimz);
											//}
										}	
									}
								}
							}
							else if(iMoShi == 0)
							{
								float cur_z = tmpSTLs[m]->GetSTLAbsolutePosZ();
								float aimz = cur_z + fcurvalue;
								//for(int k=0;k<tmpSTLs.size();k++)
								//{
								tmpSTLs[m]->SetSTLAbsolutePosZ(aimz);
								//}
							}

							tmpInfo.translate.x() = 0;
							tmpInfo.translate.y() = 0;
							tmpInfo.translate.z() = fcurvalue;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;
						}
						else if(iAxis == 3)
						{
							// 沿着机头弯管运动
							if(iMoShi == 1)
							{
								if(dir == 1)//顺时针转
								{
									double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
									double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
									if (m_bOnlyLeft)
									{
										// 纯左侧加工
										x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
										z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
									}
									else
									{
										x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
										z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
									}

									//float aimx = fcurvalue;
									osg::Vec3 curPos = tmpSTLs[m]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									//fcurvalue = aimx - curPos.x();
									//for(int k=0;k<tmpSTLs.size();k++)
									//{
									tmpSTLs[m]->SetSTLAbsolutePosX(x);
									tmpSTLs[m]->SetSTLAbsolutePosZ(z);
									//}
								}
							}
							else if(iMoShi == 0)
							{
								double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
								double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
								if (m_bOnlyLeft)
								{
									// 纯左侧加工
									x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
									z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
								}
								else
								{
									x = fcurvalue*cos(fabs(m_dJitouAngle*3.1415926/180.0));
									z = fcurvalue*sin(fabs(m_dJitouAngle*3.1415926/180.0));
								}
								float cur_x = tmpSTLs[m]->GetSTLAbsolutePosX();
								float cur_z = tmpSTLs[m]->GetSTLAbsolutePosZ();
								float aimx = cur_x + x;
								float aimz = cur_z + z;
								//for(int k=0;k<tmpSTLs.size();k++)
								//{
								//tmpSTLs[m]->SetSTLAbsolutePosX(aimx);
								//tmpSTLs[m]->SetSTLAbsolutePosZ(aimz);
								//}
							}

							double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
							double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
							if (m_bOnlyLeft)
							{
								// 纯左侧加工
								x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
								z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
							}
							else
							{
								x = fabs(fcurvalue)*cos(fabs(m_dJitouAngle*3.1415926/180.0));
								z = fabs(fcurvalue)*sin(fabs(m_dJitouAngle*3.1415926/180.0));
							}
							tmpInfo.translate.x() = x;

							tmpInfo.translate.y() = 0.0;
							tmpInfo.translate.z() = z;

							tmpInfo.rotateX = 0.0;
							tmpInfo.rotateY = 0.0;
							tmpInfo.rotateZ = 0.0;
						}

						//for(int k=0;k<tmpSTLs.size();k++)
						//{
						tmpInfo.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();
						tmpSTLs[m]->AddMoveAnimation(tmpInfo);

#if 0
						if(tmpSTLs[m]->GetSTLName()==QString::fromLocal8Bit("zj1.stl") /*|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("dj2.1.stl")*/)
						{
							QString information;
							AnimationInfo info = tmpInfo;

							information.append("start time ");
							information.append(QString::number(info.fStartTime));
							LogInfo(information);
							information.clear();

							information.append("end time ");
							information.append(QString::number(info.fEndTime));
							LogInfo(information);
							information.clear();

							information.append("pos x ");
							information.append(QString::number(info.translate.x()));
							LogInfo(information);
							information.clear();

							information.append("pos y ");
							information.append(QString::number(info.translate.y()));
							LogInfo(information);
							information.clear();

							information.append("pos z ");
							information.append(QString::number(info.translate.z()));
							LogInfo(information);
							information.clear();

							information.clear();
							information.append("step ");
							information.append(QString::number(curProcess.istep));
							information.append(curProcess.strCode);
							LogInfo(information);
						}
#endif 
						//}
					}
					else if(imovetype == 1)//旋转
					{
						tmpInfo.center3d = vectmpRotateCenter;

						if(iMoShi == 1)
						{
							if(dir == 1)
							{
								float aim = fcurvalue;

								float curAngle = tmpSTLs[m]->GetSTLAbsoluteAngle();
								fcurvalue = aim - curAngle;
								//for(int k=0;k<tmpSTLs.size();k++)
								//{
								tmpSTLs[m]->SetSTLAbsoluteAngle(aim);
								//}						
							}
						}
						else if(iMoShi == 0)
						{					
							float curAngle = tmpSTLs[m]->GetSTLAbsoluteAngle();
							float absoluteAngle = curAngle + fcurvalue;
							//for(int k=0;k<tmpSTLs.size();k++)
							//{
							tmpSTLs[m]->SetSTLAbsoluteAngle(absoluteAngle);
							//}
						}

						if(iAxis == 0)//X轴
						{
							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateX = fcurvalue/2;
								m_DivideInfo1.fdir = tmpInfo.fdir;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateX = fcurvalue/2;
								m_DivideInfo2.fdir = tmpInfo.fdir;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;
								tmpInfo.rotateX = fcurvalue;
								tmpInfo.rotateY = 0;
								tmpInfo.rotateZ = 0; 

								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 1)//Y轴
						{

							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateY = fcurvalue/2;
								m_DivideInfo1.fdir = tmpInfo.fdir;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateY = fcurvalue/2;
								m_DivideInfo2.fdir = tmpInfo.fdir;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;

								tmpInfo.rotateX = 0;
								tmpInfo.rotateZ = 0;
								tmpInfo.rotateY = fcurvalue;

								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 2)//Z轴
						{
							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateZ = fcurvalue/2;
								m_DivideInfo1.fdir = tmpInfo.fdir;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateZ = fcurvalue/2;
								m_DivideInfo2.fdir = tmpInfo.fdir;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;
								tmpInfo.rotateY = 0;
								tmpInfo.rotateX = 0;
								tmpInfo.rotateZ = fcurvalue;
								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 3)
						{
							// 相对于机头弯管的轴
							int a = 0;
						}

						//for(int k=0;k<tmpSTLs.size();k++)
						{
							//判断中心是否已存在
							std::vector<AnimationInfo> vtmpRotateInfo =tmpSTLs[m]->GetVRotateAnimation();
							if(vtmpRotateInfo.size() == 0)
							{
								if(m_bIsDivideTwoTimes == false)
								{
									tmpInfo.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();
									tmpSTLs[m]->AddRotateAnimation(tmpInfo);
								}
								else if(m_bIsDivideTwoTimes == true)
								{
									m_DivideInfo1.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();
									m_DivideInfo2.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();

									tmpSTLs[m]->AddRotateAnimation(m_DivideInfo1);
									tmpSTLs[m]->AddRotateAnimation(m_DivideInfo2);								
								}
							}
							else
							{
								vtmpRotateInfo =tmpSTLs[m]->GetVRotateAnimation();
								std::vector<AnimationInfo>::iterator itrs = vtmpRotateInfo.begin();
								for(;itrs!=vtmpRotateInfo.end();itrs++)
								{
									if(itrs->center3d == tmpInfo.center3d)
									{
										break;
									}
								}
								if(itrs != vtmpRotateInfo.end())//存在
								{

									//tmpSTLs[k]->AddRotateAnimation(tmpInfo);
									if(m_bIsDivideTwoTimes == false)
									{
										tmpInfo.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();

										tmpSTLs[m]->AddRotateAnimation(tmpInfo);
									}
									else if(m_bIsDivideTwoTimes == true)
									{
										m_DivideInfo1.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();
										m_DivideInfo2.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();

										tmpSTLs[m]->AddRotateAnimation(m_DivideInfo1);
										tmpSTLs[m]->AddRotateAnimation(m_DivideInfo2);								
									}

								}
								else if(itrs==vtmpRotateInfo.end())
								{
									//tmpSTLs[k]->AddSecondRotateAnimation(tmpInfo);
									if(m_bIsDivideTwoTimes == false)
									{
										tmpInfo.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();

										tmpSTLs[m]->AddSecondRotateAnimation(tmpInfo);

									}
									else if(m_bIsDivideTwoTimes == true)
									{
										m_DivideInfo1.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();
										m_DivideInfo2.absolutePos = tmpSTLs[m]->GetSTLAbsolutePos();

										tmpSTLs[m]->AddSecondRotateAnimation(m_DivideInfo1);
										tmpSTLs[m]->AddSecondRotateAnimation(m_DivideInfo2);								
									}
								}
							}
						}
					}
				}
#if 0

				for (int a = 0; a < tmpSTLs.size();++a)
				{
					if (tmpSTLs[a]->GetSTLName()=="zuodj.stl")
					{
						QString info("zuodj pos x ");
						info.append(QString::number(tmpSTLs[a]->GetSTLAbsolutePosX()));
						LogInfo(info);

						info.clear();
						info.append("step ");
						info.append(QString::number(curProcess.istep));
						info.append(curProcess.strCode);
						LogInfo(info);
					}

					if (tmpSTLs[a]->GetSTLName()=="zj1.stl")
					{
						QString info("zj1 pos x ");
						info.append(QString::number(tmpSTLs[a]->GetSTLAbsolutePosX()));
						LogInfo(info);

						info.clear();
						info.append("step ");
						info.append(QString::number(curProcess.istep));
						info.append(curProcess.strCode);
						LogInfo(info);
					}
				}
#endif 

				//****************管子动画
				if(iRotatePipe == 1)//转动管子
				{
					PipeRotateX(tmpInfo,tmpcolorInfo,fcurvalue,curProcess.iNthCureve-2);
				}
				else if(iRotatePipe == 0)
				{
					if(iCreatPipe == 1)//创建管子
					{
						if(iShape == 1)//直管
						{
							ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),tmpInfo,tmpcolorInfo,curProcess.iNthCureve-2,fcurvalue,curProcess.iNthCureve-2+1);
						}
						else if(iShape == 0)//弯管
						{
							osg::Vec3 pipedir;
							float tmpValue;
							if(m_vproductionvalues[curProcess.iNthCureve-2].flr == 1)
							{
								pipedir = osg::Vec3(0.0,-1.0,0.0);
								m_bIsLeft = true;
								tmpValue = -fcurvalue;
							}
							else if(m_vproductionvalues[curProcess.iNthCureve-2].flr == 0)
							{
								pipedir = osg::Vec3(0.0,1.0,0.0);
								m_bIsLeft = false;
								tmpValue = fcurvalue;
							}

							if (fCurveR == 0)
							{
								int ilayer = (int) m_vproductionvalues[curProcess.iNthCureve-2].fmm;
								float tmpCurveR = GetJT1CurLayerLMRadius(ilayer);
								fCurveR = tmpCurveR;
							}

							ShowCurvePipe(curProcess.iNthCureve-2,m_vPipeCenter[0],pipedir,tmpInfo,tmpcolorInfo,fCurveR,tmpValue,true);

						}
					}
					else if(iCreatPipe == 0)
					{

					}
				}
			}
		}
	}

	Play();
}


void MainWindow::SetPlayNew()
{
	char safedis[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, SAFE_DIS, 3, safedis);
	m_safeDis = atof(safedis);

	//清零
	for(int i=0;i<m_vSTLs.size();i++)//
	{
		m_vSTLs[i]->ClearMoveAnimation();
		m_vSTLs[i]->ClearRotateAnimation();
		m_vSTLs[i]->ResetAbsoluteAngle();

		// 恢复主夹相关变换矩阵
		std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = m_vSTLs[i]->GetTransMatrix();
		if (!trans.empty())
		{
			osg::ref_ptr<osg::MatrixTransform> moveMatrix = m_vSTLs[i]->GetModelMoveMatrix();
			osg::ref_ptr<osg::MatrixTransform> rotateMatrix = m_vSTLs[i]->GetModelSecondRotateMatrix();

			moveMatrix->removeChild(trans.size()-1);
			trans[0]->removeChild(rotateMatrix);
			moveMatrix->addChild(rotateMatrix);

			trans.clear();
		}
	}

	std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.begin();
	for (;iter != m_mapGcodeSpecialMove.end();++iter)  // 沿着机头弯管垂直运动
	{
		//找到轮模组对应的运动代码
		if (iter->second==8)
		{

		}

	}

	// 机头弯管初始角度为90度
	m_dJitouAngle = 0.0;

	ResetAllModelAbsPos();

	//clear vPipe
	osg::ref_ptr<osg::Group> pipefather = m_sceneModel->getPipeFather();//管子父节点
	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrix = m_sceneModel->getPipeFatherMatrix();//管子父节点下的变化矩阵
	if(m_vPipes.size() >= 2)
	{
		for (int i = m_vPipes.size()-1;i>=1;--i)
		{
			m_vPipes[i].GetMoveMatrix()->removeChild(m_vPipes[i-1].GetRotateMatrix());
		}
		osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
		pipefatherMatrix->removeChild(m_vPipes[m_vPipes.size() -1].GetRotateMatrix());//绑定管子最后一届到管子矩阵根节点上
		((osg::Group*)nodestl)->removeChild(pipefather);

		m_vPipes.clear();
		m_sceneModel->ResetPipeIndex();
	}

	if (!m_vMatrixAnimation.empty())
	{
		m_vMatrixAnimation.clear();
	}

	if (!m_vMatrixAnimations.empty())
	{
		m_vMatrixAnimations.clear();
	}

	if (!m_vColorInfo.empty())
	{
		m_vColorInfo.clear();
	}

	if (!m_vLongStraightPipeMaxtrixAnimation.empty())
	{
		m_vLongStraightPipeMaxtrixAnimation.clear();
	}

	if (!m_vLongStraightPipAnimation.empty())
	{
		m_vLongStraightPipAnimation.clear();
	}

	if (!m_mapMatrixInstead.empty())
	{
		m_mapMatrixInstead.clear();
	}

	//osg::ref_ptr<osg::Group> LongStraightpipefather = m_sceneModel->getLongStraightPipeFatherNode();//stl->Pipenode->trans
	osg::ref_ptr<osg::PositionAttitudeTransform> LongStraightpipefatherTrans = m_sceneModel->getLongStraightPipeTrans();

	//osg::Node* nodestl2 = m_sceneModel->getNode("NodeSTL");
	//if (m_pstraiPipe!=NULL)
	//{
	//	LongStraightpipefatherTrans->removeChild(m_pstraiPipe);
	//}
	//((osg::Group*)nodestl2)->removeChild(LongStraightpipefather);
	if (m_showLongStraightPipe ==0 )
	{
		CreatLongStraightPipe();
		//m_pstraiPipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getLongClipPlane(), osg::StateAttribute::ON);
		m_showLongStraightPipe++;
	}
	else
	{
		m_showLongStraightPipe++;
	}

	m_pstraiPipe->setNodeMask(0);

	//生成一次长直管
	float _x,_y,_z;	
	char ch_x[32],ch_y[32],ch_z[32];
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_1, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_1, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_1, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenter.push_back(osg::Vec3(_x,_y,_z));

	dtime = 1;//整个动画开始时间
	//m_v = m_pCollisionDlg->GetAnimatiuonSpeed();
	m_v = m_pSpeedDlg->GetCurSpeed();
	m_fAnimationTime = 1/m_v;//动画时间 
	m_fAnimationStopTime = 1/m_v;//间隔时间

	AnimationInfo tmpcolorInfo;
	AnimationInfo tmpTWcolorInfo;

	std::vector<std::vector<Process>>::iterator iters = m_vAllStepvProcess.begin();
	for(;iters!=m_vAllStepvProcess.end();iters++)
	{

		//同一个索引组成员 用同一个时间
		AnimationInfo tmpInfo;
		tmpInfo.fStartTime = dtime;
		tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
		dtime = tmpInfo.fEndTime + m_fAnimationStopTime;

		//if (fabs(tmpInfo.fStartTime-6.6)<0.001)
		//{
		//	int m = 0;
		//}

		for(int i=0;i<(*iters).size();i++)//遍历每组成员，执行每一个动作
		{
			Process curProcess = (*iters)[i];
			QString curCode = curProcess.strCode;//功能码
			if (curProcess.istep==280)
			{
				int a = 0;
			}

			QString tmpGroupName;//组名
			int imovetype = -1;//平移 旋转
			int iAxis = -1;//轴
			int iMoShi = -1;//相对或绝对位置
			int dir;//为0，正值为负半轴方向
			osg::Vec3 vectmpRotateCenter;
			std::vector<STL*> tmpSTLs;//组模型				
			float fcurvalue = curProcess.fvalue;//动画值
			int iRotatePipe = curProcess.iPipeRotate;
			int iCreatPipe = curProcess.iCreatPipe;
			int iShape = curProcess.iCurveOrStraight;
			float fCurveR = curProcess.fcurveR;

			tmpInfo.strProcess = curProcess.strProcess;
			std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==4)  // 沿着机头弯管垂直运动
			{
				tmpInfo.isZhujiaZ = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaZ = false;
			}

			iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==5)  // 主夹沿着机头弯管垂直运动
			{
				tmpInfo.isZhujiaX = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaX = false;
			}

			iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==3)  // 主夹沿着机头弯管运动
			{
				tmpInfo.isZhujiaR = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaR = false;
			}


			tmpInfo.fdir = curProcess.fDir;

			if(curCode == "COLOR")
			{
				tmpcolorInfo.fStartTime = tmpInfo.fStartTime;
				tmpcolorInfo.fEndTime = tmpInfo.fEndTime;
				tmpcolorInfo.pipeSeg = curProcess.iColor-1;
				tmpcolorInfo.isTW = false;
				m_vColorInfo.push_back(tmpcolorInfo);
			}
			else if (curCode == "TWCOLOR")
			{
				tmpcolorInfo.fStartTime = tmpInfo.fStartTime;
				tmpcolorInfo.fEndTime = tmpInfo.fEndTime;
				tmpcolorInfo.pipeSeg = curProcess.iTwColor-1;
				tmpcolorInfo.isTW = true;
			}
			else if (curCode == "CREATE")
			{
				//m_vMatrixAnimation.
				AnimationInfo tmpStraightInfo;
				tmpStraightInfo.fStartTime = tmpInfo.fStartTime;
				osg::Vec3 center3d = m_vPipeCenter[0];
				//m_pstraiPipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,m_param.fPipeLength,center3d,osg::Vec3(0,0,-1));
				std::pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo> tmppair = make_pair(LongStraightpipefatherTrans,tmpStraightInfo);
				m_vLongStraightPipeMaxtrixAnimation.push_back(tmppair);
				//m_vMatrixAnimation.push_back(tmppair);
			}
			else
			{
				//****************获取运动属性 和 模型数组
				for(int u=0;u<m_vGcodeFileValues.size();u++)
				{
					if(m_vGcodeFileValues[u].strGCode == curCode)
					{
						// 更新机头弯管角度
						std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.find(curCode);
						//if (curCode==/*"G30"*/"G20")
						if (iter!= m_mapGcodeSpecialMove.end() && iter->second==1)
						{
							//m_dJitouAngle = fcurvalue;
							m_dJitouAngle += fcurvalue;
						}
						//else if (curCode==/*"G3"*/"G21")
						else if (iter!= m_mapGcodeSpecialMove.end() && iter->second==2)
						{
							// m_dJitouAngle += fcurvalue;
							m_dJitouAngle = fcurvalue;
						}

						//if(curCode == "G9" || curCode == "G0")
						//{
						//	int aa;
						//	aa =1;
						//}

						tmpGroupName = m_vGcodeFileValues[u].strGroupName;
						tmpSTLs = GetGroupNodeByGroupName(tmpGroupName);
						if(tmpSTLs.empty())
						{
							/*QString strWarning;
							strWarning = strWarning.fromLocal8Bit("存在组成员为空!");
							QMessageBox::warning(this,"",strWarning);
							return;*/
						}
						imovetype = m_vGcodeFileValues[u].iMoveType;
						iAxis = m_vGcodeFileValues[u].iAxis;//轴
						iMoShi = m_vGcodeFileValues[u].irelative;
						dir = m_vGcodeFileValues[u].iDir;//为0，正值为负半轴方向
						if(iMoShi == 0)//相对
						{
							if(dir == 0)
							{
								fcurvalue = -fcurvalue;
							}
							else if (dir == 1)
							{

							}
						}
						else if (iMoShi==1) // 绝对
						{
							if(dir == 0)
							{
								fcurvalue = -fcurvalue;
							}
							else if (dir == 1)
							{

							}
						}

						if(imovetype == 1)
						{
							vectmpRotateCenter = m_vGcodeFileValues[u].vecRotateCenter;
						}
						break;
					}
				}

				//****************添加模型动画
				if(tmpSTLs.size()>0)
				{			
					if(imovetype == 0)//平移
					{
						if(iAxis == 0)//X轴
						{						
							if(iMoShi == 1) // 绝对
							{
								if(dir == 1)//顺时针转
								{
									float aimx = fcurvalue;
									osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									fcurvalue = aimx - curPos.x();
									for(int k=0;k<tmpSTLs.size();k++)
									{
										tmpSTLs[k]->SetSTLAbsolutePosX(aimx);
									}
								}
							}
							else if(iMoShi == 0)
							{
								float cur_x = tmpSTLs[0]->GetSTLAbsolutePosX();
								float aimx = cur_x + fcurvalue;
								for(int k=0;k<tmpSTLs.size();k++)
								{
									tmpSTLs[k]->SetSTLAbsolutePosX(aimx);
								}
							}

							tmpInfo.translate.x() = fcurvalue;
							tmpInfo.translate.y() = 0;
							tmpInfo.translate.z() = 0;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;

						}
						else if(iAxis == 1)//Y轴
						{
							if(iMoShi == 1) // 绝对位置
							{
								if(dir == 1) // 与当前坐标轴方向相同
								{
									float aimy = fcurvalue;
									osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									fcurvalue = aimy - curPos.y();
									for(int k=0;k<tmpSTLs.size();k++)
									{
										tmpSTLs[k]->SetSTLAbsolutePosY(aimy);
									}
								}
								else if (dir==0) //与当前坐标轴方向相反
								{
									float aimy = -fcurvalue; // 坐标位置先按照流程文件设置，前面反过一次
									osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									fcurvalue = aimy - curPos.y();
									for(int k=0;k<tmpSTLs.size();k++)
									{
										tmpSTLs[k]->SetSTLAbsolutePosY(aimy);
									}

									fcurvalue *= -1.0;
								}

							}
							else if(iMoShi == 0)
							{
								float cur_y = tmpSTLs[0]->GetSTLAbsolutePosY();
								float aimy = cur_y + fcurvalue;
								for(int k=0;k<tmpSTLs.size();k++)
								{
									tmpSTLs[k]->SetSTLAbsolutePosY(aimy);
								}
							}

							tmpInfo.translate.x() = 0;
							tmpInfo.translate.z() = 0;
							tmpInfo.translate.y() = fcurvalue;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;
						}
						else if(iAxis == 2)//Z轴
						{
							if(iMoShi == 1)
							{
								if(dir == 1)
								{
									if (curProcess.iDirectly == 11)//判读小车是否往复
									{
										osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
										float pipe_z = m_vPipeCenter[0].z();
										float curdis = fabs(curPos.z()) - fabs(pipe_z);
										if (curdis <= m_safeDis)
										{
											float aimz = fcurvalue;
											fcurvalue = aimz - curPos.z();
											for(int k=0;k<tmpSTLs.size();k++)
											{
												tmpSTLs[k]->SetSTLAbsolutePosZ(aimz);
											}
										}
										else
										{
											fcurvalue = 0;
										}
									}
									else
									{
										if (curProcess.iDirectly == 12)
										{
											float aimz = -fcurvalue;
											osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
											//float z = curPos.z();
											//LogInfo(QString(("%d is cur pos")).arg(z));
											fcurvalue = aimz - curPos.z();
											for(int k=0;k<tmpSTLs.size();k++)
											{
												tmpSTLs[k]->SetSTLAbsolutePosZ(aimz);
											}
										}
										else
										{
											float aimz = fcurvalue;
											osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
											//float z = curPos.z();
											//LogInfo(QString(("%d is cur pos")).arg(z));
											fcurvalue = aimz - curPos.z();
											for(int k=0;k<tmpSTLs.size();k++)
											{
												tmpSTLs[k]->SetSTLAbsolutePosZ(aimz);
											}
										}	
									}
								}
							}
							else if(iMoShi == 0)
							{
								float cur_z = tmpSTLs[0]->GetSTLAbsolutePosZ();
								float aimz = cur_z + fcurvalue;
								for(int k=0;k<tmpSTLs.size();k++)
								{
									tmpSTLs[k]->SetSTLAbsolutePosZ(aimz);
								}
							}

							tmpInfo.translate.x() = 0;
							tmpInfo.translate.y() = 0;
							tmpInfo.translate.z() = fcurvalue;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;
						}
						else if(iAxis == 3)
						{
							// 沿着机头弯管运动
							if(iMoShi == 1)
							{
								if(dir == 1)//顺时针转
								{
									double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
									double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
									if (m_bOnlyLeft)
									{
										// 纯左侧加工
										x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
										z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
									}
									else
									{
										x = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
										z = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
									}

									//float aimx = fcurvalue;
									osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									//fcurvalue = aimx - curPos.x();
									for(int k=0;k<tmpSTLs.size();k++)
									{
										tmpSTLs[k]->SetSTLAbsolutePosX(x);
										tmpSTLs[k]->SetSTLAbsolutePosZ(z);
									}
								}
							}
							else if(iMoShi == 0)
							{
								double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
								double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
								if (m_bOnlyLeft)
								{
									// 纯左侧加工
									x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
									z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
								}
								else
								{
									x = fabs(fcurvalue)*cos(fabs(m_dJitouAngle*3.1415926/180.0));
									z = fabs(fcurvalue)*sin(fabs(m_dJitouAngle*3.1415926/180.0));
								}
								float cur_x = tmpSTLs[0]->GetSTLAbsolutePosX();
								float cur_z = tmpSTLs[0]->GetSTLAbsolutePosZ();
								float aimx = cur_x + x;
								float aimz = cur_z + z;
								for(int k=0;k<tmpSTLs.size();k++)
								{
									tmpSTLs[k]->SetSTLAbsolutePosX(aimx);
									tmpSTLs[k]->SetSTLAbsolutePosZ(aimz);
								}
							}

							double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
							double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
							if (m_bOnlyLeft)
							{
								// 纯左侧加工
								x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
								z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
							}
							else
							{
								x = fabs(fcurvalue)*cos(fabs(m_dJitouAngle*3.1415926/180.0));
								z = fabs(fcurvalue)*sin(fabs(m_dJitouAngle*3.1415926/180.0));
							}
							tmpInfo.translate.x() = x;

							tmpInfo.translate.y() = 0.0;
							tmpInfo.translate.z() = z;

							tmpInfo.rotateX = 0.0;
							tmpInfo.rotateY = 0.0;
							tmpInfo.rotateZ = 0.0;
						}

						for(int k=0;k<tmpSTLs.size();k++)
						{
							tmpInfo.absolutePos = tmpSTLs[k]->GetSTLAbsolutePos();
							tmpSTLs[k]->AddMoveAnimation(tmpInfo);
							if(tmpSTLs[k]->GetSTLName()==QString::fromLocal8Bit("youdjdz.stl"))
							{

								QString information;
								AnimationInfo info = tmpInfo;

								information.append("start time ");
								information.append(QString::number(info.fStartTime));
								LogInfo(information);
								information.clear();

								information.append("end time ");
								information.append(QString::number(info.fEndTime));
								LogInfo(information);
								information.clear();

								information.append("pos x ");
								information.append(QString::number(info.translate.x()));
								LogInfo(information);
								information.clear();

								information.append("pos y ");
								information.append(QString::number(info.translate.y()));
								LogInfo(information);
								information.clear();

								information.append("pos z ");
								information.append(QString::number(info.translate.z()));
								LogInfo(information);
								information.clear();

								information.clear();
								information.append("step ");
								information.append(QString::number(curProcess.istep));
								information.append(curProcess.strCode);
								LogInfo(information);
							}
						}
					}
					else if(imovetype == 1)//旋转
					{
						tmpInfo.center3d = vectmpRotateCenter;

						if(iMoShi == 1)
						{
							if(dir == 1)
							{
								float aim = fcurvalue;

								float curAngle = tmpSTLs[0]->GetSTLAbsoluteAngle();
								fcurvalue = aim - curAngle;
								for(int k=0;k<tmpSTLs.size();k++)
								{
									tmpSTLs[k]->SetSTLAbsoluteAngle(aim);
								}						
							}
						}
						else if(iMoShi == 0)
						{					
							float curAngle = tmpSTLs[0]->GetSTLAbsoluteAngle();
							float absoluteAngle = curAngle + fcurvalue;
							for(int k=0;k<tmpSTLs.size();k++)
							{
								tmpSTLs[k]->SetSTLAbsoluteAngle(absoluteAngle);
							}
						}

						if(iAxis == 0)//X轴
						{
							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateX = fcurvalue/2;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateX = fcurvalue/2;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;
								tmpInfo.rotateX = fcurvalue;
								tmpInfo.rotateY = 0;
								tmpInfo.rotateZ = 0; 

								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 1)//Y轴
						{

							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateY = fcurvalue/2;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateY = fcurvalue/2;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;

								tmpInfo.rotateX = 0;
								tmpInfo.rotateZ = 0;
								tmpInfo.rotateY = fcurvalue;

								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 2)//Z轴
						{
							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateZ = fcurvalue/2;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateZ = fcurvalue/2;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;
								tmpInfo.rotateY = 0;
								tmpInfo.rotateX = 0;
								tmpInfo.rotateZ = fcurvalue;
								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 3)
						{
							// 相对于机头弯管的轴
							int a = 0;
						}

						for(int k=0;k<tmpSTLs.size();k++)
						{
							//判断中心是否已存在
							std::vector<AnimationInfo> vtmpRotateInfo =tmpSTLs[k]->GetVRotateAnimation();
							if(vtmpRotateInfo.size() == 0)
							{
								if(m_bIsDivideTwoTimes == false)
								{
									tmpSTLs[k]->AddRotateAnimation(tmpInfo);

								}
								else if(m_bIsDivideTwoTimes == true)
								{
									tmpSTLs[k]->AddRotateAnimation(m_DivideInfo1);
									tmpSTLs[k]->AddRotateAnimation(m_DivideInfo2);								
								}

							}
							else
							{
								vtmpRotateInfo =tmpSTLs[k]->GetVRotateAnimation();
								std::vector<AnimationInfo>::iterator itrs = vtmpRotateInfo.begin();
								for(;itrs!=vtmpRotateInfo.end();itrs++)
								{
									if(itrs->center3d == tmpInfo.center3d)
									{
										break;
									}
								}
								if(itrs != vtmpRotateInfo.end())//存在
								{

									//tmpSTLs[k]->AddRotateAnimation(tmpInfo);
									if(m_bIsDivideTwoTimes == false)
									{
										tmpSTLs[k]->AddRotateAnimation(tmpInfo);

									}
									else if(m_bIsDivideTwoTimes == true)
									{
										tmpSTLs[k]->AddRotateAnimation(m_DivideInfo1);
										tmpSTLs[k]->AddRotateAnimation(m_DivideInfo2);								
									}

								}
								else if(itrs==vtmpRotateInfo.end())
								{
									//tmpSTLs[k]->AddSecondRotateAnimation(tmpInfo);
									if(m_bIsDivideTwoTimes == false)
									{
										tmpSTLs[k]->AddSecondRotateAnimation(tmpInfo);

									}
									else if(m_bIsDivideTwoTimes == true)
									{
										tmpSTLs[k]->AddSecondRotateAnimation(m_DivideInfo1);
										tmpSTLs[k]->AddSecondRotateAnimation(m_DivideInfo2);								
									}

								}
							}

						}
					}
				}

				//for (int a = 0; a < tmpSTLs.size();++a)
				//{
				//	if (tmpSTLs[a]->GetSTLName()=="zj1.stl")
				//	{
				//		QString info("zj1 pos x ");
				//		info.append(QString::number(tmpSTLs[a]->GetSTLAbsolutePosX()));
				//		LogInfo(info);

				//		info.clear();
				//		info.append("step ");
				//		info.append(QString::number(curProcess.istep));
				//		info.append(curProcess.strCode);
				//		LogInfo(info);
				//	}
				//}

				//****************管子动画
				if(iRotatePipe == 1)//转动管子
				{
					PipeRotateX(tmpInfo,tmpcolorInfo,fcurvalue,curProcess.iNthCureve-2);
				}
				else if(iRotatePipe == 0)
				{
					if(iCreatPipe == 1)//创建管子
					{
						if(iShape == 1)//直管
						{
							ShowStraightPipe(m_vPipeCenter[0],osg::Vec3(0,0,-1),tmpInfo,tmpcolorInfo,curProcess.iNthCureve-2,fcurvalue,curProcess.iNthCureve-2+1);
						}
						else if(iShape == 0)//弯管
						{
							osg::Vec3 pipedir;
							float tmpValue;
							if(m_vproductionvalues[curProcess.iNthCureve-2].flr == 1)
							{
								pipedir = osg::Vec3(0.0,-1.0,0.0);
								m_bIsLeft = true;
								tmpValue = -fcurvalue;
							}
							else if(m_vproductionvalues[curProcess.iNthCureve-2].flr == 0)
							{
								pipedir = osg::Vec3(0.0,1.0,0.0);
								m_bIsLeft = false;
								tmpValue = fcurvalue;
							}

							if (fCurveR == 0)
							{
								int ilayer = (int) m_vproductionvalues[curProcess.iNthCureve-2].fmm;
								float tmpCurveR = GetJT1CurLayerLMRadius(ilayer);
								fCurveR = tmpCurveR;
							}

							ShowCurvePipe(curProcess.iNthCureve-2,m_vPipeCenter[0],pipedir,tmpInfo,tmpcolorInfo,fCurveR,tmpValue,true);

						}
					}
					else if(iCreatPipe == 0)
					{

					}
				}
			}
		}
	}

	Play();
}
#endif

void MainWindow::ReadHeaderPara()
{
	float _x,_y,_z;	
	char ch_x[32],ch_y[32],ch_z[32];

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LONG_PIPE_CENTER_X, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LONG_PIPE_CENTER_Y, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LONG_PIPE_CENTER_Z, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_LongPipeCenter = osg::Vec3(_x,_y,_z);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LONG_PIPE_NORMAL_X, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LONG_PIPE_NORMAL_Y, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, LONG_PIPE_NORMAL_Z, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_LongPipeNormal = osg::Vec3(_x,_y,_z);

	m_vHeaderCenters.clear();
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_1, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_1, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_1, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_2, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_2, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_2, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_3, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_3, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_3, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_4, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_4, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_4, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_5, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_5, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_5, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_6, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_6, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_6, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_7, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_7, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_7, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_8, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_8, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_8, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_9, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_9, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_9, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_X_10, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Y_10, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, HEADER_CENTER_Z_10, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vHeaderCenters.push_back(osg::Vec3(_x,_y,_z));

	m_vPipeCenters.clear();
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_1, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_1, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_1, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_2, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_2, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_2, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_3, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_3, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_3, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_4, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_4, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_4, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_5, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_5, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_5, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_6, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_6, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_6, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_7, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_7, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_7, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_8, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_8, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_8, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_9, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_9, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_9, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_X_10, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Y_10, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_CENTER_Z_10, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeCenters.push_back(osg::Vec3(_x,_y,_z));

	m_vPipeNormals.clear();
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_1, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_1, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_1, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_2, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_2, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_2, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_3, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_3, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_3, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_4, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_4, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_4, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_5, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_5, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_5, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_6, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_6, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_6, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_7, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_7, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_7, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_8, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_8, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_8, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_9, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_9, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_9, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	// 不同编号的机头生成直管时的方向
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_X_10, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Y_10, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_NORMAL_Z_10, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPipeNormals.push_back(osg::Vec3(_x,_y,_z));

	m_vPipeRotateAngles.clear();
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_1, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_2, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_3, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_4, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_5, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_6, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_7, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_8, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_9, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PIPE_ROTATE_ANGLE_10, 3, ch_x);
	_x = atof(ch_x);
	m_vPipeRotateAngles.push_back(_x*PI/180.0);

	m_vPlaneCenters.clear();
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_1, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_1, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_1, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_2, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_2, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_2, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_3, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_3, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_3, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_4, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_4, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_4, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_5, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_5, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_5, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_6, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_6, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_6, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_7, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_7, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_7, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_8, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_8, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_8, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_9, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_9, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_9, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_X_10, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Y_10, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_CENTER_Z_10, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneCenters.push_back(osg::Vec3(_x,_y,_z));

	m_vPlaneNormals.clear();
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_1, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_1, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_1, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_2, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_2, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_2, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_3, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_3, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_3, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_4, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_4, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_4, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_5, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_5, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_5, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_6, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_6, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_6, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_7, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_7, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_7, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_8, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_8, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_8, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_9, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_9, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_9, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));

	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_X_10, 3, ch_x);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Y_10, 3, ch_y);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PLANE_NORMAL_Z_10, 3, ch_z);
	_x = atof(ch_x);
	_y = atof(ch_y);
	_z = atof(ch_z);
	m_vPlaneNormals.push_back(osg::Vec3(_x,_y,_z));
}

// 根据节点结构生成各个模型的运动动画
void MainWindow::SetPlayNew2()
{
	char safedis[64] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, SAFE_DIS, 3, safedis);
	m_safeDis = atof(safedis);

	ReadHeaderPara();

	//清零
	for(size_t i=0;i<m_vNodes.size();i++)//
	{
		m_vNodes[i]->ClearMoveAnimation();
		m_vNodes[i]->ClearRotateAnimation();
		m_vNodes[i]->ResetAbsoluteAngle();
		osg::Vec3 pos(0.0,0.0,0.0);
		m_vNodes[i]->ResetAbsolutePos(pos);
		// 恢复主夹相关变换矩阵
		//std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = m_vSTLs[i]->GetTransMatrix();
		//if (!trans.empty())
		//{
		//	osg::ref_ptr<osg::MatrixTransform> moveMatrix = m_vSTLs[i]->GetModelMoveMatrix();
		//	osg::ref_ptr<osg::MatrixTransform> rotateMatrix = m_vSTLs[i]->GetModelSecondRotateMatrix();

		//	moveMatrix->removeChild(trans.size()-1);
		//	trans[0]->removeChild(rotateMatrix);
		//	moveMatrix->addChild(rotateMatrix);

		//	trans.clear();
		//}
	}

	std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.begin();
	for (;iter != m_mapGcodeSpecialMove.end();++iter)  // 沿着机头弯管垂直运动
	{
		//找到轮模组对应的运动代码
		if (iter->second==8)
		{

		}
	}

	// 机头弯管初始角度为90度
	m_dJitouAngle = 0.0;

	// 根据GroupToModels.csv组配置文件中间的模型位置设置模型初始位置
	//ResetAllModelAbsPos();
	ResetAllNodeInitPos();

	//clear vPipe
    //管子父节点
	osg::ref_ptr<osg::Group> pipefatherRight = m_sceneModel->getPipeFatherRight();
    //管子父节点下的变化矩阵
	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrixRight = m_sceneModel->getPipeFatherMatrixRight();

    //管子父节点
    osg::ref_ptr<osg::Group> pipefatherLeft = m_sceneModel->getPipeFatherLeft();
    //管子父节点下的变化矩阵
	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrixLeft = m_sceneModel->getPipeFatherMatrixLeft();

    osg::ref_ptr<osg::Group> left = nullptr;
    osg::ref_ptr<osg::Group> right = nullptr;
	for (size_t i = 0;i < m_vPipes.size();++i)
	{
        PIPE* pipe = m_vPipes[i];
        if (pipe->GetTip()==0)
        {
            // 长管右端加工
            if (right==nullptr)
            {
                right = pipe->GetRotateMatrix();
                //right = pipe.GetPipeRootNode();
            }
            else
            {
                pipe->GetRotateMatrix()->removeChild(right);
                right = pipe->GetRotateMatrix();
                 //right = pipe.GetPipeRootNode();
            }
        }
        else if (pipe->GetTip()==1)
        {
            // 长管左端加工
            if (left==nullptr)
            {
                left = pipe->GetRotateMatrix();
                //left = pipe.GetPipeRootNode();
            }
            else
            {
                pipe->GetRotateMatrix()->removeChild(left);
                left = pipe->GetRotateMatrix();
                //left = pipe.GetPipeRootNode();
            }
        }
        else
        {
            QString info("Unknown tip type : ");
            info.append(QString::number(pipe->GetTip()));
            QMessageBox::warning(NULL, "warning", info, QMessageBox::Yes);
        }

		//m_vPipes[i].GetMoveMatrix()->removeChild(m_vPipes[i-1].GetRotateMatrix());
	}

	osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");

    if (right!=nullptr)
    {
        //删除绑定管子最后一截到管子矩阵根节点上
        pipefatherMatrixRight->removeChild(right);
	    ((osg::Group*)nodestl)->removeChild(pipefatherRight);
    }

    if (left!=nullptr)
    {
        //删除绑定管子最后一截到管子矩阵根节点上
        pipefatherMatrixLeft->removeChild(left);
	    ((osg::Group*)nodestl)->removeChild(pipefatherLeft);
    }

	m_vPipes.clear();
	m_sceneModel->ResetPipeIndex();

	if (!m_vMatrixAnimation.empty())
	{
		m_vMatrixAnimation.clear();
	}

	if (m_mapMatrixHeader.empty())
	{
		m_mapMatrixHeader.clear();
	}

	if (!m_vMatrixAnimations.empty())
	{
		m_vMatrixAnimations.clear();
	}

	if (!m_vColorInfo.empty())
	{
		m_vColorInfo.clear();
	}

	if (!m_vLongStraightPipeMaxtrixAnimation.empty())
	{
		m_vLongStraightPipeMaxtrixAnimation.clear();
	}

	if (!m_vPipeBackMatrixAnimation.empty())
	{
		m_vPipeBackMatrixAnimation.clear();
	}

	if (!m_mapMatrixInstead.empty())
	{
		m_mapMatrixInstead.clear();
	}

	if (!m_vLongStraightPipAnimation.empty())
	{
		m_vLongStraightPipAnimation.clear();
	}

	//osg::ref_ptr<osg::Group> LongStraightpipefather = m_sceneModel->getLongStraightPipeFatherNode();//stl->Pipenode->trans
	osg::ref_ptr<osg::PositionAttitudeTransform> LongStraightpipefatherTrans = m_sceneModel->getLongStraightPipeTrans();

	//osg::Node* nodestl2 = m_sceneModel->getNode("NodeSTL");
	//if (m_pstraiPipe!=NULL)
	//{
	//	LongStraightpipefatherTrans->removeChild(m_pstraiPipe);
	//}
	//((osg::Group*)nodestl2)->removeChild(LongStraightpipefather);
	if (m_showLongStraightPipe ==0 )
	{
		CreatLongStraightPipe();
		//m_pstraiPipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getLongClipPlane(), osg::StateAttribute::ON);
		m_showLongStraightPipe++;
	}
	else
	{
		m_showLongStraightPipe++;
	}

	//m_pstraiPipe->setNodeMask(0);

	dtime = 1;//整个动画开始时间
	//m_v = m_pCollisionDlg->GetAnimatiuonSpeed();
	m_v = m_pSpeedDlg->GetCurSpeed();
	m_fAnimationTime = 1/m_v;//动画时间 
	m_fAnimationStopTime = 1/m_v;//间隔时间

	AnimationInfo tmpcolorInfo;
	AnimationInfo tmpTWcolorInfo;

	std::vector<std::vector<Process>>::iterator iters = m_vAllStepvProcess.begin();
	for(;iters!=m_vAllStepvProcess.end();iters++)
	{
		//同一个索引组成员 用同一个时间
		AnimationInfo tmpInfo;
		tmpInfo.fStartTime = dtime;
		tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
		dtime = tmpInfo.fEndTime + m_fAnimationStopTime;

		//if (fabs(tmpInfo.fStartTime-6.6)<0.001)
		//{
		//	int m = 0;
		//}

		for(size_t i=0;i<(*iters).size();i++)//遍历每组成员，执行每一个动作
		{
			Process curProcess = (*iters)[i];
			QString curCode = curProcess.strCode;//功能码
			//if (curProcess.istep==360 || curProcess.istep==546)
			//{
			//	int a = 0;
			//}

			QString tmpGroupName;//组名
			int imovetype = -1;//平移 旋转
			int iAxis = -1;//轴
			int iMoShi = -1;//相对或绝对位置
			int dir;//为0，正值为负半轴方向
			osg::Vec3 vectmpRotateCenter;
			std::vector<STL*> tmpSTLs;   // 组模型
			std::vector<GroupChildren*> tempNodes;  // 节点组

			float fcurvalue = curProcess.fvalue;//动画值
			int iRotatePipe = curProcess.iPipeRotate;
			int iCreatPipe = curProcess.iCreatPipe;
			int iShape = curProcess.iCurveOrStraight;
			float fCurveR = curProcess.fcurveR;
			// 机头是从1开始编号，数组索引从0开始，所以要减一
			int header = curProcess.iHeader - 1;
			int tip = curProcess.iTip;

			tmpInfo.strProcess = curProcess.strProcess;
			std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==4)  // 沿着机头弯管垂直运动
			{
				tmpInfo.isZhujiaZ = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaZ = false;
			}

			iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==5)  // 主夹沿着机头弯管垂直运动
			{
				tmpInfo.isZhujiaX = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaX = false;
			}

			iter = m_mapGcodeSpecialMove.find(curCode);
			if (iter != m_mapGcodeSpecialMove.end() && iter->second==3)  // 主夹沿着机头弯管运动
			{
				tmpInfo.isZhujiaR = true;
				//m_dTDistance= atoi(Res[3].c_str());
				//m_dTDistance = fabs(m_dTDistance);
			}
			else
			{
				tmpInfo.isZhujiaR = false;
			}

			tmpInfo.fdir = curProcess.fDir;

			if(curCode == "COLOR")
			{
				tmpcolorInfo.fStartTime = tmpInfo.fStartTime;
				tmpcolorInfo.fEndTime = tmpInfo.fEndTime;
				tmpcolorInfo.pipeSeg = curProcess.iColor-1;
				tmpcolorInfo.isTW = false;
				m_vColorInfo.push_back(tmpcolorInfo);
			}
			else if (curCode == "TWCOLOR")
			{
				tmpcolorInfo.fStartTime = tmpInfo.fStartTime;
				tmpcolorInfo.fEndTime = tmpInfo.fEndTime;
				tmpcolorInfo.pipeSegTw = curProcess.iTwColor;
				tmpcolorInfo.isTW = true;
			}
			else if (curCode == "CREATE")
			{
				//m_vMatrixAnimation.
				AnimationInfo tmpStraightInfo;
				tmpStraightInfo.fStartTime = tmpInfo.fStartTime;
				osg::Vec3 center3d = m_vPipeCenters[0];
				//m_pstraiPipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,m_param.fPipeLength,center3d,osg::Vec3(0,0,-1));
				std::pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo> tmppair = make_pair(LongStraightpipefatherTrans,tmpStraightInfo);
				m_vLongStraightPipeMaxtrixAnimation.push_back(tmppair);
				//m_vMatrixAnimation.push_back(tmppair);
			}
			else
			{
				//****************获取运动属性 和 模型数组
				for(size_t u=0;u<m_vGcodeFileValues.size();u++)
				{
					if(m_vGcodeFileValues[u].strGCode == curCode)
					{
						// 更新机头弯管角度
						std::map<QString,int>::iterator iter = m_mapGcodeSpecialMove.find(curCode);
						//if (curCode==/*"G30"*/"G20")
						if (iter!= m_mapGcodeSpecialMove.end() && iter->second==1)
						{
							//m_dJitouAngle = fcurvalue;
							m_dJitouAngle += fcurvalue;
						}
						//else if (curCode==/*"G3"*/"G21")
						else if (iter!= m_mapGcodeSpecialMove.end() && iter->second==2)
						{
							// m_dJitouAngle += fcurvalue;
							m_dJitouAngle = fcurvalue;
						}

						//if(curCode == "G9" || curCode == "G0")
						//{
						//	int aa;
						//	aa =1;
						//}

						tmpGroupName = m_vGcodeFileValues[u].strGroupName;
						tmpSTLs = GetGroupNodeByGroupName(tmpGroupName);

						if (tmpSTLs.empty())
						{
							QString strWarning;
							strWarning = strWarning.fromLocal8Bit("Node ");
							strWarning.append(tmpGroupName);
							strWarning.append(" empty !");
							//QMessageBox::warning(this,"",strWarning);

							LogInfo(strWarning);
							//return;
						}

						tempNodes = GetGroupNodesByGroupName(tmpGroupName);

						if(tmpSTLs.empty())
						{
							/*QString strWarning;
							strWarning = strWarning.fromLocal8Bit("存在组成员为空!");
							QMessageBox::warning(this,"",strWarning);
							return;*/
						}

						imovetype = m_vGcodeFileValues[u].iMoveType;
						iAxis = m_vGcodeFileValues[u].iAxis;//轴
						iMoShi = m_vGcodeFileValues[u].irelative;
						dir = m_vGcodeFileValues[u].iDir;//为0，正值为负半轴方向
						if(iMoShi == 0)//相对
						{
							if(dir == 0)
							{
								fcurvalue = -fcurvalue;
							}
							else if (dir == 1)
							{

							}
						}
						else if (iMoShi==1) // 绝对
						{
							if(dir == 0)
							{
								fcurvalue = -fcurvalue;
							}
							else if (dir == 1)
							{

							}
						}

						if(imovetype == 1)
						{
							vectmpRotateCenter = m_vGcodeFileValues[u].vecRotateCenter;
						}
						break;
					}
				}

				//****************添加模型动画
				if(tmpSTLs.size()>0)
				{			
					if(imovetype == 0)//平移
					{
						if(iAxis == 0)//X轴
						{						
							if(iMoShi == 1) // 绝对
							{
								if(dir == 1)//顺时针转
								{
									float aimx = fcurvalue;
									//osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置

									osg::Vec3 curPos = tempNodes[0]->currentPos;
									fcurvalue = aimx - curPos.x();
									for (size_t k = 0; k < tempNodes.size();++k)
									{
										curPos = tempNodes[k]->currentPos;
										tempNodes[k]->currentPos = osg::Vec3d(aimx,curPos.y(),curPos.z());
									}

									//for(int k=0;k<tmpSTLs.size();k++)
									//{
									//	tmpSTLs[k]->SetSTLAbsolutePosX(aimx);
									//}
								}
							}
							else if(iMoShi == 0)  // 相对运动
							{
								// float cur_x = tmpSTLs[0]->GetSTLAbsolutePosX();
								//osg::Vec3d curPos = tempNodes[0]->currentPos;

								for(size_t k=0;k<tempNodes.size();k++)
								{
									osg::Vec3d curPos = tempNodes[k]->currentPos;
									float aimx = curPos.x() + fcurvalue;
									tempNodes[k]->currentPos = osg::Vec3d(aimx,curPos.y(),curPos.z());
								}
								//for(int k=0;k<tmpSTLs.size();k++)
								//{
								//	tmpSTLs[k]->SetSTLAbsolutePosX(aimx);
								//}
							}

							tmpInfo.translate.x() = fcurvalue;
							tmpInfo.translate.y() = 0;
							tmpInfo.translate.z() = 0;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;

						}
						else if(iAxis == 1)//Y轴
						{
							if(iMoShi == 1) // 绝对位置
							{
								if(dir == 1) // 与当前坐标轴方向相同
								{
									float aimy = fcurvalue;
									osg::Vec3 curPos = tempNodes[0]->currentPos;//此组模型的当前绝对位置
									fcurvalue = aimy - curPos.y();
									for(size_t k=0;k < tempNodes.size();k++)
									{
										curPos = tempNodes[k]->currentPos;
										tempNodes[k]->currentPos = osg::Vec3d(curPos.x(),aimy,curPos.z());
									}
								}
								else if (dir==0) //与当前坐标轴方向相反
								{
									//float aimy = -fcurvalue; // 坐标位置先按照流程文件设置，前面反过一次
									//osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									//fcurvalue = aimy - curPos.y();
									//for(int k=0;k<tmpSTLs.size();k++)
									//{
									//	tmpSTLs[k]->SetSTLAbsolutePosY(aimy);
									//}

									//fcurvalue *= -1.0;
								}

							}
							else if(iMoShi == 0)
							{

								for(size_t k=0;k<tempNodes.size();k++)
								{
									osg::Vec3d curPos = tempNodes[k]->currentPos;
									float aimy = curPos.y() + fcurvalue;
									tempNodes[k]->currentPos = osg::Vec3d(curPos.x(),aimy,curPos.z());
								}
							}

							tmpInfo.translate.x() = 0;
							tmpInfo.translate.z() = 0;
							tmpInfo.translate.y() = fcurvalue;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;
						}
						else if(iAxis == 2)//Z轴
						{
							if(iMoShi == 1)
							{
								if(dir == 1)
								{
									if (curProcess.iDirectly == 11)//判读小车是否往复
									{
										osg::Vec3 curPos = tempNodes[0]->currentPos;//此组模型的当前绝对位置
										float pipe_z = m_vPipeCenters[0].z();
										float curdis = fabs(curPos.z()) - fabs(pipe_z);
										if (curdis <= m_safeDis)
										{
											float aimz = fcurvalue;
											fcurvalue = aimz - curPos.z();
											for(size_t k=0;k < tempNodes.size();k++)
											{
												curPos = tempNodes[k]->currentPos;
												tempNodes[k]->currentPos = osg::Vec3d(curPos.x(),curPos.y(),aimz);
											}
										}
										else
										{
											fcurvalue = 0;
										}
									}
									else
									{
										if (curProcess.iDirectly == 12)
										{
											float aimz = -fcurvalue;
											//osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
											osg::Vec3d curPos = tempNodes[0]->currentPos;
											//float z = curPos.z();
											//LogInfo(QString(("%d is cur pos")).arg(z));
											fcurvalue = aimz - curPos.z();
											for(size_t k=0;k < tempNodes.size(); k++)
											{
												curPos = tempNodes[k]->currentPos;
												tempNodes[k]->currentPos = osg::Vec3d(curPos.x(),curPos.y(),aimz);
												//tmpSTLs[k]->SetSTLAbsolutePosZ(aimz);
											}
										}
										else
										{
											float aimz = fcurvalue;
											osg::Vec3 curPos = tempNodes[0]->currentPos;//此组模型的当前绝对位置
											//float z = curPos.z();
											//LogInfo(QString(("%d is cur pos")).arg(z));
											fcurvalue = aimz - curPos.z();
											for(size_t k=0;k<tempNodes.size();k++)
											{
												curPos = tempNodes[k]->currentPos;
												tempNodes[k]->currentPos = osg::Vec3d(curPos.x(),curPos.y(),aimz);
											}
										}	
									}
								}
								else if (dir==0)
								{
									float aimz = fcurvalue;
									osg::Vec3 curPos = tempNodes[0]->currentPos;
									fcurvalue = aimz - curPos.z();
									for (size_t k = 0; k < tempNodes.size();++k)
									{
										curPos = tempNodes[k]->currentPos;
										tempNodes[k]->currentPos = osg::Vec3d(curPos.x(),curPos.y(),aimz);
									}
								}
							}
							else if(iMoShi == 0)
							{
								//float cur_z = tmpSTLs[0]->GetSTLAbsolutePosZ();
								//float aimz = cur_z + fcurvalue;
								//for(int k=0;k<tmpSTLs.size();k++)
								//{
								//	tmpSTLs[k]->SetSTLAbsolutePosZ(aimz);
								//}
								for (size_t k = 0; k < tempNodes.size();++k)
								{
									osg::Vec3d curPos = tempNodes[k]->currentPos;
									float aimz = curPos.z() + fcurvalue;
									tempNodes[k]->currentPos = osg::Vec3d(curPos.x(),curPos.y(),aimz);
								}
							}

							tmpInfo.translate.x() = 0;
							tmpInfo.translate.y() = 0;
							tmpInfo.translate.z() = fcurvalue;

							tmpInfo.rotateX = 0;
							tmpInfo.rotateY = 0;
							tmpInfo.rotateZ = 0;
						}
						else if(iAxis == 3)
						{
							// 沿着机头弯管运动
							if(iMoShi == 1)
							{
								if(dir == 1)//顺时针转
								{
									double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
									double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
									if (m_bOnlyLeft)
									{
										// 纯左侧加工
										x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
										z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
									}
									else
									{
										x = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
										z = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
									}

									//float aimx = fcurvalue;
									//osg::Vec3 curPos = tmpSTLs[0]->GetSTLAbsolutePos();//此组模型的当前绝对位置
									//fcurvalue = aimx - curPos.x();
									//for(int k=0;k<tmpSTLs.size();k++)
									//{
									//	tmpSTLs[k]->SetSTLAbsolutePosX(x);
									//	tmpSTLs[k]->SetSTLAbsolutePosZ(z);
									//}
								}
							}
							else if(iMoShi == 0)
							{
								double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
								double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
								if (m_bOnlyLeft)
								{
									// 纯左侧加工
									x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
									z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
								}
								else
								{
									x = fabs(fcurvalue)*cos(fabs(m_dJitouAngle*3.1415926/180.0));
									z = fabs(fcurvalue)*sin(fabs(m_dJitouAngle*3.1415926/180.0));
								}
								//float cur_x = tmpSTLs[0]->GetSTLAbsolutePosX();
								//float cur_z = tmpSTLs[0]->GetSTLAbsolutePosZ();
								osg::Vec3d curPos = tempNodes[0]->currentPos;
								float aimx = curPos.x() + x;
								float aimz = curPos.z() + z;
								//for(int k=0;k<tmpSTLs.size();k++)
								//{
								//	tmpSTLs[k]->SetSTLAbsolutePosX(aimx);
								//	tmpSTLs[k]->SetSTLAbsolutePosZ(aimz);
								//}
							}

							double x;// = fabs(fcurvalue)*cos(m_dJitouAngle*3.1415926/180.0);
							double z;// = fabs(fcurvalue)*sin(m_dJitouAngle*3.1415926/180.0);
							if (m_bOnlyLeft)
							{
								// 纯左侧加工
								x = fcurvalue*cos(m_dJitouAngle*3.1415926/180.0);
								z = fcurvalue*sin(m_dJitouAngle*3.1415926/180.0);
							}
							else
							{
								x = fabs(fcurvalue)*cos(fabs(m_dJitouAngle*3.1415926/180.0));
								z = fabs(fcurvalue)*sin(fabs(m_dJitouAngle*3.1415926/180.0));
							}

							//tmpInfo.translate.x() = x;
							//tmpInfo.translate.y() = 0.0;
							//tmpInfo.translate.z() = z;

							tmpInfo.translate.x() = fcurvalue;
							tmpInfo.translate.y() = 0.0;
							tmpInfo.translate.z() = 0.0;

							tmpInfo.rotateX = 0.0;
							tmpInfo.rotateY = 0.0;
							tmpInfo.rotateZ = 0.0;
						}

						for(int k=0;k<tmpSTLs.size();k++)
						{
							// 设置移动动画
							tmpSTLs[k]->AddMoveAnimation(tmpInfo);
						}
					}
					else if(imovetype == 1)//旋转
					{
						tmpInfo.center3d = vectmpRotateCenter;

						if(iMoShi == 1)
						{
							if(dir == 1)
							{
								float aim = fcurvalue;

								/*	float curAngle = tmpSTLs[0]->GetSTLAbsoluteAngle();
								fcurvalue = aim - curAngle;
								for(int k=0;k<tmpSTLs.size();k++)
								{
								tmpSTLs[k]->SetSTLAbsoluteAngle(aim);
								}*/		
								float angle = tempNodes[0]->m_fAngle;
								fcurvalue = aim - angle;
								for (size_t k = 0; k < tempNodes.size();++k)
								{
									tempNodes[k]->m_fAngle = aim;
								}
							}
						}
						else if(iMoShi == 0)
						{					
							//float curAngle = tmpSTLs[0]->GetSTLAbsoluteAngle();
							//float absoluteAngle = curAngle + fcurvalue;
							//for(int k=0;k<tmpSTLs.size();k++)
							//{
							//	tmpSTLs[k]->SetSTLAbsoluteAngle(absoluteAngle);
							//}

							for (size_t k = 0; k < tempNodes.size(); ++k)
							{
								tempNodes[k]->m_fAngle += fcurvalue; 
							}
						}

						if(iAxis == 0)//X轴
						{
							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateX = fcurvalue/2;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateX = fcurvalue/2;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;
								tmpInfo.rotateX = fcurvalue;
								tmpInfo.rotateY = 0;
								tmpInfo.rotateZ = 0; 

								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 1)//Y轴
						{
							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateY = fcurvalue/2;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateY = fcurvalue/2;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;

								tmpInfo.rotateX = 0;
								tmpInfo.rotateZ = 0;
								tmpInfo.rotateY = fcurvalue;

								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 2)//Z轴
						{
							if(fcurvalue>=180 || fcurvalue<=-180)
							{
								m_bIsDivideTwoTimes = true;
								InfoClear(m_DivideInfo1);
								InfoClear(m_DivideInfo2);

								m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
								m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
								m_DivideInfo1.center3d = tmpInfo.center3d;
								m_DivideInfo1.rotateZ = fcurvalue/2;
								m_DivideInfo1.strProcess = tmpInfo.strProcess;

								m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
								m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
								m_DivideInfo2.center3d = tmpInfo.center3d;
								m_DivideInfo2.rotateZ = fcurvalue/2;
								m_DivideInfo2.strProcess = tmpInfo.strProcess;
							}
							else
							{
								m_bIsDivideTwoTimes = false;
								tmpInfo.rotateY = 0;
								tmpInfo.rotateX = 0;
								tmpInfo.rotateZ = fcurvalue;
								tmpInfo.translate.y() = 0;
								tmpInfo.translate.x() = 0;
								tmpInfo.translate.z() = 0;
							}
						}
						else if(iAxis == 3)
						{
							// 相对于机头弯管的轴
							int a = 0;
						}

						for(size_t k=0;k<tmpSTLs.size();k++)
						{
							//判断中心是否已存在
							std::vector<AnimationInfo> vtmpRotateInfo =tmpSTLs[k]->GetVRotateAnimation();
							if(vtmpRotateInfo.size() == 0)
							{
								if(m_bIsDivideTwoTimes == false)
								{
									tmpSTLs[k]->AddRotateAnimation(tmpInfo);

								}
								else if(m_bIsDivideTwoTimes == true)
								{
									tmpSTLs[k]->AddRotateAnimation(m_DivideInfo1);
									tmpSTLs[k]->AddRotateAnimation(m_DivideInfo2);								
								}
							}
							else
							{
								vtmpRotateInfo = tmpSTLs[k]->GetVRotateAnimation();
								std::vector<AnimationInfo>::iterator itrs = vtmpRotateInfo.begin();
								for(;itrs!=vtmpRotateInfo.end();itrs++)
								{
									if(itrs->center3d == tmpInfo.center3d)
									{
										break;
									}
								}
								if(itrs != vtmpRotateInfo.end())//存在
								{

									//tmpSTLs[k]->AddRotateAnimation(tmpInfo);
									if(m_bIsDivideTwoTimes == false)
									{
										tmpSTLs[k]->AddRotateAnimation(tmpInfo);

									}
									else if(m_bIsDivideTwoTimes == true)
									{
										tmpSTLs[k]->AddRotateAnimation(m_DivideInfo1);
										tmpSTLs[k]->AddRotateAnimation(m_DivideInfo2);								
									}

								}
								else if(itrs==vtmpRotateInfo.end())
								{
									//tmpSTLs[k]->AddSecondRotateAnimation(tmpInfo);
									if(m_bIsDivideTwoTimes == false)
									{
										tmpSTLs[k]->AddSecondRotateAnimation(tmpInfo);

									}
									else if(m_bIsDivideTwoTimes == true)
									{
										tmpSTLs[k]->AddSecondRotateAnimation(m_DivideInfo1);
										tmpSTLs[k]->AddSecondRotateAnimation(m_DivideInfo2);								
									}

								}
							}
						}
					}
				}

				//****************管子动画
				if(iRotatePipe == 1)//转动管子
				{
					PipeRotateX(tmpInfo,tmpcolorInfo,fcurvalue,curProcess.iNthCureve-2);
				}
				else if(iRotatePipe == 0)
				{
					if(iCreatPipe == 1)//创建管子
					{
						if(iShape == 1)//直管
						{
							// ShowStraightPipe(m_PipeCenter,osg::Vec3(0,0,-1),tmpInfo,tmpcolorInfo,curProcess.iNthCureve-2,fcurvalue,curProcess.iNthCureve-2+1);
							ShowStraightPipeNew(tip,header,m_vPipeCenters[header],m_vPipeNormals[header],tmpInfo,tmpcolorInfo,
                                curProcess.iNthCureve-2,fcurvalue,curProcess.iNthCureve-2+1,curProcess.strProcess);
						}	
						else if(iShape == 0)//弯管
						{
							osg::Vec3 pipedir;
							float tmpValue;
							if(m_vproductionvalues[curProcess.iNthCureve-2].flr == 1)
							{
								pipedir = osg::Vec3(0.0,-1.0,0.0);
								m_bIsLeft = true;
								tmpValue = -fcurvalue;
							}
							else if(m_vproductionvalues[curProcess.iNthCureve-2].flr == 0)
							{
								pipedir = osg::Vec3(0.0,1.0,0.0);
								m_bIsLeft = false;
								tmpValue = fcurvalue;
							}

							if (fCurveR == 0)
							{
								int ilayer = (int) m_vproductionvalues[curProcess.iNthCureve-2].fmm;
								float tmpCurveR = GetJT1CurLayerLMRadius(ilayer);
								fCurveR = tmpCurveR;
							}

							ShowCurvePipe(curProcess.iNthCureve-2,tip,header,m_vHeaderCenters[header],pipedir,tmpInfo,tmpcolorInfo,fCurveR,tmpValue,true);
						}
					}
					else if(iCreatPipe == 0)
					{

					}
				}
			}
		}
	}

	AddModelsToCollision();
	//AddPipesToCollision();

	PlayNew();
}


void MainWindow::SetPlay()
{
#if 0


	//清零
	m_MapLoopTovLoopvProcess.erase(m_MapLoopTovLoopvProcess.begin(),m_MapLoopTovLoopvProcess.end());
	m_vLoopvProcess.clear();
	for(int i=0;i<m_vSTLs.size();i++)//
	{
		m_vSTLs[i]->ClearMoveAnimation();
		m_vSTLs[i]->ClearRotateAnimation();
	}

	//clear vPipe
	osg::ref_ptr<osg::Group> pipefather = m_sceneModel->getPipeFather();//管子父节点
	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrix = m_sceneModel->getPipeFatherMatrix();//管子父节点下的变化矩阵
	if(m_vPipes.size() >= 2)
	{
		for (int i = m_vPipes.size()-1;i>=1;--i)
		{
			m_vPipes[i].GetMoveMatrix()->removeChild(m_vPipes[i-1].GetRotateMatrix());
		}
		osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
		pipefatherMatrix->removeChild(m_vPipes[m_vPipes.size() -1].GetRotateMatrix());//绑定管子最后一届到管子矩阵根节点上
		((osg::Group*)nodestl)->removeChild(pipefather);

		m_vPipes.clear();
		m_sceneModel->ResetPipeIndex();
	}
	if (!m_vMatrixAnimation.empty())
	{
		m_vMatrixAnimation.clear();
	}

	if (!m_vMatrixAnimations.empty())
	{
		m_vMatrixAnimations.clear();
	}

	//生成一次长直管
	if(m_showLongStraightPipe == 0)
	{
		CreatLongStraightPipe();
	}
	else
	{
		m_showLongStraightPipe++;
	}	

	//更新循环数据
	std::map<int,std::map<int,std::vector<Process>>>::iterator itit = m_MapLoopToMapStepTovProcessFileValues.begin();
	for(;itit != m_MapLoopToMapStepTovProcessFileValues.end();itit++)//遍历每一弯的循环部分
	{
		std::vector<std::vector<Process>> CurvLoopvProcess;;
		std::map<int,std::vector<Process>>::iterator it1 = itit->second.begin();//每个同步组
		for(;it1!=itit->second.end();it1++)
		{
			std::vector<Process>::iterator it2 = it1->second.begin();//每个动作
			for(;it2!=it1->second.end();it2++)
			{
				if((*it2).bIsLoop == true)//循环部分的动作
				{
					CurvLoopvProcess.push_back(it1->second);
					break;
				}
			}

		}
		if(CurvLoopvProcess.size()>0)
			m_MapLoopTovLoopvProcess[itit->first] = CurvLoopvProcess;
	}

	std::map<int,std::vector<std::vector<Process>>>  a = m_MapLoopTovLoopvProcess;

	bool isDoLoop = true;	
	dtime = 1;//整个动画开始时间
	//m_v = m_pCollisionDlg->GetAnimatiuonSpeed();
	m_v = m_pSpeedDlg->GetCurSpeed();
	m_fAnimationTime = 1/m_v;//动画时间 
	m_fAnimationStopTime = 1/m_v;//间隔时间

	std::map<int,std::vector<Process>>::iterator iter = m_MapStepTovProcessFileValues.begin();
	for(;iter!=m_MapStepTovProcessFileValues.end();iter++)//执行每一组动作
	{
		//判断次组是否在循环中
		Process LoopProcess = (iter->second)[0];

		if(LoopProcess.bIsLoop == false/* && isDoLoop == false*/)//非循环动作
		{
			//同一个索引组成员 用同一个时间
			AnimationInfo tmpInfo;
			tmpInfo.fStartTime = dtime;
			tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
			dtime = tmpInfo.fEndTime + m_fAnimationStopTime;

			//遍历每组成员，执行每一个动作
			for (int i=0;i<iter->second.size();i++)
			{
				Process curProcess = (iter->second)[i];
				QString curCode = curProcess.strCode;//功能码
				QString tmpGroupName;//组名
				int imovetype = -1;//平移 旋转
				int iAxis = -1;//轴
				int iMoShi = -1;//相对或绝对位置
				osg::Vec3 vectmpRotateCenter;
				//osg::Vec3 vectmpMoveToPos;
				std::vector<STL*> tmpSTLs;//组模型				
				float fcurvalue = curProcess.fvalue;//动画值
				if(curProcess.iNegative == 1)//
				{
					fcurvalue = -fcurvalue;
				}
				/*	fcurvalue = 0;*/

				//获取动作组参数
				for(int j=0;j<m_vGcodeFileValues.size();j++)
				{
					if(m_vGcodeFileValues[j].strGCode == curCode)//获取运动属性 和 模型数组
					{
						if(curCode == "M1002")
							int aa=1;
						tmpGroupName = m_vGcodeFileValues[j].strGroupName;
						tmpSTLs = GetGroupNodeByGroupName(tmpGroupName);
						if(tmpSTLs.empty())
						{
							QString strWarning;
							strWarning = strWarning.fromLocal8Bit("存在组成员为空!");
							QMessageBox::warning(this,"",strWarning);
							return;
						}
						imovetype = m_vGcodeFileValues[j].iMoveType;
						iAxis = m_vGcodeFileValues[j].iAxis;//轴
						//iMoShi = m_vGcodeFileValues[j].irelative;//相对或绝对位置,
						//vectmpMoveToPos = m_vGcodeFileValues[j].vecMoveToPos;
						if(imovetype == 1)
						{
							vectmpRotateCenter = m_vGcodeFileValues[j].vecRotateCenter;
						}
						break;
					}
				}
				//填加动画
				if(imovetype == 0)//平移
				{
					if(iAxis == 0)//X轴
					{

						tmpInfo.translate.x() = fcurvalue;
						tmpInfo.translate.y() = 0;
						tmpInfo.translate.z() = 0;

						tmpInfo.rotateX = 0;
						tmpInfo.rotateY = 0;
						tmpInfo.rotateZ = 0;

					}
					else if(iAxis == 1)//Y轴
					{
						tmpInfo.translate.y() = fcurvalue;
						tmpInfo.translate.x() = 0;
						tmpInfo.translate.z() = 0;

						tmpInfo.rotateX = 0;
						tmpInfo.rotateY = 0;
						tmpInfo.rotateZ = 0;
					}
					else if(iAxis == 2)//Z轴
					{
						tmpInfo.translate.y() = 0;
						tmpInfo.translate.x() = 0;
						tmpInfo.translate.z() = fcurvalue;

						tmpInfo.rotateX = 0;
						tmpInfo.rotateY = 0;
						tmpInfo.rotateZ = 0;
					}

					for(int k=0;k<tmpSTLs.size();k++)
					{
						tmpSTLs[k]->AddMoveAnimation(tmpInfo);
					}
				}
				else if(imovetype == 1)
				{
					tmpInfo.center3d = vectmpRotateCenter;

					if(iAxis == 0)//X轴
					{
						tmpInfo.rotateX = fcurvalue;
						tmpInfo.rotateY = 0;
						tmpInfo.rotateZ = 0;

						tmpInfo.translate.y() = 0;
						tmpInfo.translate.x() = 0;
						tmpInfo.translate.z() = 0;

					}
					else if(iAxis == 1)//Y轴
					{
						tmpInfo.rotateX = 0;
						tmpInfo.rotateZ = 0;
						tmpInfo.rotateY = fcurvalue;

						tmpInfo.translate.y() = 0;
						tmpInfo.translate.x() = 0;
						tmpInfo.translate.z() = 0;

					}
					else if(iAxis == 2)//Z轴
					{
						tmpInfo.rotateY = 0;
						tmpInfo.rotateX = 0;
						tmpInfo.rotateZ = fcurvalue;

						tmpInfo.translate.y() = 0;
						tmpInfo.translate.x() = 0;
						tmpInfo.translate.z() = 0;
					}

					for(int k=0;k<tmpSTLs.size();k++)
					{
						//tmpSTLs[k]->AddRotateAnimation(tmpInfo);

						//判断中心是否已存在
						std::vector<AnimationInfo> vtmpRotateInfo =tmpSTLs[k]->GetVRotateAnimation();

						if(vtmpRotateInfo.size() == 0)
						{
							tmpSTLs[k]->AddRotateAnimation(tmpInfo);

						}
						else
						{
							vtmpRotateInfo =tmpSTLs[k]->GetVRotateAnimation();
							std::vector<AnimationInfo>::iterator itrs = vtmpRotateInfo.begin();
							for(;itrs!=vtmpRotateInfo.end();itrs++)
							{
								if(itrs->center3d == tmpInfo.center3d)
								{
									break;
								}
							}
							if(itrs != vtmpRotateInfo.end())//存在
							{
								tmpSTLs[k]->AddRotateAnimation(tmpInfo);

							}
							else if(itrs==vtmpRotateInfo.end())
							{
								tmpSTLs[k]->AddSecondRotateAnimation(tmpInfo);

							}
						}
					}
				}

			}
		}
		else if(LoopProcess.bIsLoop == true && isDoLoop == true)
		{
			isDoLoop = false;

			//执行循环动作
			if(m_MapLoopTovLoopvProcess.empty() == false)
			{
				std::map<int,std::vector<std::vector<Process>>>::iterator _iter = m_MapLoopTovLoopvProcess.begin();//弯数To当前弯动作
				for (;_iter!=m_MapLoopTovLoopvProcess.end();_iter++)
				{
					m_vLoopvProcess = _iter->second;//当前弯动作
					for(int j=0;j<m_vLoopvProcess.size();j++)//遍历循环里的每一组
					{

						//同一个索引组成员 用同一个时间
						AnimationInfo tmpInfo;
						tmpInfo.fStartTime = dtime;
						tmpInfo.fEndTime = tmpInfo.fStartTime + m_fAnimationTime;
						dtime = tmpInfo.fEndTime + m_fAnimationStopTime;

						std::vector<Process>::iterator itr = m_vLoopvProcess[j].begin();
						for (;itr!=m_vLoopvProcess[j].end();itr++)//遍历同一索引即同一时间的每个动作
						{

							Process curProcess = (*itr);
							QString curCode = curProcess.strCode;//功能码
							if(curCode == "M1000")
							{
								int a =0;
							}

							QString tmpGroupName;//组名
							int imovetype = -1;//平移 旋转
							int iAxis = -1;//轴
							int iMoShi = -1;//相对或绝对位置
							osg::Vec3 vectmpRotateCenter;
							//osg::Vec3 vectmpMoveToPos;
							std::vector<STL*> tmpSTLs;//组模型				
							float fcurvalue = curProcess.fvalue;//动画值
							if(curProcess.iNegative == 1)
							{
								fcurvalue = -fcurvalue;
							}
							/*fcurvalue = 0;*/
							int iRotatePipe = curProcess.iPipeRotate;
							int iCreatPipe = curProcess.iCreatPipe;
							int iShape = curProcess.iCurveOrStraight;
							float fCurveR = curProcess.fcurveR;

							for(int u=0;u<m_vGcodeFileValues.size();u++)
							{
								if(m_vGcodeFileValues[u].strGCode == curCode)//获取运动属性 和 模型数组
								{
									if(curCode == "M1000")
										int aa=1;

									tmpGroupName = m_vGcodeFileValues[u].strGroupName;
									tmpSTLs = GetGroupNodeByGroupName(tmpGroupName);
									if(tmpSTLs.empty())
									{
										QString strWarning;
										strWarning = strWarning.fromLocal8Bit("存在组成员为空!");
										QMessageBox::warning(this,"",strWarning);
										return;
									}
									imovetype = m_vGcodeFileValues[u].iMoveType;
									iAxis = m_vGcodeFileValues[u].iAxis;//轴
									//iMoShi = m_vGcodeFileValues[j].irelative;//相对或绝对位置,
									//vectmpMoveToPos = m_vGcodeFileValues[j].vecMoveToPos;
									if(imovetype == 1)
									{
										vectmpRotateCenter = m_vGcodeFileValues[u].vecRotateCenter;
									}
									break;
								}
							}


							//添加动画
							if(imovetype == 0)//平移
							{
								if(iAxis == 0)//X轴
								{
									tmpInfo.translate.x() = fcurvalue;
									tmpInfo.translate.y() = 0;
									tmpInfo.translate.z() = 0;

									tmpInfo.rotateX = 0;
									tmpInfo.rotateY = 0;
									tmpInfo.rotateZ = 0;
								}
								else if(iAxis == 1)//Y轴
								{
									tmpInfo.translate.x() = 0;
									tmpInfo.translate.z() = 0;
									tmpInfo.translate.y() = fcurvalue;

									tmpInfo.rotateX = 0;
									tmpInfo.rotateY = 0;
									tmpInfo.rotateZ = 0;
								}
								else if(iAxis == 2)//Z轴
								{
									tmpInfo.translate.x() = 0;
									tmpInfo.translate.y() = 0;
									tmpInfo.translate.z() = fcurvalue;

									tmpInfo.rotateX = 0;
									tmpInfo.rotateY = 0;
									tmpInfo.rotateZ = 0;
								}

								for(int k=0;k<tmpSTLs.size();k++)
								{
									tmpSTLs[k]->AddMoveAnimation(tmpInfo);
								}
							}
							else if(imovetype == 1)//旋转
							{
								tmpInfo.center3d = vectmpRotateCenter;

								if(iAxis == 0)//X轴
								{
									if(fcurvalue>=180 || fcurvalue<=-180)
									{
										m_bIsDivideTwoTimes = true;
										InfoClear(m_DivideInfo1);
										InfoClear(m_DivideInfo2);

										m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
										m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
										m_DivideInfo1.center3d = tmpInfo.center3d;
										m_DivideInfo1.rotateX = fcurvalue/2;

										m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
										m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
										m_DivideInfo2.center3d = tmpInfo.center3d;
										m_DivideInfo2.rotateX = fcurvalue/2;
									}
									else
									{
										m_bIsDivideTwoTimes = false;
										tmpInfo.rotateX = fcurvalue;
										tmpInfo.rotateY = 0;
										tmpInfo.rotateZ = 0;

										tmpInfo.translate.y() = 0;
										tmpInfo.translate.x() = 0;
										tmpInfo.translate.z() = 0;
									}



								}
								else if(iAxis == 1)//Y轴
								{

									if(fcurvalue>=180 || fcurvalue<=-180)
									{
										m_bIsDivideTwoTimes = true;
										InfoClear(m_DivideInfo1);
										InfoClear(m_DivideInfo2);

										m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
										m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
										m_DivideInfo1.center3d = tmpInfo.center3d;
										m_DivideInfo1.rotateY = fcurvalue/2;

										m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
										m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
										m_DivideInfo2.center3d = tmpInfo.center3d;
										m_DivideInfo2.rotateY = fcurvalue/2;
									}
									else
									{
										m_bIsDivideTwoTimes = false;

										tmpInfo.rotateX = 0;
										tmpInfo.rotateZ = 0;
										tmpInfo.rotateY = fcurvalue;

										tmpInfo.translate.y() = 0;
										tmpInfo.translate.x() = 0;
										tmpInfo.translate.z() = 0;
									}





								}
								else if(iAxis == 2)//Z轴
								{
									if(fcurvalue>=180 || fcurvalue<=-180)
									{
										m_bIsDivideTwoTimes = true;
										InfoClear(m_DivideInfo1);
										InfoClear(m_DivideInfo2);

										m_DivideInfo1.fStartTime = tmpInfo.fStartTime;
										m_DivideInfo1.fEndTime = m_DivideInfo1.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
										m_DivideInfo1.center3d = tmpInfo.center3d;
										m_DivideInfo1.rotateZ = fcurvalue/2;

										m_DivideInfo2.fStartTime = m_DivideInfo1.fEndTime;
										m_DivideInfo2.fEndTime = tmpInfo.fEndTime;
										m_DivideInfo2.center3d = tmpInfo.center3d;
										m_DivideInfo2.rotateZ = fcurvalue/2;
									}
									else
									{
										m_bIsDivideTwoTimes = false;
										tmpInfo.rotateY = 0;
										tmpInfo.rotateX = 0;
										tmpInfo.rotateZ = fcurvalue;
										tmpInfo.translate.y() = 0;
										tmpInfo.translate.x() = 0;
										tmpInfo.translate.z() = 0;


									}
								}

								for(int k=0;k<tmpSTLs.size();k++)
								{

									//判断中心是否已存在
									std::vector<AnimationInfo> vtmpRotateInfo =tmpSTLs[k]->GetVRotateAnimation();
									if(vtmpRotateInfo.size() == 0)
									{
										if(m_bIsDivideTwoTimes == false)
										{
											tmpSTLs[k]->AddRotateAnimation(tmpInfo);

										}
										else if(m_bIsDivideTwoTimes == true)
										{
											tmpSTLs[k]->AddRotateAnimation(m_DivideInfo1);
											tmpSTLs[k]->AddRotateAnimation(m_DivideInfo2);								
										}

									}
									else
									{
										vtmpRotateInfo =tmpSTLs[k]->GetVRotateAnimation();
										std::vector<AnimationInfo>::iterator itrs = vtmpRotateInfo.begin();
										for(;itrs!=vtmpRotateInfo.end();itrs++)
										{
											if(itrs->center3d == tmpInfo.center3d)
											{
												break;
											}
										}
										if(itrs != vtmpRotateInfo.end())//存在
										{

											//tmpSTLs[k]->AddRotateAnimation(tmpInfo);
											if(m_bIsDivideTwoTimes == false)
											{
												tmpSTLs[k]->AddRotateAnimation(tmpInfo);

											}
											else if(m_bIsDivideTwoTimes == true)
											{
												tmpSTLs[k]->AddRotateAnimation(m_DivideInfo1);
												tmpSTLs[k]->AddRotateAnimation(m_DivideInfo2);								
											}

										}
										else if(itrs==vtmpRotateInfo.end())
										{
											//tmpSTLs[k]->AddSecondRotateAnimation(tmpInfo);
											if(m_bIsDivideTwoTimes == false)
											{
												tmpSTLs[k]->AddSecondRotateAnimation(tmpInfo);

											}
											else if(m_bIsDivideTwoTimes == true)
											{
												tmpSTLs[k]->AddSecondRotateAnimation(m_DivideInfo1);
												tmpSTLs[k]->AddSecondRotateAnimation(m_DivideInfo2);								
											}

										}
									}

								}
							}


							//管子
							if(iRotatePipe == 1)//转动管子
							{
								PipeRotateX(tmpInfo,PipeInfo,fcurvalue,_iter->first);
							}
							else if(iRotatePipe == 0)
							{
								if(iCreatPipe == 1)//创建管子
								{
									if(iShape == 1)//直管
									{

										ShowStraightPipe(m_PipeCenter,osg::Vec3(0,0,-1),tmpInfo,PipeInfo,_iter->first,fcurvalue,_iter->first+1);
									}
									else if(iShape == 0)//弯管
									{
										osg::Vec3 pipedir;

										if(m_vproductionvalues[_iter->first].flr == 1)
										{
											pipedir = osg::Vec3(0.0,-1.0,0.0);
											m_bIsLeft = true;

										}
										else if(m_vproductionvalues[_iter->first].flr == 0)
										{
											pipedir = osg::Vec3(0.0,1.0,0.0);
											m_bIsLeft = false;

										}
										ShowCurvePipe(_iter->first,m_PipeCenter,pipedir,tmpInfo,PipeInfo,fCurveR,fcurvalue,true);

									}
								}
								else if(iCreatPipe == 0)
								{

								}
							}
						}
					}
				}
			}
		}
	}

	Play();
#endif
}

void MainWindow::SetSegStartTimeMap()
{
	if (!m_mSegStartTime.empty())
	{
		m_mSegStartTime.clear();
	}

	for (size_t i = 0; i < m_vMatrixAnimation.size();++i)
	{
		const AnimationInfo& info = m_vMatrixAnimation[i].second;
		if (info.pipeSeg == -1)
		{
			continue;
		}

		if (m_mSegStartTime.find(info.pipeSeg)==m_mSegStartTime.end())
		{
			m_mSegStartTime[info.pipeSeg] = info.fStartTime;
		}
		else
		{
			if (m_mSegStartTime[info.pipeSeg]>info.fStartTime)
			{
				m_mSegStartTime[info.pipeSeg] = info.fStartTime;
			}
		}
	}
}

osg::ref_ptr<osg::AnimationPathCallback> getModelMoveCallBack1(float start,float end,double x,double y,double z)
{
	double _lastPos_X = 0.0;
	double _currentPos_X = 0.0;
	double _lastPos_Y = 0.0;
	double _currentPos_Y = 0.0;
	double _lastPos_Z = 0.0;
	double _currentPos_Z = 0.0;

	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);

	for (int i = 0; i < 1;++i)
	{
		if(x != 0)
		{
			if(i == 0)
			{
				pAnimationPath->insert(start, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
				_lastPos_X = 0;
				_lastPos_Y = 0;
				_lastPos_Z = 0;

			}
			else
			{
				pAnimationPath->insert(start, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			}

			//_currentPos_X += info.translate.x();
			//_lastPos_X = _currentPos_X;
			_currentPos_X += x;
			_currentPos_Y += y;
			_currentPos_Z += z;
			_lastPos_X = _currentPos_X;
			_lastPos_Y = _currentPos_Y;
			_lastPos_Z = _currentPos_Z;
			pAnimationPath->insert(end, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos_X, _currentPos_Y, _currentPos_Z), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}
		if(y != 0)
		{
			if(i == 0)
			{
				pAnimationPath->insert(start, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
				_lastPos_X = 0;
				_lastPos_Y = 0;
				_lastPos_Z = 0;
			}
			else
			{
				pAnimationPath->insert(start, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			}

			//_currentPos_Y += info.translate.y();
			//_lastPos_Y = _currentPos_Y;
			_currentPos_X += x;
			_currentPos_Y += y;
			_currentPos_Z += z;
			_lastPos_X = _currentPos_X;
			_lastPos_Y = _currentPos_Y;
			_lastPos_Z = _currentPos_Z;
			pAnimationPath->insert(end, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos_X, _currentPos_Y, _currentPos_Z), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}
		if(z != 0)
		{
			if(i == 0)
			{
				pAnimationPath->insert(start, osg::AnimationPath::ControlPoint(osg::Vec3(0.0, 0.0, 0.0), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
				_lastPos_X = 0;
				_lastPos_Y = 0;
				_lastPos_Z = 0;
			}
			else
			{
				pAnimationPath->insert(end, osg::AnimationPath::ControlPoint(osg::Vec3(_lastPos_X, _lastPos_Y, _lastPos_Z), 
					osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
			}

			//_currentPos_Z += info.translate.z();
			//_lastPos_Z = _currentPos_Z;
			_currentPos_X += x;
			_currentPos_Y += y;
			_currentPos_Z += z;
			_lastPos_X = _currentPos_X;
			_lastPos_Y = _currentPos_Y;
			_lastPos_Z = _currentPos_Z;
			pAnimationPath->insert(end, osg::AnimationPath::ControlPoint(osg::Vec3(_currentPos_X, _currentPos_Y, _currentPos_Z), 
				osg::Quat(0.0, osg::X_AXIS, 0.0, osg::Vec3f(0.0,-1.0,0.0), 0.0, osg::Z_AXIS)));
		}
	}

	//创建更新动画回调
	return new osg::AnimationPathCallback(pAnimationPath.get());
}


osg::ref_ptr<osg::AnimationPathCallback> getModelRotateCallBack1(osg::Vec3d vecCenter,double rotateY,float fStartTime,float fEndTime)
{
	osg::ref_ptr<osg::AnimationPath> pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
	osg::ref_ptr<osg::AnimationPathCallback> nc;
	std::vector<osg::ref_ptr<osg::AnimationPathCallback>> vnc;
	double _currentPos = 0.0;
	double _currentAngleX = 0.0;
	double _currentAngleY = 0.0;
	double _currentAngleZ = 0.0;
	double _lastAngleX = 0.0;
	double _lastAngleY = 0.0;
	double _lastAngleZ = 0.0;

	double _wheelRadius = 0.0;

	for (int i = 0; i < 1;++i)
	{
		//if(info.iAnimationType == 2)//旋转
		{

			if(rotateY != 0)
			{
				// 沿Y轴旋转
				if(i==0)
				{
					pAnimationPath->insert(fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));
				}
				else
				{

					pAnimationPath->insert(fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
						osg::Quat( osg::DegreesToRadians(_lastAngleY), osg::Vec3f(0.0,-1.0,0.0))));

				}


				/*pAnimationPath->insert(info.fStartTime, osg::AnimationPath::ControlPoint(vecCenter, 
				osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));*/

				_currentAngleY += rotateY;
				_lastAngleY = _currentAngleY;
				pAnimationPath->insert(fEndTime, osg::AnimationPath::ControlPoint(vecCenter, 
					osg::Quat( osg::DegreesToRadians(_currentAngleY), osg::Vec3f(0.0,-1.0,0.0))));
			}
		}	
	}	

	nc = new osg::AnimationPathCallback(pAnimationPath);
	nc->setPivotPoint(vecCenter);
	return nc;
}

#if 0
void MainWindow::Play()
{
	SetSegStartTimeMap();

	//刷新颜色
	m_sceneModel->GetHudtext()->ResetAllColor();
	m_productionTable->ResetAllColor();
	m_twTable->ResetAllColor();
	//使能暂停键
	ui.actionLight->setEnabled(true);
	//取消暂停选中状态
	emit sigPartsPause(true);
	ui.actionLight->setChecked(false);

	for (int a=0;a<m_vAccumulateFileSTLs.size();a++)
	{
		//m_vAccumulateFileSTLs.
	}

	osg::ref_ptr<osg::Group> pipefatherRight = m_sceneModel->getPipeFatherRight();//管子父节点
	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrixRight = m_sceneModel->getPipeFatherMatrixRight();//管子父节点下的变化矩阵
	//连接管子矩阵
	if(m_vPipes.size() >= 2)
	{
		for (int i = m_vPipes.size()-1;i>=1;--i)
		{
			m_vPipes[i].GetMoveMatrix()->addChild(m_vPipes[i-1].GetRotateMatrix());
		}
		osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
		pipefatherMatrixRight->addChild(m_vPipes[m_vPipes.size() -1].GetRotateMatrix());//绑定管子最后一届到管子矩阵根节点上
		((osg::Group*)nodestl)->addChild(pipefatherRight);
	}

	//设置管子动画
	for (int i= 0; i < m_vMatrixAnimation.size();++i)
	{
		std::vector<AnimationInfo> tempVector;//
		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vMatrixAnimation[i].first;

		tempVector.push_back(m_vMatrixAnimation[i].second);
		osg::ref_ptr<osg::Geode> group;
		for (int j= 0; j < m_vPipeBackMatrixAnimation.size();++j)
		{
			if(matrix == m_vPipeBackMatrixAnimation[j].first)
			{
				tempVector.push_back(m_vPipeBackMatrixAnimation[j].second);
				group = m_vPipeBackMatrixAnimation[j].second.group;
			}
		}

		int pipeSeg = m_vMatrixAnimation[i].second.pipeSeg;
		int pipeSegTw = m_vMatrixAnimation[i].second.pipeSegTw;
		double s = m_vMatrixAnimation[i].second.fColorStartTime;
		double e = m_vMatrixAnimation[i].second.fColorEndTime;
		bool istw = m_vMatrixAnimation[i].second.isTW;
		if(tempVector.size()>1)
		{
			matrix->setUpdateCallback(getBackPipePathCallBack(tempVector));		
			osg::ref_ptr<PipeHide> hide = new PipeHide(tempVector.crbegin()->fEndTime,group);
			matrix->addUpdateCallback(hide);
			//osg::ref_ptr<osg::Geode> Gnode = m_vMatrixAnimation[i].second.group;
			//Gnode->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(), osg::StateAttribute::OFF);
			//m_vLongStraightPipAnimation.push_back();
		}
		else
		{
			osg::ref_ptr<osg::Geode> Gnode = m_vMatrixAnimation[i].second.group;
			matrix->setUpdateCallback(getPipePathCallBack(tempVector));
			osg::ref_ptr<PipeShow> pshow = new PipeShow(tempVector.crbegin()->fEndTime,Gnode,m_sceneModel->getClipPlane());
			matrix->addUpdateCallback(pshow);
		}
		if(s!=-1 && e!=-1 && pipeSeg!=-1)
		{

			osg::ref_ptr<PipeInfoCallback> tmp = new PipeInfoCallback(s,e,pipeSeg,pipeSegTw,istw,this,m_bAutoPlaning);
			matrix->addUpdateCallback(tmp);
		}
	}

	//長直管显示时间
	for (int pi=0;pi<m_vLongStraightPipeMaxtrixAnimation.size();pi++)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vLongStraightPipeMaxtrixAnimation[pi].first;
		std::vector<AnimationInfo> vtmp;
		vtmp.push_back(m_vLongStraightPipeMaxtrixAnimation[pi].second);
		matrix->setUpdateCallback(getLongStaightPipePathCallBack(m_vLongStraightPipAnimation));

		osg::ref_ptr<LongStraightPipe> longStraight = new LongStraightPipe(m_vLongStraightPipeMaxtrixAnimation[pi].second.fStartTime,m_pstraiPipe,
			matrix,m_vPipes,m_sceneModel->getClipPlane());
		matrix->addUpdateCallback(longStraight);
	}

	for(int i=0;i<m_vSTLs.size();i++)
	{
		if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("jitouwanguan1.stl") /*|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("dj2.1.stl")*/)
		{
			m_vJitouwanguanAnims = m_vSTLs[i]->GetVMoveAnimation();
			//continue;
			CombineSamePeriodAnimation(m_vJitouwanguanAnims);
		}

		if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit(m_strLunmoName.toStdString().c_str()) /*|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("dj2.1.stl")*/)
		{
			m_vLunMoAnims = m_vSTLs[i]->GetVMoveAnimation();
			//continue;

			CombineSamePeriodAnimation(m_vLunMoAnims);
		}
	}

	//模型动画
	for(int i=0;i<m_vSTLs.size();i++)
	{
		if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		{
			int a = 0;
#if 0
			osg::ref_ptr<osg::MatrixTransform> moveMatrix = m_vSTLs[i]->GetModelMoveMatrix();
			osg::ref_ptr<osg::MatrixTransform> rotateMatrix = m_vSTLs[i]->GetModelSecondRotateMatrix();

			std::vector<AnimationInfo> Vmove = m_vSTLs[i]->GetVMoveAnimation();
			CombineSamePeriodAnimation(Vmove);

			std::vector<AnimationInfo> Vrotate = m_vSTLs[i]->GetVRotateAnimation();
			SetTransMatrixNew(m_vSTLs[i],Vmove,Vrotate);

			osg::ref_ptr<osg::MatrixTransform> mtRotate00 = new osg::MatrixTransform();

			//OSG确保只有STATIC数据可以进行图形渲染
			//mt->setMatrix(osg::Matrix::translate( -bs.center()) * osg::Matrix::scale(size, size, size) * osg::Matrix::rotate(osg::inDegrees(-180.0f), 0.0f,0.0f,1.0f));
			// mtRotate00->addChild(zhujia.get());

			mtRotate00->setUpdateCallback(getModelMoveCallBack1(0.0,5.0,110.0,0.0,0.0));

			osg::ref_ptr<osg::MatrixTransform> mtRotate0 = new osg::MatrixTransform();
			//osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
			//设置更新回调
			mtRotate0->setUpdateCallback(getModelRotateCallBack1(osg::Vec3d(110.0,0.0,0.0),180.0,5.0,10.0));
			//pat->addChild(mt.get());

			mtRotate0->addChild(mtRotate00);

			osg::ref_ptr<osg::MatrixTransform> mtRotate1 = new osg::MatrixTransform();
			mtRotate1->setUpdateCallback(getModelMoveCallBack1(10.0,20.0,-110.0,0.0,0.0));
			mtRotate1->addChild(mtRotate0);

			osg::ref_ptr<osg::MatrixTransform> mtRotate2 = new osg::MatrixTransform();
			osg::Vec3d center1 =  osg::Vec3d(50.0,0.0,0.0);
			mtRotate2->setUpdateCallback(getModelRotateCallBack1(center1,-180.0,25.0,30.0));

			mtRotate2->addChild(mtRotate1);

			osg::ref_ptr<osg::MatrixTransform> mtRotate3 = new osg::MatrixTransform();
			osg::Vec3d center2 = osg::Vec3d(80.0,0.0,0.0);
			mtRotate3->setUpdateCallback(getModelRotateCallBack1(center2,180.0,35.0,40.0));
			mtRotate3->addChild(mtRotate2);

			moveMatrix->removeChild(rotateMatrix);
			moveMatrix->addChild(mtRotate3);
			mtRotate00->addChild(rotateMatrix);
#endif
		}

		if (m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("lm1.stl"))
		{
#if 0
			osg::ref_ptr<osg::MatrixTransform> moveMatrix = m_vSTLs[i]->GetModelMoveMatrix();
			osg::ref_ptr<osg::MatrixTransform> rotateMatrix = m_vSTLs[i]->GetModelSecondRotateMatrix();

			std::vector<AnimationInfo> Vmove = m_vSTLs[i]->GetVMoveAnimation();
			CombineSamePeriodAnimation(Vmove);

			std::vector<AnimationInfo> Vrotate = m_vSTLs[i]->GetVRotateAnimation();
			SetTransMatrixNew(m_vSTLs[i],Vmove,Vrotate);


			osg::ref_ptr<osg::MatrixTransform> mtMove0 = new osg::MatrixTransform();
			//mtMove0->addChild(lm);

			std::map<double,osg::Vec3d> mapTimePos;
			osg::ref_ptr<osg::MatrixTransform> mtMove1 = new osg::MatrixTransform();
			mtMove1->setUpdateCallback(getModelMoveCallBack1(0.0,5.0,110.0,0.0,0.0));
			//lmCenterPos += osg::Vec3d(110,0.0,0.0);
			mapTimePos[0.0]=osg::Vec3d(110,0.0,0.0);

			mtMove1->addChild(mtMove0);

			osg::ref_ptr<osg::MatrixTransform> mtMove2 = new osg::MatrixTransform();
			mtMove2->setUpdateCallback(getModelMoveCallBack1(20.0,25.0,-60.0,0.0,0.0));
			mapTimePos[20.0] = osg::Vec3d(50.0,0.0,0.0);
			mtMove2->addChild(mtMove1);

			osg::ref_ptr<osg::MatrixTransform> mtMove3 = new osg::MatrixTransform();
			mtMove3->setUpdateCallback(getModelMoveCallBack1(30.0,35.0,30.0,0.0,0.0));
			mapTimePos[30.0] = osg::Vec3d(80.0,0.0,0.0);

			mtMove3->addChild(mtMove2);

			moveMatrix->removeChild(rotateMatrix);
			moveMatrix->addChild(mtMove3);
			mtMove0->addChild(rotateMatrix);
#endif
		}

		osg::ref_ptr<osg::MatrixTransform> matrix = m_vSTLs[i]->GetModelMatrix();
		osg::ref_ptr<osg::MatrixTransform> Rotatematrix = m_vSTLs[i]->GetModelRotateMatrix();
		osg::ref_ptr<osg::MatrixTransform> Movematrix = m_vSTLs[i]->GetModelMoveMatrix();
		//osg::ref_ptr<osg::MatrixTransform> Movematrix = m_vSTLs[i]->GetPosMatrix();

		std::vector<AnimationInfo> Vmove = m_vSTLs[i]->GetVMoveAnimation();
		CombineSamePeriodAnimation(Vmove);
		//if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_1.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		//{
		//	Vmove.clear();
		//}
		std::vector<AnimationInfo> Vrotate = m_vSTLs[i]->GetVRotateAnimation();
		//if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_1.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		//{
		//	std::vector<AnimationInfo> temp = Vrotate;
		//	Vrotate.clear();
		//	for (int i = 0; i < 1;++i)
		//	{
		//		Vrotate.push_back(temp[i]);
		//	}
		//}
		std::vector<AnimationInfo> VSecondrotate = m_vSTLs[i]->GetSecondVRotateAnimation();

		// both move and rotate
		//if (Vmove.size()!=0 && Vrotate.size()!=0)
		//{
		//	int m = 0;
		//}

#if 0
		if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("youdjdz.stl") /*|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("dj2.1.stl")*/)
		{
			QString information;
			for (int m = 0; m < Vmove.size();++m)
			{
				AnimationInfo info = Vmove[m];

				information.append("move start time ");
				information.append(QString::number(info.fStartTime));
				LogInfo(information);
				information.clear();

				information.append("end time ");
				information.append(QString::number(info.fEndTime));
				LogInfo(information);
				information.clear();

				information.append("pos x ");
				information.append(QString::number(info.translate.x()));
				LogInfo(information);
				information.clear();

				information.append("pos y ");
				information.append(QString::number(info.translate.y()));
				LogInfo(information);
				information.clear();

				information.append("pos z ");
				information.append(QString::number(info.translate.z()));
				LogInfo(information);
				information.clear();

				information.append(QString::fromLocal8Bit(info.strProcess.c_str()));
				LogInfo(information);
				information.clear();
			}

			for (int m = 0; m < Vrotate.size();++m)
			{
				AnimationInfo info = Vrotate[m];

				information.append("rotate start time ");
				information.append(QString::number(info.fStartTime));
				LogInfo(information);
				information.clear();

				information.append("end time ");
				information.append(QString::number(info.fEndTime));
				LogInfo(information);
				information.clear();

				information.append("rot x ");
				information.append(QString::number(info.rotateX));
				LogInfo(information);
				information.clear();

				information.append("rot y ");
				information.append(QString::number(info.rotateY));
				LogInfo(information);
				information.clear();

				information.append("rot z ");
				information.append(QString::number(info.rotateZ));
				LogInfo(information);
				information.clear();

				information.append(QString::fromLocal8Bit(info.strProcess.c_str()));
				LogInfo(information);
				information.clear();
			}
		}
#endif

		bool bFlage = true;
		// 机头弯管旋转的时候需要同步旋转的模型
		if (m_MapGroupToGroupChildren.find("ZhuJia_Y")!=m_MapGroupToGroupChildren.end())
		{
			std::vector<QString> children= m_MapGroupToGroupChildren.find("ZhuJia_Y")->second.vChildrenNames;
			for (int k = 0; k < children.size();++k)
			{
				if (children[k]==m_vSTLs[i]->GetSTLName() && (!Vmove.empty() || !Vrotate.empty()))
				{
					//SetTransMatrix(m_vSTLs[i],Vmove,Vrotate);
					SetTransMatrixNew(m_vSTLs[i],Vmove,Vrotate);
					bFlage = false;
					break;
				}
			}
		}

		//if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_1.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		//if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj3.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj4.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zhujiazuo.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("tuiwanlun.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		//{
		//for (int i = 0; i < Vrotate.size();++i)
		//{
		//	AnimationInfo& anim = Vrotate[i];
		//	//anim.center3d -= osg::Vec3(-110.0,0.0,0.0);
		//}

		//Rotatematrix->setUpdateCallback(getModelMoveRotateCallBack(Vmove,Vrotate));
		//osg::Transform::ReferenceFrame frame = Rotatematrix->getReferenceFrame();
		//Rotatematrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		//SetTransMatrix(m_vSTLs[i],Vmove,Vrotate);

#if 0
		//Rotatematrix->setUpdateCallback(getModelRotateCallBack(Vrotate));
		std::vector<AnimationInfo> left,mid,right,test;
		MoveCenter(110.0,110.0,Vmove,Vrotate,left,mid,right,test);
		if (!right.empty())
		{
			Rotatematrix->setUpdateCallback(getModelRotateCallBack(right));
		}

		if (!left.empty())
		{
			m_vSTLs[i]->GetModelLeftRotateMatrix()->setUpdateCallback(getModelRotateCallBack(left));
			// m_vSTLs[i]->GetModelLeftRotateMatrix()->setUpdateCallback(getModelMoveRotateCallBack(left));
			//ModelCenterMove* moveCallBack = new ModelCenterMove(left[0].center3d,Vmove,Vrotate);
			//m_vSTLs[i]->GetModelLeftRotateMatrix()->addUpdateCallback(moveCallBack);
		}

		if (!mid.empty())
		{
			m_vSTLs[i]->GetModelMidRotateMatrix()->setUpdateCallback(getModelRotateCallBack(mid));
		}

		if (!test.empty())
		{
			m_vSTLs[i]->GetModelTestRotateMatrix()->setUpdateCallback(getModelRotateCallBack(test));
		}

		//Movematrix = m_vSTLs[i]->GetPosMatrix();

		if(Vmove.size()!=0)
		{
			Movematrix->setUpdateCallback(getModelMoveCallBackNew(Vmove));
		}
#endif 

		//}
		//else
		if (bFlage && (!Vmove.empty() || !Vrotate.empty())) // 模型旋转是否因为中心变换的原因已经被替代？
		{
			if(Vmove.size()!=0)
			{
				Movematrix->setUpdateCallback(getModelMoveCallBackNew(Vmove));
			}

			if(Vrotate.size()!=0)
			{
				Rotatematrix->setUpdateCallback(getModelRotateCallBack(Vrotate));
			}

#if 0
			std::map<float,AnimationInfo> mapTimeAnims;
			for (int m = 0; m < Vmove.size();++m)
			{
				AnimationInfo info = Vmove[m];
				info.iAnimationType = 1;
				mapTimeAnims[info.fStartTime] = info;
			}

			for (int m = 0; m < Vrotate.size();++m)
			{
				AnimationInfo& info = Vrotate[m];
				info.iAnimationType = 2;
				mapTimeAnims[info.fStartTime] = info;
			}

			std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = m_vSTLs[i]->GetTransMatrix();
			std::map<float,AnimationInfo>::iterator iter = mapTimeAnims.begin();
			for (;iter!=mapTimeAnims.end();++iter)
			{
				std::vector<AnimationInfo> infos;
				AnimationInfo info = iter->second;
				osg::ref_ptr<osg::MatrixTransform> tempMatrix = new osg::MatrixTransform;
				// 旋转,靠近旋转轴

				infos.push_back(info);

				if (info.iAnimationType==1)
				{
					tempMatrix->setUpdateCallback(getModelMoveCallBackNew(infos));
				}
				else if (info.iAnimationType==2)
				{
					tempMatrix->setUpdateCallback(getModelRotateCallBack(infos));
				}

				trans.push_back(tempMatrix);
			}

			if (trans.empty())
			{
				return;
			}

			for (int m = 1; m < trans.size();++m)
			{
				trans[m]->addChild(trans[m-1]);
			}

			osg::ref_ptr<osg::MatrixTransform> moveMatrix = m_vSTLs[i]->GetModelMoveMatrix();
			osg::ref_ptr<osg::MatrixTransform> rotateMatrix = m_vSTLs[i]->GetModelSecondRotateMatrix();

			moveMatrix->removeChild(rotateMatrix);
			moveMatrix->addChild(trans[trans.size()-1]);
			trans[0]->addChild(rotateMatrix);
#endif
			//if(Vmove.size()!=0)
			//{
			//	Movematrix->setUpdateCallback(getModelMoveCallBackNew(Vmove));
			//}
		}

		if(VSecondrotate.size()!=0)
		{
			osg::ref_ptr<osg::MatrixTransform> SecondRotatematrix = m_vSTLs[i]->GetModelSecondRotateMatrix();

			SecondRotatematrix->setUpdateCallback(getModelSecondRotateCallBack(VSecondrotate));
		}
	}

	SetCollisionDetectParts();//設置碰撞對象
	startTimer(dtime);

	m_sceneView->UpdateView();
}
#endif

void MainWindow::CombinePipesAnimations()
{
	m_vPipesMatrixAnimation.clear();
	std::map<float,pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> tempMap;

	for (size_t i = 0; i < m_vMatrixAnimation.size();++i)
	{
		pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>& pairs = m_vMatrixAnimation[i];
		tempMap[pairs.second.fStartTime] = pairs;
	}

	for (size_t i = 0; i < m_vPipeBackMatrixAnimation.size();++i)
	{
		pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>& pairs = m_vPipeBackMatrixAnimation[i];
		tempMap[pairs.second.fStartTime] = pairs;
	}

	std::map<float,pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>>::iterator iter = tempMap.begin();
	for (; iter != tempMap.end();++iter)
	{
		m_vPipesMatrixAnimation.push_back(iter->second);
	}
}

void MainWindow::CombineSameMatrix(const std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>>& pairs)
{
	std::vector<AnimationInfo> infos;
	std::vector<bool> flags;
	flags.resize(pairs.size(),false);
	for (size_t i = 0; i < pairs.size();++i)
	{
		if (flags[i])
		{
			continue;
		}

		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = pairs[i].first;
		AnimationInfo info = pairs[i].second;
		infos.clear();
		infos.push_back(pairs[i].second);
		for (size_t j = i+1; j < pairs.size();++j)
		{
			if (!flags[j])
			{
				if (matrix == pairs[j].first)
				{
					infos.push_back(pairs[j].second);
					flags[j] = true;
				}
			}
		}

		m_vMatrixAnimations.push_back(make_pair(matrix,infos));
	}
}

void MainWindow::PlayNew()
{
	SetSegStartTimeMap();

	//刷新颜色
	m_sceneModel->GetHudtext()->ResetAllColor();
	m_productionTable->ResetAllColor();
	m_twTable->ResetAllColor();
	//使能暂停键
	ui.actionLight->setEnabled(true);
	//取消暂停选中状态
	emit sigPartsPause(true);

	// 设置可以碰撞检测
	CollisionCallBack::SetPause(false);

	ui.actionLight->setChecked(false);

	for (size_t a=0;a<m_vAccumulateFileSTLs.size();a++)
	{
		//m_vAccumulateFileSTLs.
	}

	osg::ref_ptr<osg::Group> pipefatherRight = m_sceneModel->getPipeFatherRight();//管子父节点
    osg::ref_ptr<osg::Group> pipefatherLeft = m_sceneModel->getPipeFatherLeft();//管子父节点

    //管子父节点下的变化矩阵
	osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrixRight = m_sceneModel->getPipeFatherMatrixRight();
    osg::ref_ptr<osg::PositionAttitudeTransform> pipefatherMatrixLeft = m_sceneModel->getPipeFatherMatrixLeft();

	//连接管子矩阵
    osg::ref_ptr<osg::Group> left = nullptr;
    osg::ref_ptr<osg::Group> right = nullptr;

	for (size_t i = 0;i <m_vPipes.size();++i)
	{
			// m_vPipes[i].GetMoveMatrix()->addChild(m_vPipes[i-1].GetRotateMatrix());
		osg::ref_ptr<PIPE> pipe = m_vPipes[i];
			
            if (pipe->GetTip()==0)
            {
                // 弯出的管子在长管右侧
                if (right == nullptr)
                {
                    right = pipe->GetRotateMatrix();
                    //right = pipe.GetPipeRootNode();
                }
                else
                {
                    pipe->GetMoveMatrix()->addChild(right);
                    right = pipe->GetRotateMatrix();
                    //right = pipe.GetPipeRootNode();
                }

            }
            else if (pipe->GetTip()==1)
            {
                // 弯出的管子在长管左侧
                if (left == nullptr)
                {
                    left = pipe->GetRotateMatrix();
                    //left = pipe.GetPipeRootNode();
                }
                else
                {
                    pipe->GetMoveMatrix()->addChild(left);
                    left = pipe->GetRotateMatrix();
                    //left = pipe.GetPipeRootNode();
                }
            }
            else
            {
                QString info("Unknown tip type : ");
                info.append(QString::number(pipe->GetTip()));
                QMessageBox::warning(NULL, "warning", info, QMessageBox::Yes);
            }

			// 管子添加碰撞检测回调
			// osg::ref_ptr<osg::Node> node = pipe.GetPipeNode();
			//osg::ref_ptr<osg::PositionAttitudeTransform> node = pipe.GetRotateMatrix();
			//QString name = pipe.GetName();
			//node->setName(name.toStdString().append(" pos"));

			//osg::ref_ptr<CollisionCallBack> call = new CollisionCallBack(name.toStdString());
			//node->setUpdateCallback(call);
	}

	osg::Node* nodestl = m_sceneModel->getNode("NodeSTL");
	//osg::Node* nodestl = m_sceneModel->GetNodeScene();

    if (right!=nullptr)
    {
        pipefatherMatrixRight->addChild(right);//绑定管子最后一截到管子矩阵根节点上
		((osg::Group*)nodestl)->addChild(pipefatherRight);
    }

    if (left!=nullptr)
    {
        pipefatherMatrixLeft->addChild(left);//绑定管子最后一截到管子矩阵根节点上
		((osg::Group*)nodestl)->addChild(pipefatherLeft);
    }

    // 删除非当前机头遮挡平面的作用
    for (size_t i = 0;i <m_vPipes.size();++i)
    {
		osg::ref_ptr<PIPE> p = m_vPipes[i];
        // 机头编号从1开始
        int iHeader = p->GetHeader()-1;
        int size = m_sceneModel->getClipPlaneSize();
        for (int j = 0; j < size;++j)
        {
            if (j!=iHeader)
            {
                p->GetPipeNode()->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(j), osg::StateAttribute::OFF);
            }
        }
    }

	// 更新回退管子的节点矩阵,有些管子被删除了，节点也要更新
	for (size_t i = 0; i < m_vPipeBackMatrixAnimation.size();++i)
	{
		std::pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>& pairs = m_vPipeBackMatrixAnimation[i];
		if (m_mapMatrixInstead.find(pairs.first)!=m_mapMatrixInstead.end())
		{
			pairs.first = m_mapMatrixInstead.find(pairs.first)->second;
		}
	}

	for (size_t i = 0; i < m_vMatrixAnimation.size();++i)
	{
		std::pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>& pairs = m_vMatrixAnimation[i];
		if (m_mapMatrixInstead.find(pairs.first)!=m_mapMatrixInstead.end())
		{
			pairs.first = m_mapMatrixInstead.find(pairs.first)->second;
		}
	}

	CombinePipesAnimations();

	CombineSameMatrix(m_vMatrixAnimation);


	//设置管子动画
	for (size_t i= 0; i < m_vMatrixAnimations.size();++i)
	{
		#if 0
		QString information;
		for (size_t j = 0; j < m_vMatrixAnimations[i].second.size();++j)
		{
			AnimationInfo info = m_vMatrixAnimations[i].second[j];

			information.append("pipe start time ");
			information.append(QString::number(info.fStartTime));
			LogInfo(information);
			information.clear();

			information.append("end time ");
			information.append(QString::number(info.fEndTime));
			LogInfo(information);
			information.clear();

			information.append("move x ");
			information.append(QString::number(info.translate.x()));
			LogInfo(information);
			information.clear();

			information.append("move y ");
			information.append(QString::number(info.translate.y()));
			LogInfo(information);
			information.clear();

			information.append("move z ");
			information.append(QString::number(info.translate.z()));
			LogInfo(information);
			information.clear();

			information.append("rotate x ");
			information.append(QString::number(info.rotateX));
			LogInfo(information);
			information.clear();

			information.append("rotate y ");
			information.append(QString::number(info.rotateY));
			LogInfo(information);
			information.clear();

			information.append("rotate z ");
			information.append(QString::number(info.rotateZ));
			LogInfo(information);
			information.clear();

			information.append(QString::fromLocal8Bit(info.strProcess.c_str()));
			LogInfo(information);
			information.clear();
		}
#endif

		std::vector<AnimationInfo> tempVector;//
		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vMatrixAnimations[i].first;
		int iheader = -1;
		if (m_mapMatrixHeader.find(matrix)!=m_mapMatrixHeader.end())
		{
			iheader = m_mapMatrixHeader[matrix];
		}
		else
		{
			QMessageBox::warning(NULL, "warning", "Find header from matrix error !", QMessageBox::Yes);

			return;
		}

		tempVector = m_vMatrixAnimations[i].second;
		osg::ref_ptr<osg::Geode> group;
		PIPE* pipe = NULL;
		for (size_t j= 0; j < m_vPipeBackMatrixAnimation.size();++j)
		{
			if(matrix == m_vPipeBackMatrixAnimation[j].first)
			{
				tempVector.push_back(m_vPipeBackMatrixAnimation[j].second);
				group = m_vPipeBackMatrixAnimation[j].second.group;
				pipe = m_vPipeBackMatrixAnimation[j].second.pipe;
			}
		}

		std::map<float,AnimationInfo> tempMap;

		for (size_t m = 0; m < tempVector.size();++m)
		{
			AnimationInfo info = tempVector[m];
			tempMap[info.fStartTime] = info;
		}
		tempVector.clear();

		std::map<float,AnimationInfo>::iterator iter = tempMap.begin();
		for (;iter!= tempMap.end();++iter)
		{
			tempVector.push_back(iter->second);
		}

		double s = m_vMatrixAnimations[i].second[0].fColorStartTime;
		double e = m_vMatrixAnimations[i].second[0].fColorEndTime;
		bool istw = m_vMatrixAnimations[i].second[0].isTW;

		int pipeSeg = m_vMatrixAnimations[i].second[0].pipeSeg;
		int pipeSegTw = m_vMatrixAnimations[i].second[0].pipeSegTw;
		//if (istw && m_vMatrixAnimation[i].second.pipeSegTw!=-1
		//	/*&& m_vMatrixAnimation[i].second.pipeSeg == -1*/)
		//{
		//	pipeSeg = m_vMatrixAnimation[i].second.pipeSegTw;
		//}
		//else
		//{
		//	pipeSeg = m_vMatrixAnimation[i].second.pipeSeg;
		//	istw = false;
		//}
		if(tempVector.size()>1 
			&& tempVector[0].rotateY==0.0
			&& tempVector[0].rotateZ==0.0
			&& tempVector.crbegin()->rotateY==0.0
			&& tempVector.crbegin()->rotateZ==0.0)
		{
			// 直管前进，后退的情况
			DevideInfo(tempVector);
			matrix->setUpdateCallback(getBackPipePathCallBack(tempVector));	
			// 并不是有回退的管子一定要隐藏，要判断什么样的管子才需要隐藏

			double pipeLen = -1.0;
			bool isHide = false;
			for (size_t m = 0; m < m_vPipes.size();++m)
			{
				osg::ref_ptr<PIPE> p = m_vPipes[m];
				if (p->GetMoveMatrix()==matrix || p->GetRotateMatrix()== matrix)
				{
					pipeLen = p->GetPipeLength();
					isHide = m_vPipes[m]->IsHide();
					group = m_vPipes[m]->GetPipeNode();
					break;
				}
			}

			if (fabs(tempVector.back().translate.z())==pipeLen || isHide)
			{
				// 有回退的动作和前进的动作综合后没有回退的动作了，如何判断管子隐藏？
				// 管子是需要回退的整根管子管子
				osg::ref_ptr<PipeHide> hide = new PipeHide(tempVector.back().fEndTime,group); // 隐藏回退的管子
				matrix->addUpdateCallback(hide);
			}
			//else
			//{

			   if (pipe!=NULL)
			   {
				   	// 去掉遮挡平面的作用
					LogInfo(QString("Add pipe show %1 !").arg(pipe->GetName()));
					osg::ref_ptr<PipeShow> pshow = new PipeShow(tempVector.begin()->fStartTime,pipe,group,m_sceneModel->getClipPlane(iheader-1),&m_pairPipeGroupMask);
					matrix->addUpdateCallback(pshow);
			   }

			//}

			//osg::ref_ptr<osg::Geode> Gnode = m_vMatrixAnimation[i].second.group;
			//Gnode->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(), osg::StateAttribute::OFF);
			//m_vLongStraightPipAnimation.push_back();
		}
		else if (tempVector.size()>1
			&& tempVector[0].rotateY!=0
			&& tempVector[1].rotateY ==0)
		{
			// 弯管绕弯，回退的情况
			// 直管前进，沿着Z轴旋转的情况
			osg::ref_ptr<osg::Geode> Gnode = m_vMatrixAnimations[i].second[0].group;
			PIPE* pipe = m_vMatrixAnimations[i].second[0].pipe;
			//DevideInfo(tempVector);
			std::vector<AnimationInfo> move;
			//move.push_back(tempVector[1]);
   //         if (tempVector.size()==3)
   //         {
   //             move.push_back(tempVector[2]);
   //         }

            for (size_t i=1; i<tempVector.size(); ++i)
            {
                move.push_back(tempVector[i]);
            }

			std::vector<AnimationInfo> rotate;
			rotate.push_back(tempVector[0]);
			matrix->setUpdateCallback(getModelMoveRotateCallBack(move,rotate));

			if (pipe!=NULL)
			{
				LogInfo(QString("Add pipe show %1 !").arg(pipe->GetName()));
				osg::ref_ptr<PipeShow> pshow = new PipeShow(tempVector.begin()->fEndTime,pipe,Gnode,m_sceneModel->getClipPlane(iheader-1),&m_pairPipeGroupMask);
				matrix->addUpdateCallback(pshow);
			}

		}
		else
		{
			osg::ref_ptr<osg::Geode> Gnode = m_vMatrixAnimations[i].second[0].group;
			pipe = m_vMatrixAnimations[i].second[0].pipe;
			DevideInfo(tempVector);
			matrix->setUpdateCallback(getPipePathCallBack(tempVector));

			if (pipe!=NULL)
			{
				LogInfo(QString("Add pipe show %1 !").arg(pipe->GetName()));
				osg::ref_ptr<PipeShow> pshow = new PipeShow(tempVector.back().fEndTime,pipe,Gnode,m_sceneModel->getClipPlane(iheader-1),&m_pairPipeGroupMask);
				matrix->addUpdateCallback(pshow);
			}
		}

		if(s!=-1 && e!=-1 && pipeSeg!=-1)
		{
			osg::ref_ptr<PipeInfoCallback> tmp = new PipeInfoCallback(s,e,pipeSeg,pipeSegTw,istw,this,m_bAutoPlaning);
			matrix->addUpdateCallback(tmp);
		}

		osg::ref_ptr<CollisionCallBack> call = new CollisionCallBack(this,matrix->getName(),&m_MapCollisionPair);
		matrix->addUpdateCallback(call);
	}

	//長直管显示时间
	/*
	for (size_t pi=0;pi<m_vLongStraightPipeMaxtrixAnimation.size();pi++)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vLongStraightPipeMaxtrixAnimation[pi].first;
		int iheader = -1;
		if (m_mapMatrixHeader.find(matrix)!=m_mapMatrixHeader.end())
		{
			iheader = m_mapMatrixHeader[matrix];
		}
		else
		{
			QMessageBox::warning(NULL, "warning", "Find header from matrix error !", QMessageBox::Yes);

			return;
		}

		std::vector<AnimationInfo> vtmp;
		vtmp.push_back(m_vLongStraightPipeMaxtrixAnimation[pi].second);
		// matrix->setUpdateCallback(getLongStaightPipePathCallBack(m_vLongStraightPipAnimation));

		matrix->setUpdateCallback(getModelMoveCallBackNew(m_vLongStraightPipAnimation));

		// 机头编号从1开始，机头对应的遮挡平面索引从0开始
		osg::ref_ptr<LongStraightPipe> longStraight = new LongStraightPipe(m_vLongStraightPipeMaxtrixAnimation[pi].second.fStartTime,m_pstraiPipe,
			matrix,m_vPipes,m_sceneModel->getClipPlane(iheader-1));
		matrix->addUpdateCallback(longStraight);
	}
	*/

	//for(int i=0;i<m_vNodes.size();i++)
	//{
	//	if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("jitouwanguan1.stl") /*|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("dj2.1.stl")*/)
	//	{
	//		m_vJitouwanguanAnims = m_vSTLs[i]->GetVMoveAnimation();
	//		//continue;
	//		CombineSamePeriodAnimation(m_vJitouwanguanAnims);
	//	}

	//	if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit(m_strLunmoName.toStdString().c_str()) /*|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("dj2.1.stl")*/)
	//	{
	//		m_vLunMoAnims = m_vSTLs[i]->GetVMoveAnimation();
	//		//continue;

	//		CombineSamePeriodAnimation(m_vLunMoAnims);
	//	}
	//}

	//模型动画
	for(size_t i=0;i<m_vNodes.size();i++)
	{
		osg::ref_ptr<osg::MatrixTransform> matrix = m_vNodes[i]->GetModelMatrix();
		osg::ref_ptr<osg::MatrixTransform> Rotatematrix = m_vNodes[i]->GetModelRotateMatrix();
		osg::ref_ptr<osg::MatrixTransform> Movematrix = m_vNodes[i]->GetModelMoveMatrix();

		//osg::ref_ptr<osg::MatrixTransform> Movematrix = m_vSTLs[i]->GetPosMatrix();

		std::vector<AnimationInfo> Vmove = m_vNodes[i]->GetVMoveAnimation();
		CombineSamePeriodAnimation(Vmove);
		//if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_1.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		//{
		//	Vmove.clear();
		//}
		std::vector<AnimationInfo> Vrotate = m_vNodes[i]->GetVRotateAnimation();
		//if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_1.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		//{
		//	std::vector<AnimationInfo> temp = Vrotate;
		//	Vrotate.clear();
		//	for (int i = 0; i < 1;++i)
		//	{
		//		Vrotate.push_back(temp[i]);
		//	}
		//}
		std::vector<AnimationInfo> VSecondrotate = m_vNodes[i]->GetSecondVRotateAnimation();

		// both move and rotate
		//if (Vmove.size()!=0 && Vrotate.size()!=0)
		//{
		//	int m = 0;
		//}

#if 0
		if(m_vNodes[i]->GetSTLName()==QString::fromLocal8Bit("MoZuo") /*|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("dj2.1.stl")*/)
		{
			QString information;
			for (int m = 0; m < Vmove.size();++m)
			{
				AnimationInfo info = Vmove[m];

				information.append("move start time ");
				information.append(QString::number(info.fStartTime));
				LogInfo(information);
				information.clear();

				information.append("end time ");
				information.append(QString::number(info.fEndTime));
				LogInfo(information);
				information.clear();

				information.append("pos x ");
				information.append(QString::number(info.translate.x()));
				LogInfo(information);
				information.clear();

				information.append("pos y ");
				information.append(QString::number(info.translate.y()));
				LogInfo(information);
				information.clear();

				information.append("pos z ");
				information.append(QString::number(info.translate.z()));
				LogInfo(information);
				information.clear();

				information.append(QString::fromLocal8Bit(info.strProcess.c_str()));
				LogInfo(information);
				information.clear();
			}

			for (int m = 0; m < Vrotate.size();++m)
			{
				AnimationInfo info = Vrotate[m];

				information.append("rotate start time ");
				information.append(QString::number(info.fStartTime));
				LogInfo(information);
				information.clear();

				information.append("end time ");
				information.append(QString::number(info.fEndTime));
				LogInfo(information);
				information.clear();

				information.append("rot x ");
				information.append(QString::number(info.rotateX));
				LogInfo(information);
				information.clear();

				information.append("rot y ");
				information.append(QString::number(info.rotateY));
				LogInfo(information);
				information.clear();

				information.append("rot z ");
				information.append(QString::number(info.rotateZ));
				LogInfo(information);
				information.clear();

				information.append(QString::fromLocal8Bit(info.strProcess.c_str()));
				LogInfo(information);
				information.clear();
			}
		}
#endif

		bool bFlage = true;
		// 机头弯管旋转的时候需要同步旋转的模型
		//if (m_MapGroupToGroupChildren.find("ZhuJia_Y")!=m_MapGroupToGroupChildren.end())
		//{
		//	std::vector<QString> children= m_MapGroupToGroupChildren.find("ZhuJia_Y")->second.vChildrenNames;
		//	for (int k = 0; k < children.size();++k)
		//	{
		//		if (children[k]==m_vSTLs[i]->GetSTLName() && (!Vmove.empty() || !Vrotate.empty()))
		//		{
		//			//SetTransMatrix(m_vSTLs[i],Vmove,Vrotate);
		//			SetTransMatrixNew(m_vSTLs[i],Vmove,Vrotate);
		//			bFlage = false;
		//			break;
		//		}
		//	}
		//}

		//if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_1.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2_2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		//if(m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj2.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj3.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj4.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zhujiazuo.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("tuiwanlun.stl")
		//	|| m_vSTLs[i]->GetSTLName()==QString::fromLocal8Bit("zj1.stl"))
		//{
		//for (int i = 0; i < Vrotate.size();++i)
		//{
		//	AnimationInfo& anim = Vrotate[i];
		//	//anim.center3d -= osg::Vec3(-110.0,0.0,0.0);
		//}

		//Rotatematrix->setUpdateCallback(getModelMoveRotateCallBack(Vmove,Vrotate));
		//osg::Transform::ReferenceFrame frame = Rotatematrix->getReferenceFrame();
		//Rotatematrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		//SetTransMatrix(m_vSTLs[i],Vmove,Vrotate);

#if 0
		//Rotatematrix->setUpdateCallback(getModelRotateCallBack(Vrotate));
		std::vector<AnimationInfo> left,mid,right,test;
		MoveCenter(110.0,110.0,Vmove,Vrotate,left,mid,right,test);
		if (!right.empty())
		{
			Rotatematrix->setUpdateCallback(getModelRotateCallBack(right));
		}

		if (!left.empty())
		{
			m_vSTLs[i]->GetModelLeftRotateMatrix()->setUpdateCallback(getModelRotateCallBack(left));
			// m_vSTLs[i]->GetModelLeftRotateMatrix()->setUpdateCallback(getModelMoveRotateCallBack(left));
			//ModelCenterMove* moveCallBack = new ModelCenterMove(left[0].center3d,Vmove,Vrotate);
			//m_vSTLs[i]->GetModelLeftRotateMatrix()->addUpdateCallback(moveCallBack);
		}

		if (!mid.empty())
		{
			m_vSTLs[i]->GetModelMidRotateMatrix()->setUpdateCallback(getModelRotateCallBack(mid));
		}

		if (!test.empty())
		{
			m_vSTLs[i]->GetModelTestRotateMatrix()->setUpdateCallback(getModelRotateCallBack(test));
		}

		//Movematrix = m_vSTLs[i]->GetPosMatrix();

		if(Vmove.size()!=0)
		{
			Movematrix->setUpdateCallback(getModelMoveCallBackNew(Vmove));
		}
#endif 

		//}
		//else
		if (bFlage && (!Vmove.empty() || !Vrotate.empty())) // 模型旋转是否因为中心变换的原因已经被替代？
		{
			if(Vmove.size()!=0)
			{
				Movematrix->setUpdateCallback(getModelMoveCallBackNew(Vmove));
			}

			if(Vrotate.size()!=0)
			{
				Rotatematrix->setUpdateCallback(getModelRotateCallBack(Vrotate));
			}

#if 0
			std::map<float,AnimationInfo> mapTimeAnims;
			for (int m = 0; m < Vmove.size();++m)
			{
				AnimationInfo info = Vmove[m];
				info.iAnimationType = 1;
				mapTimeAnims[info.fStartTime] = info;
			}

			for (int m = 0; m < Vrotate.size();++m)
			{
				AnimationInfo& info = Vrotate[m];
				info.iAnimationType = 2;
				mapTimeAnims[info.fStartTime] = info;
			}

			std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = m_vSTLs[i]->GetTransMatrix();
			std::map<float,AnimationInfo>::iterator iter = mapTimeAnims.begin();
			for (;iter!=mapTimeAnims.end();++iter)
			{
				std::vector<AnimationInfo> infos;
				AnimationInfo info = iter->second;
				osg::ref_ptr<osg::MatrixTransform> tempMatrix = new osg::MatrixTransform;
				// 旋转,靠近旋转轴

				infos.push_back(info);

				if (info.iAnimationType==1)
				{
					tempMatrix->setUpdateCallback(getModelMoveCallBackNew(infos));
				}
				else if (info.iAnimationType==2)
				{
					tempMatrix->setUpdateCallback(getModelRotateCallBack(infos));
				}

				trans.push_back(tempMatrix);
			}

			if (trans.empty())
			{
				return;
			}

			for (int m = 1; m < trans.size();++m)
			{
				trans[m]->addChild(trans[m-1]);
			}

			osg::ref_ptr<osg::MatrixTransform> moveMatrix = m_vSTLs[i]->GetModelMoveMatrix();
			osg::ref_ptr<osg::MatrixTransform> rotateMatrix = m_vSTLs[i]->GetModelSecondRotateMatrix();

			moveMatrix->removeChild(rotateMatrix);
			moveMatrix->addChild(trans[trans.size()-1]);
			trans[0]->addChild(rotateMatrix);
#endif
			//if(Vmove.size()!=0)
			//{
			//	Movematrix->setUpdateCallback(getModelMoveCallBackNew(Vmove));
			//}
		    QString name = m_vNodes[i]->GetSTLName();
			std::vector<STL*> models = GetSTLsByGroupName(name);
			for (int m = 0; m < models.size();++m)
			{
				STL* mode = models[m];
				QString modelName = mode->GetSTLName();
				if (modelName.indexOf(".stl")!=-1)
				{
					osg::ref_ptr<osg::Node> node = mode->GetSTLNode();
					if (IsModelCollision(modelName))
					{
						// node->setName(modelName.toStdString().append("pos"));
						osg::ref_ptr<CollisionCallBack> call = new CollisionCallBack(this,modelName.toStdString(),&m_MapCollisionPair);
						if(Vmove.size()!=0)
						{
							Movematrix->addUpdateCallback(call);
						}

						if(Vrotate.size()!=0)
						{
							Rotatematrix->addUpdateCallback(call);
						}
					}
				}
			}
		}

		if(VSecondrotate.size()!=0)
		{
			osg::ref_ptr<osg::MatrixTransform> SecondRotatematrix = m_vNodes[i]->GetModelSecondRotateMatrix();

			SecondRotatematrix->setUpdateCallback(getModelSecondRotateCallBack(VSecondrotate));
		}

	}

	//startTimer(dtime);

	m_sceneView->UpdateView();
}

// 将?管ü子哩?动ˉ画-中D转羇动ˉ角?度è大洙?于?80或ò小?于?180拆e分?成é两?段?
void MainWindow::DevideInfo(std::vector<AnimationInfo>& infos)
{
	std::vector<AnimationInfo> tempVector = infos;
	infos.clear();
	for (size_t i = 0; i < tempVector.size();++i)
	{
		AnimationInfo tmpInfo = tempVector[i];
		if (tmpInfo.rotateX>180.0 || tmpInfo.rotateX < -180.0)
		{
			AnimationInfo info1;
			AnimationInfo info2;

			info1.fStartTime = tmpInfo.fStartTime;
			info1.fEndTime = tmpInfo.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
			info1.center3d = tmpInfo.center3d;
			info1.rotateX = tmpInfo.rotateX/2;
			info1.fdir = tmpInfo.fdir;
			info1.strProcess = tmpInfo.strProcess;

			info2.fStartTime = info1.fEndTime;
			info2.fEndTime = tmpInfo.fEndTime;
			info2.center3d = tmpInfo.center3d;
			info2.rotateX = tmpInfo.rotateX/2;
			info2.fdir = tmpInfo.fdir;
			info2.strProcess = tmpInfo.strProcess;

			infos.push_back(info1);
			infos.push_back(info2);
		}
		else if (tmpInfo.rotateY>180.0 || tmpInfo.rotateY < -180.0)
		{
			AnimationInfo info1;
			AnimationInfo info2;

			info1.fStartTime = tmpInfo.fStartTime;
			info1.fEndTime = tmpInfo.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
			info1.center3d = tmpInfo.center3d;
			info1.rotateY = tmpInfo.rotateY/2;
			info1.fdir = tmpInfo.fdir;
			info1.strProcess = tmpInfo.strProcess;

			info2.fStartTime = info1.fEndTime;
			info2.fEndTime = tmpInfo.fEndTime;
			info2.center3d = tmpInfo.center3d;
			info2.rotateY = tmpInfo.rotateY/2;
			info2.fdir = tmpInfo.fdir;
			info2.strProcess = tmpInfo.strProcess;

			infos.push_back(info1);
			infos.push_back(info2);
		}
		else if (tmpInfo.rotateZ>180.0 || tmpInfo.rotateZ < -180.0)
		{
			AnimationInfo info1;
			AnimationInfo info2;

			info1.fStartTime = tmpInfo.fStartTime;
			info1.fEndTime = tmpInfo.fStartTime + (tmpInfo.fEndTime - tmpInfo.fStartTime)/2;
			info1.center3d = tmpInfo.center3d;
			info1.rotateZ = tmpInfo.rotateZ/2;
			info1.fdir = tmpInfo.fdir;
			info1.strProcess = tmpInfo.strProcess;

			info2.fStartTime = info1.fEndTime;
			info2.fEndTime = tmpInfo.fEndTime;
			info2.center3d = tmpInfo.center3d;
			info2.rotateZ = tmpInfo.rotateZ/2;
			info2.fdir = tmpInfo.fdir;
			info2.strProcess = tmpInfo.strProcess;

			infos.push_back(info1);
			infos.push_back(info2);
		}
		else
		{
			infos.push_back(tmpInfo);
		}
	}
}

void MainWindow::SetTransMatrix(STL* stl,
	const std::vector<AnimationInfo>& move,
	const std::vector<AnimationInfo>& rotate)
{
	//m_vJitouwanguanAnims;
	if (move.empty() && rotate.empty())
	{
		return;
	}

	int signR = 0;
	int signT = 0;
	double dTDistance = 0.0;
	double dRDistance = m_dRDistance;
	std::map<double,AnimationInfo> map0;
	for (size_t i = 0; i < m_vJitouwanguanAnims.size();++i)
	{
		AnimationInfo info = m_vJitouwanguanAnims[i];
		map0[info.fStartTime] = info;
	}

	// 相对于机头弯管是否有运动
	std::vector<AnimationInfo> singleMove;
	for (size_t i = 0; i < move.size();++i)
	{
		AnimationInfo info = move[i];
		if ((info.isZhujiaZ ||  // 主夹相对于机头弯管在Z方向移动
			info.isZhujiaX ||  // 主夹相对于机头弯管有夹紧或松开的动作
			(map0.find(info.fStartTime)==map0.end() && (info.translate.x()!=0.0 || info.translate.z()!=0.0))) || // 主夹有单独的动作,x,z方向位移不是0
			(map0.find(info.fStartTime)!=map0.end() &&
			(map0.find(info.fStartTime)->second.translate.x()!=info.translate.x() ||  map0.find(info.fStartTime)->second.translate.z()!=info.translate.z())))
		{
			if (map0.find(info.fStartTime)!=map0.end())
			{
				info.translate.x() -= map0.find(info.fStartTime)->second.translate.x();
				info.translate.z() -= map0.find(info.fStartTime)->second.translate.z();
			}

			singleMove.push_back(info);
		}
	}

	std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = stl->GetTransMatrix();
	std::map<double,AnimationInfo> mapTimeAnims;
	for (size_t i = 0; i < singleMove.size();++i)
	{
		AnimationInfo info = singleMove[i];
		info.iAnimationType = 1;
		mapTimeAnims[info.fStartTime] = info;
	}

	for (size_t i = 0; i < rotate.size();++i)
	{
		AnimationInfo info = rotate[i];
		info.iAnimationType = 2;
		if (mapTimeAnims.find(info.fStartTime)!=mapTimeAnims.end())
		{
			// mapTimeAnims[info.fStartTime] = info;
			QString information(info.strProcess.c_str());
			information.append("time same as");
			information.append(mapTimeAnims.find(info.fStartTime)->second.strProcess.c_str());
			QMessageBox::warning(NULL, "warning", information, QMessageBox::Yes);
			LogInfo(information);
		}

		mapTimeAnims[info.fStartTime] = info;
	}

	if (mapTimeAnims.empty())
	{
		return;
	}

	if (trans.empty())
	{
		std::map<double,AnimationInfo>::iterator iter = mapTimeAnims.begin();
		osg::Vec3 offset0(0.0,0.0,0.0);
		osg::Vec3 offset1(0.0,0.0,0.0);
		//bool isNear = false;
		for (;iter != mapTimeAnims.end();++iter)
		{
			std::vector<AnimationInfo> infos;
			AnimationInfo info = iter->second;

			if (info.iAnimationType==1)
			{
				// 平移
				if (info.translate.x()!=0.0
					/*|| (info.isZhujiaR && info.translate.x()!=0.0)*/
					/*|| fabs(info.translate.x())==m_dRDistance*/)
				{
					//isNear = true;
					if (m_bOnlyLeft)
					{
						// 纯左侧加工
						offset0 = osg::Vec3(-m_dRDistance,0.0,0.0);
					}
					else
					{
						//offset0 = osg::Vec3(m_dRDistance,0.0,0.0);
						//if (sign%2==0)
						//{ 
						//	dRDistance = fabs(info.translate.x());
						//	++sign;
						//}
						//else
						//{
						//	dRDistance = 0.0;
						//	++sign;
						//}

						//if (info.fdir==0.0  /*signR!=1 && signR!=13*/) // 右侧加工
						//{
						//	dRDistance += fabs(info.translate.x());
						//	++signR;
						//}
						//else if (info.fdir==1.0) // 左侧加工
						//{
						//	dRDistance += fabs(info.translate.x());
						//	++signR;
						//}
						if (signR!=4 && signR!=3)
						{
							dRDistance += fabs(info.translate.x());
							++signR;
						}
						else if (signR==3)
						{
							dRDistance += fabs(info.translate.x());
							++signR;
						}
						else if (signR==4)
						{
							dRDistance += fabs(info.translate.x());
							++signR;
						}
						else
						{
							dRDistance = 0.0;
						}

						offset0 = osg::Vec3(dRDistance,0.0,0.0);
					}
				}
				else
				{
					//isNear = false;
					offset0 = osg::Vec3(0.0,0.0,0.0);
				}

				// if (fabs(info.translate.z())==m_dTDistance)
				if (/*info.isZhujiaZ && fabs(info.translate.z())!=0.0*/info.translate.z()!=0.0)
				{
					//isNear = true;
					if (m_bOnlyLeft)
					{
						// 纯左侧加工
						offset1 = osg::Vec3(0.0,0.0,info.translate.z());
					}
					else
					{
						if (info.fdir==0.0/*signT!=1 && signT!=13*//*signT%2==0*/)
						{ 
							dTDistance += -info.translate.z();
							//++signT;
						}
						else if (info.fdir==1.0)
						{
							dTDistance += info.translate.z();
							//dTDistance = 90.0;
							//++signT;
						}
						else
						{
							dTDistance = 0.0;
						}

						offset1 = osg::Vec3(0.0,0.0,dTDistance);
					}
				}
				//else
				//{
				//	//isNear = false;
				//	offset1 = osg::Vec3(0.0,0.0,0.0);
				//}

				//infos.push_back(info);
				continue;
			}
			else if (info.rotateX!=0.0 || info.rotateY!=0.0 || info.rotateZ!=0.0)
			{
				osg::ref_ptr<osg::MatrixTransform> tempMatrix = new osg::MatrixTransform;
				// 旋转,靠近旋转轴
				info.center3d += osg::Vec3(offset0+offset1);
				//offset0 = osg::Vec3(0.0,0.0,0.0);
				//offset1 = osg::Vec3(0.0,0.0,0.0);
				dRDistance = 0.0;
				dTDistance = 0.0;
				//if (!infos.empty())
				//{
				//	AnimationInfo end = infos.back();
				//	if (end.fStartTime==info.fStartTime && end.fEndTime==info.fEndTime)
				//	{
				//		infos.pop_back();
				//	}
				//}
				infos.push_back(info);
				// 变换当前旋转的旋转中心
				tempMatrix->setUpdateCallback(getModelRotateCallBack(infos));
				trans.push_back(tempMatrix);
			}
			else
			{
				dRDistance = 0.0;
				dTDistance = 0.0;
			}
		}

		if (trans.empty())
		{
			return;
		}

		for (size_t i = 1; i < trans.size();++i)
		{
			trans[i-1]->addChild(trans[i]);
		}

		osg::ref_ptr<osg::MatrixTransform> moveMatrix = stl->GetModelMoveMatrix();
		osg::ref_ptr<osg::MatrixTransform> rotateMatrix = stl->GetModelSecondRotateMatrix();

		moveMatrix->removeChild(rotateMatrix);
		moveMatrix->addChild(trans[0]);
		trans[trans.size()-1]->addChild(rotateMatrix);
	}

}

void MainWindow::SetTransMatrixNew(STL* stl,
	const std::vector<AnimationInfo>& move,
	const std::vector<AnimationInfo>& rotateold)
{
	if (m_vJitouwanguanAnims.empty())
	{
		return;
	}

	if (move.empty())
	{
		return;
	}

	//AnimationInfo lunmo = m_vJitouwanguanAnims[0];
	//AnimationInfo model = move[0];
	// 记录模型相对于轮模X方向的位置偏差
	//double deltax = model.absolutePos.x()-lunmo.absolutePos.x();
	//// 记录模型相对于轮模Z方向的位置偏差
	//double deltaz = model.absolutePos.z()-lunmo.absolutePos.z();

	AnimationInfo inforNear;
	std::vector<AnimationInfo> rotate = rotateold;
	for (size_t i = 0; i < rotate.size();++i)
	{
		AnimationInfo& infoRotate = rotate[i];
		for (size_t j = 0; j < m_vJitouwanguanAnims.size();++j)
		{
			AnimationInfo infoLunmo = m_vJitouwanguanAnims[j];
			if (infoLunmo.fStartTime<infoRotate.fStartTime)
			{
				inforNear = infoLunmo;
			}
		}

		infoRotate.center3d = osg::Vec3(inforNear.absolutePos.x(),infoRotate.center3d.y(),inforNear.absolutePos.z());
		//infoRotate.dDeltaX = inforNear.absolutePos.x();
		//infoRotate.dDeltaZ = inforNear.absolutePos.z();
		//infoRotate.dDeltaX = infoRotate.absolutePos.x()-inforNear.absolutePos.x() /*- deltax*/;
		//infoRotate.dDeltaZ = infoRotate.absolutePos.z()-inforNear.absolutePos.z() /*- deltaz*/;

		//infoRotate.dDeltaX = inforNear.absolutePos.x() /*- deltax*/;
		//infoRotate.dDeltaZ = inforNear.absolutePos.z() /*- deltaz*/;

		//infoRotate.dDeltaX = 0.0;
		//infoRotate.dDeltaZ = 0.0;

		//deltax = infoRotate.absolutePos.x()-inforNear.absolutePos.x();
		//deltaz = infoRotate.absolutePos.z()-inforNear.absolutePos.z();
	}

	std::map<float,AnimationInfo> mapTimeAnims;
	for (size_t i = 0; i < move.size();++i)
	{
		AnimationInfo info = move[i];
		info.iAnimationType = 1;
		mapTimeAnims[info.fStartTime] = info;
	}

	for (size_t i = 0; i < rotate.size();++i)
	{
		AnimationInfo& info = rotate[i];
		info.iAnimationType = 2;
		mapTimeAnims[info.fStartTime] = info;
	}

#if 1
	if (stl->GetSTLName()=="zj1.stl")
	{
		QString information;
		std::map<float,AnimationInfo>::iterator iter = mapTimeAnims.begin();
		for (;iter!=mapTimeAnims.end();++iter)
		{
			AnimationInfo info = iter->second;

			information.append("Animation start time ");
			information.append(QString::number(info.fStartTime));
			LogInfo(information);
			information.clear();

			information.append("end time ");
			information.append(QString::number(info.fEndTime));
			LogInfo(information);
			information.clear();

			information.append("pos x ");
			information.append(QString::number(info.translate.x()));
			LogInfo(information);
			information.clear();

			information.append("pos y ");
			information.append(QString::number(info.translate.y()));
			LogInfo(information);
			information.clear();

			information.append("pos z ");
			information.append(QString::number(info.translate.z()));
			LogInfo(information);
			information.clear();

			information.append("rotate x ");
			information.append(QString::number(info.rotateX));
			LogInfo(information);
			information.clear();

			information.append("rotate y ");
			information.append(QString::number(info.rotateY));
			LogInfo(information);
			information.clear();

			information.append("rotate z ");
			information.append(QString::number(info.rotateZ));
			LogInfo(information);
			information.clear();

			information.clear();
			//information.append("step ");
			//information.append(QString::number(curProcess.istep));
			information.append(QString::fromLocal8Bit(info.strProcess.c_str()));
			LogInfo(information);
			information.clear();
		}
	}

#endif 

	std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = stl->GetTransMatrix();
	if (trans.empty())
	{
		//osg::Vec3 offset0(0.0,0.0,0.0);
		//osg::Vec3 offset1(0.0,0.0,0.0);
		//bool isNear = false;

		//for (int i = 0; i < rotate.size();++i)
		std::map<float,AnimationInfo>::iterator iter = mapTimeAnims.begin();

		for (;iter!=mapTimeAnims.end();++iter)
		{
			std::vector<AnimationInfo> infos;
			AnimationInfo info = iter->second;
			osg::ref_ptr<osg::MatrixTransform> tempMatrix = new osg::MatrixTransform;
			// 旋转,靠近旋转轴

			infos.push_back(info);

			if (info.iAnimationType==1)
			{
				tempMatrix->setUpdateCallback(getModelMoveCallBackNew(infos));
			}
			else if (info.iAnimationType==2)
			{
				tempMatrix->setUpdateCallback(getModelRotateCallBack(infos));
			}

			trans.push_back(tempMatrix);
		}

		if (trans.empty())
		{
			return;
		}

		for (size_t i = 1; i < trans.size();++i)
		{
			trans[i]->addChild(trans[i-1]);
		}

		osg::ref_ptr<osg::MatrixTransform> moveMatrix = stl->GetModelMoveMatrix();
		osg::ref_ptr<osg::MatrixTransform> rotateMatrix = stl->GetModelSecondRotateMatrix();

		moveMatrix->removeChild(rotateMatrix);
		moveMatrix->addChild(trans[trans.size()-1]);
		trans[0]->addChild(rotateMatrix);
	}
}

void MainWindow::MoveCenter(double leftMove,double rightMove,
	const std::vector<AnimationInfo>& move,
	const std::vector<AnimationInfo>& rotate,
	std::vector<AnimationInfo>& left,std::vector<AnimationInfo>& mid,
	std::vector<AnimationInfo>& right,std::vector<AnimationInfo>& test)
{
	std::map<double,AnimationInfo> map0;
	for (size_t i = 0; i < m_vJitouwanguanAnims.size();++i)
	{
		AnimationInfo info = m_vJitouwanguanAnims[i];
		map0[info.fStartTime] = info;
	}

	// 主夹在运动，机头弯管没有动的运动
	std::vector<AnimationInfo> singleMove;
	for (size_t i = 0; i < move.size();++i)
	{
		AnimationInfo info = move[i];
		if (map0.find(info.fStartTime)==map0.end()
			&& (info.translate.x() != 0.0
			|| info.translate.y() != 0.0
			|| info.translate.z() != 0.0))
		{
			singleMove.push_back(info);
		}
	}

	std::map<double,AnimationInfo> mapTimeAnims;
	for (size_t i = 0; i < singleMove.size();++i)
	{
		AnimationInfo info = singleMove[i];
		info.iAnimationType = 1;
		mapTimeAnims[info.fStartTime] = info;
	}

	for (size_t i = 0; i < rotate.size();++i)
	{
		AnimationInfo info = rotate[i];
		info.iAnimationType = 2;
		mapTimeAnims[info.fStartTime] = info;
	}

	std::map<double,AnimationInfo>::iterator iter = mapTimeAnims.begin();
	osg::Vec3 offset(0.0,0.0,0.0);
	double delta = 0.00001;
	uchar type = 0;  // 0 mid,1 left,2 right
	for (;iter != mapTimeAnims.end();++iter)
	{
		std::vector<AnimationInfo> infos;
		AnimationInfo info = iter->second;

		if (info.iAnimationType==1)
		{
			// 平移
			//tempMatrix->setUpdateCallback(getModelMoveCallBackNew(infos));
			//offset[0] += info.translate.x();
			//offset[2] += info.translate.z();

			//offset[0] += info.translate.x();
			//offset[1] += info.translate.y();
			//offset[2] += info.translate.z();
			if (fabs(info.translate.x()-leftMove)<delta)
			{
				// 左夹紧
				//info.center3d += osg::Vec3(leftMove,0.0,0.0);
				offset = osg::Vec3(leftMove,0.0,0.0);
				type = 1;
				//left.push_back(info);
			}
			else if (fabs(info.translate.x()+rightMove)<delta)
			{
				// 右夹紧
				//info.center3d += osg::Vec3(rightMove,0.0,0.0);
				offset = osg::Vec3(fabs(rightMove),0.0,0.0);
				type = 2;
			}
			else
			{
				offset = osg::Vec3(0.0,0.0,0.0);
				type = 0;
			}
		}
		else
		{
			if (type == 1)
			{
				info.center3d += offset;
				left.push_back(info);
			}
			else if (type==2)
			{
				info.center3d += offset;
				right.push_back(info);
			}
			else
			{
				mid.push_back(info);
			}
		}
	}
	//double delta = 0.01;
	//std::vector<bool> flags;
	//flags.resize(rotate.size(),false);
	//// 找到连续两个90度的运动
	//for (int i = 1; i < rotate.size();++i)
	//{
	//	AnimationInfo info0 = rotate[i-1];
	//	AnimationInfo info1 = rotate[i];

	//	if (fabs(info0.rotateY-info1.rotateY)<=delta
	//		&& fabs(fabs(info1.rotateY)-90.0)<=delta)
	//	{
	//		flags[i-1] = true;
	//		flags[i] = true;
	//	}
	//}

	//std::vector<bool> labels;
	//// false 表示右边加工，2换边动作，true表示左边加工
	//labels.resize(flags.size(),false);
	//bool flag = false;
	//for (int i = 0;i < flags.size();++i)
	//{
	//	labels[i] = flag;
	//	if (flags[i])
	//	{
	//		flag = !flag;
	//		++i;
	//	}
	//}

	//for (int i = 0; i < flags.size();++i)
	//{
	//	// 换边
	//	if (flags[i])
	//	{
	//		mid.push_back(rotate[i]);
	//		continue;
	//	}

	//	AnimationInfo info = rotate[i];
	//	// 右边加工
	//	if (!labels[i])
	//	{
	//		//if (info.rotateY < 0.0)
	//		//{
	//		//	info.center3d[0] += 113.0;

	//		//	test.push_back(info);

	//		//	continue;
	//		//}

	//		info.center3d[0] += 113.0;
	//		right.push_back(info);
	//		continue;
	//	}
	//	else
	//	{
	//		// 左边加工
	//		if (info.rotateY>0.0)
	//		{

	//			info.center3d[0] += 37.0 -  28.47;
	//			//info.center3d[2] += 106.2518;
	//			test.push_back(info);

	//			continue;
	//		}

	//		//AnimationInfo info = rotate[i];
	//		info.center3d[0] += 113.0;
	//		left.push_back(info);
	//	}
	//}
}

void MainWindow::SlotSetAnimation()
{

	QString str;
	double dSTime;
	double dETime;
	int iAnimationType;
	int iAxis;
	float fvalue;


	str = m_pSettingAnimationDlg->GetModelName();
	dSTime = m_pSettingAnimationDlg->GetStartTime();
	dETime = m_pSettingAnimationDlg->GetEndTime();
	iAnimationType = m_pSettingAnimationDlg->GetCurAnimationType();
	iAxis = m_pSettingAnimationDlg->GetAxis();
	fvalue = m_pSettingAnimationDlg->GetValue();


	//设置模型动画
	for(size_t i=0;i<m_vSTLs.size();i++)
	{
		if(m_vSTLs[i]->GetSTLName() == str)
		{
			AnimationInfo info;
			info.fStartTime = dSTime;
			info.fEndTime = dETime;
			info.iAnimationType = iAnimationType;
			info.iAxis = iAxis;

			if(iAnimationType == 1)//平移
			{
				if(iAxis == 1 )//X轴
				{
					info.translate.x() = fvalue;
					info.translate.y() = 0;
					info.translate.z() = 0;
				}
				else if(iAxis == 2 )//Y
				{
					info.translate.x() = 0;
					info.translate.z() = 0;
					info.translate.y() = fvalue;
				}
				else if(iAxis == 3 )//Z
				{
					info.translate.x() = 0;
					info.translate.y() = 0;
					info.translate.z() = fvalue;
				}

				m_vSTLs[i]->AddMoveAnimation(info);
			}
			else if(iAnimationType == 2)//旋转
			{
				osg::Vec3 center = m_pSettingAnimationDlg->GetRotateCenter();
				info.center3d = center;

				if(iAxis == 1 )//X轴
				{
					info.rotateX = fvalue;
					info.rotateY = 0;
					info.rotateZ = 0;
				}
				else if(iAxis == 2)//Y
				{
					info.rotateX = 0;
					info.rotateZ = 0;
					info.rotateY = fvalue;
				}
				else if(iAxis == 3)//Z
				{
					info.rotateX = 0;
					info.rotateY = 0;
					info.rotateZ = fvalue;
				}

				m_vSTLs[i]->AddRotateAnimation(info);
			}
			else//静止
			{}
		}
	}

	AnimationInfo TimeInfo;
	TimeInfo.fStartTime = dSTime;
	TimeInfo.fEndTime = dETime;


	//设置管子动画
	bool isPipe = m_pSettingAnimationDlg->GetIsCreatPipe();
	if(isPipe)
	{
		osg::Vec3 _vec3PipeCenter = m_pSettingAnimationDlg->GetPipeCenter();
		osg::Vec3 _vec3PipeDir = m_pSettingAnimationDlg->GetPipeDir();
		float _fPipeLen = m_pSettingAnimationDlg->GetPipeLength();
		int _iNth = m_pSettingAnimationDlg->GetCurNth();

		int iPipetype = m_pSettingAnimationDlg->GetPipeStyle();
		if(iPipetype == 1)//直管
		{
			if(m_mark == 0)	
			{
				ShowStraightPipe(0,1,_vec3PipeCenter,_vec3PipeDir,TimeInfo,PipeInfo,_iNth-1,_fPipeLen,_iNth-1);	
				m_LongStraightPipeCenter = _vec3PipeCenter;
			}
			else
			{
				ShowStraightPipe(0,1,_vec3PipeCenter,_vec3PipeDir,TimeInfo,PipeInfo,_iNth-1,_fPipeLen,_iNth);
			}
			m_mark++;
		}
		else if(iPipetype == 2)//弯管
		{
			float _CurvePipeR = m_pSettingAnimationDlg->GetCurvePipeR();
			float _CurvePipeAngle = m_pSettingAnimationDlg->GetCurvePipeAngle();
			int _PipeSide = m_pSettingAnimationDlg->GetCurPipeSide();
			if(_PipeSide == 1)
				m_bIsLeft = true;
			else if(_PipeSide == 2)
				m_bIsLeft = false;
			ShowCurvePipe(_iNth-1,0,1,_vec3PipeCenter,_vec3PipeDir,TimeInfo,PipeInfo,_CurvePipeR,_CurvePipeAngle);
		}

	}


	//更新表格
	QString tmpStToEt;
	tmpStToEt = QString::number(dSTime) + QString::fromLocal8Bit("-") +  QString::number(dETime);
	QString strMovetype;
	QString strRotateCenter;

	if(iAnimationType == 1)
	{
		strMovetype = QString::fromLocal8Bit("平移");
		strRotateCenter = QString::fromLocal8Bit("————");
	}
	else if(iAnimationType == 2)
	{
		strMovetype = QString::fromLocal8Bit("旋转");
		osg::Vec3 tmpcenter = m_pSettingAnimationDlg->GetRotateCenter();

		strRotateCenter = QString::fromLocal8Bit("(") + QString::number(tmpcenter.x()) 
			+ QString::fromLocal8Bit(",") + QString::number(tmpcenter.y())
			+ QString::fromLocal8Bit(",") + QString::number(tmpcenter.z())
			+ QString::fromLocal8Bit(")");

	}
	else if(iAnimationType == 3)
	{
		strMovetype = QString::fromLocal8Bit("静止");
		strRotateCenter = QString::fromLocal8Bit("————");

	}

	QString strAxis;
	if(iAxis == 1)
		strAxis = QString::fromLocal8Bit("X轴");
	else if(iAxis == 2)
		strAxis = QString::fromLocal8Bit("Y轴");
	else if(iAxis == 3)
		strAxis = QString::fromLocal8Bit("Z轴");

	m_ShowTableDlg->AddTableRow(30,QStringList()<<str<<QString::number(dSTime)<<QString::number(dETime)<<strMovetype<<QString::number(fvalue)<<strAxis);



}

void MainWindow::SlotChangeTableBtnState()
{
	ui.actionFull_Screen->setChecked(false);
	ui.actionFull_Screen->setEnabled(true);
}

void MainWindow::SlotSetGroupToChildren()
{
	if(m_pSettingNodeGroupDlg == NULL)
		return;

	if (!m_MapGroupToSTL.empty())
	{
		return;
	}

	//清空
	m_MapGroupToSTL.erase(m_MapGroupToSTL.begin(), m_MapGroupToSTL.end());
	m_MapGroupToGroupChildren.erase(m_MapGroupToGroupChildren.begin(), m_MapGroupToGroupChildren.end());

	//m_MapGroupToChildren.erase(m_MapGroupToChildren.begin(), m_MapGroupToChildren.end());
	m_Groups.clear();
	for(size_t i=0;i<m_Groups.size();i++)
	{
		delete m_Groups[i];//释放组对象
	}

	//重新存储
	m_MapGroupToGroupChildren = m_pSettingNodeGroupDlg->GetGroupToGroupChildrenMap();
	//重构文件
	ReBuildGroupToSTLsFile(m_MapGroupToGroupChildren);

	//组对应STL
	if(!m_MapGroupToGroupChildren.empty())
	{
		std::map<QString,GroupChildren>::iterator it = m_MapGroupToGroupChildren.begin();
		for(;it!=m_MapGroupToGroupChildren.end();it++)
		{
			std::vector<STL*> vtmp;
			std::vector<QString>::iterator iter = it->second.vChildrenNames.begin();
			for(;iter!=it->second.vChildrenNames.end();iter++)
			{
				for(size_t i=0;i<m_vSTLs.size();i++)
				{
					if(*iter == m_vSTLs[i]->GetSTLName())
					{
						osg::Vec3 Gpos(it->second.fx,it->second.fy,it->second.fz);
						m_vSTLs[i]->SetSTLAbsolutePos(Gpos);
						vtmp.push_back(m_vSTLs[i]);
					}
				}
			}

			m_MapGroupToSTL[it->first] = vtmp;
		}
	}

	//生成组对象
	std::map<QString,std::vector<STL*>>::iterator iter = m_MapGroupToSTL.begin();
	for(;iter!=m_MapGroupToSTL.end();iter++)
	{
		xModelGroup *pGroup = new xModelGroup(iter->first,iter->second);
		m_Groups.push_back(pGroup);
	}

	//bool rebuild = m_pSettingNodeGroupDlg->GetIsRebuildLM();
	//if(rebuild)
	//{
	//	ReBuildLm();
	//}
}

void MainWindow::SlotSetJTSideModels()
{

	std::vector<STL*> a = m_vSTLs;
	m_JTNumToSideModelNamesMap = m_pSettingNodeGroupDlg->GetJTSideModelNames();
	BuildNormalUserFile(m_JTNumToSideModelNamesMap);
	SetMapToMapTypeToSTLs(m_JTNumToSideModelNamesMap);
	//更新普通用户界面
	//m_pNormalUserWidget->InitTable(m_JTNumToSideModelNamesMap);
}

void MainWindow::SlotSlightAdjust()
{
	int iAxis = m_pSlightAdjustDlg->GetAxis();
	QString strModelName = m_pSlightAdjustDlg->GetChosenModelName();
	STL * curSTL = GetCurSTLByName(strModelName);
	osg::Vec3 moveValue = m_pSlightAdjustDlg->GetAdjustValue();

	//调试修改模型后的中心
	osg::Vec3 ttt = (curSTL->GetSTLNode())->getBound().center()*osg::computeLocalToWorld((curSTL->GetSTLNode())->getParentalNodePaths()[0]);
	//osg::Vec3 beforeCenter = curSTL->GetCurModelCenter();

	//变换模型位置
	osg::ref_ptr<osg::MatrixTransform> tmpMatrix = curSTL->GetPosMatrix();
	osg::Matrixd mat0 = tmpMatrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::translate(moveValue);
	tmpMatrix->setMatrix(mat0*mat1);

	// Vec3d pos = tmpMatrix->getMatrix().getTrans();

	//更新模型中心
	osg::Vec3 tmp ;
	tmp = (curSTL->GetSTLNode())->getBound().center()*osg::computeLocalToWorld((curSTL->GetSTLNode())->getParentalNodePaths()[0]);

	QString info(QString("%1 model center is %2 %3 %4  !").arg(strModelName).arg(tmp.x()).arg(tmp.y()).arg(tmp.z()));
	LogInfo(info);
	//osg::Vec3 AfterCenter(beforeCenter.x()+moveValue.x(),beforeCenter.y()+moveValue.y(),beforeCenter.z()+moveValue.z());
	curSTL->SetModelCenter(tmp);
	//更新文件里的数据
	// UpdateInitPosFile(strModelName,pos);
	UpdateInitPosFile(strModelName,tmp);
}

void MainWindow::SlotReplaceWheelModel()
{
	QString strModelName = m_pReplaceWheelModelDlg->GetChosenModelName();
	double dLmr = m_pReplaceWheelModelDlg->GetLmR();
	m_LMupLen = m_pReplaceWheelModelDlg->GetUpLen();
	m_LMdownLen = m_pReplaceWheelModelDlg->GetDownLen();
	//m_isHaveTu = m_pReplaceWheelModelDlg->GetIsHaveTu();
	m_LMtuLen = m_pReplaceWheelModelDlg->GetTuLen();
	m_LMTuSide = m_pReplaceWheelModelDlg->GetTuSide();
	m_LMJTNth = m_pReplaceWheelModelDlg->GetJTNth();
	m_LMLayer = GetLMLayerByName(strModelName);

	UpdateLmrFile(strModelName,dLmr);//会重构模型

	//更新ModifyLMTable界面
	m_pModifyLMTable->InitDlg(m_vLMRFileValues);
}

void MainWindow::SlotReplaceJMModel()
{
	QString strModelName = m_pReplaceWheelModelDlg->GetChosenModelName();
	m_dJMLen = m_pReplaceWheelModelDlg->GetJMLen();
	m_dJMWide = m_pReplaceWheelModelDlg->GetJMWide();
	m_LMupLen = m_pReplaceWheelModelDlg->GetUpLen();
	m_LMdownLen = m_pReplaceWheelModelDlg->GetDownLen();
	m_JMJTNth = m_pReplaceWheelModelDlg->GetJTNth();
	m_JMAoSide = m_pReplaceWheelModelDlg->GetAoSide();
	m_JMAid = m_pReplaceWheelModelDlg->GetJMAid();
	//m_dOriginalJMLen = m_pReplaceWheelModelDlg->GetJMOriginalLen();
	//m_dCurJMLen = m_pReplaceWheelModelDlg->GetJMCurLen();
	UpdateJMFile(strModelName);//会重构模型


}

void MainWindow::SlotAddModels()
{
	QString copyModelName = m_pAddModelDlg->GetChosenModelName();
	QString newModelName = m_pAddModelDlg->GetNewModelName();
	int upUnits = m_pAddModelDlg->GetUpUnits();
	m_iJTNthInAccumulate = m_pAddModelDlg->GetNthJT();
	UpdateAccumulateNewModelFile(copyModelName,newModelName,upUnits);

	//LoadAccumulateModelsFromTheFile();

}

void MainWindow::SlotSetMoveRange()
{
	/*double max = m_pMoveRangeDlg->GetRangeMax();
	double min = m_pMoveRangeDlg->GetRangeMin();
	QString strModelName = m_pMoveRangeDlg->GetChosenModelName();
	STL * curSTL = GetCurSTLByName(strModelName);
	curSTL->SetRangeMax(max);
	curSTL->SetRangeMin(min);*/

}

void MainWindow::SlotSetMoveModelHighLight()
{
	//QString str = m_pMoveRangeDlg->GetHighLightModelName();
	//STL* curSTL = GetCurSTLByName(str);


	//UpdateModelColor(HIGHTLIGHT_COLOR,curSTL);
}

void MainWindow::SlotPlayRotateTest()
{
	m_vRotateTestAnimation.clear();
	double dAngle = m_pRotateTestDlg->GetRotateAngle();
	int iAxis = m_pRotateTestDlg->GetAxis();
	osg::Vec3 vecCenter = m_pRotateTestDlg->GetRotateCenter();
	QString str = m_pRotateTestDlg->GetChosenModelName();
	STL *pRotateTestSTL = GetCurSTLByName(str);
	osg::ref_ptr<osg::MatrixTransform>/*osg::ref_ptr<osg::PositionAttitudeTransform>*/ RotateTestMatrix = pRotateTestSTL->GetModelMatrix();
	AnimationInfo TestInfo;
	TestInfo.center3d = vecCenter;
	TestInfo.iAnimationType = 2;
	if(iAxis == 1)//X轴
	{
		TestInfo.rotateX = dAngle;
	}
	else if(iAxis == 2)
	{
		TestInfo.rotateY = dAngle;
	}
	else if(iAxis == 3)
	{
		TestInfo.rotateZ = dAngle;

	}
	TestInfo.fStartTime = 0.5;
	TestInfo.fEndTime = 1.5;
	m_vRotateTestAnimation.push_back(TestInfo);

	//归零
	if(iAxis == 1)//X轴
	{
		TestInfo.rotateX = -dAngle;
	}
	else if(iAxis == 2)
	{
		TestInfo.rotateY = -dAngle;
	}
	else if(iAxis == 3)
	{
		TestInfo.rotateZ = -dAngle;

	}
	TestInfo.fStartTime = 1.5;
	TestInfo.fEndTime = 2.5;
	m_vRotateTestAnimation.push_back(TestInfo);
	RotateTestMatrix->setUpdateCallback(getModelRotateCallBack(m_vRotateTestAnimation));
}

void MainWindow::SlotSetLanguage()
{
	QString curLanguage = m_pLanguageDlg->GetCurLanguage();
	m_strDefaultLanguageType = curLanguage;
	std::vector<QString> curWords;
	std::vector<std::vector<QString>>::iterator it = m_vLanguagevWords.begin();
	for(;it!=m_vLanguagevWords.end();it++)
	{
		std::vector<QString>::iterator itt = (*it).begin();
		int key = itt[0].toInt();
		if (itt[1] == curLanguage)
		{
			m_NumToDefalutLanguagePair = make_pair(key,curLanguage);

			curWords = (*it);
		}
	}

	SetLanguageInMainWindow(curWords);

	ReBuildLanguageDefaultFile();
	ReadModelDiplayNameFile();
}

void MainWindow::SlotReplaceByModifyLMTableModel()
{
	std::vector<LMR> tmp = m_pModifyLMTable->GetCurTableLMValues();
	for(size_t i=0;i<tmp.size();i++)
	{
		for (size_t j=0;j<m_vSTLs.size();j++)
		{
			if (tmp[i].strModelName == m_vSTLs[j]->GetSTLName())
			{
				ReplaceModelByLmTable(tmp[i],m_vSTLs[j]);
			}
		}

	}

}



void MainWindow::SlotShowModifyLMTableModel()
{
	std::map<QString,int> tmp = m_pModifyLMTable->GetModelIsShow();
	std::map<QString,int>::iterator it = tmp.begin();
	for(;it!=tmp.end();it++)
	{
		for (size_t i=0;i<m_vSTLs.size();i++)
		{
			if (it->first == m_vSTLs[i]->GetSTLName())
			{
				if(it->second == 1)
				{
					m_vSTLs[i]->GetSTLNode()->setNodeMask(-1);

				}
				else if(it->second == 0)
				{
					m_vSTLs[i]->GetSTLNode()->setNodeMask(0);

				}
			}
		}
	}
}

void MainWindow::SlotModifyLMTableFinished()
{
	std::vector<LMR> tmp = m_pModifyLMTable->GetCurTableLMValues();
	for(size_t i=0;i<tmp.size();i++)
	{
		for (size_t j=0;j<m_vSTLs.size();j++)
		{
			if (tmp[i].strModelName == m_vSTLs[j]->GetSTLName())
			{			
				ReplaceModelByLmTable(tmp[i],m_vSTLs[j]);		
			}
		}
	}

	std::vector<JM> tmp2 = m_pModifyLMTable->GetCurTableJMValues();
	for(size_t i=0;i<tmp2.size();i++)
	{
		for (size_t j=0;j<m_vSTLs.size();j++)
		{
			if (tmp2[i].strModelName == m_vSTLs[j]->GetSTLName())
			{			
				ReplaceModelByJmTable(tmp2[i],m_vSTLs[j]);		
			}
		}
	}
}

void MainWindow::SlotSetDetectedFinished()
{
	m_IsDetected = m_pCollisionDlg->GetIsDetected();
}

void MainWindow::SlotZoomIn()
{
	double rate = 1.2;
	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::scale(osg::Vec3(rate, rate, rate));
	matrix->setMatrix(mat0*mat1);

	//m_sceneView->getOsgView()->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN);

	//获得相机朝向，归一化
	//osg::Vec3f eye1,center1,up1;
	//m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye1,center1,up1);
	//osg::Vec3f viewdir =  center1 - eye1;
	////viewdir.normalize();   

	////osg::Node* root = m_sceneModel->getScene();
	////float radius;
	////if (root->getBound().radius() < 0)
	////	radius = -root->getBound().radius()*rate;
	////else
	////	radius = root->getBound().radius()*rate;

	////osg::Vec3d eye(root->getBound().center() + viewdir*radius);
	//osg::Vec3d eye = center1 + viewdir*0.5;
	////osg::Vec3d dst(root->getBound().center().x(), root->getBound().center().y(), root->getBound().center().z());
	//m_sceneView->getOsgView()->getCamera()->setViewMatrixAsLookAt(eye, center1, up1);
	////m_sceneView->getOsgView()->home();

	m_dRat *= rate;
}

void MainWindow::SlotZoomOut()
{
	double rate = 0.8;
	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::scale(osg::Vec3(rate, rate, rate));
	matrix->setMatrix(mat0*mat1);

	//m_sceneView->getOsgView()->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);

	m_dRat *= rate;
}

void MainWindow::SlotUpMove()
{
	double length = 100.0;
	//获得相机朝向，归一化
	osg::Vec3f eye1,center1,up1;
	m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye1,center1,up1);

	if (m_bFirstSet)
	{
		m_eye = eye1;
		m_center = center1;
		m_up = up1;

		m_bFirstSet = false;
	}

	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::translate(up1*length);
	matrix->setMatrix(mat0*mat1);

	//m_sceneView->getOsgView()->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_2D);

	m_dX += length;
}

void MainWindow::SlotDownMove()
{
	double length = -100.0;
	osg::Vec3f eye1,center1,up1;
	m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye1,center1,up1);

	if (m_bFirstSet)
	{
		m_eye = eye1;
		m_center = center1;
		m_up = up1;

		m_bFirstSet = false;
	}

	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::translate(up1*length);
	matrix->setMatrix(mat0*mat1);
	//m_sceneView->getOsgView()->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_RIGHT);

	m_dX += length;
}

void MainWindow::SlotLeftMove()
{
	double length = 100.0;
	osg::Vec3f eye1,center1,up1;
	m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye1,center1,up1);
	if (m_bFirstSet)
	{
		m_eye = eye1;
		m_center = center1;
		m_up = up1;

		m_bFirstSet = false;
	}

	osg::Vec3f viewdir =  center1 - eye1;
	viewdir.normalize();

	osg::Vec3f cross = up1^viewdir;
	cross.normalize();

	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::translate(cross*length);
	matrix->setMatrix(mat0*mat1);

	m_dY += length;
}

void MainWindow::SlotRightMove()
{
	double length = -100.0;
	osg::Vec3f eye1,center1,up1;
	m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye1,center1,up1);
	if (m_bFirstSet)
	{
		m_eye = eye1;
		m_center = center1;
		m_up = up1;

		m_bFirstSet = false;
	}

	osg::Vec3f viewdir =  center1 - eye1;
	viewdir.normalize();

	osg::Vec3f cross = up1^viewdir;
	cross.normalize();

	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::translate(cross*length);
	matrix->setMatrix(mat0*mat1);

	m_dY += length;
}

void MainWindow::SlotCWRotate()
{
	double degree = -10.0;
	osg::Vec3f eye1,center1,up1;
	m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye1,center1,up1);
	if (m_bFirstSet)
	{
		m_eye = eye1;
		m_center = center1;
		m_up = up1;

		m_bFirstSet = false;
	}

	osg::Vec3f viewdir =  eye1 - center1;
	viewdir.normalize();

	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::rotate(osg::inDegrees(degree),viewdir);
	matrix->setMatrix(mat0*mat1);

	m_dDegree += degree;
}

void MainWindow::SlotCCWRotate()
{
	double degree = 10.0;
	osg::Vec3f eye1,center1,up1;
	m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye1,center1,up1);
	if (m_bFirstSet)
	{
		m_eye = eye1;
		m_center = center1;
		m_up = up1;

		m_bFirstSet = false;
	}

	osg::Vec3f viewdir =  eye1 - center1;
	viewdir.normalize();

	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::rotate(osg::inDegrees(degree),viewdir);
	matrix->setMatrix(mat0*mat1);

	m_dDegree += degree;
}

void MainWindow::SlotResume()
{

#if 1
	osg::Vec3f eye,center,up;
	m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye,center,up);

	osg::Vec3f viewdir =  eye - center;
	viewdir.normalize();

	osg::Vec3f cross = up^viewdir;
	cross.normalize();
	//m_sceneView->getOsgView()->getCameraManipulator()->setHomePosition(m_eye,m_center,m_up);
	//m_sceneView->home();
	m_bFirstSet = true;

	double scal = 1.0 / m_dRat;
	double x = -m_dX;
	double y = -m_dY;
	double degree = -m_dDegree;
	osg::ref_ptr<osg::MatrixTransform> matrix = m_sceneModel->getStlNodeMatrix();
	osg::Matrixd mat0 = matrix->getMatrix();
	osg::Matrixd mat1 = osg::Matrix::translate(cross*y)*    //v表示移动量，是一个Vec3，比如（0， 0， 1）表示向z轴移动一个单位
		osg::Matrix::translate(up*x)*
		osg::Matrix::scale(scal,scal,scal)*     //size表示物体缩放比例，如果size=2，则物体尺寸将放大2倍
		osg::Matrix::rotate(osg::inDegrees(degree),viewdir);
	matrix->setMatrix(mat0*mat1);

	m_dRat = 1.0;
	m_dX = 0.0;
	m_dY = 0.0;
	m_dDegree = 0.0;

#endif

	m_sceneView->ResumeView();

}

void MainWindow::SlotSignInAdvanceUser()
{
	//QString strPassword = m_pAdvanceUserDlg->GetPassword();
	//if (strPassword == "666666")
	//{
	//	HideAction(true);
	//}
}

void MainWindow::On_modelColor_change(const QString& name)
{
	for (size_t i = 0; i < m_vSTLs.size();++i)
	{
		if (m_vSTLs[i]->GetSTLName()==name)
		{
			int a =0;
			if (m_vSTLs[i] != NULL)
			{
				int b=0;
				m_vSTLs[i]->SetSelected();
			}
		}
		else
		{
			m_vSTLs[i]->ResumeColor();
		}
	}
}

void MainWindow::InitModel()
{
	//长直管	
	osg::Vec3 center3d(-119.4888507633,599.90808739,1088.0019632417);
	m_pstraiPipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,m_param.fPipeLength,center3d,osg::Vec3(0,0,-1));
	osg::Node* Nodestl = m_sceneModel->getNode("NodeSTL");
	((osg::Group*)Nodestl)->addChild(m_pstraiPipe);

	//车头中心
	osg::Vec3 OriginalCheTouCenter(-119.4888507633,599.90808739,-35.2927818298);
	m_OriginalCheTouCenter = OriginalCheTouCenter;

	//主轮中心
	osg::Vec3 OriginalZhuLunCenter(-77.414334392,617.45861722,1088.0019632417);
	m_CurZhuLunCenter = OriginalZhuLunCenter;

	osg::Vec3 tmpcenter(m_OriginalCheTouCenter.x(),m_OriginalCheTouCenter.y(),m_CurZhuLunCenter.z());
	m_vPipeCenters.push_back(tmpcenter);

	//获取抓料模中心坐标
	osg::Node* zlmNode = m_sceneModel->getNode("zhuanjiao.stl");
	m_CurZhuaLiaoMoCenter = zlmNode->getBound().center()*osg::computeLocalToWorld(zlmNode->getParentalNodePaths()[0]);
}

void MainWindow::CreatLongStraightPipe()
{
	//长直管	
	ReadHeaderPara();

	// osg::Vec3 center3d = m_vPipeCenters[0];
	// m_pstraiPipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,m_param.fPipeLength,center3d,osg::Vec3(0,0,-1));
	m_pstraiPipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,m_param.fPipeLength,m_LongPipeCenter,m_LongPipeNormal);

	osg::Node* Nodestl = m_sceneModel->getNode("NodeSTL");
	osg::ref_ptr<osg::PositionAttitudeTransform> LongStraightpipefatherTrans = m_sceneModel->getLongStraightPipeTrans();
	LongStraightpipefatherTrans->addChild(m_pstraiPipe);
}

void MainWindow::InfoClear(AnimationInfo& info)
{
	Point3D tmp(0,0,0);
	info.fStartTime = 0;
	info.fEndTime = 0; 
	info.translate = tmp;
	info.radius = 0;
	info.rotateX = 0;
	info.rotateY = 0;
	info.rotateZ = 0;

	//memset(&info, 0, sizeof(AnimationInfo));

}

void MainWindow::GetZhuaLiaoMoCenter()
{
	//m_vec3ZhuaLiaoMoCenter = zlmNode->getBound().center()*osg::computeLocalToWorld(zlmNode->getParentalNodePaths()[0]);
}


void MainWindow::JiTouDiZuoAnimation(float& fcurModelLayer,float fmm,double& dtime,AnimationInfo& info,float& curlayer)
{
	if(fcurModelLayer != fmm)
	{
		int movelayer = fmm - (int)fcurModelLayer;
		InfoClear(info);
		info.fStartTime = dtime;
		info.fEndTime = info.fStartTime+m_fAnimationTime; // 小车移动时间
		info.translate.y() = -movelayer*m_LmHeight;
		dtime = info.fEndTime+m_fAnimationStopTime; // 暂停时间

		m_vJiTouDiZuoAnimation.push_back(info);
		fcurModelLayer = fmm;
		m_vec3RotateCenter = osg::Vec3(m_vec3RotateCenter.x(),m_vec3RotateCenter.y()+info.translate.y(),m_vec3RotateCenter.z());
	}
}


void MainWindow::AdvanceZhuanJiao(AnimationInfo& ZhuaLiaoMoinfo,double& dtime)
{
	//转角
	InfoClear(ZhuaLiaoMoinfo);
	ZhuaLiaoMoinfo.fStartTime = dtime;
	ZhuaLiaoMoinfo.fEndTime = ZhuaLiaoMoinfo.fStartTime + m_fAnimationTime;
	//ZhuaLiaoMoinfo.rotateX = m_vproductionvalues[i].fbtm * MYDEG_TO_RAD;
	dtime = ZhuaLiaoMoinfo.fEndTime + m_fAnimationStopTime;
	m_vZhuaLiaoMoAnimation.push_back(ZhuaLiaoMoinfo);

}

void MainWindow::AdvanceTuiWan(AnimationInfo& JiTouWanGuan1info,double& dtime)
{
	//提前退弯
	//InfoClear(JiTouWanGuan1info);
	//JiTouWanGuan1info.fStartTime = dtime;
	//JiTouWanGuan1info.fEndTime = JiTouWanGuan1info.fStartTime + m_fAnimationTime;
	////JiTouWanGuan1info.rotateY = m_vproductionvalues[i].fctm * MYDEG_TO_RAD;
	//dtime = JiTouWanGuan1info.fEndTime + m_fAnimationStopTime;
	//m_lastangle = 
}

void MainWindow::CarReturn(int i,AnimationInfo& XiaoCheinfo,double& dtime)
{
	//动作流程的动作
	int iLayer2 = (int)m_vproductionvalues[i].fmm -1;
	float fSafemoveDis2;
	//小车可以一次移动的距离 当前zlm_X - (导模长+导模安全预留)
	if(m_bIsLeft == false)
	{
		fSafemoveDis2 = m_zlm_X - (m_param.RDmlength[iLayer2] + m_param.fDmaqDistance);

	}
	else if(m_bIsLeft == true)
	{
		fSafemoveDis2 = m_zlm_X - (m_param.LDmlength[iLayer2] + m_param.fDmaqDistance);

	}
	if(fSafemoveDis2 < m_vproductionvalues[i].fmy)
	{
		InfoClear(XiaoCheinfo);
		XiaoCheinfo.fStartTime = dtime;
		XiaoCheinfo.fEndTime = dtime + m_fAnimationTime;
		XiaoCheinfo.translate.x() = m_zlm_X - freturnDis;
		dtime = XiaoCheinfo.fEndTime + m_fAnimationStopTime;
		m_vXiaoCheAnimation.push_back(XiaoCheinfo);
		m_zlm_X = m_zlm_X - XiaoCheinfo.translate.x();
	}
}

// 回退一根管子的长度不够，回退两根管子
void MainWindow::RefreshBackTwoPipe(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float length,int iNth)
{
	osg::ref_ptr<PIPE> pipe1 = m_vPipes[m_vPipes.size()-1];
	double pipeLength = pipe1->GetPipeLength();
	osg::ref_ptr<PIPE> pipe0 = m_vPipes[m_vPipes.size()-2];
    pipeLength += pipe0->GetPipeLength();

	osg::ref_ptr<TextTransform> moveMatrix0 = pipe0->GetMoveMatrix();
	osg::ref_ptr<TextTransform> moveMatrix1 = pipe1->GetMoveMatrix();

	int begin0 = -1;
	int begin1 = -1;
	for (size_t i = 0; i < m_vMatrixAnimation.size();++i)
	{
		if (moveMatrix0==m_vMatrixAnimation[i].first)
		{
			begin0 = i;
		}
	}

	for (size_t i = 0; i < m_vMatrixAnimation.size();++i)
	{
		if (moveMatrix1==m_vMatrixAnimation[i].first)
		{
			begin1 = i;
		}
	}

	AnimationInfo PipeInfo0;
	AnimationInfo PipeInfo1;
	if (begin0>=0 && begin0 < m_vMatrixAnimation.size())
	{
		PipeInfo0 = m_vMatrixAnimation[begin0].second;
	}

	if (begin1 >=0 && begin1 < m_vMatrixAnimation.size())
	{
		PipeInfo1 = m_vMatrixAnimation[begin1].second;
	}

	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> tempInfos0;
	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> tempInfos1;

	for (size_t i = begin0 +1; i < begin1;++i)
	{
		tempInfos0.push_back(m_vMatrixAnimation[i]);
	}

	for (size_t i = begin1 +1; i < m_vMatrixAnimation.size();++i)
	{
		tempInfos1.push_back(m_vMatrixAnimation[i]);
	}

	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> tempAnimations = m_vMatrixAnimation;
	m_vMatrixAnimation.clear();

	if (m_mapMatrixHeader.empty())
	{
		m_mapMatrixHeader.clear();
	}

	m_vMatrixAnimation = std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>>(tempAnimations.begin(),tempAnimations.begin()+begin0);

	m_vPipes.pop_back();
    m_vPipes.pop_back();
	// m_vLongStraightPipAnimation.pop_back();

	double length0 = fabs(pipeLength) - fabs(length);
	//AnimationInfo temp;
	//temp.fStartTime = PipeInfo0.fColorStartTime;
	//temp.fStartTime = PipeInfo0.fStartTime;
	//temp.fEndTime = PipeInfo0.fStartTime + 0.5*(PipeInfo0.fEndTime-PipeInfo0.fStartTime);
	//第一节管子
	AddPipeAndInfos(vec3Center,dir,PipeInfo0,PipeInfo,i,length0,iNth,tempInfos0,*pipe0);
	double length1 = fabs(pipeLength) - length0;

	//temp.fStartTime = PipeInfo0.fStartTime + 0.5*(PipeInfo0.fEndTime-PipeInfo0.fStartTime);
	//temp.fEndTime = PipeInfo0.fEndTime;
	// 第二节管子
	AddPipeAndInfos(vec3Center,dir,PipeInfo1,PipeInfo,i,length1,iNth,tempInfos1,*pipe1);

	// 回退的管子
	ShowStraightPipe(tip,header,vec3Center,dir,XiaoCheinfo,PipeInfo,i,length,iNth);
}

// header 加工这截管子的机头
void MainWindow::ReturnPipeToZeroAndCreate(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,
    AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth,const std::string& process)
{
    AnimationInfo colorInfo = PipeInfo;

    InfoClear(PipeInfo);
    PipeInfo.fStartTime = XiaoCheinfo.fStartTime;
    PipeInfo.fEndTime = XiaoCheinfo.fStartTime+0.5*(XiaoCheinfo.fEndTime-XiaoCheinfo.fStartTime);

    PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
    PipeInfo.translate.z() = -m_dPipePos;

    string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
    name.append("front");
    m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
    m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

    PipeInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
	PipeInfo.pipe = m_vPipes[m_vPipes.size()-1];

    // 前一段管子向前
    m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),PipeInfo));

	m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetMoveMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();
	// 长直管向前
    m_vLongStraightPipAnimation.push_back(PipeInfo);

    m_dPipePos = 0.0;

    osg::ref_ptr<osg::Geode> pipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,flength,vec3Center,dir);//管子半径,长my,中心模坐标，生成管子的方向
    QString PipeName = QString(pipe->getName().c_str());
	osg::ref_ptr<PIPE> p = new PIPE(pipe,PipeName,Straight,vec3Center,m_sceneModel->GetHudtext(),m_productionTable,m_twTable,flength);
	p->SetTip(tip);
	// header编号从1开始
	p->SetHeader(header+1);

	// 机头编号从1开始，机头上的遮挡平面索引从0开始
    pipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);

    InfoClear(PipeInfo);

    PipeInfo.fStartTime = XiaoCheinfo.fStartTime + 0.5*(XiaoCheinfo.fEndTime-XiaoCheinfo.fStartTime);
    PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;
    /*PipeInfo.fColorStartTime = colorInfo.fStartTime;
    PipeInfo.fColorEndTime = colorInfo.fEndTime;*/

    PipeInfo.fColorStartTime = PipeInfo.fStartTime;
    PipeInfo.fColorEndTime = PipeInfo.fEndTime;

    //PipeInfo.pipeSeg = iNth-1;  // 设置当前加工的管子段属于所需加工的第几段管子端
    PipeInfo.pipeSeg = colorInfo.pipeSeg;;
    PipeInfo.group = p->GetPipeNode();
	PipeInfo.pipe = p;

    if(dir == osg::Vec3(0,0,1) || dir == osg::Vec3(0,0,-1))
        PipeInfo.translate.z() = flength;
    else if(dir == osg::Vec3(-1,0,0) || dir == osg::Vec3(1,0,0))
        PipeInfo.translate.x() = flength;
    else if(dir == osg::Vec3(0,1,0) || dir == osg::Vec3(0,-1,0))
        PipeInfo.translate.y() = flength;

    m_vMatrixAnimation.push_back(make_pair(p->GetMoveMatrix(),PipeInfo/*colorInfo*/));
	m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

    m_vPipes.push_back(p);

	//osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
	//temp->addChild(p.GetPipeNode().get());
	//osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(p.GetName().toStdString(), temp);
	//LogInfo(QString("add collision pipe object %1 ").arg(p->GetName()));

    return;
}

// 检查直管回退距离和直管长度是否一致，不一致，则处理
// 拆分成生成两段管子，一段不回退的直管，一段回退的直管
void MainWindow::RefreshBackPipe(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float length,int iNth)
{
	osg::ref_ptr<PIPE> pipe = m_vPipes[m_vPipes.size()-1];
	double pipeLength = pipe->GetPipeLength();
	//AnimationInfo PipeInfo0 = m_vMatrixAnimation.back().second;

	osg::ref_ptr<TextTransform> moveMatrix = m_vPipes.back()->GetMoveMatrix();
	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> tempInfos;
	int begin = -1;
	// 找到删除直管前进运动动画
	for (size_t i = 0; i < m_vMatrixAnimation.size();++i)
	{
		if (moveMatrix == m_vMatrixAnimation[i].first.get())
		{
			begin = i;
		}
	}

	AnimationInfo PipeInfo0;
	if (begin >= 0 && begin < m_vMatrixAnimation.size())
	{
		tempInfos.clear();
		PipeInfo0 = m_vMatrixAnimation[begin].second;
		for (size_t i = begin+1; i < m_vMatrixAnimation.size();++i)
		{
			tempInfos.push_back(m_vMatrixAnimation[i]);
		}
	}

	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> tempPairs = m_vMatrixAnimation;
	m_vMatrixAnimation.clear();

	if (m_mapMatrixHeader.empty())
	{
		m_mapMatrixHeader.clear();
	}

	m_vMatrixAnimation = std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>>(tempPairs.begin(),tempPairs.begin()+begin);
	
	//m_vMatrixAnimation.pop_back();
	m_vPipes.pop_back();
	//m_vLongStraightPipAnimation.pop_back();

	double length0 = fabs(pipeLength) - fabs(length);
	AnimationInfo temp;
	//temp.fStartTime = PipeInfo0.fColorStartTime;
	temp.fStartTime = PipeInfo0.fStartTime;
	temp.fEndTime = PipeInfo0.fStartTime + 0.5*(PipeInfo0.fEndTime-PipeInfo0.fStartTime);
	//第一节管子
	ShowStraightPipe(tip,header,vec3Center,dir,temp,PipeInfo,i,length0,iNth);
	double length1 = fabs(pipeLength) - length0;

	temp.fStartTime = PipeInfo0.fStartTime + 0.5*(PipeInfo0.fEndTime-PipeInfo0.fStartTime);
	temp.fEndTime = PipeInfo0.fEndTime;
	// 第二节管子
	AddPipeAndInfos(vec3Center,dir,temp,PipeInfo,i,length1,iNth,tempInfos,*pipe);

	// 回退的管子
	ShowStraightPipe(tip,header,vec3Center,dir,XiaoCheinfo,PipeInfo,i,length,iNth);
}

void MainWindow::ShowStraightPipeNew(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,
    AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth,const std::string& process)//iNth:文字高亮显示行
{
    // if(flength == 0.0)
	//{
	//	return;
	//}
	// 有可能出现my-myt'-myth'（1723-1823-（-100））为0，实际运动不为0的情况

    AnimationInfo colorInfo = PipeInfo;
	if (process.find("P")!=std::string::npos)
	{
		// 置管，管子不是因为运动生成
		osg::ref_ptr<osg::Geode> pipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,flength,vec3Center,dir);//管子半径,长my,中心模坐标，生成管子的方向
        QString name = QString(pipe->getName().c_str());
		osg::ref_ptr<PIPE> p = new PIPE(pipe,name,Straight,vec3Center,m_sceneModel->GetHudtext(),m_productionTable,m_twTable,flength);
		p->SetHide(false);
		p->SetTip(tip);
		// 机头编号从1开始
		p->SetHeader(header+1);

		// 置管不用遮挡
        // pipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);
        InfoClear(PipeInfo);

        PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
        PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;
        PipeInfo.fColorStartTime = colorInfo.fStartTime;
        PipeInfo.fColorEndTime = colorInfo.fEndTime;
        //PipeInfo.pipeSeg = iNth-1;  // 设置当前加工的管子段属于所需加工的第几段管子端
        PipeInfo.pipeSeg = colorInfo.pipeSeg;;
        PipeInfo.group = p->GetPipeNode();
		PipeInfo.pipe = p;

        //if(dir == osg::Vec3(0,0,1) || dir == osg::Vec3(0,0,-1))
        //    PipeInfo.translate.z() = flength;
        //else if(dir == osg::Vec3(-1,0,0) || dir == osg::Vec3(1,0,0))
        //    PipeInfo.translate.x() = flength;
        //else if(dir == osg::Vec3(0,1,0) || dir == osg::Vec3(0,-1,0))
        //    PipeInfo.translate.y() = flength;

        m_vMatrixAnimation.push_back(make_pair(p->GetMoveMatrix(),PipeInfo/*colorInfo*/));
		m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

        m_vPipes.push_back(p);

		// osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
		// temp->addChild(p.GetPipeNode().get());
		//osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(p.GetName().toStdString(), temp);
		//LogInfo(QString("add collision pipe object %1 ").arg(p->GetName()));

        return;
	}

    if (process.find("1)")!=std::string::npos         // my
        || (process.find("9)")!=std::string::npos )   // myt
        || (process.find("14)")!=std::string::npos))  // myth
    {
        LogInfo("this is myt");
		if (flength == 0.0)
		{
			return;
		}
        else if (flength > 0.0)
        {
            if (m_dPipePos < 0.0)
            {
                // 先前进一段，管子位置到0，方便和后面管子接上
                ReturnPipeToZeroAndCreate(tip,header,vec3Center,dir,XiaoCheinfo,PipeInfo, i,flength,iNth, process);

                return;
            }

            osg::ref_ptr<osg::Geode> pipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,flength,vec3Center,dir);//管子半径,长my,中心模坐标，生成管子的方向
            QString name = QString(pipe->getName().c_str());
			osg::ref_ptr<PIPE> p  = new PIPE(pipe,name,Straight,vec3Center,m_sceneModel->GetHudtext(),m_productionTable,m_twTable,flength);
			p->SetHide(false);
			p->SetTip(tip);
			// header编号从1开始，管子中心位置数组从0开始
			p->SetHeader(header+1);

			// 机头编号从1开始，机头对应的遮挡平面索引从0开始
            pipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);
            InfoClear(PipeInfo);

            PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
            PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;
            PipeInfo.fColorStartTime = colorInfo.fStartTime;
            PipeInfo.fColorEndTime = colorInfo.fEndTime;
            //PipeInfo.pipeSeg = iNth-1;  // 设置当前加工的管子段属于所需加工的第几段管子端
            PipeInfo.pipeSeg = colorInfo.pipeSeg;;
            PipeInfo.group = p->GetPipeNode();
			PipeInfo.pipe = p;

            if(dir == osg::Vec3(0,0,1) || dir == osg::Vec3(0,0,-1))
                PipeInfo.translate.z() = flength;
            else if(dir == osg::Vec3(-1,0,0) || dir == osg::Vec3(1,0,0))
                PipeInfo.translate.x() = flength;
            else if(dir == osg::Vec3(0,1,0) || dir == osg::Vec3(0,-1,0))
                PipeInfo.translate.y() = flength;

            m_vMatrixAnimation.push_back(make_pair(p->GetMoveMatrix(),PipeInfo/*colorInfo*/));
			m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

            m_vPipes.push_back(p);

			//osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
			//temp->addChild(p->GetPipeNode().get());
			//osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(p->GetName().toStdString(), temp);
			//LogInfo(QString("add collision pipe object %1 ").arg(p->GetName()));

            return;
        }
        else
        {
            // 长度小于0,直接回退
            InfoClear(PipeInfo);
			PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
			PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;

			PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
			PipeInfo.translate.z() = flength;

			string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
			name.append("back");
			m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
			m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

			PipeInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
			PipeInfo.pipe = m_vPipes[m_vPipes.size()-1];

			m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),PipeInfo));
			m_vLongStraightPipAnimation.push_back(PipeInfo);

            m_dPipePos += flength;

			return;
        }
    }
    else
    {
        LogInfo("this is my'- myt - myth");
        // 当前弯的直管长度
        float lengthMy = m_vproductionvalues[iNth].fmy;
        if (lengthMy==0)
        {
            return;
        }

        if (lengthMy < 0.0)
        {
            // 长度小于0,直接回退
            InfoClear(PipeInfo);
			PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
			PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;

			PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
			PipeInfo.translate.z() = lengthMy;

			string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
			name.append("back");
			m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
			m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

			PipeInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
			PipeInfo.pipe = m_vPipes[m_vPipes.size()-1];

			m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),PipeInfo));
			m_vLongStraightPipAnimation.push_back(PipeInfo);

            m_dPipePos += flength;

            return;
        }
        else
        {
            float lengthMyt = m_vproductionvalues[iNth-1].fmyt;
            if (lengthMyt<=0.0)
            {
                lengthMyt = 0.0;
            }

            float lengthMyth = m_vproductionvalues[iNth-1].fmyth;
            if (lengthMyth<=0.0)
            {
                lengthMyth = 0.0;
            }

            float length = lengthMy -lengthMyt - lengthMyth;
			if (length == 0.0)
			{
				return;
			}
            else if (length>0.0)
            {
                if (m_dPipePos < 0.0)
                {
                    // 先前进一段，管子位置到0，方便和后面管子接上
                    ReturnPipeToZeroAndCreate(tip,header,vec3Center,dir,XiaoCheinfo,PipeInfo, i,length,iNth, process);

                    return;
                }

                osg::ref_ptr<osg::Geode> pipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,length,vec3Center,dir);//管子半径,长my,中心模坐标，生成管子的方向
                QString name = QString(pipe->getName().c_str());
				osg::ref_ptr<PIPE> p = new PIPE(pipe,name,Straight,vec3Center,m_sceneModel->GetHudtext(),m_productionTable,m_twTable,length);
				p->SetHide(false);
				p->SetTip(tip);
				// 机头编号从1开始
				p->SetHeader(header+1);

                pipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);
                InfoClear(PipeInfo);

                PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
                PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;
                PipeInfo.fColorStartTime = colorInfo.fStartTime;
                PipeInfo.fColorEndTime = colorInfo.fEndTime;
                //PipeInfo.pipeSeg = iNth-1;  // 设置当前加工的管子段属于所需加工的第几段管子端
                PipeInfo.pipeSeg = colorInfo.pipeSeg;;
                PipeInfo.group = p->GetPipeNode();
				PipeInfo.pipe = p;

                if(dir == osg::Vec3(0,0,1) || dir == osg::Vec3(0,0,-1))
                    PipeInfo.translate.z() = length;
                else if(dir == osg::Vec3(-1,0,0) || dir == osg::Vec3(1,0,0))
                    PipeInfo.translate.x() = length;
                else if(dir == osg::Vec3(0,1,0) || dir == osg::Vec3(0,-1,0))
                    PipeInfo.translate.y() = length;

                m_vMatrixAnimation.push_back(make_pair(p->GetMoveMatrix(),PipeInfo/*colorInfo*/));
				m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

                m_vPipes.push_back(p);

				//osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
				//temp->addChild(p.GetPipeNode().get());
				//osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(p.GetName().toStdString(), temp);
				//LogInfo(QString("add collision pipe object %1 ").arg(p->GetName()));

            }
            else if (length<0.0)
            {
                if (fabs(length)<fabs(m_vPipes[m_vPipes.size()-1]->GetPipeLength()) && m_vPipes[m_vPipes.size()-1]->GetType()==Straight)
                {
                    // 管子分成三段，两段前进，后一段后退
                    RefreshBackPipe(tip,header,vec3Center,dir,XiaoCheinfo,PipeInfo,i,length,iNth);
                    return;
                }
                else if (fabs(length)<fabs(m_vPipes[m_vPipes.size()-1]->GetPipeLength()+m_vPipes[m_vPipes.size()-2]->GetPipeLength())
                    && (m_vPipes[m_vPipes.size()-1]->GetType()==Straight && m_vPipes[m_vPipes.size()-2]->GetType()==Straight))
                {
                    RefreshBackTwoPipe(tip,header,vec3Center,dir,XiaoCheinfo,PipeInfo,i,length,iNth);
                }
            }
        }
    }

	//if(flength < 0.0) // 管子往回拉
	//{
	//	// 回退的管子比实际的管子短
	//	if (fabs(flength)<fabs(m_vPipes[m_vPipes.size()-1].GetPipeLength()) && m_vPipes[m_vPipes.size()-1].GetType()==Straight)
	//	{
	//		// 管子分成三段，两段前进，后一段后退
	//		RefreshBackPipe(vec3Center,dir,XiaoCheinfo,PipeInfo,i,flength,iNth);
	//		return;
	//	}
	//	else if (m_vPipes[m_vPipes.size()-1].GetType()==Curve)
	//	{
	//		// 没有实质性的管子形状，增加一段回退
	//		InfoClear(PipeInfo);
	//		PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
	//		PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;

	//		PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
	//		PipeInfo.translate.z() = flength;

	//		string name = m_vPipes[m_vPipes.size()-1].GetPipeNode()->getName();
	//		name.append("CurveBack");
	//		m_vPipes[m_vPipes.size()-1].GetPipeNode()->setName(name);
	//		m_vPipes[m_vPipes.size()-1].SetName(QString(name.c_str()));

	//		PipeInfo.group = m_vPipes[m_vPipes.size()-1].GetPipeNode();
	//		m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetMoveMatrix(),PipeInfo));
	//		m_vLongStraightPipAnimation.push_back(PipeInfo);

	//		return;
	//	}
	//	else
	//	{
	//		//小车后退
	//		InfoClear(PipeInfo);
	//		PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
	//		PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;

	//		PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
	//		PipeInfo.translate.z() = flength;

	//		string name = m_vPipes[m_vPipes.size()-1].GetPipeNode()->getName();
	//		name.append("back");
	//		m_vPipes[m_vPipes.size()-1].GetPipeNode()->setName(name);
	//		m_vPipes[m_vPipes.size()-1].SetName(QString(name.c_str()));

	//		PipeInfo.group = m_vPipes[m_vPipes.size()-1].GetPipeNode();
	//		m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetMoveMatrix(),PipeInfo));
	//		m_vLongStraightPipAnimation.push_back(PipeInfo);

	//		return;
	//	}
	//}

	
	//m_vLongStraightPipAnimation.push_back(PipeInfo);
}

void MainWindow::AddPipeAndInfos(osg::Vec3 vec3Center,osg::Vec3 dir,AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth,
	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>>& infos,PIPE& oldPipe)
{
	if(flength == 0.0)
	{
		int a = 0;
		return;
	}

	AnimationInfo colorInfo = PipeInfo;

	if(flength < 0.0)//管子往回拉
	{
		// 回退的管子比实际的管子短
		if (fabs(flength)<fabs(m_vPipes[m_vPipes.size()-1]->GetPipeLength()) && m_vPipes[m_vPipes.size()-1]->GetType()==Straight)
		{
			// 管子分成三段，两段前进，后一段后退
			RefreshBackPipe(m_vPipes[m_vPipes.size()-1]->GetTip(),m_vPipes[m_vPipes.size()-1]->GetHeader(),vec3Center,dir,XiaoCheinfo,PipeInfo,i,flength,iNth);
			return;
		}
		else if (m_vPipes[m_vPipes.size()-1]->GetType()==Curve)
		{
			// 没有实质性的管子形状，增加一段回退
			InfoClear(PipeInfo);
			PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
			PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;

			PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
			PipeInfo.translate.z() = flength;

			string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
			name.append("CurveBack");
			m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
			m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

			PipeInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
			PipeInfo.pipe = m_vPipes[m_vPipes.size()-1];
			m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),PipeInfo));
			m_vLongStraightPipAnimation.push_back(PipeInfo);

			return;
		}
		else
		{
			//小车后退
			InfoClear(PipeInfo);
			PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
			PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;

			PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
			PipeInfo.translate.z() = flength;

			string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
			name.append("back");
			m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
			m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

			PipeInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
			PipeInfo.pipe = m_vPipes[m_vPipes.size()-1];

			m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),PipeInfo));
			//m_vLongStraightPipAnimation.push_back(PipeInfo);

			return;
		}
	}

	osg::ref_ptr<osg::Geode> pipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,flength,vec3Center,dir);//管子半径,长my,中心模坐标，生成管子的方向
	QString name = QString(pipe->getName().c_str());
	osg::ref_ptr<PIPE> p = new PIPE(pipe,name,Straight,vec3Center,m_sceneModel->GetHudtext(),m_productionTable,m_twTable,flength);
	p->SetHide(false);
	p->SetTip(m_vPipes[m_vPipes.size()-1]->GetTip());
	int header = m_vPipes[m_vPipes.size()-1]->GetHeader();
	p->SetHeader(m_vPipes[m_vPipes.size()-1]->GetHeader());

	// 机头编号从1开始，机头对应的遮挡平面编号从0开始
	pipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header-1), osg::StateAttribute::ON);
	InfoClear(PipeInfo);

	PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
	PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;
	PipeInfo.fColorStartTime = colorInfo.fStartTime;
	PipeInfo.fColorEndTime = colorInfo.fEndTime;
	//PipeInfo.pipeSeg = iNth-1;  // 设置当前加工的管子段属于所需加工的第几段管子端
	PipeInfo.pipeSeg = colorInfo.pipeSeg;;
	PipeInfo.group = p->GetPipeNode();
	PipeInfo.pipe = p;

	if(dir == osg::Vec3(0,0,1) || dir == osg::Vec3(0,0,-1))
		PipeInfo.translate.z() = flength;
	else if(dir == osg::Vec3(-1,0,0) || dir == osg::Vec3(1,0,0))
		PipeInfo.translate.x() = flength;
	else if(dir == osg::Vec3(0,1,0) || dir == osg::Vec3(0,-1,0))
		PipeInfo.translate.y() = flength;

	// 移动的动作被替换
	m_vMatrixAnimation.push_back(make_pair(p->GetMoveMatrix(),PipeInfo/*colorInfo*/));
	m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

	for (size_t i = 0; i < infos.size(); ++i)
	{
		// 旋转的动作都添加进新管子
		// m_vMatrixAnimation.push_back(make_pair(p.GetRotateMatrix(),infos[i].second));
		m_vMatrixAnimation.push_back(infos[i]);
	}

	m_mapMatrixInstead[oldPipe.GetMoveMatrix()] = p->GetMoveMatrix();
	m_mapMatrixInstead[oldPipe.GetRotateMatrix()] = p->GetRotateMatrix();
	m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

	m_vPipes.push_back(p);

	//osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
	//temp->addChild(p.GetPipeNode().get());
	//osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(p.GetName().toStdString(), temp);
	//LogInfo(QString("add collision pipe object %1 ").arg(p->GetName()));

}

void MainWindow::ShowStraightPipe(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth)
{
	if(flength == 0.0)
	{
		int a = 0;
		return;
	}

	AnimationInfo colorInfo = PipeInfo;

	if(flength < 0.0)//管子往回拉
	{
		// 回退的管子比实际的管子短
		if (fabs(flength)<fabs(m_vPipes[m_vPipes.size()-1]->GetPipeLength()) && m_vPipes[m_vPipes.size()-1]->GetType()==Straight)
		{
			// 管子分成三段，两段前进，后一段后退
			RefreshBackPipe(tip,header,vec3Center,dir,XiaoCheinfo,PipeInfo,i,flength,iNth);
			return;
		}
		else if (m_vPipes[m_vPipes.size()-1]->GetType()==Curve)
		{
			// 没有实质性的管子形状，增加一段回退
			InfoClear(PipeInfo);
			PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
			PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;

			PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
			PipeInfo.translate.z() = flength;

			string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
			name.append("CurveBack");
			m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
			m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

			PipeInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
			PipeInfo.pipe = m_vPipes[m_vPipes.size()-1];

			m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),PipeInfo));
			m_vLongStraightPipAnimation.push_back(PipeInfo);

			m_dPipePos += flength;

			return;
		}
		else
		{
			//小车后退
			InfoClear(PipeInfo);
			PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
			PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;

			PipeInfo.pipeSeg = iNth;  // 设置当前加工的管子段属于所需加工的第几段管子端
			PipeInfo.translate.z() = flength;

			string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
			name.append("back");
			m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
			m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

			m_vPipes[m_vPipes.size()-1]->SetHide(true);

			PipeInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
			PipeInfo.pipe = m_vPipes[m_vPipes.size()-1];

			m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),PipeInfo));
			// 已经弯出来的管子往后，运动完后往后的一截直管会消失，如果长直管也往后，会出现管子空挡
			//m_vLongStraightPipAnimation.push_back(PipeInfo);

			//m_dPipePos += flength;

			return;
		}
	}

	osg::ref_ptr<osg::Geode> pipe = m_sceneModel->creatStraitPipe(m_param.fPipeDiameter/2,flength,vec3Center,dir);//管子半径,长my,中心模坐标，生成管子的方向
	QString name = QString(pipe->getName().c_str());
	osg::ref_ptr<PIPE> p = new PIPE(pipe,name,Straight,vec3Center,m_sceneModel->GetHudtext(),m_productionTable,m_twTable,flength);
	p->SetHide(false);
	p->SetTip(tip);

	// 机头编号从1开始
	p->SetHeader(header+1);

	// 机头编号从1开始，机头对应的遮挡平面索引从0开始
	pipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);
	InfoClear(PipeInfo);

	PipeInfo.fStartTime = XiaoCheinfo.fStartTime /*- (m_fTimeOff/(i+0.85))*/;
	PipeInfo.fEndTime = XiaoCheinfo.fEndTime /*- (m_fTimeOff/(i+0.85))*/;
	PipeInfo.fColorStartTime = colorInfo.fStartTime;
	PipeInfo.fColorEndTime = colorInfo.fEndTime;
	//PipeInfo.pipeSeg = iNth-1;  // 设置当前加工的管子段属于所需加工的第几段管子端
	PipeInfo.pipeSeg = colorInfo.pipeSeg;;
	PipeInfo.group = p->GetPipeNode();
	PipeInfo.pipe = p;

	if(dir == osg::Vec3(0,0,1) || dir == osg::Vec3(0,0,-1))
		PipeInfo.translate.z() = flength;
	else if(dir == osg::Vec3(-1,0,0) || dir == osg::Vec3(1,0,0))
		PipeInfo.translate.x() = flength;
	else if(dir == osg::Vec3(0,1,0) || dir == osg::Vec3(0,-1,0))
		PipeInfo.translate.y() = flength;

	m_vMatrixAnimation.push_back(make_pair(p->GetMoveMatrix(),PipeInfo/*colorInfo*/));
	m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

	m_vPipes.push_back(p);

	//osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
	//temp->addChild(p.GetPipeNode().get());
	//osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(p.GetName().toStdString(), temp);
	//LogInfo(QString("add collision pipe object %1 ").arg(p->GetName()));

	m_mapMatrixInstead[p->GetMoveMatrix()] = p->GetMoveMatrix();
	m_mapMatrixInstead[p->GetRotateMatrix()] = p->GetRotateMatrix();
	//m_vLongStraightPipAnimation.push_back(PipeInfo);
}

void MainWindow::ShowCurvePipe(int i,int tip,int header,osg::Vec3 center,osg::Vec3 dir,AnimationInfo& JiTouWanGuan1info,
	                           AnimationInfo PipeInfo, float fcurveR ,float fcurveAngle, bool bIsTuiWan/*,bool bIsAngleChange*/)
{
	if(fcurveAngle == 0.0)
	{
		int a = 0;
		return;
	}

	if (m_dPipePos<0.0)
	{
		// 先将已经生成的管子末端回退到原点
		AnimationInfo& infoFront = m_vMatrixAnimation.back().second;

		AnimationInfo info;
		if (m_vPipeBackMatrixAnimation.empty())
		{
			infoFront.fEndTime = infoFront.fStartTime + 0.5*(infoFront.fEndTime - infoFront.fStartTime);
			info = infoFront;

			AnimationInfo newInfo;
			InfoClear(newInfo);
			newInfo.fStartTime = info.fEndTime;
			newInfo.fEndTime = info.fEndTime+info.fEndTime-info.fStartTime;

			newInfo.fColorStartTime = info.fColorStartTime;
			newInfo.fColorEndTime = info.fColorEndTime;

			newInfo.pipeSeg = info.pipeSeg;  // 设置当前加工的管子段属于所需加工的第几段管子端
			newInfo.translate.z() = -m_dPipePos;

			string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
			name.append("front");
			m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
			m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

			newInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
			newInfo.pipe = m_vPipes[m_vPipes.size()-1];

			// 前一段管子向前
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),newInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetMoveMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();

			// 长直管向前
			m_vLongStraightPipAnimation.push_back(newInfo);

			m_dPipePos = 0.0;
		}
		else
		{
			AnimationInfo& infoBack = m_vPipeBackMatrixAnimation.back().second;
			if (infoBack.fStartTime>infoFront.fStartTime)
			{
				// 先缩后伸，合并处理
				if (infoBack.translate.z()-m_dPipePos > 0)
				{
					// 合并后向前运动
					info = infoBack;

					AnimationInfo newInfo;
					InfoClear(newInfo);
					newInfo.fStartTime = info.fStartTime;
					newInfo.fEndTime = info.fEndTime;

					newInfo.fColorStartTime = info.fColorStartTime;
					newInfo.fColorEndTime = info.fColorEndTime;

					newInfo.pipeSeg = info.pipeSeg;  // 设置当前加工的管子段属于所需加工的第几段管子端
					newInfo.translate.z() = -m_dPipePos+infoBack.translate.z();

					string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
					name.append("front");
					m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
					m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

					newInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
					newInfo.pipe = m_vPipes[m_vPipes.size()-1];

					m_vLongStraightPipAnimation.pop_back();
					// 前一段管子向前
					m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),newInfo));
					m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetMoveMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();

					// 长直管向前
					//newInfo.translate.z() = -m_dPipePos;
					//m_vLongStraightPipAnimation.push_back(newInfo);

					m_dPipePos = 0.0;
					m_vPipeBackMatrixAnimation.pop_back();
					//m_vLongStraightPipAnimation.pop_back();
				}
				else
				{
					// 合并后向后运动
					infoBack.translate.z() = infoBack.translate.z()-m_dPipePos;
					m_vLongStraightPipAnimation.back().translate.z() = infoBack.translate.z()-m_dPipePos;

					m_dPipePos += -m_dPipePos;
				}
				
			}
			else
			{
				infoFront.fEndTime = infoFront.fStartTime + 0.5*(infoFront.fEndTime - infoFront.fStartTime);
			    info = infoFront;

				AnimationInfo newInfo;
				InfoClear(newInfo);
				newInfo.fStartTime = info.fEndTime;
				newInfo.fEndTime = info.fEndTime+info.fEndTime-info.fStartTime;

				newInfo.fColorStartTime = info.fColorStartTime;
				newInfo.fColorEndTime = info.fColorEndTime;

				newInfo.pipeSeg = info.pipeSeg;  // 设置当前加工的管子段属于所需加工的第几段管子端
				newInfo.translate.z() = -m_dPipePos;

				string name = m_vPipes[m_vPipes.size()-1]->GetPipeNode()->getName();
				name.append("front");
				m_vPipes[m_vPipes.size()-1]->GetPipeNode()->setName(name);
				m_vPipes[m_vPipes.size()-1]->SetName(QString(name.c_str()));

				newInfo.group = m_vPipes[m_vPipes.size()-1]->GetPipeNode();
				newInfo.pipe = m_vPipes[m_vPipes.size()-1];

				// 前一段管子向前
				m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetMoveMatrix(),newInfo));
				m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetMoveMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();

				// 长直管向前
				m_vLongStraightPipAnimation.push_back(newInfo);

				m_dPipePos = 0.0;
			}
		}
	}

	AnimationInfo colorInfo = PipeInfo;

	if (m_bIsLeft)
	{
		center[0]-= m_param.fPipeDiameter*0.5;
	}
	else
	{
		center[0]+= m_param.fPipeDiameter*0.5;
	}

	//if (fcurveAngle<0.0)
	//{
	//	dir *= -1.0;
	//	fcurveAngle *= -1.0;
	//}

	if (fabs(fcurveAngle)<180.0)
	{
		//弯管弧半径,管子半径,弯管角度,，弯曲方向
		osg::ref_ptr<osg::Geode> pipe = m_sceneModel->creatArcPipe(fcurveR/*+m_param.fPipeDiameter/2*/,m_param.fPipeDiameter/2,m_vPipeRotateAngles[header],fcurveAngle,center,dir);
		QString name("Curve");
		name.append(QString::number(fcurveAngle));
		osg::ref_ptr<PIPE> p = new PIPE(pipe,name,Curve,center,m_sceneModel->GetHudtext(),m_productionTable,m_twTable,fcurveR*fcurveAngle*PI/180.0);
		p->SetHide(false);
	    pipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);
        //p.GetPipeRootNode()->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);

		//m_pstraiPipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getLongClipPlane(), osg::StateAttribute::ON);
		p->SetTip(tip);
		// 机头编号从1开始
		p->SetHeader(header+1);

		float ftmpMove = 0;//移动圆心
		InfoClear(PipeInfo);
		if(m_bIsLeft == true)//X轴平移
		{
			p->GetPosMatrix()->setPosition(osg::Vec3((fcurveR+m_param.fPipeDiameter/2),0,0));
			ftmpMove = (fcurveR+m_param.fPipeDiameter/2);
			PipeInfo.fdir = 1;
		}
		else if(m_bIsLeft == false)
		{
			p->GetPosMatrix()->setPosition(osg::Vec3(-(fcurveR+m_param.fPipeDiameter/2),0,0));
			ftmpMove = -(fcurveR+m_param.fPipeDiameter/2 );
			PipeInfo.fdir = -1;
		}	

		PipeInfo.fStartTime = JiTouWanGuan1info.fStartTime;
		PipeInfo.fEndTime = JiTouWanGuan1info.fEndTime;
		PipeInfo.fColorStartTime = colorInfo.fStartTime;
		PipeInfo.fColorEndTime = colorInfo.fEndTime;

		if(bIsTuiWan == false)//这种不需要的
		{
			//PipeInfo.rotateY = fcurveAngle;

			//if(m_bIsLeft == true)
			//{
			//	PipeInfo.center3d = osg::Vec3(m_vec3RotateCenter.x(),m_vec3RotateCenter.y(),m_vec3RotateCenter.z() - m_param.fPipeDiameter/2 + 8.3);
			//}
			//else
			//{
			//	PipeInfo.center3d = osg::Vec3(m_vec3RotateCenter.x(),m_vec3RotateCenter.y(),m_vec3RotateCenter.z() + m_param.fPipeDiameter/2 + 5);
			//}

		}
		else if(bIsTuiWan == true)
		{
			PipeInfo.rotateY = fcurveAngle;
			PipeInfo.center3d = osg::Vec3(center.x()+ftmpMove,center.y(),center.z());
		}

		//PipeInfo.pipeSeg = i;
		PipeInfo.pipeSeg = colorInfo.pipeSeg;
		PipeInfo.group = p->GetPipeNode();
		PipeInfo.pipe = p;

		m_vMatrixAnimation.push_back(make_pair(p->GetMoveMatrix(),PipeInfo/*colorInfo*/));
		m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

		m_vPipes.push_back(p);

		//osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
		//temp->addChild(p.GetPipeNode().get());
		//osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(p.GetName().toStdString(), temp);
		//LogInfo(QString("add collision pipe object %1 ").arg(p->GetName()));

		PipeInfo.translate.z() = fcurveR*fcurveAngle*PI/180.0;
	}
	else
	{
		// 如果圆弧圆心角大于180度，分成两段，解决圆弧跑出遮挡平面的问题
		for (int i = 0; i < 2;++i)
		{
			osg::ref_ptr<osg::Geode> pipe = m_sceneModel->creatArcPipe(fcurveR/*+m_param.fPipeDiameter/2*/,m_param.fPipeDiameter/2,m_vPipeRotateAngles[header],fcurveAngle*0.5,center,dir);//弯管弧半径,管子半径,弯管角度,，弯曲方向
			QString name("Curve");
			name.append(QString::number(fcurveAngle*0.5));
			osg::ref_ptr<PIPE> p = new PIPE(pipe,name,Curve,center,m_sceneModel->GetHudtext(),m_productionTable,m_twTable,0.5*fcurveR*fcurveAngle*PI/180.0);
			p->SetHide(false);
			 pipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);
            // 剪切平面从根节点绑定，避免管子运动的时候，剪切平面也运动，起不到遮挡作用
            //p.GetPipeRootNode()->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getClipPlane(header), osg::StateAttribute::ON);
			p->SetTip(tip);
			// 机头编号从1开始
			p->SetHeader(header+1);
			//m_pstraiPipe->getOrCreateStateSet()->setAttributeAndModes(m_sceneModel->getLongClipPlane(), osg::StateAttribute::ON);

			float ftmpMove = 0;//移动圆心
			InfoClear(PipeInfo);
			if(m_bIsLeft == true)//X轴平移
			{
				p->GetPosMatrix()->setPosition(osg::Vec3((fcurveR+m_param.fPipeDiameter/2),0,0));
				ftmpMove = (fcurveR+m_param.fPipeDiameter/2);
				PipeInfo.fdir = 1;
			}
			else if(m_bIsLeft == false)
			{
				p->GetPosMatrix()->setPosition(osg::Vec3(-(fcurveR+m_param.fPipeDiameter/2),0,0));
				ftmpMove = -(fcurveR+m_param.fPipeDiameter/2 );
				PipeInfo.fdir = -1;
			}	

			PipeInfo.fStartTime = JiTouWanGuan1info.fStartTime+i*(JiTouWanGuan1info.fEndTime-JiTouWanGuan1info.fStartTime)*0.5;
			PipeInfo.fEndTime = JiTouWanGuan1info.fEndTime+(i-1)*(JiTouWanGuan1info.fEndTime-JiTouWanGuan1info.fStartTime)*0.5;
			PipeInfo.fColorStartTime = colorInfo.fStartTime+i*(colorInfo.fEndTime-colorInfo.fStartTime)*0.5;
			PipeInfo.fColorEndTime = colorInfo.fEndTime+(i-1)*(colorInfo.fEndTime-colorInfo.fStartTime)*0.5;

			if(bIsTuiWan == false)//这种不需要的
			{
				//PipeInfo.rotateY = fcurveAngle;

				//if(m_bIsLeft == true)
				//{
				//	PipeInfo.center3d = osg::Vec3(m_vec3RotateCenter.x(),m_vec3RotateCenter.y(),m_vec3RotateCenter.z() - m_param.fPipeDiameter/2 + 8.3);
				//}
				//else
				//{
				//	PipeInfo.center3d = osg::Vec3(m_vec3RotateCenter.x(),m_vec3RotateCenter.y(),m_vec3RotateCenter.z() + m_param.fPipeDiameter/2 + 5);
				//}

			}
			else if(bIsTuiWan == true)
			{
				PipeInfo.rotateY = fcurveAngle*0.5;
				PipeInfo.center3d = osg::Vec3(center.x()+ftmpMove,center.y(),center.z());
			}

			//PipeInfo.pipeSeg = i;
			PipeInfo.pipeSeg = colorInfo.pipeSeg;
			PipeInfo.group = p->GetPipeNode();
			PipeInfo.pipe = p;

			m_vMatrixAnimation.push_back(make_pair(p->GetMoveMatrix(),PipeInfo/*colorInfo*/));
			m_mapMatrixHeader[p->GetMoveMatrix()] = p->GetHeader();

			m_vPipes.push_back(p);

			//osg::ref_ptr<MatrixTransform> temp = new MatrixTransform();
			//temp->addChild(p.GetPipeNode().get());
			//osgCollisionDetection::CollisionInterface::Instence()->addCollisionObject(p.GetName().toStdString(), temp);
			//LogInfo(QString("add collision pipe object %1 ").arg(p->GetName()));

			PipeInfo.translate.z() = 0.5*fcurveR*fcurveAngle*PI/180.0;
		}
	}
	
	//m_vLongStraightPipAnimation.push_back(PipeInfo);
}

void MainWindow::PipeRotateX(AnimationInfo& ZhuaLiaoMoinfo,AnimationInfo PipeInfo,float frotateX ,int i)
{
	if(frotateX == 0.0)
	{
		return;
	}

	if ((frotateX > 90.0 && frotateX <=180.0)
		|| (frotateX<-90.0 && frotateX >= -180.0))
	{
		// 分成两段
		InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime;
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fStartTime+0.5*(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime);	
		PipeInfo.rotateZ = frotateX*0.5;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();

			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

		InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime+0.5*(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime);
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fEndTime;	
		PipeInfo.rotateZ = frotateX*0.5;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();

			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

		return;
	}
	else if ((frotateX > 180.0 && frotateX <=270.0)
		|| (frotateX<-180.0 && frotateX >= -270.0))
	{
		// 分成三段
		InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime;
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fStartTime+(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/3.0;	
		PipeInfo.rotateZ = frotateX/3.0;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();
			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

		InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime + (ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/3.0;
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fStartTime + 2.0*(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/3.0;	
		PipeInfo.rotateZ = frotateX/3.0;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();
			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

	    InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime + 2.0*(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/3.0;
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fEndTime;	
		PipeInfo.rotateZ = frotateX/3.0;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();
			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

		return;
	}
	else if ((frotateX > 270.0 && frotateX <=360.0)
	|| (frotateX<-270.0 && frotateX >= -360.0))
	{
		// 分成四段
		InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime;
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fStartTime+(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/4.0;	
		PipeInfo.rotateZ = frotateX/4.0;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();
			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

		InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime + (ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/4.0;
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fStartTime + 2.0*(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/4.0;	
		PipeInfo.rotateZ = frotateX/4.0;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();
			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

	    InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime + 2.0*(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/4.0;
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fStartTime + 3.0*(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/4.0;	
		PipeInfo.rotateZ = frotateX/4.0;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();
			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

		InfoClear(PipeInfo);
		PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime + 3.0*(ZhuaLiaoMoinfo.fEndTime-ZhuaLiaoMoinfo.fStartTime)/4.0;
		PipeInfo.fEndTime = ZhuaLiaoMoinfo.fEndTime;	
		PipeInfo.rotateZ = frotateX/4.0;
		PipeInfo.center3d =  m_vPipeCenters[0];
		m_vPipeAnimation.push_back(PipeInfo);
		if(m_vPipes.size() >= 1)
		{
			m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
			m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();
			//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
		}

		return;
	}

	//m_vPipeAnimation.clear();
	//AnimationInfo colorInfo = PipeInfo;
	InfoClear(PipeInfo);
	PipeInfo.fStartTime = ZhuaLiaoMoinfo.fStartTime;
	PipeInfo.fEndTime = ZhuaLiaoMoinfo.fEndTime;	
	PipeInfo.rotateZ = frotateX;
	PipeInfo.center3d =  m_vPipeCenters[0];
	m_vPipeAnimation.push_back(PipeInfo);
	if(m_vPipes.size() >= 1)
	{
		m_vMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1]->GetRotateMatrix(),PipeInfo));
		m_mapMatrixHeader[m_vPipes[m_vPipes.size()-1]->GetRotateMatrix()] = m_vPipes[m_vPipes.size()-1]->GetHeader();

		//m_vPipeBackMatrixAnimation.push_back(make_pair(m_vPipes[m_vPipes.size()-1].GetRotateMatrix(),PipeInfo));
	}
}

const char* MainWindow::QStringToCharptr(QString filename)
{
	std::string str = filename.toStdString();
	const char* ch = str.c_str();
	return ch;
}

char* MainWindow::QStringToChar(QString str)
{
	char* ch;
	QByteArray ba = str.toLocal8Bit();
	ch=ba.data();
	return ch;
}

void MainWindow::QStringToChar(QString str,char *ch)
{
	QByteArray ba = str.toLocal8Bit();
	ch=ba.data();
}

STL * MainWindow::GetCurSTLByName(QString strName)
{
	for (size_t i=0;i<m_vSTLs.size();i++)
	{
		if(m_vSTLs[i]->GetSTLName() == strName)
		{
			return m_vSTLs[i];
		}
	}

	return NULL;
}

void MainWindow::MeshInitialPosition()
{
	/*QString strName;
	strName = strName.fromLocal8Bit("E:/Osg/osg3DViewer-master/Model_2021/加工值.csv");*/

	//获取中心模中心坐标
	osg::Node* zxmNode = m_sceneModel->getNode("zhongxinmo.stl");
	m_vec3ZxmCenter = zxmNode->getBound().center()*osg::computeLocalToWorld(zxmNode->getParentalNodePaths()[0]);

	char strzhongxinmoHeight[256] = "";
	float fzhongxinmoHeight;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_ZHONGXINMOHEIGHT, 3, strzhongxinmoHeight);
	fzhongxinmoHeight = atof(strzhongxinmoHeight);
	m_ZhongXinMoHeight = fzhongxinmoHeight;


	char zhujia2BasicLength[256] = "", daojia2BasicLength[256] = "",lunmoRadius[256] = "";
	float fzhujia2BasicLen = 0.0, fdaojia2BasicLen = 0.0, flunmoRadius = 0.0;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_ZHUJIA2, 3, zhujia2BasicLength);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_DAOJIA2, 3, daojia2BasicLength);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_LunMO_R, 3, lunmoRadius);

	fzhujia2BasicLen = atof(zhujia2BasicLength);
	fdaojia2BasicLen = atof(daojia2BasicLength);
	flunmoRadius = atof(lunmoRadius);
	m_foriginalR = flunmoRadius;
	if(sma(fzhujia2BasicLen, 1.0, 1.0) || sma(fdaojia2BasicLen, 1.0, 1.0),sma(flunmoRadius, 1.0, 1.0))
	{
		//AfxMessageBox(L"主夹2或者导夹2长度定义出错", MB_OK);
		return;
	}

	char daojiaheight[256] = "";
	float fdaojiaheight;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_DAOJIAHEIGHT, 3, daojiaheight);
	fdaojiaheight = atof(daojiaheight);


	char zlmOff[32] = "";
	char daojiaYoff[32] = "";
	char lunmoHeight[32] = "";
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_ZHUALIAOMO, 3, zlmOff);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_DAOJIA2_YOFF, 3, daojiaYoff);
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_JITOUWANGUAN2, 3, lunmoHeight);

	float flunmoHeight;
	flunmoHeight = atof(lunmoHeight);
	m_LmHeight = flunmoHeight;



	//导模累加
	if(m_param.LDmValueNum>1)
	{		 
		for(int i=0;i<m_param.LDmValueNum;i++)
		{
			if(i == 0)
			{
				//原始导夹变换
				osg::ref_ptr<osg::MatrixTransform> dj2mtrix = m_sceneModel->getDaoJia2ScaleMtrix();
				dj2mtrix->setMatrix(osg::Matrix::translate(0, 0, -(m_param.LmRadius[i]-flunmoRadius)) * osg::Matrixd::scale(m_param.RDmlength[i]/fdaojia2BasicLen, 1.0, 1.0));

				osg::ref_ptr<osg::MatrixTransform> dj2_1mtrix = m_sceneModel->getDaoJia2_1ScaleMtrix();
				dj2_1mtrix->setMatrix(osg::Matrix::translate(0, 0, -(m_param.LmRadius[i]-flunmoRadius)) *osg::Matrixd::scale(m_param.RDmlength[i]/fdaojia2BasicLen, 1.0, 1.0));

			}
			else
			{
				//累加导夹(osg::Node *)(node->clone(osg::CopyOp::DEEP_COPY_ALL)); 
				osg::ref_ptr<osg::Node> tmpydj2 = (osg::Node *)(m_sceneModel->getNode("dj2.stl"))->clone(osg::CopyOp::DEEP_COPY_ALL);
				osg::ref_ptr<osg::Node> tmpzdj2 = (osg::Node *)(m_sceneModel->getNode("dj2.1.stl"))->clone(osg::CopyOp::DEEP_COPY_ALL);

				//动作矩阵-大小矩阵-节点
				osg::ref_ptr<osg::MatrixTransform> ydj1mtrix = m_sceneModel->getDaoJia1FatherMatrix();
				osg::ref_ptr<osg::MatrixTransform> ydjmtrix = new osg::MatrixTransform;
				osg::ref_ptr<osg::MatrixTransform> ydjmtrixScale = new osg::MatrixTransform;
				ydj1mtrix->addChild(ydjmtrix);
				ydjmtrix->addChild(ydjmtrixScale);
				ydjmtrixScale->addChild(tmpydj2);
				ydjmtrixScale->setMatrix(osg::Matrix::translate(0, flunmoHeight*i,-(m_param.LmRadius[i]-flunmoRadius))*osg::Matrix::scale(m_param.RDmlength[i]/fdaojia2BasicLen, 1.0, 1.0));
				m_vRAddDaoJiaMatrix.push_back(ydjmtrix);

				osg::ref_ptr<osg::MatrixTransform> zdjmtrix = new osg::MatrixTransform;
				osg::ref_ptr<osg::MatrixTransform> zdjmtrixScale = new osg::MatrixTransform;
				osg::ref_ptr<osg::MatrixTransform> zdj1_1mtrix = m_sceneModel->getDaoJia1_1FatherMatrix();
				zdj1_1mtrix->addChild(zdjmtrix);
				zdjmtrix->addChild(zdjmtrixScale);
				zdjmtrixScale->addChild(tmpzdj2);
				zdjmtrixScale->setMatrix(osg::Matrix::translate(0, flunmoHeight*i,-(m_param.LmRadius[i]-flunmoRadius))*osg::Matrix::scale(m_param.RDmlength[i]/fdaojia2BasicLen, 1.0, 1.0));
				m_vLAddDaoJiaMatrix.push_back(zdjmtrix);
			}

		}
	}
	//主夹累加
	if(m_param.LDmValueNum>1)
	{		 
		for(int i=0;i<m_param.LDmValueNum;i++)
		{
			if(i == 0)
			{
				//原始主夹变换
				osg::ref_ptr<osg::MatrixTransform> zj2mtrixscale = m_sceneModel->getZhuJia2ScaleMatrix();
				zj2mtrixscale->setMatrix(osg::Matrix::translate(0, 0, -(m_param.LmRadius[i]-flunmoRadius)) * osg::Matrixd::scale(m_param.Zmlength[i]/fzhujia2BasicLen, 1.0, 1.0));

			}
			else
			{
				//累加主夹(osg::Node *)(node->clone(osg::CopyOp::DEEP_COPY_ALL)); 
				osg::ref_ptr<osg::Node> tmpzj2 = (osg::Node *)(m_sceneModel->getNode("zj2.stl"))->clone(osg::CopyOp::DEEP_COPY_ALL);
				osg::ref_ptr<osg::MatrixTransform> allzj2mtrix = m_sceneModel->getAllZhuJia2Matrix();

				osg::ref_ptr<osg::MatrixTransform> zj2Addmtrix = new osg::MatrixTransform;
				osg::ref_ptr<osg::MatrixTransform> zj2AddScalemtrix = new osg::MatrixTransform;
				allzj2mtrix->addChild(zj2Addmtrix);
				zj2Addmtrix->addChild(zj2AddScalemtrix);
				zj2AddScalemtrix->addChild(tmpzj2);
				zj2AddScalemtrix->setMatrix(osg::Matrix::translate(0, flunmoHeight*i,-(m_param.LmRadius[i]-flunmoRadius))*osg::Matrix::scale(m_param.Zmlength[i]/fzhujia2BasicLen, 1.0, 1.0));
				m_vAddZhuJiaMatrix.push_back(zj2Addmtrix);
			}

		}
	}
	//获取凹槽初始值
	char LmAc[256] = "";
	float fLmAc;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MODEL_LMAOCAO_R, 3, LmAc);
	fLmAc = atof(LmAc);
	m_fOriginalLmAc = fLmAc;
	//默认的加紧距离 
	char zoremove[32] = "";
	float fzoremove;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, ZORE_MOVE, 3, zoremove);
	fzoremove = atof(zoremove);


	//轮模累加
	if(m_param.LmValueNum>1)
	{		 
		float rate = 1.0;
		for(int i=0;i<m_param.LmValueNum;i++)
		{
			if(i == 0)
			{
				//原始轮模变换
				osg::ref_ptr<osg::MatrixTransform> jitou2mtrix= m_sceneModel->getJiTouWanGuan2FatherMatrix();
				jitou2mtrix->setMatrix(osg::Matrix::scale(m_param.LmRadius[i]/flunmoRadius,1.0f,m_param.LmRadius[i]/flunmoRadius));
				m_param.LmAcRadius[i] = fLmAc*(m_param.LmRadius[i]/flunmoRadius);
				rate = m_param.LmRadius[i]/flunmoRadius;
				//	m_vfzoremove[i] = 30 + (fLmAc - m_param.LmAcRadius[i]);
			}
			else
			{
				//累加轮模(osg::Node *)(node->clone(osg::CopyOp::DEEP_COPY_ALL)); 
				osg::ref_ptr<osg::Node> tmpLm = (osg::Node *)(m_sceneModel->getNode("jitouwanguan2.stl"))->clone(osg::CopyOp::DEEP_COPY_ALL);
				osg::ref_ptr<osg::Node> tmpLm2 = (osg::Node *)(m_sceneModel->getNode("jitouwanguan3.stl"))->clone(osg::CopyOp::DEEP_COPY_ALL);


				osg::ref_ptr<osg::MatrixTransform> lmmtrix = new osg::MatrixTransform;
				osg::ref_ptr<osg::MatrixTransform> lmmtrixPos = new osg::MatrixTransform;
				osg::ref_ptr<osg::MatrixTransform> lmmtrix2 = new osg::MatrixTransform;

				lmmtrix->addChild(tmpLm);
				lmmtrix2->addChild(tmpLm2);
				lmmtrix->addChild(lmmtrix2);
				osg::ref_ptr<osg::MatrixTransform> jitouwangaun2mtrix = m_sceneModel->getJiTouWanGuan2FatherMatrix();
				jitouwangaun2mtrix->addChild(lmmtrix);
				/*	if(i==1)
				{
				lmmtrix->setMatrix(osg::Matrix::scale(0.5,1.0f,0.5));
				lmmtrix->setMatrix(osg::Matrix::translate(0, flunmoHeight*i, 0));

				}
				else
				{*/
				lmmtrix->setMatrix(osg::Matrix::scale(m_param.LmRadius[i]/flunmoRadius/rate,1.0f,m_param.LmRadius[i]/flunmoRadius/rate)*osg::Matrix::translate(0, flunmoHeight*i, 0));

				//	}

				m_param.LmAcRadius[i] = fLmAc*(m_param.LmRadius[i]/flunmoRadius/rate);
				//m_vfzoremove[i] = 30 + (fLmAc - m_param.LmAcRadius[i]);
				m_vAddJiTouWanGuan2Matrix.push_back(lmmtrix);
				m_vAddJiTouWanGuan3Matrix.push_back(lmmtrix2);

			}

		}
	}


	//小车移动
	{
		osg::ref_ptr<osg::MatrixTransform> straightpipeclipmtrix = m_sceneModel->getStraightPipeClipMatrix();
		osg::ref_ptr<osg::MatrixTransform> xiaochescalemtrix = m_sceneModel->getXiaoCheScaleMatrix();
		xiaochescalemtrix->setMatrix(osg::Matrix::translate(osg::Vec3f(-(-m_param.fZgPos - atof(zlmOff)), 0, 0)));
		straightpipeclipmtrix->setMatrix(osg::Matrix::translate(osg::Vec3f(-(-m_param.fZgPos - atof(zlmOff)), 0, 0)));

		osg::ref_ptr<osg::MatrixTransform> zhualiaomomtrix = m_sceneModel->getZhuaLiaoMoScaleMtrix();
		Point3D zlmmove(35,-4,-62.42431);
		zhualiaomomtrix->setMatrix(osg::Matrix::translate(zlmmove.x(),zlmmove.y(),zlmmove.z()));
		osg::ref_ptr<osg::Node> zlmNode = m_sceneModel->getNode("zlm.stl");
		osg::Vec3 point = zlmNode->getBound().center()*osg::computeLocalToWorld(zlmNode->getParentalNodePaths()[0]);
		m_3dZhuaLiaoMoCenter.x() = point.x()+zlmmove.x();
		m_3dZhuaLiaoMoCenter.y()= point.y()+zlmmove.y();
		m_3dZhuaLiaoMoCenter.z() = point.z()+zlmmove.z();


		if(g_modelType == 1)
		{
			osg::ref_ptr<osg::MatrixTransform> zhongxinmomtrix = m_sceneModel->getZhongXinMoFatherMatrix();
			osg::ref_ptr<osg::MatrixTransform> wanqumo1mtrix = m_sceneModel->getWanQuMo1FatherMatrix();
			osg::ref_ptr<osg::MatrixTransform> wanqumo2mtrix = m_sceneModel->getWanQuMo2FatherMatrix();
			osg::ref_ptr<osg::MatrixTransform> wanqumo1_1mtrix = m_sceneModel->getWanQuMo1_1FatherMatrix();

			//osg::ref_ptr<osg::MatrixTransform> jitouwanguan3mtrix = m_sceneModel->getJiTouWanGuan3FatherMatrix();

			/*if(m_param.fZxmRadius = 1)
			{
			zhongxinmomtrix->setMatrix(osg::Matrixd::scale(m_param.fZxmRadius/80.0, 1.0f, m_param.fZxmRadius/80.0));
			}
			else 
			{*/
			if(m_param.fZxmRadius > 0)
			{
				zhongxinmomtrix->setMatrix(osg::Matrix::translate(0, flunmoHeight*(m_param.LmValueNum /*- 1*/) + atof(daojiaYoff), 0)*osg::Matrixd::scale(m_param.fZxmRadius/80.0, 1.0f, m_param.fZxmRadius/80.0));
				m_fcurZxmAc = 55.0*(m_param.fZxmRadius/80.0);

				wanqumo1mtrix->setMatrix(osg::Matrix::translate(0, flunmoHeight*(m_param.LmValueNum /*- 1*/)+ atof(daojiaYoff), /*5+*/flunmoRadius-m_param.fZxmRadius -25));

				wanqumo2mtrix->setMatrix(osg::Matrix::translate(-80, flunmoHeight*(m_param.LmValueNum /*- 1*/)+ atof(daojiaYoff), /*5+*/flunmoRadius-m_param.fZxmRadius -25));

				wanqumo1_1mtrix->setMatrix(osg::Matrix::translate(0, flunmoHeight*(m_param.LmValueNum /*- 1*/)+ atof(daojiaYoff), /*5+*/flunmoRadius-m_param.fZxmRadius));

				//jitouwanguan3mtrix->setMatrix(osg::Matrix::translate(-80, flunmoHeight*(m_param.LmValueNum /*- 1*/)+ atof(daojiaYoff), /*5+*/flunmoRadius-m_param.fZxmRadius -25));
			}
			else if(m_param.fZxmRadius == 0)
			{

				zhongxinmomtrix->removeChild(zxmNode);
				m_fcurZxmAc = 0;
				osg::Node* wqm1_1 = m_sceneModel->getNode("wanqumo1.1.stl");
				osg::Node* wqm1 = m_sceneModel->getNode("wanqumo1.stl");
				osg::Node* wqm2 = m_sceneModel->getNode("wanqumo2.stl");
				wanqumo1mtrix->removeChild(wqm1);
				wanqumo2mtrix->removeChild(wqm2);
				wanqumo1_1mtrix->removeChild(wqm1_1);
			}


			//}
		}

	}
}

void MainWindow::ReadYBCExt(std::wstring fileName)
{
	m_YBCExtArray.clear();

	std::wifstream fp(fileName.c_str());
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		memset(buf, 0, maxChar*sizeof(wchar_t));

		// 第一行，获取管子名称
		fp.getline(buf, maxChar);
		std::wstring str(buf);
		str = str.substr(str.find(',')+1, str.size()); 
		str = str.substr(str.find(',')+1, str.size()); 
		QString strtmp = "ybc2";
		wchar_t* t  = (WCHAR*)strtmp.utf16();
		assert(str.compare(std::wstring(t))==0);
		memset(buf, 0, maxChar*sizeof(wchar_t));

		// 跳过第二行
		fp.getline(buf, maxChar);
		memset(buf, 0, maxChar*sizeof(wchar_t));

		YBCExt extData;
		// 以下遍历每一个弯的加工数据
		for (size_t i=0; i<m_radiusIndexArray.size(); ++i)
		{
			fp.getline(buf, maxChar);
			std::wstring str = std::wstring(buf);

			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.myb = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.myb);
			extData.myb = stdWToQString(str).toFloat();

			// mys
			//fp.getline(buf, maxChar);

			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.mbb = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.mbb);
			extData.mbb = stdWToQString(str).toFloat();

			// mbs
			//fp.getline(buf, maxChar);

			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.mcb = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.mcb);
			extData.mcb = stdWToQString(str).toFloat();

			// mcs
			//fp.getline(buf, maxChar);

			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.myt = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.myt);
			extData.myt = stdWToQString(str).toFloat();


			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.mz = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.mz);
			extData.mz = stdWToQString(str).toFloat();


			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.myth = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.myth);
			extData.myth = stdWToQString(str).toFloat();



			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.lc = 0;
			//swscanf(str.c_str(), _T("%d"), &extData.lc);
			extData.lc = stdWToQString(str).toInt();



			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.mmd = 0;
			//swscanf(str.c_str(), _T("%d"), &extData.mmd);
			extData.mmd = stdWToQString(str).toInt();



			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.mty = 0;
			//swscanf(str.c_str(), _T("%d"), &extData.mty);
			extData.mty = stdWToQString(str).toInt();



			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.mtb = 0;
			//swscanf(str.c_str(), _T("%d"), &extData.mtb);
			extData.mtb = stdWToQString(str).toInt();



			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.jy = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.jy);
			extData.jy = stdWToQString(str).toFloat();

			//twy
			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.twy = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.twy);
			extData.twy = stdWToQString(str).toFloat();

			//twbj
			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.twbj = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.twbj);
			extData.twbj = stdWToQString(str).toFloat();

			//twc
			memset(buf, 0, maxChar*sizeof(wchar_t));
			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			extData.twc = 0;
			//swscanf(str.c_str(), _T("%f"), &extData.twc);
			extData.twc = stdWToQString(str).toFloat();



			m_YBCExtArray.push_back(extData);
		}
		fp.close();
	}
	else
	{
		//MessageBox(_T("缺少这个文件，无法读取1！"), _T("错误"), MB_OK);
	}
}


void MainWindow::ReadYBCExt2(std::wstring fileName)
{
	m_YBCExt2.yd = 0.0;
	m_YBCExt2.zm.clear();
	m_YBCExt2.dm.clear();
	m_YBCExt2.ptvalue.clear();
	g_finalCylinderLen = 2000.0;

	std::wifstream fp(fileName.c_str());
	fp.imbue(std::locale(std::locale(),"",LC_CTYPE));
	if (fp)
	{
		const int maxChar = 512;
		wchar_t buf[maxChar];
		memset(buf, 0, maxChar*sizeof(wchar_t));

		// 第一行，获取pt标志
		fp.getline(buf, maxChar);
		std::wstring str(buf);
		str = str.substr(str.find(',')+1, str.size()); 
		str = str.substr(str.find(',')+1, str.size()); 
		QString strtmp = "pt";
		wchar_t* t  = (WCHAR*)strtmp.utf16();
		assert(str.compare(std::wstring(t))==0);
		memset(buf, 0, maxChar*sizeof(wchar_t));

		// 跳过第二行
		fp.getline(buf, maxChar);
		memset(buf, 0, maxChar*sizeof(wchar_t));

		//第三行
		fp.getline(buf, maxChar);
		str = std::wstring (buf);
		str = str.substr(str.find(',')+1, str.size()); 
		str = str.substr(str.find(',')+1, str.size()); 
		m_YBCExt2.yd = stdWToQString(str).toFloat();
		//swscanf(str.c_str(), _T("%f"), &m_YBCExt2.yd);
		memset(buf, 0, maxChar*sizeof(wchar_t));
		g_finalCylinderLen = m_fPipeLength;

		// 以下遍历每一个弯的加工数据
		while(fp.getline(buf, maxChar))
		{
			str = std::wstring(buf);
			QString strtmp = "\\zm";
			wchar_t* t  = (WCHAR*)strtmp.utf16();
			QString strtmp2 = "\\dm";
			wchar_t* t2  = (WCHAR*)strtmp2.utf16();

			if (str.find(t) != std::string::npos)
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(str.find(',')+1, str.size()); 
				//str = str.substr(0, str.find(',')); 
				float zm;
				zm = stdWToQString(str).toFloat();
				//swscanf(str.c_str(), _T("%f"), &zm);
				memset(buf, 0, maxChar*sizeof(wchar_t));
				m_YBCExt2.zm.push_back(zm);
			}
			else if (str.find(t2) != std::string::npos)
			{
				str = str.substr(str.find(',')+1, str.size()); 
				str = str.substr(str.find(',')+1, str.size()); 
				//str = str.substr(0, str.find(',')); 
				float dm;
				dm = stdWToQString(str).toFloat();
				//swscanf(str.c_str(), _T("%f"), &dm);
				memset(buf, 0, maxChar*sizeof(wchar_t));
				m_YBCExt2.dm.push_back(dm);
			}
			else
			{
				break;
			}
		}

		for (size_t i=0; i<m_radiusIndexArray.size(); ++i)
		{
			YBCExt2Base base;
			str = std::wstring(buf);
			QString strtmp = ",";
			wchar_t* t  = (WCHAR*)strtmp.utf16();

			if (str.find(t) == std::string::npos) break;
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			//swscanf(str.c_str(), _T("%f"), &base.zgp);
			base.zgp = stdWToQString(str).toFloat();
			memset(buf, 0, maxChar*sizeof(wchar_t));

			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			if (str.find(t) == std::string::npos) break;
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			//swscanf(str.c_str(), _T("%f"), &base.dywp);
			base.dywp = stdWToQString(str).toFloat();
			memset(buf, 0, maxChar*sizeof(wchar_t));

			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			if (str.find(t) == std::string::npos) break;
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			//swscanf(str.c_str(), _T("%f"), &base.wqmr);
			base.wqmr = stdWToQString(str).toFloat();
			memset(buf, 0, maxChar*sizeof(wchar_t));

			fp.getline(buf, maxChar);

			str = std::wstring(buf);
			if (str.find(t) == std::string::npos) break;
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			//swscanf(str.c_str(), _T("%f"), &base.mjl);
			base.mjl = stdWToQString(str).toFloat();
			memset(buf, 0, maxChar*sizeof(wchar_t));

			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			if (str.find(t) == std::string::npos) break;
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			//swscanf(str.c_str(), _T("%f"), &base.zxmr);
			base.zxmr = stdWToQString(str).toFloat();
			memset(buf, 0, maxChar*sizeof(wchar_t));

			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			if (str.find(t) == std::string::npos) break;
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			//swscanf(str.c_str(), _T("%f"), &base.dmal);
			base.dmal = stdWToQString(str).toFloat();
			memset(buf, 0, maxChar*sizeof(wchar_t));

			fp.getline(buf, maxChar);
			str = std::wstring(buf);
			if (str.find(t) == std::string::npos) break;
			str = str.substr(str.find(',')+1, str.size()); 
			str = str.substr(str.find(',')+1, str.size()); 
			//swscanf(str.c_str(), _T("%f"), &base.wtwp);
			base.wtwp = stdWToQString(str).toFloat();
			memset(buf, 0, maxChar*sizeof(wchar_t));

			m_YBCExt2.ptvalue.push_back(base);

			fp.getline(buf, maxChar);//再给头上读取一行
		}
		fp.close();
	}
	else
	{
		//MessageBox(_T("缺少这个文件，无法读取2！"), _T("错误"), MB_OK);
		QMessageBox::warning(NULL, "warning", "Cannot read 2", QMessageBox::Yes);
		LogInfo("Cannot read 2");
	}

	return;

	//20121029
	if(g_modelType == 1)
	{
		if(m_YBCExt2.ptvalue[0].zxmr > 0)//对于需要载入推弯模具的（是否载入推弯模具根据m_YBCExt2.ptvalue[0].zxmr 是否大于0来判别）
		{
			m_radiusArray.push_back(m_YBCExt2.ptvalue[0].zxmr);//首先添加一个虚拟轮模半径
			for(int i = 0; i < (int)m_InputDataArray.size(); i++)//对于每一弯处理
			{
				if(m_YBCExtArray[i].twc > 0)//如果该弯的twc大于0说明是推弯模具
				{
					m_InputDataArray[i].radius = m_YBCExtArray[i].twbj;//修改本弯的半径值
					//根据弧长和半径计算角度公式  （180*弧长）/(PI * R) 
					m_InputDataArray[i].point.z() = (180*m_YBCExtArray[i].twy)/(3.1415926*m_YBCExtArray[i].twbj);//修改本弯的C值
					m_YBCExtArray[i].mcb = 0.0;//同时由于推弯模具的话mcb值就失效了把它修改为0
					m_radiusIndexArray[i] = m_radiusArray.size();//把这个数组修改为最上层的模具
					m_YBCExtArray[i].mmd = m_radiusArray.size();
				}
			}
		}
	}
	else//20130716
	{
		if(m_YBCExt2.ptvalue[0].zxmr > 0)//对于需要载入推弯模具的（是否载入推弯模具根据m_YBCExt2.ptvalue[0].zxmr 是否大于0来判别）
		{
			m_radiusArray.insert(m_radiusArray.begin()+g_topLunMoNum, m_YBCExt2.ptvalue[0].zxmr);//首先添加一个虚拟轮模半径
			m_radiusArray.push_back(m_YBCExt2.ptvalue[0].zxmr);//首先添加一个虚拟轮模半径

			m_YBCExt2.zm.insert(m_YBCExt2.zm.begin()+g_topLunMoNum, 0);
			m_YBCExt2.zm.push_back(0);
			m_YBCExt2.dm.insert(m_YBCExt2.dm.begin()+g_topLunMoNum, 0);
			m_YBCExt2.dm.push_back(0);

			for(int i = 0; i < (int)m_InputDataArray.size(); i++)//对于每一弯处理
			{
				if(m_YBCExtArray[i].twc > 0)//如果该弯的twc大于0说明是推弯模具
				{
					m_InputDataArray[i].radius = m_YBCExtArray[i].twbj;//修改本弯的半径值
					//根据弧长和半径计算角度公式  （180*弧长）/(PI * R) 
					m_InputDataArray[i].point.z() = (180*m_YBCExtArray[i].twy)/(3.1415926*m_YBCExtArray[i].twbj);//修改本弯的C值
					m_YBCExtArray[i].mcb = 0.0;//同时由于推弯模具的话mcb值就失效了把它修改为0
					if(m_radiusIndexArray[i] > 0) 
					{
						m_radiusIndexArray[i] = g_topLunMoNum+1;//把这个数组修改为最上层的模具
						m_YBCExtArray[i].mmd = g_topLunMoNum+1;
					}
					if(m_radiusIndexArray[i] < 0)
					{
						m_radiusIndexArray[i] = -(m_radiusArray.size()-2-g_topLunMoNum+1);//把这个数组修改为最下层的模具
						m_YBCExtArray[i].mmd = m_radiusArray.size();
					}
				}
				else
				{
					if(m_YBCExtArray[i].mmd > g_topLunMoNum)
						m_YBCExtArray[i].mmd +=1;
				}
			}
		}
	}
}

void MainWindow::PostReadYBCExt2()
{
	//20121029
	if(g_modelType == 1)
	{
		if(m_YBCExt2.ptvalue[0].zxmr > 0)//对于需要载入推弯模具的（是否载入推弯模具根据m_YBCExt2.ptvalue[0].zxmr 是否大于0来判别）
		{
			m_radiusArray.push_back(m_YBCExt2.ptvalue[0].zxmr);//首先添加一个虚拟轮模半径
			for(int i = 0; i < (int)m_InputDataArray.size(); i++)//对于每一弯处理
			{
				if(m_YBCExtArray[i].twc > 0)//如果该弯的twc大于0说明是推弯模具
				{
					m_InputDataArray[i].radius = m_YBCExtArray[i].twbj;//修改本弯的半径值
					//根据弧长和半径计算角度公式  （180*弧长）/(PI * R) 
					m_InputDataArray[i].point.z() = (180*m_YBCExtArray[i].twy)/(3.1415926*m_YBCExtArray[i].twbj);//修改本弯的C值
					m_YBCExtArray[i].mcb = 0.0;//同时由于推弯模具的话mcb值就失效了把它修改为0
					m_radiusIndexArray[i] = m_radiusArray.size();//把这个数组修改为最上层的模具
					m_YBCExtArray[i].mmd = m_radiusArray.size();
				}
			}
		}
	}
	else//20130716
	{
		if(m_YBCExt2.ptvalue[0].zxmr > 0)//对于需要载入推弯模具的（是否载入推弯模具根据m_YBCExt2.ptvalue[0].zxmr 是否大于0来判别）
		{
			m_radiusArray.insert(m_radiusArray.begin()+g_topLunMoNum, m_YBCExt2.ptvalue[0].zxmr);//首先添加一个虚拟轮模半径
			m_radiusArray.push_back(m_YBCExt2.ptvalue[0].zxmr);//首先添加一个虚拟轮模半径

			m_YBCExt2.zm.insert(m_YBCExt2.zm.begin()+g_topLunMoNum, 0);
			m_YBCExt2.zm.push_back(0);
			m_YBCExt2.dm.insert(m_YBCExt2.dm.begin()+g_topLunMoNum, 0);
			m_YBCExt2.dm.push_back(0);


			for(int i = 0; i < (int)m_InputDataArray.size(); i++)//对于每一弯处理

			{
				if(m_YBCExtArray[i].twc > 0)//如果该弯的twc大于0说明是推弯模具
				{
					m_InputDataArray[i].radius = m_YBCExtArray[i].twbj;//修改本弯的半径值
					//根据弧长和半径计算角度公式  （180*弧长）/(PI * R) 
					m_InputDataArray[i].point.z() = (180*m_YBCExtArray[i].twy)/(3.1415926*m_YBCExtArray[i].twbj);//修改本弯的C值
					m_YBCExtArray[i].mcb = 0.0;//同时由于推弯模具的话mcb值就失效了把它修改为0
					if(m_radiusIndexArray[i] > 0) 
					{
						m_radiusIndexArray[i] = g_topLunMoNum+1;//把这个数组修改为最上层的模具
						m_YBCExtArray[i].mmd = g_topLunMoNum+1;
					}
					if(m_radiusIndexArray[i] < 0)
					{
						m_radiusIndexArray[i] = -(m_radiusArray.size()-2-g_topLunMoNum+1);//把这个数组修改为最下层的模具
						m_YBCExtArray[i].mmd = m_radiusArray.size();
					}
				}
				else
				{
					if(m_YBCExtArray[i].mmd > g_topLunMoNum)
						m_YBCExtArray[i].mmd +=1;
				}
			}
		}
	}

	// 根据加工扩展值对加工值进行修改!
	assert(m_InputDataArray.size() <= m_YBCExtArray.size());
	//assert(m_InputDataArray.size() <= m_YBCExt2.ptvalue.size());
	for (size_t i=0; i<m_InputDataArray.size(); ++i)
	{  
		if (Mathf::FAbs(m_YBCExtArray[i].myb)>10*MYEPSILON)
		{
			m_InputDataArray[i].point.x() += m_YBCExtArray[i].myb;
		}
		if (Mathf::FAbs(m_YBCExtArray[i].mbb)>10*MYEPSILON)
		{
			m_InputDataArray[i].point.y() += m_YBCExtArray[i].mbb;
		}
		if (Mathf::FAbs(m_YBCExtArray[i].mcb)>10*MYEPSILON)
		{
			m_InputDataArray[i].point.z() += m_YBCExtArray[i].mcb;
		}
	}
}

int MainWindow::GetNthValueFromTxt(char *filename, char *key1, char *key2, int n, char *rvalue)
{
	// LogInfo(filename);
	// LogInfo("Enter Get NthValue FromTxt values !");

	QString path = QCoreApplication::applicationDirPath();
	path.append(filename);

	std::string newpath = path.toStdString();
	const char* wholefilename = newpath.c_str();
	if(key1 == NULL||key2 == NULL) return 1;
	if (wholefilename == NULL) return 1;
	if (n < 1) return 1;
	if (rvalue == NULL) return 1;

	FILE *m_file = NULL;

	m_file = fopen(wholefilename,"r");
	if (m_file == NULL)
	{  
		LogInfo(wholefilename);
		LogInfo("read file name failed !");
		return -1;
	}

	char *strs[MAX_STR_NUM] ={NULL};
	for(int i = 0; i < MAX_STR_NUM; i++)
		strs[i] = new char[MAX_STR_LENGTH];

	int numStr = 0;
	char data[1024] = "";
	while (!feof(m_file))
	{
		fgets(data, 1024, m_file);
		CF_TXT_SplitToStrs(data, ",", strs, numStr);
		if (numStr < 2 || numStr < n) continue;
		removeStrSpace(strs[0]);
		removeStrSpace(strs[1]);
		if(n <=  numStr && strcmp(strs[0], key1) == 0 && strcmp(strs[1], key2) == 0)
		{
			removeStrSpace(strs[n-1]);
			strcpy(rvalue, strs[n-1]);
			break;
		}
	}

	fclose(m_file);
	for(int i = 0; i < MAX_STR_NUM; i++)
		delete []strs[i];

	// LogInfo("Exit Get NthValue FromTxt values !");

	return 0;
}

int MainWindow::GetMachineValueFromTxt(char *filename, char *key, char *rvalue)
{
	FILE *m_file = NULL;

	m_file = fopen(filename,"r");
	if (m_file == NULL)
	{   
		return -1;
	}

	char data[256] = "";
	char value[256] = "";
	while (!feof(m_file))
	{
		fgets(data, 256, m_file);
		if (strstr(data,key) != 0)
		{
			if(GetThirdStr(data,",", value) == 0)
				break;
		}
	}
	fclose(m_file);

	if (strlen(value) == 0) 
	{
		return -1;
	}
	CF_TXT_RemoveSpaceHead(value);
	CF_TXT_RemoveSpaceTail(value);
	CF_TXT_RemoveTailFileEndSign(value);
	strcpy(rvalue, value);
	return 0;
}

int MainWindow::CF_TXT_SplitToStrs(char *instr, char* key, char **strs, int &numStr)
{
	if (strlen(instr) == 0) return 0;
	numStr = 0;
	char *copyInStr = (char *)malloc(strlen(instr) + 1);
	strcpy(copyInStr,instr);
	char *p = strtok(copyInStr,key);//第一次调用,传要处理的串参数地址s 
	while(p != NULL)
	{
		strcpy(strs[numStr++],p);
		p = strtok(NULL,key);
	}
	free(copyInStr);
	return 0;
}

int MainWindow::removeStrSpace(char * str)
{
	CF_TXT_RemoveSpaceHead(str);
	CF_TXT_RemoveSpaceTail(str);
	CF_TXT_RemoveTailFileEndSign(str);
	return 0;
}

int MainWindow::CF_TXT_RemoveSpaceHead(char * instr)
{
	if (NULL == instr)
		return 1;

	char *copyInStr = (char *)malloc(strlen(instr) + 1);
	strcpy(copyInStr,instr);

	int i = 0;
	while(copyInStr[i] == ' ' || copyInStr[i] == ' ' || copyInStr[i] == 10)
		++i;
	strcpy(instr, &(copyInStr[i]));
	free(copyInStr);
	return 0;
}

int MainWindow::CF_TXT_RemoveSpaceTail(char * str)
{
	if (NULL == str)
		return 1;

	int i = strlen(str);
	i--;
	while(i >= 0 && (str[i] == ' ' || str[i] == ' ' || str[i] == 10 ))
	{
		str[i] = '\0';
		i--;
	}

	return 0;
}

int MainWindow::CF_TXT_RemoveTailFileEndSign(char * str)
{
	if (NULL == str)
		return 1;
	int len = (int)strlen(str);
	if (len == 0) return 0;
	if (str[len -1] == 10)
	{
		str[len - 1] = '\0';
	}
	return 0;
}

int MainWindow::GetThirdStr(char *data, char* key, char *third)
{
	if(data == NULL) return 0;
	char *p = strtok(data,key);//第一次调用,传要处理的串参数地址s 
	if(p == NULL) return 1;
	p = strtok(NULL,key);//第二次调用
	if(p == NULL) return 1;
	p = strtok(NULL,key);//第三次调用
	if(p == NULL) return 1;
	strcpy(third,p);
	/*while(p)
	{
	p = strtok(NULL,key);
	}*/
	return 0;
}

std::wstring MainWindow::qToStdWString(const QString &str)
{
#ifdef _MSC_VER
	return std::wstring((const wchar_t *)str.utf16());
#else
	return str.toStdWString();
#endif
}


QString MainWindow::stdWToQString(const std::wstring &str)
{
#ifdef _MSC_VER
	return QString::fromUtf16((const ushort *)str.c_str());
#else
	return QString::fromStdWString(str);
#endif

}

osg::ref_ptr<osg::Node> MainWindow::createQuade(std::string strFileName)
{
	osg::ref_ptr<osg::Geometry> geom=new osg::Geometry();//定义一个几何体
	osg::ref_ptr<osg::LineWidth> lineSize=new osg::LineWidth();
	lineSize->setWidth(10.0);//设置线的宽度
	geom->getOrCreateStateSet()->setAttributeAndModes(lineSize.get(),osg::StateAttribute::ON);

	std::string filename = strFileName;
	std::fstream _in(filename.c_str(), std::ios_base::in);
	if (!_in)
		return false;

	Vertex  v, n;
	std::string line;
	std::string garbage;
	std::stringstream strstream;
	bool facet_normal(false);

	std::vector<Vertex> vers;
	std::vector<Vertex> nors;

	while(_in && !_in.eof()) 
	{
		// Get one line
		std::getline(_in, line);
		if (_in.bad())
			return false;

		// Normal found?
		if (line.find("normal") != std::string::npos)
		{
			strstream.str(line);
			strstream.clear();
			// facet
			strstream >> garbage >> garbage >> n._x >> n._y >> n._z;
			nors.push_back(n);
		}
		// Detected a triangle
		if ((line.find("outer") != std::string::npos) || (line.find("OUTER") != std::string::npos)) 
		{
			for (int i=0; i<3; ++i) 
			{
				// Get one vertex
				std::getline(_in, line);
				strstream.str(line);
				strstream.clear();
				strstream >> garbage >>v._x >> v._y >> v._z;
				vers.push_back(v);
			}

		}
	}

	//首先定义四个点
	osg::ref_ptr<osg::Vec3Array> vs=new osg::Vec3Array(nors.size()*3);//定义一个几何体坐标集合
	osg::ref_ptr<osg::Vec3Array> ns=new osg::Vec3Array(nors.size()*3);//定义了一个法线绑定到该四方体中
	for (size_t i = 0; i < nors.size();++i)
	{
		Vertex& v0 = vers[i*3];
		Vertex& v1 = vers[i*3+1];
		Vertex& v2 = vers[i*3+2];
		Vertex& nor = nors[i];
		//定义法线
		(*vs)[3*i].set(v0._x, v0._y, v0._z);
		(*vs)[3*i+1].set(v1._x, v1._y, v1._z);
		(*vs)[3*i+2].set(v2._x, v2._y, v2._z);

		(*ns)[3*i].set(nor._x, nor._y, nor._z);
		(*ns)[3*i+1].set(nor._x, nor._y, nor._z);
		(*ns)[3*i+2].set(nor._x, nor._y, nor._z);
	}

	geom->setVertexArray(vs);//将坐标设置到几何体节点中
	geom->setNormalArray(ns, osg::Array::BIND_PER_VERTEX);//添加法线到几何体中

	osg::ref_ptr<osg::DrawArrays> Primitives1 = new osg::DrawArrays(GL_TRIANGLES, 0, 3*nors.size());
	geom->addPrimitiveSet(Primitives1.get());

	//关闭显示列表，开启顶点缓存对象
	geom->setUseVertexBufferObjects(true);

	//定义一个几何组结点，并把几何体结点加入到几何组结点当中
	osg::ref_ptr<osg::Geode> geode=new osg::Geode();
	//geode->addDrawable(geom.get());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHT1, osg::StateAttribute::ON);

	osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel; //用于选择光照模型（如全局光和双面光照）
	lightModel->setTwoSided(true);
	stateset->setAttributeAndModes(lightModel.get() );

	osg::ref_ptr<osg::Material> mat = new osg::Material;
	float dif = Mathf::IntervalRandom(0.1f, 1.0f, 0.1);
	float amb = Mathf::IntervalRandom(0.1f, 1.0f, 0.3);
	float shin = Mathf::IntervalRandom(0.1f, 1.0f, 0.2);

	//2016.06.01. 通常，对环境光与漫反射光的反射程度决定了材料的颜色，并且它们应该值相同。
	mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(dif,dif,0.0,1.0f));
	mat->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(amb,amb,0.0,1.0f));
	mat->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(shin,shin,shin,1.0f));
	mat->setShininess( osg::Material::FRONT_AND_BACK, 100.0f );
	stateset->setAttributeAndModes(mat.get(),osg::StateAttribute::ON);

	geom->setStateSet(stateset);
	geode->addDrawable(geom.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(geom);
	optimizer.reset();

	return geode.get();
}


void MainWindow::JitouRotate(double& fTimeStart,double angle)
{
	AnimationInfo info;
	info.fStartTime = fTimeStart;
	info.fEndTime = info.fStartTime + m_fAnimationTime;  // 具体运行多长时间需要计算得出
	info.rotateY = angle * MYDEG_TO_RAD/*Mathf::MYDEG_TO_RAD*/;
	fTimeStart = info.fEndTime+m_fAnimationStopTime; 
	m_vJiTouWanGuan1Animation.push_back(info);
}

void MainWindow::LeftFutuiDown(double& fTimeStart)
{
	// 当前在左侧，先将左侧导夹后撤
	Point3D fTrn(0,0,0);
	fTrn.x() = -150.0;
	fTrn.y() = -150.0;
	fTrn.z() = 0.0;

	AnimationInfo info;
	info.fStartTime = fTimeStart;
	info.fEndTime = info.fStartTime + m_fAnimationTime;  // 具体运行多长时间需要计算得出
	info.translate = fTrn;
	fTimeStart = info.fEndTime+m_fAnimationStopTime; 
	m_vZuoFuTuiAnimation.push_back(info);
}


void MainWindow::RightFutuiDown(double& fTimeStart)
{
	Point3D fTrn(0,0,0);
	fTrn.x() = -150.0;
	fTrn.y() = -150.0;
	fTrn.z() = 0.0;

	AnimationInfo info;
	info.fStartTime = fTimeStart;
	info.fEndTime = info.fStartTime + m_fAnimationTime;  // 具体运行多长时间需要计算得出
	info.translate = fTrn;
	fTimeStart = info.fEndTime+m_fAnimationStopTime; 
	m_vYouFuTuiAnimation.push_back(info);
}

void MainWindow::LeftFutuiUp(double& fTimeStart)
{
	Point3D fTrn(0,0,0);
	fTrn.x() = 150.0;
	fTrn.y() = 150.0;
	fTrn.z() = 0.0;

	AnimationInfo info;
	info.fStartTime = fTimeStart;
	info.fEndTime = info.fStartTime + m_fAnimationTime;  // 具体运行多长时间需要计算得出
	info.translate = fTrn;
	fTimeStart = info.fEndTime+m_fAnimationStopTime; 
	m_vZuoFuTuiAnimation.push_back(info);
}

void MainWindow::RightFutuiUp(double& fTimeStart)
{
	Point3D fTrn(0,0,0);
	fTrn.x() = 150.0;
	fTrn.y() = 150.0;
	fTrn.z() = 0.0;

	AnimationInfo info;
	info.fStartTime = fTimeStart;
	info.fEndTime = info.fStartTime + m_fAnimationTime;  // 具体运行多长时间需要计算得出
	info.translate = fTrn;
	fTimeStart = info.fEndTime+m_fAnimationStopTime; 
	m_vYouFuTuiAnimation.push_back(info);
}


void MainWindow::DaoJiaMoveForward(float fmove,AnimationInfo info,int i)
{
	if(m_bIsLeft == false)
	{
		if((int)(m_vproductionvalues[i].fmm) == 1)//原始右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vDaoJia2Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 2)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vRFuiTuiDaoJia0Animation.push_back(tmp);
			//m_vRAddDaoJiaMatrix[m_vproductionvalues[i].fmm-2]->setUpdateCallback(new DaoJia2Callback(m_vDaoJiaTmpAnimation));
		}
		else if(m_vproductionvalues[i].fmm == 3)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vRFuiTuiDaoJia1Animation.push_back(tmp);
			//m_vRAddDaoJiaMatrix[m_vproductionvalues[i].fmm-2]->setUpdateCallback(new DaoJia2Callback(m_vDaoJiaTmpAnimation));
		}
		else if(m_vproductionvalues[i].fmm == 4)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vRFuiTuiDaoJia2Animation.push_back(tmp);
			//m_vRAddDaoJiaMatrix[m_vproductionvalues[i].fmm-2]->setUpdateCallback(new DaoJia2Callback(m_vDaoJiaTmpAnimation));
		}
		else if(m_vproductionvalues[i].fmm == 5)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vRFuiTuiDaoJia3Animation.push_back(tmp);
			//m_vRAddDaoJiaMatrix[m_vproductionvalues[i].fmm-2]->setUpdateCallback(new DaoJia2Callback(m_vDaoJiaTmpAnimation));
		}
	}
	else if(m_bIsLeft == true)
	{
		if((int)(m_vproductionvalues[i].fmm) == 1)//原始右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vDaoJia2_1Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 2)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vLFuiTuiDaoJia0Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 3)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vLFuiTuiDaoJia1Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 4)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vLFuiTuiDaoJia2Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 5)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_fFuTuiDaoJiaDis = tmp.translate.x();
			m_vLFuiTuiDaoJia3Animation.push_back(tmp);
		}
	}


}

void MainWindow::DaoJiaMoveBack(float fmove,AnimationInfo info,int i)
{
	if(m_bIsLeft == false)
	{
		if((int)(m_vproductionvalues[i].fmm) == 1)//原始右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vDaoJia2Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 2)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vRFuiTuiDaoJia0Animation.push_back(tmp);
			//m_vRAddDaoJiaMatrix[m_vproductionvalues[i].fmm-2]->setUpdateCallback(new DaoJia2Callback(m_vDaoJiaTmpAnimation));
		}
		else if(m_vproductionvalues[i].fmm == 3)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vRFuiTuiDaoJia1Animation.push_back(tmp);
			//m_vRAddDaoJiaMatrix[m_vproductionvalues[i].fmm-2]->setUpdateCallback(new DaoJia2Callback(m_vDaoJiaTmpAnimation));
		}
		else if(m_vproductionvalues[i].fmm == 4)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vRFuiTuiDaoJia2Animation.push_back(tmp);
			//m_vRAddDaoJiaMatrix[m_vproductionvalues[i].fmm-2]->setUpdateCallback(new DaoJia2Callback(m_vDaoJiaTmpAnimation));
		}
		else if(m_vproductionvalues[i].fmm == 5)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vRFuiTuiDaoJia3Animation.push_back(tmp);
			//m_vRAddDaoJiaMatrix[m_vproductionvalues[i].fmm-2]->setUpdateCallback(new DaoJia2Callback(m_vDaoJiaTmpAnimation));
		}
	}
	else if(m_bIsLeft == true)
	{
		if((int)(m_vproductionvalues[i].fmm) == 1)//原始右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vDaoJia2_1Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 2)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vLFuiTuiDaoJia0Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 3)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vLFuiTuiDaoJia1Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 4)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vLFuiTuiDaoJia2Animation.push_back(tmp);
		}
		else if(m_vproductionvalues[i].fmm == 5)//增加出来的右导夹
		{
			AnimationInfo tmp;
			tmp.fStartTime = info.fStartTime;
			tmp.fEndTime = info.fEndTime;
			tmp.translate.x() = fmove;
			m_vLFuiTuiDaoJia3Animation.push_back(tmp);
		}
	}
}

void MainWindow::startTimer(double time)
{
	int multi = 500;
	//m_closeTimer->start(time*multi);	
	//connect(m_closeTimer,SIGNAL(timeout()),this,SLOT(closeTimer()));

	m_bAutoPlaning = m_bIsAutoPlan;  // 记录此次碰撞检测是否是为了加工自动规划
	//m_collisionTimer->start(multi/m_v);
	int interval = m_collisionTimer->interval();
	if (interval==0)
	{
		// m_collisionTimer->start(multi/5.0);
		//m_collisionTimer->start(100);
	    //m_collisionTimer->start(50+10*m_v);
		connect(m_collisionTimer,SIGNAL(timeout()),this,SLOT(collisionDetect()));
		m_collisionTimer->start(multi/m_v);

	}
	else
	{
		m_collisionTimer->start();
	}

	//interval = SetTimer->getinterval();

	//if (0==interval)
	//{
	//	SetTimer->setinterval(multi/m_v);
	//	connect(SetTimer,SIGNAL(timeout()),this,SLOT(handleTimeout()));//定时器时间到，运行超时处理函数handleTimeout()
	//}
	//else

 //   SetTimer->start();//定时器开始工作
}

void MainWindow::CreateSameModel(osg::Node* pNode,osg::ref_ptr<osg::Geode>& aGeode,osg::ref_ptr<osg::Material>& mat)
{
	aGeode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> aGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> aPoints = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> aNormals = new osg::Vec3Array();

	osg::Geode *pGeode = dynamic_cast<osg::Geode*>(pNode);
	osg::Drawable *pDrawable = pGeode->getDrawable(0);
	osg::Geometry *pGeometry = pDrawable->asGeometry();
	osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*>(pGeometry->getVertexArray());
	osg::ref_ptr<osg::Vec3Array> normals = dynamic_cast<osg::Vec3Array*>(pGeometry->getNormalArray());
	for (size_t i = 0;i<vertices->size();++i)
	{
		aPoints->push_back((*vertices)[i]);
	}

	for (size_t i = 0;i<normals->size();++i)
	{
		aNormals->push_back((*normals)[i]);
	}

	aGeometry->setVertexArray(aPoints);//将坐标设置到几何体节点中
	aGeometry->setNormalArray(aNormals, osg::Array::BIND_PER_VERTEX);//添加法线到几何体中

	osg::ref_ptr<osg::DrawArrays> Primitives1 = new osg::DrawArrays(GL_TRIANGLES, 0, aNormals->size());
	aGeometry->addPrimitiveSet(Primitives1.get());

	//关闭显示列表，开启顶点缓存对象
	aGeometry->setUseVertexBufferObjects(true);

	//定义一个几何组结点，并把几何体结点加入到几何组结点当中
	//osg::ref_ptr<osg::Geode> geode=new osg::Geode();
	//geode->addDrawable(geom.get());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHT1, osg::StateAttribute::ON);

	osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel; //用于选择光照模型（如全局光和双面光照）
	lightModel->setTwoSided(true);
	stateset->setAttributeAndModes(lightModel.get() );

	mat = new osg::Material;
	mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mat->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mat->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.3,0.3,0.3,1.0f));
	mat->setShininess( osg::Material::FRONT_AND_BACK, 100.0f );
	//2016.06.01. 通常，对环境光与漫反射光的反射程度决定了材料的颜色，并且它们应该值相同。
	stateset->setAttributeAndModes(mat.get(),osg::StateAttribute::ON);

	aGeometry->setStateSet(stateset);
	aGeode->addDrawable(aGeometry.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(aGeometry);
	optimizer.reset();
}

void MainWindow::SetCollisionDetectParts()
{
	ReadCollisionDectionFile();
	ReadAlterModelName();

	m_vCollisionParts.clear();
	m_vPairCollisionPartsToAlter.clear();
	if (m_IsDetected == false)
	{
		m_vCollisionParts.clear();
		m_vPairCollisionPartsToAlter.clear();
	}
	else if (m_IsDetected == true)
	{
		std::vector<QString> v_DetectItemName;
		std::map<QString,std::vector<std::pair<QString,int>>> tmpMap = m_pCollisionDlg->GetCollisonTableValues();
		std::map<QString,std::vector<std::pair<QString,int>>>::iterator it = tmpMap.begin();
		for(;it!=tmpMap.end();it++)
		{
			std::vector<std::pair<QString,int>> models = it->second;
			for (size_t i=0;i<models.size();i++)
			{
				v_DetectItemName.push_back(models[i].first);
			}
		}

		for(size_t i = 0;i< v_DetectItemName.size();i++)
		{
			for(size_t j=0;j<m_vSTLs.size();j++)
			{
				if(v_DetectItemName[i] == m_vSTLs[j]->GetSTLName())
				{
					if(m_vSTLs[j]->GetIsShow() == 1)
					{
						std::pair<STL*,STL*> _pair;

						STL* tmp1 = m_vSTLs[j];
						_pair.first = tmp1;
						std::map<QString,QString>::iterator itit;
						itit = m_MapModelNameToAlterModelName.find(v_DetectItemName[i]);//遍历alter文件数据，找到替换模型名
						if (itit!=m_MapModelNameToAlterModelName.end())
						{
							for (size_t t=0;t<m_vAlterModelSTL.size();t++)
							{
								if (m_vAlterModelSTL[t]->GetSTLName() == itit->second)
								{
									STL* tmp2 = m_vAlterModelSTL[t];
									_pair = make_pair(tmp1,tmp2);
								}
							}				
						}
						else
						{
							STL* tmp2 = nullptr;
							_pair = make_pair(tmp1,tmp2);
						}

						if (_pair.first != nullptr
							|| _pair.second != nullptr)
						{
							m_vPairCollisionPartsToAlter.push_back(_pair);
							m_vCollisionParts.push_back(tmp1->GetModelMatrix());
						}

						break;
					}

				}
			}
		}
	}



#if 0


	//if(v_DetectItemName[i] == "JiTouWanGuan")
	//{
	//	osg::ref_ptr<osg::Group> tmp1 = m_sceneModel->getJiTouWanGuan1FatherMatrix();

	//	// 添加过父节点就不用单独添加子节点，父节点会遍历子节点
	//	//osg::ref_ptr<osg::Group> tmp2 = m_sceneModel->getJiTouWanGuan2FatherMatrix();
	//	//osg::ref_ptr<osg::Group> tmp3 = m_sceneModel->getJiTouWanGuan3FatherMatrix();

	//	m_vCollisionParts.push_back(tmp1);
	//	//m_vCollisionParts.push_back(tmp2);
	//	//m_vCollisionParts.push_back(tmp3);

	//	// 添加过父节点，不用单独添加子节点 20220226
	//	//for(int i=0;i<m_vAddJiTouWanGuan2Matrix.size();i++)
	//	//{
	//	//	m_vCollisionParts.push_back(m_vAddJiTouWanGuan2Matrix[i]);
	//	//}
	//	//for(int i=0;i<m_vAddJiTouWanGuan3Matrix.size();i++)
	//	//{
	//	//	m_vCollisionParts.push_back(m_vAddJiTouWanGuan3Matrix[i]);
	//	//}
	//}
	//else if(v_DetectItemName[i] == "JiTouDiZuo")
	//{
	//	osg::ref_ptr<osg::Group> tmp = m_sceneModel->getJiTouDiZuoFatherMatrix();
	//	m_vCollisionParts.push_back(tmp);
	//}
	//else if(v_DetectItemName[i] == "JiTouMoZuo")
	//{
	//	osg::ref_ptr<osg::Group> tmp = m_sceneModel->getJiTouMoZuoFatherMatrix();
	//	m_vCollisionParts.push_back(tmp);
	//}
	//else if(v_DetectItemName[i] == "XiaoChe")
	//{
	//	osg::ref_ptr<osg::Group> tmp = m_sceneModel->getXiaoCheFatherMatrix();
	//	m_vCollisionParts.push_back(tmp);
	//}
	//else if(v_DetectItemName[i] == "ZhuJia")
	//{
	//	osg::ref_ptr<osg::Group> tmp = m_sceneModel->getZhuJia1FatherMatrix();

	//	m_vCollisionParts.push_back(tmp);

	//	for(int i =0;i<m_vAddZhuJiaMatrix.size();i++)
	//	{
	//		m_vCollisionParts.push_back(m_vAddZhuJiaMatrix[i]);
	//	}
	//}
	//else if(v_DetectItemName[i] == "DaoJia")
	//{
	//	//右侧导夹
	//	osg::ref_ptr<osg::Group> tmp1 = m_sceneModel->getDaoJia1FatherMatrix();
	//	m_vCollisionParts.push_back(tmp1);
	//	osg::ref_ptr<osg::Group> tmp2 = m_sceneModel->getDaoJia2FatherMatrix();
	//	m_vCollisionParts.push_back(tmp2);
	//	for(int i =0;i<m_vRAddDaoJiaMatrix.size();i++)
	//	{
	//		m_vCollisionParts.push_back(m_vRAddDaoJiaMatrix[i]);
	//	}
	//	//左侧导夹
	//	osg::ref_ptr<osg::Group> tmp1_1 = m_sceneModel->getDaoJia1_1FatherMatrix();
	//	m_vCollisionParts.push_back(tmp1_1);
	//	osg::ref_ptr<osg::Group> tmp2_1 = m_sceneModel->getDaoJia2_1FatherMatrix();
	//	m_vCollisionParts.push_back(tmp2_1);
	//	for(int i =0;i<m_vLAddDaoJiaMatrix.size();i++)
	//	{
	//		m_vCollisionParts.push_back(m_vLAddDaoJiaMatrix[i]);
	//	}
	//}
	//else if(v_DetectItemName[i] == "DiMian")
	//{
	//	osg::ref_ptr<osg::Group> tmp = m_sceneModel->getDiMianFatherMatrix();
	//	m_vCollisionParts.push_back(tmp);
	//}
	//else if(v_DetectItemName[i] == "JiShen")
	//{
	//	osg::ref_ptr<osg::Group> tmp = m_sceneModel->getJiShenFatherMatrix();
	//	m_vCollisionParts.push_back(tmp);
	//}

#endif
}

void MainWindow::CombineSamePeriodAnimation(std::vector<AnimationInfo>& anims)
{
	if (anims.empty())
	{
		return;
	}

	std::vector<AnimationInfo> tempAnims = anims;
	std::vector<std::vector<AnimationInfo>> vAnims;
	anims.clear();
	for (size_t i = 0; i < tempAnims.size();++i)
	{
		std::vector<AnimationInfo> temp;
		AnimationInfo info0 = tempAnims[i];
		temp.push_back(info0);
		if (i==tempAnims.size())
		{
			vAnims.push_back(temp);
			break;
		}

		for (size_t j = i+1;j<tempAnims.size();++j)
		{
			AnimationInfo info1 = tempAnims[j];
			if (info0.fStartTime==info1.fStartTime && info0.fEndTime == info1.fEndTime)
			{
				temp.push_back(info1);
				i = j;
			}
			else
			{
				break;
			}
		}

		vAnims.push_back(temp);
	}

	for (size_t i = 0; i < vAnims.size();++i)
	{
		std::vector<AnimationInfo> infos = vAnims[i];
		if (infos.size()==1)
		{
			anims.push_back(infos[0]);
		}
		else
		{
			AnimationInfo info = infos[0];
			for (size_t j = 1; j < infos.size();++j)
			{
				info.translate += infos[j].translate;
				info.isZhujiaZ = info.isZhujiaZ || infos[j].isZhujiaZ;
				info.strProcess += infos[j].strProcess;
			}

			// 更新绝对位置
			info.absolutePos = infos.back().absolutePos;

			anims.push_back(info);
		}
	}
}

std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> MainWindow::GetAnmiPipeSeg()
{
	std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> matrixs;
	// 从后向前遍历，找到
	for (int i = m_vPipesMatrixAnimation.size()-1; i>=0;--i)
	{
		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vPipesMatrixAnimation[i].first->getUpdateCallback());
		AnimationInfo info = m_vPipesMatrixAnimation[i].second;
		if (callback!=NULL)
		{
			double time = callback->getAnimationTime();
			// 判断动画是否在管子段运行的时间段
			if (info.fStartTime < time /*&& time <= info.fEndTime*/)
			{
				//return m_vMatrixAnimation[i].first;
				matrixs.push_back(m_vPipesMatrixAnimation[i].first);
			}
			//else if (time > info.fEndTime)
			//{
			//	return NULL;
			//}

		}
	}

	return matrixs;
}

bool MainWindow::IsModelCollision(QString name)
{
	for (int i = 0; i < m_vPairCollisionPartsToAlter.size();++i)
	{
		const std::pair<STL*,STL*>& pair = m_vPairCollisionPartsToAlter[i];
		if (pair.first!=nullptr)
		{
			if (pair.first->GetSTLName()==name)
			{
				return true;
			}
		}

		if (pair.second!=nullptr)
		{
			if (pair.second->GetSTLName()==name)
			{
				return true;
			}
		}
	}

	return false;
}

void MainWindow::SetTimeOffset(double val)
{
	//管子与加工值显示动画
	for (size_t i= 0; i < m_vMatrixAnimation.size();++i)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vMatrixAnimation[i].first;
		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(matrix->getUpdateCallback());

		if (callback!=NULL)
		{
			callback->setTimeOffset(val);
		}
	}

	for (size_t i= 0; i < m_vLongStraightPipeMaxtrixAnimation.size();++i)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vLongStraightPipeMaxtrixAnimation[i].first;
		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(matrix->getUpdateCallback());

		if (callback!=NULL)
		{
			callback->setTimeOffset(val);
		}
	}


	//模型
	for(size_t i=0;i<m_vSTLs.size();i++)
	{
		osg::AnimationPathCallback* Movecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vSTLs[i]->GetModelMoveMatrix()->getUpdateCallback());
		osg::AnimationPathCallback* Rotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vSTLs[i]->GetModelRotateMatrix()->getUpdateCallback());

		osg::AnimationPathCallback* MidRotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vSTLs[i]->GetModelMidRotateMatrix()->getUpdateCallback());
		osg::AnimationPathCallback* LeftRotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vSTLs[i]->GetModelLeftRotateMatrix()->getUpdateCallback());

		std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = m_vSTLs[i]->GetTransMatrix();
		if (!trans.empty())
		{
			for (size_t i = 0; i < trans.size();++i)
			{
				osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(trans[i]->getUpdateCallback());
				if (callback!=NULL)
				{
					callback->setTimeOffset(val);
				}

			}
		}

		osg::AnimationPathCallback* SecondRotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vSTLs[i]->GetModelSecondRotateMatrix()->getUpdateCallback());

		if (Movecallback!=NULL)
		{
			Movecallback->setTimeOffset(val);
		}

		if (Rotatecallback!=NULL)
		{
			Rotatecallback->setPause(val);
		}

		if (MidRotatecallback!=NULL)
		{
			MidRotatecallback->setTimeOffset(val);
		}

		if (LeftRotatecallback!=NULL)
		{
			LeftRotatecallback->setTimeOffset(val);
		}

		if (SecondRotatecallback!=NULL)
		{
			SecondRotatecallback->setTimeOffset(val);
		}
	}
}

void MainWindow::InitiMultiLmLoopParam(int i)
{
	if (m_sMultiLmLoop.isInitied && i == m_sMultiLmLoop.index)
	{
		// 相同弯的循环参数已经初始化
		return;
	}
	else
	{
		m_sMultiLmLoop.isInitied = false;
		m_sMultiLmLoop.index = -1; // 弯索引

		m_sMultiLmLoop.vfMyt.clear();
		m_sMultiLmLoop.vfMyth.clear();
		m_sMultiLmLoop.vfBtm.clear();
		m_sMultiLmLoop.vfCtm.clear();
		m_sMultiLmLoop.vfMz.clear();
		m_sMultiLmLoop.vDz.clear();
		m_sMultiLmLoop.vLc.clear();
		m_sMultiLmLoop.vMmd.clear();
		m_sMultiLmLoop.vLr.clear();
	}

	m_sMultiLmLoop.vfMyt.push_back(0.0);
	m_sMultiLmLoop.vfMyt.push_back(50.0);

	m_sMultiLmLoop.vfMyth.push_back(0.0);
	m_sMultiLmLoop.vfMyth.push_back(100.0);

	m_sMultiLmLoop.vfBtm.push_back(0.0);
	m_sMultiLmLoop.vfBtm.push_back(15.0);

	m_sMultiLmLoop.vfCtm.push_back(0.0);
	m_sMultiLmLoop.vfCtm.push_back(20.0);

	m_sMultiLmLoop.vfMz.push_back(0.0);
	m_sMultiLmLoop.vfMz.push_back(20.0);

	m_sMultiLmLoop.vDz.push_back(0);
	//m_sMultiLmLoop.vDz.push_back(1);
	//m_sMultiLmLoop.vDz.push_back(10);
	//m_sMultiLmLoop.vDz.push_back(11);
	//m_sMultiLmLoop.vDz.push_back(20);
	//m_sMultiLmLoop.vDz.push_back(21);

	//for (int j = 0; j < 19;++j)
	//{
	//	if (j%10==9)
	//	{
	//		continue;
	//	}

	//	m_sMultiLmLoop.vLc.push_back(j);
	//}
	m_sMultiLmLoop.vLc.push_back(0);

	if (m_iLoopRecord.empty() || m_iLoopRecord[0]!=i)
	{
		m_iLoopRecord.clear();

		m_iLoopRecord.push_back(i);
		for (int m = 0; m < 7;++m)
		{
			m_iLoopRecord.push_back(0);
		}
	}

	ProductionValue& paras = m_vproductionvalues[i];
	m_sMultiLmLoop.vMmd.push_back(paras.fmmd);
	if (i<=m_vproductionvalues.size()-2)
	{
		m_sMultiLmLoop.vMmd.push_back(m_vproductionvalues[i+1].fmm);
	}

	double angle = paras.fmb;
	//vLr.push_back(0);
	//vLr.push_back(1);
	//vLr.push_back(2);  // 反转右侧转角
	//vLr.push_back(3);  // 反转左侧转角
	if (fabs(paras.flr)<0.5)  //右侧加工
	{
		m_sMultiLmLoop.vLr.push_back(std::make_pair(0,angle)); //参数不变加工
		double newAngle;
		if (angle>0.0)
		{
			newAngle = angle - 360.0;
		}
		else
		{
			newAngle = +360.0;
		}
		m_sMultiLmLoop.vLr.push_back(std::make_pair(0,newAngle)); //转角翻转

		if (angle>0.0)
		{
			angle -= 180.0;
		}
		else if (angle<0.0)
		{
			angle += 180.0;
		}
		m_sMultiLmLoop.vLr.push_back(std::make_pair(1,angle));  // 换侧加工
		if (angle>0.0)
		{
			newAngle = angle - 360.0;
		}
		else
		{
			newAngle = angle + 360.0;
		}
		m_sMultiLmLoop.vLr.push_back(std::make_pair(1,newAngle)); //转角翻转

	}
	else // 左侧加工
	{
		m_sMultiLmLoop.vLr.push_back(std::make_pair(1,angle)); //参数不变加工
		double newAngle;
		if (angle>0.0)
		{
			newAngle = angle - 360.0;
		}
		else
		{
			newAngle = +360.0;
		}
		m_sMultiLmLoop.vLr.push_back(std::make_pair(1,newAngle)); //转角翻转

		if (angle>0.0)
		{
			angle -= 180.0;
		}
		else if (angle<0.0)
		{
			angle += 180.0;
		}
		m_sMultiLmLoop.vLr.push_back(std::make_pair(0,angle));  // 换侧加工
		if (angle>0.0)
		{
			newAngle = angle - 360.0;
		}
		else
		{
			newAngle = angle + 360.0;
		}
		m_sMultiLmLoop.vLr.push_back(std::make_pair(0,newAngle)); //转角翻转
	}

	m_sMultiLmLoop.isInitied = true;
	m_sMultiLmLoop.index = i; // 弯索引

}

// 优化加工值文件参数
void MainWindow::OptimizeMachinePara(size_t i)
{
	LogInfo("curve index is "+QString::number(i));

	if (i < 0)
	{
		return;
	}

	float delta = 0.00001;
	double time = -1.0*(m_mSegStartTime[i]-3.0);

	if (m_param.LmValueNum==1)
	{
		// 单层模
		std::vector<float> vfMyt;
		vfMyt.push_back(0.0);
		vfMyt.push_back(50.0);

		std::vector<float> vfBtm;
		vfBtm.push_back(0.0);
		vfBtm.push_back(15.0);

		std::vector<float> vfCtm;
		vfCtm.push_back(0.0);
		vfCtm.push_back(20.0);

		std::vector<float> vfMz;
		vfMz.push_back(0.0);
		vfMz.push_back(20.0);

		std::vector<int> vDz;
		vDz.push_back(0);
		vDz.push_back(1);
		vDz.push_back(10);
		vDz.push_back(11);
		vDz.push_back(20);
		vDz.push_back(21);

		std::vector<int> vLc;
		for (int i = 0; i < 19;++i)
		{
			if (i%10==9)
			{
				continue;
			}

			vLc.push_back(i);
		}

		ProductionValue& paras = m_vproductionvalues[i];
		for (size_t a = 0; a < vfMyt.size();++a)
		{
			paras.fmyt = vfMyt[a];

			for (size_t c = 0; c < vfBtm.size();++c)
			{
				paras.fbtm = vfBtm[c];
				paras.fctm = vfCtm[c];
				paras.fmz = vfMz[c];

				for (size_t f = 0; f < vDz.size();++f)
				{
					paras.fdz = vDz[f];

					for (size_t g = 0; g < vLc.size();++g)
					{
						paras.flc = vLc[g];
						AllStepsNew();
						SetPlayNew2();

						//SetTimeOffset(time/*-18.5*/);
					}
				}
			}
		}
	}
	else if (m_param.LmValueNum>1)
	{
		InitiMultiLmLoopParam(i);
		ProductionValue& paras = m_vproductionvalues[i];
		for (size_t a = m_iLoopRecord[1]; a < m_sMultiLmLoop.vfMyt.size();++a)
		{
			paras.fmyt = m_sMultiLmLoop.vfMyt[a];
			for (size_t b = m_iLoopRecord[2]; b < m_sMultiLmLoop.vfMyth.size();++b)
			{
				paras.fmyth = m_sMultiLmLoop.vfMyth[b];

				for (size_t c = m_iLoopRecord[3]; c < m_sMultiLmLoop.vfBtm.size();++c)
				{
					paras.fbtm = m_sMultiLmLoop.vfBtm[c];
					paras.fctm = m_sMultiLmLoop.vfCtm[c];
					paras.fmz = m_sMultiLmLoop.vfMz[c];
					for (size_t d = m_iLoopRecord[4]; d < m_sMultiLmLoop.vMmd.size();++d)
					{
						paras.fmmd = m_sMultiLmLoop.vMmd[d];

						for (size_t e = m_iLoopRecord[5]; e < m_sMultiLmLoop.vLc.size();++e)
						{
							paras.flc = m_sMultiLmLoop.vLc[e];
							//if (paras.flr == vLr[e])
							//{
							//	// 加工侧和加工文件一致，弯管角度不变
							//}
							//else 
							//{
							//	if (paras.fmb>0.0)
							//	{
							//		paras.fmb -= 180.0;
							//	}
							//	else if (paras.fmb<0.0)
							//	{
							//		paras.fmb += 180.0;
							//	}

							//    paras.flr = vLr[e];
							//}
							//paras.flr = m_sMultiLmLoop.vLr[e].first;
							//paras.fmb = m_sMultiLmLoop.vLr[e].second;

							//if (i<m_vproductionvalues.size()-2)
							//{
							//	ProductionValue& next = m_vproductionvalues[i+1];
							//	if (fabs(next.flr - paras.flr)>0.5)
							//	{
							//		//要换边
							//		if (next.fmb>0.0)
							//		{
							//			next.fmb -= 180.0;
							//		}
							//		else if (next.fmb<=0.0)
							//		{
							//			next.fmb += 180.0;
							//		}
							//	}
							//}

							for (size_t f = m_iLoopRecord[6]; f < m_sMultiLmLoop.vDz.size();++f)
							{
								paras.fdz = m_sMultiLmLoop.vDz[f];

								for (size_t g = m_iLoopRecord[7]; g < m_sMultiLmLoop.vLr.size();++g)
								{
									//paras.flc = m_sMultiLmLoop.vLc[g];
									paras.flr = m_sMultiLmLoop.vLr[g].first;
									paras.fmb = m_sMultiLmLoop.vLr[g].second;

									if (i<m_vproductionvalues.size()-2)
									{
										ProductionValue& next = m_vproductionvalues[i+1];
										if (fabs(next.flr - paras.flr)>0.5)
										{
											//要换边
											if (next.fmb>0.0)
											{
												next.fmb -= 180.0;
											}
											else if (next.fmb<=0.0)
											{
												next.fmb += 180.0;
											}
										}
									}

									bool detectedBak = m_IsDetected;
									m_IsDetected = true;
									m_bIsAutoPlan = true;

									m_iLoopRecord[1] = a;
									m_iLoopRecord[2] = b;
									m_iLoopRecord[3] = c;
									m_iLoopRecord[4] = d;
									m_iLoopRecord[5] = e;
									m_iLoopRecord[6] = f;
									if (g+1==m_sMultiLmLoop.vLr.size())
									{
										m_iLoopRecord[7] = 0;
										if (f+1==m_sMultiLmLoop.vDz.size())
										{
											m_iLoopRecord[6] = 0;
											if (e+1 == m_sMultiLmLoop.vLc.size())
											{
												m_iLoopRecord[5] = 0;
												if (d+1==m_sMultiLmLoop.vMmd.size())
												{
													m_iLoopRecord[4] = 0;
													if (c+1==m_sMultiLmLoop.vfBtm.size())
													{
														m_iLoopRecord[3] = 0;
														if (b+1 == m_sMultiLmLoop.vfMyth.size())
														{
															m_iLoopRecord[2]=0;
															if (a+1==m_sMultiLmLoop.vfMyt.size())
															{
																on_actionLight_triggered(true);
																// 循环完后没有合适的
																QString info("Can not optimize product line ");
																info.append(QString::number(m_productionTable->GetRow()));
																int result= QMessageBox::information(NULL, info,"","CurveToStraight","ModifyYBC","Exit");
																//if (m_pOptimizeDlg!=NULL)
																//{
																//	m_pOptimizeDlg->show();
																//	return;
																//}

																switch (result)
																{
																case 0 :
																	//on_actionLight_triggered2(true);
																	//emit sigPartsPause(false);
																	//break;
																	LogInfo("Curve to straight !");
																	paras.fmy += fabs(m_param.LmRadius[paras.fmm]*paras.fmc*PI/180.0);
																	paras.fmc = 0.0;
																	break;

																case 1:
																	LogInfo("Modify YBC !");
																	time = 0.0;
																	break;

																case 2:
																	LogInfo("Exit !");
																	return;

																	//case QMessageBox::No:
																	//on_actionLight_triggered(true);
																	//	//emit sigPartsPause(false);
																	//	break;

																default:
																	//break;
																	return;
																}
															}
															else
															{
																m_iLoopRecord[1] = a+1;
															}
														}
														else
														{
															m_iLoopRecord[2] = b+1;
														}
													}
													else
													{
														m_iLoopRecord[3] = c+1;
													}
												}
												else
												{
													m_iLoopRecord[4] = d+1;
												}
											}
											else
											{
												m_iLoopRecord[5] = e+1;
											}
										}
										else
										{
											m_iLoopRecord[6] = f+1;
										}
									}
									else
									{
										m_iLoopRecord[7] = g+1;
									}
									for (int n =0;n<8; ++n)
									{
										LogInfo("index "+ QString::number(n) +" "+ QString::number(m_iLoopRecord[n]));
									}

									AllStepsNew();
									SetPlayNew2();
									SetTimeOffset(time);

									m_IsDetected = detectedBak;
									m_bIsAutoPlan = false;  // 判断是否是自动优化

									return;
								}
							}
						}
					}
				}
			}
		}
	}
}

static int index = 0;

QString createMultipleFolders(const QString path)
{
	QDir dir(path);
	if (dir.exists(path))
	{
		return path;
	}

	QString parentDir = createMultipleFolders(path.mid(0, path.lastIndexOf('/')));
	QString dirName = path.mid(path.lastIndexOf('/') + 1);
	QDir parentPath(parentDir);
	if (!dirName.isEmpty())
	{
		parentPath.mkpath(dirName);
	}
	return parentDir + "/" + dirName;
}

void MainWindow::writePipeStl(osg::PositionAttitudeTransform * pMatrix)
{
	osg::Matrixd matrix1;
	if (pMatrix)
	{
		osg::Matrix matrix;
		osg::Matrixd matrix0;
		matrix.makeScale(pMatrix->getScale());   // 先缩放
		matrix0 = matrix;
		matrix.makeTranslate(-pMatrix->getPivotPoint()); // 移动到旋转中心点
		matrix0 = matrix0*matrix;
		matrix.makeRotate(pMatrix->getAttitude());      // 旋转
		matrix0 = matrix0*matrix;

		osg::Vec3d vec = pMatrix->getPosition();
		vec[0] = 0.0;
		vec[1] = 0.0;
		matrix.makeTranslate(vec);            // 平移
		matrix0 = matrix0*matrix;
		matrix.makeTranslate(pMatrix->getPivotPoint()); // 移动回去
		matrix0 = matrix0*matrix;

		matrix1 = matrix0*matrix1;
	}

	unsigned int uNum = pMatrix->getNumChildren();
	QString message("child number is ");
	message.append(QString::number(uNum));
	LogInfo(message.toStdString().c_str());
	for (size_t j = 0; j < uNum; j++)
	{
		osg::Node *pNode1 = pMatrix->getChild(j);
		if (pNode1)
		{
			// 获取包围盒
			std::vector<VecArrayName> vVertices1;

			GetAllPipeSegment(QString(pNode1->getName().c_str()),pNode1,matrix1,vVertices1);

			QString info("vVertices1 size is ");
			info.append(QString::number(vVertices1.size()));
			LogInfo(info.toStdString().c_str());
			for (size_t m = 0; m < vVertices1.size();++m)
			{
				std::vector<osg::Vec3> normals1;
				size_t size = vVertices1[m].array->size()/3;
				normals1.reserve(size);
				for (size_t n = 0; n < size;++n)
				{
					normals1.push_back(CalcTriNormal((*vVertices1[m].array)[3*n],(*vVertices1[m].array)[3*n+1],(*vVertices1[m].array)[3*n+2]));
				}

				QString fold("D:/testdata/");
				fold.append(QString::number(index));
				fold.append("/");

				createMultipleFolders(fold);

				//fold.append("/");
				//fold.append(QString::number(j));
				fold.append(vVertices1[m].name);


				//fold.append("/");
				//fold.append(QString::number(m));
				fold.append(".stl");

				SaveStlASCIINew(fold.toStdString().c_str(),vVertices1[m].array,normals1,false);
			}
		}
	}
}

void MainWindow::exportCollisionPairs(std::string n0,std::string n1)
{
	QString fold("D:/testdata/");
	fold.append(QString::number(index));
	fold.append("/");
	createMultipleFolders(fold);

	index++;

	if (n0.find(".stl")!=std::string::npos)
	{
		QString file(fold);
		file.append(n0.c_str());

		for (int i = 0; i < m_vSTLs.size(); ++i)
		{
			if (m_vSTLs[i]->GetSTLName()==QString(n0.c_str()))
			{
				SaveModel(file.toStdString().c_str(),m_vSTLs[i]);
			}
		}	
	}
	else
	{
		QString pipeFile(fold);
		pipeFile.append(n0.c_str());
		pipeFile.append(".stl");
		for (int i = 0; i < m_vPipes.size();++i)
		{
			if (m_vPipes[i]->GetName()==QString(n0.c_str()))
			{
				SavePipe(pipeFile.toStdString().c_str(),m_vPipes[i]);
			}
		}
	}

	if (n1.find(".stl")!=std::string::npos)
	{
		QString file(fold);
		file.append(n1.c_str());

		for (int i = 0; i < m_vSTLs.size(); ++i)
		{
			if (m_vSTLs[i]->GetSTLName()==QString(n1.c_str()))
			{
				SaveModel(file.toStdString().c_str(),m_vSTLs[i]);
			}
		}
		
	}
	else
	{
		QString pipeFile(fold);
		pipeFile.append(n1.c_str());
		pipeFile.append(".stl");
		for (int i = 0; i < m_vPipes.size();++i)
		{
			if (m_vPipes[i]->GetName()==QString(n1.c_str()))
			{
				SavePipe(pipeFile.toStdString().c_str(),m_vPipes[i]);
			}
		}
	}
}

void MainWindow::collisionDetectedNew(std::string n0,std::string n1)
{
	LogInfo("Enter collision detected new !");

	std::vector<QString> vtmp;
	vtmp.push_back(n0.c_str());
	vtmp.push_back(n1.c_str());
	ChangeMutiModelColor(vtmp);

	exportCollisionPairs(n0, n1);

	emit sigPartsPause(true);

	QString info(m_vCurLanguageWords[98]);
	info.append(n0.c_str());
	info.append(" and ");
	info.append(n1.c_str());

	//LogInfo(info);

	//return;

	int result;
	if (m_strDefaultLanguageType=="中文")
	{
		result = QMessageBox::information(NULL, "", info,"确定");
	}
	else if ("English"==m_strDefaultLanguageType)
	{
		result = QMessageBox::information(NULL, "", info,QMessageBox::Yes);
	}

	// 动画暂停 wangwenbo 20241005
	// on_actionLight_triggered(true);
	switch (result)
	{
	case QMessageBox::Yes:
		//on_actionLight_triggered2(false);
		//emit sigPartsPause(false);
		break;

		//case QMessageBox::No:
		//	//on_actionLight_triggered(false);
		//	//emit sigPartsPause(false);
		//	break;

	default:
		break;
	}
}

void MainWindow::collisionDetect()
{
	LogInfo("Enter collision detect !");

	clock_t curTime = clock();
	LogInfo(QString("time out is %1").arg(curTime));

	time_t start = clock();
	if (/*m_vCollisionParts*/m_vPairCollisionPartsToAlter.empty())
	{
		// 未设置碰撞检测
		return;
	}

	//osg::ref_ptr<osg::Group> pipefather = m_sceneModel->getPipeFather();
	std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> matrixs = GetAnmiPipeSeg();
	LogInfo("matrixs size is " + QString::number(matrixs.size()));
	if (matrixs.empty() || matrixs.size()==0)
	{
		return;
	}

	index++;

	QString name1;
	osg::ref_ptr<osg::Group> root;
	for (size_t m = 0; m < m_vNodes.size();++m)
	{
		if (m_vNodes[m]->GetSTLName()=="Root")
		{
			root = m_vNodes[m]->GetModelMatrix();
			break;
		}
	}

	QString name0;
#if 0

	writePipeStl(matrixs[0]);
	writePipeStl(m_vLongStraightPipeMaxtrixAnimation[0].first);

	std::vector<VecArrayName> vVertices0;
	name0 = "Root";
	osg::Matrixd matrix0;
	GetAllLineSegment(name0,root, matrix0,vVertices0);
	for (int j = 0; j < vVertices0.size();++j)
	{
		QString fold("D:/testdata/");
		fold.append(QString::number(index));
		fold.append("/model/");

		createMultipleFolders(fold);

		fold.append(vVertices0[j].name);

		std::vector<osg::Vec3> normals1;
		int size = vVertices0[j].array->size()/3;
		normals1.reserve(size);
		for (int n = 0; n < size;++n)
		{
			normals1.push_back(CalcTriNormal((*vVertices0[j].array)[3*n],(*vVertices0[j].array)[3*n+1],(*vVertices0[j].array)[3*n+2]));
		}

		if (vVertices0[j].name.contains("Straight",Qt::CaseSensitive))
		{
			SaveStlASCIINew(fold.toStdString().c_str(),vVertices0[j].array,normals1,false);
		}
	}

 //   osg::Node* node = (osg::Node*)matrixs[0];
	//osg::PositionAttitudeTransform *pMatrix = (osg::PositionAttitudeTransform*)node;
	//LogInfo(QString("Matrix pos z is %1 !").arg(pMatrix->getPosition()[2]));
	//LogInfo(QString("Matrix size %d !").arg(matrixs.size()));
#endif 

	osg::ref_ptr<osg::Group> SongLiao;
	for (size_t m = 0; m < m_vNodes.size();++m)
	{
		if (m_vNodes[m]->GetSTLName()=="SongLiao")
		{
			SongLiao = m_vNodes[m]->GetModelMatrix();
			break;
		}
	}

	// if (HitDetectionNew(m_sceneModel->GetNodeScene(),matrixs[0],m_vPairCollisionPartsToAlter,name0,name1,m_vSTLs))
	QString type0;
	if (HitDetection2(root,SongLiao,matrixs[0],m_vPairCollisionPartsToAlter,name0,type0,name1,m_vSTLs))
	{
		LogInfo("Hit detection !");

		if (m_bAutoPlaning)
		{
			time_t end = clock()- start;
			LogInfo("collision and auto plan time consume "+QString::number(end));

			// 如果是加工自动规划，不弹窗
			OptimizeMachinePara(m_productionTable->GetRow());
			//QString info("product line ");
			//info.append(QString::number(m_productionTable->GetRow()));
			//QMessageBox:: StandardButton result= QMessageBox::information(NULL, "", info, QMessageBox::Yes);
			//switch (result)
			//{
			//case QMessageBox::Yes:
			//	//on_actionLight_triggered2(false);
			//	//emit sigPartsPause(false);
			//	break;

			//	//case QMessageBox::No:
			//	//	//on_actionLight_triggered(false);
			//	//	//emit sigPartsPause(false);
			//	//	break;

			//default:
			//	break;
			//}

			return;
		}
		//on_actionLight_triggered(true);

		//On_modelColor_change(name0);
		//On_modelColor_change(name1);
		std::vector<QString> vtmp;
		vtmp.push_back(name0);
		vtmp.push_back(name1);
		ChangeMutiModelColor(vtmp);

		emit sigPartsPause(true);
		QString info(m_vCurLanguageWords[98]);

		// 如果是要回退的管子，不做碰撞检测
		if (name1.contains("back",Qt::CaseSensitive))
		{
			return;
		}

		std::map<QString,QString>::iterator it;
		if (name0.contains("Straight",Qt::CaseSensitive) || name0.contains("Curve",Qt::CaseSensitive))
		{
			name0 = m_vCurLanguageWords[100];
		}
		else
		{
			// it = m_ModelNameToDisplayName.find(type0);
			it = m_ModelNameToDisplayName.find(name0);
			if (it!=m_ModelNameToDisplayName.end())
			{
				name0 = it->second;
			}

			/*it = m_ModelNameToDisplayName.find(name1);
			if (it!=m_ModelNameToDisplayName.end())
			{
			name1 = it->second;
			}
			info.append(name1);
			info.append(" !");*/
		}
		info.append(name0);
		//info.append(m_vCurLanguageWords[99]);

		if (name1.contains("Straight",Qt::CaseSensitive) || name1.contains("Curve",Qt::CaseSensitive))
		{
			// name1 = m_vCurLanguageWords[100];
		}
		else
		{
			//std::map<QString,QString>::iterator it;
			//it = m_ModelNameToDisplayName.find(name0);
			//if (it!=m_ModelNameToDisplayName.end())
			//{
			//name0 = it->second;
			//}
			//info.append(name0);
			//info.append(m_vCurLanguageWords[99]);
			it = m_ModelNameToDisplayName.find(name1);
			if (it!=m_ModelNameToDisplayName.end())
			{
				name1 = it->second;
			}

		}
		// info.append(name1);
		info.append(" !");

		int result;
		if (m_strDefaultLanguageType=="中文")
		{
			result = QMessageBox::information(NULL, "", info,"确定");
		}
		else if ("English"==m_strDefaultLanguageType)
		{
			result = QMessageBox::information(NULL, "", info,QMessageBox::Yes);
		}

		matrixs.clear();
		on_actionLight_triggered(true);
		switch (result)
		{
		case QMessageBox::Yes:
			//on_actionLight_triggered2(false);
			//emit sigPartsPause(false);
			break;

			//case QMessageBox::No:
			//	//on_actionLight_triggered(false);
			//	//emit sigPartsPause(false);
			//	break;

		default:
			break;
		}

	}

	time_t end = clock()- start;
	LogInfo("no collision time consume "+QString::number(end));

}

// 写加工值文件
void MainWindow::WriteProductionFile(QString file0)
{

	//执行读取加工值文件
	QString file;
	QString strtmp;
	std::string stmp;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PRODUCTION_VALUES_FILES_PATH, 3, production_values_files_path);
	GetWholePath(production_values_files_path);
	stmp = production_values_files_path;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	file = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	file = file + QString::fromLocal8Bit("/加工值新.csv");

	std::ofstream outFile;

	std::string str = std::string((const char *)file.toLocal8Bit().constData());

	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile <<"my"<<','<<"myb"<<','<<"mb"<<','<<"mbb"<<','<<"mc"<<','<<"mcb"<<','<<"mm"<<','<<"lr"<<','<<"myt"<<','<<"mz" <<','
		<<"btm"<<','<<"ctm"<<','<<"dz"<<','<<"myth"<<','<<"lc"<<','<<"mmd"<<','<<"jm"<<','<<"xb"<<','<<"jy"<<','<<"twy" <<','
		<<"twbj"<<','<<"twc"<<','<<"twb"<<','<<"twlc" << std::endl;

	//写入数据-字符串
	std::vector<ProductionValue>::iterator it = m_vproductionvalues.begin();
	for(;it!=m_vproductionvalues.end();it++)
	{	
		std::string my = QString::number((*it).fmy).toStdString();
		std::string myb = QString::number((*it).fmyb).toStdString();
		std::string mb = QString::number((*it).fmb).toStdString();
		std::string mbb = QString::number((*it).fmbb).toStdString();
		std::string mc = QString::number((*it).fmc).toStdString();
		std::string mcb = QString::number((*it).fmcb).toStdString();
		std::string mm = QString::number((*it).fmm).toStdString();
		std::string lr = QString::number((*it).flr).toStdString();
		std::string myt = QString::number((*it).fmyt).toStdString(); 
		std::string mz = QString::number((*it).fmz).toStdString();
		std::string btm = QString::number((*it).fbtm).toStdString(); 
		std::string ctm = QString::number((*it).fctm).toStdString(); 
		std::string dz = QString::number((*it).fdz).toStdString();
		std::string myth = QString::number((*it).fmyth).toStdString();
		std::string lc = QString::number((*it).flc).toStdString();
		std::string mmd = QString::number((*it).fmmd).toStdString(); 
		std::string jm = QString::number((*it).fjm).toStdString();
		std::string xb = QString::number((*it).fxb).toStdString();
		std::string jy = QString::number((*it).fjy).toStdString();
		std::string twy = QString::number((*it).ftwy).toStdString(); 
		std::string twbj = QString::number((*it).ftwbj).toStdString();
		std::string twc = QString::number((*it).ftwc).toStdString(); 
		std::string twb = QString::number((*it).ftwb).toStdString(); 
		std::string twlc = QString::number((*it).ftwlc).toStdString();

		outFile<< my   <<','<< myb <<','<< mb <<','<<  mbb   <<','<<mc<<','<< mcb <<','<< mm <<','<<lr <<','<<myt<<','<< mz <<','<<
			btm  <<','<< ctm <<','<< dz <<','<<  myth  <<','<<lc<<','<< mmd <<','<< jm <<','<<xb <<','<<jy<<','<< twy <<','<<
			twbj <<','<< twc <<','<< twb <<','<< twlc << std::endl;
	}

	outFile.close();
}

void MainWindow::AddChildToFatherMatrix(QString strChildName,QString strFatherName)
{
	//声明模型类对象

	if(strChildName == QString::fromLocal8Bit(m_curNode->getName().c_str()) && strChildName.isEmpty()==false && strFatherName.isEmpty()==false)
	{
		STL *curstl = new STL(m_curNode,m_curNode->getName());
		//获取父模型矩阵
		for(size_t i = 0;i<m_vSTLs.size();i++)
		{
			if(m_vSTLs[i]->GetSTLName() == strFatherName)
			{
				if(strFatherName == "NodeScene")
				{
					m_sceneModel->GetNodeScene()->addChild(curstl->GetModelMatrix());

				}
				else
				{
					m_vSTLs[i]->GetModelMatrix()->addChild(curstl->GetModelMatrix());

				}

				//NodeDlg->AddFatherToChildrenMap(strChildName,"");
			}
		}


		m_vSTLs.push_back(curstl);
		//m_sceneView->RunSlotUpdateModel();
		//m_sceneView->UpdateView();
	}

}

void MainWindow::DelChildToFatherMatrix(QString strChildName,QString strFatherName)
{
	STL *tmpChild;
	STL *tmpFather;
	if(strChildName.isEmpty() == false && strFatherName.isEmpty()==false)
	{
		for(size_t i = 0;i<m_vSTLs.size();i++)
		{
			if(m_vSTLs[i]->GetSTLName() == strChildName)//获得子节点
			{
				tmpChild = m_vSTLs[i];
			}
			if(m_vSTLs[i]->GetSTLName() == strFatherName)//获得父节点
			{
				tmpFather = m_vSTLs[i];
			}
		}
		//删除连接
		if(tmpFather->GetSTLName() == "NodeScene")
		{
			m_sceneModel->GetNodeScene()->removeChild(tmpChild->GetModelMatrix());
		}
		else
		{
			tmpFather->GetModelMatrix()->removeChild(tmpChild->GetModelMatrix());
		}
		//删除模型对象
		std::vector<QString> vAllNodeDel = NodeDlg->GetAllDelNode();
		for(size_t t=0;t<vAllNodeDel.size();t++)
		{
			std::vector<STL*>::iterator iter = m_vSTLs.begin();
			for (; iter != m_vSTLs.end(); ++iter)
			{
				if((*iter)->GetSTLName() == vAllNodeDel[t])
				{
					m_vSTLs.erase(iter);
					break;
				}
			}
		}


		//m_sceneView->RunSlotUpdateModel();

	}

	UpdataTableRow();//删除表格里对应的模型行
}

void MainWindow::SlotRightMenu(const QPoint& pos)
{
	if(cmenu)
	{
		cmenu->exec(QCursor::pos());
	}
}

void MainWindow::SlotMenuTablecheckbox_clicked(bool show)
{
	if (show)
	{
		ui.dockWidget_Properties->show();
	}
	else
	{
		ui.dockWidget_Properties->hide();
	}
}

void MainWindow::SlotMenuBtnscheckbox_clicked(bool show)
{
	if (show)
	{
		ui.dockWidget_Model->show();
	}
	else
	{
		ui.dockWidget_Model->hide();
	}
}

void MainWindow::SlotMenuLookAt_clicked(bool i)
{
	osg::Vec3f eye1,center1,up1;
	m_sceneView->getOsgView()->getCamera()->getViewMatrixAsLookAt(eye1,center1,up1);

	std::ofstream outFile;
	QString file = need_filePath + QString::fromLocal8Bit("/ViewLookAt.csv");

	std::string str = std::string((const char *)file.toLocal8Bit().constData());

	outFile.open(str,std::ios::out | std::ios::trunc);
	//写标题
	outFile<<"eye"<<','<<"center"<<','<<"up"<<std::endl;
	//写入数据-字符串
	std::string se_x = QString::number(eye1.x()).toStdString();
	std::string se_y = QString::number(eye1.y()).toStdString();
	std::string se_z = QString::number(eye1.z()).toStdString();
	outFile<<"\"("<<se_x<<','<<se_y<<','<<se_z<<")\""<<',';

	std::string sc_x = QString::number(center1.x()).toStdString();
	std::string sc_y = QString::number(center1.y()).toStdString();
	std::string sc_z = QString::number(center1.z()).toStdString();
	outFile<<"\"("<<sc_x<<','<<sc_y<<','<<sc_z<<")\""<<',';

	std::string su_x = QString::number(up1.x()).toStdString();
	std::string su_y = QString::number(up1.y()).toStdString();
	std::string su_z = QString::number(up1.z()).toStdString();
	outFile<<"\"("<<su_x<<','<<su_y<<','<<su_z<<")\""<<std::endl;

	outFile.close();
}

void MainWindow::closeTimer()
{
	m_closeTimer->stop();
	m_collisionTimer->stop();
}

void MainWindow::JiTouMoZuoPos(Point3D fTrn, double& fTimeStart)
{
	AnimationInfo info;
	info.fStartTime = fTimeStart;
	info.fEndTime = info.fStartTime + m_fAnimationTime;
	info.translate = fTrn;
	fTimeStart = info.fEndTime + m_fAnimationStopTime; 
	m_vJiTouMoZuoAnimation.push_back(info);
}

void MainWindow::JiTouDiZuoPos(Point3D fTrn, double& fTimeStart)
{
	AnimationInfo info;
	info.fStartTime = fTimeStart;
	info.fEndTime = info.fStartTime + m_fAnimationTime;
	info.translate = fTrn;
	fTimeStart = info.fEndTime + m_fAnimationStopTime; 
	m_vJiTouDiZuoAnimation.push_back(info);
}

void MainWindow::slotNewLoadedFile(osg::ref_ptr<osg::Node> node,
	osg::ref_ptr<osg::Material> material,const QString& name)
{
	// reset the cursor
	QApplication::restoreOverrideCursor();

	resetViews(true);

	if (!node)
	{
		return;
	}

	//slotSettingNodeFinished(node, name);
	//return;

	////xSettingNodeDlg NodeDlg(this);

	///*NodeDlg.setCurLineEditFileName(QFileInfo(name).fileName());
	//NodeDlg.setNodeValue(node);*/
	//NodeDlg->setWindowModality(Qt::WindowModal);
	////NodeDlg.getNodeTypeCombox()->setCurrentIndex(0);
	//NodeDlg->setCurNodeName(name);
	//NodeDlg->setWindowTitle(name);
	//NodeDlg->initCombox();
	//NodeDlg->setIsDel(false);
	//if(m_bIsFirstSTL == true)//添加根节点到父节点下拉框中
	//{
	//	NodeDlg->InitFatherCombox();
	//	STL *scene = new STL(m_sceneModel->GetNodeScene(),m_sceneModel->GetNodeScene()->getName());
	//	m_vSTLs.push_back(scene);
	//}
	//NodeDlg->show();

	//if (NodeDlg->exec()==0)
	//{	
	slotSettingNodeFinishedNew(1,1,"","",node,material,name);
	//}
	//m_bIsFirstSTL == false;
}

void MainWindow::slotNewLoadedFileNew(osg::Node * node,const QString& name,bool IsFirst)
{
	QApplication::restoreOverrideCursor();

	resetViews(true);

	if (!node)
	{
		return;
	}

	//建立连接
	m_curNode = m_objectLoader->GetLoadedModel();
	//if(m_bIsFirstSTL == true)
	//{
	//	STL *firststl = new STL(m_curNode,name.toStdString());
	//	m_vSTLs.push_back(firststl);

	//	m_sceneModel->GetNodeScene()->addChild(firststl->GetModelMatrix());
	//}
	//else

	if(NodeDlg->GetIsAdd() == true)
	{
		AddChildToFatherMatrix(NodeDlg->GetCurChildName(),NodeDlg->GetCurFatherName());
	}

	slotSettingNodeFinishedMy(node, name,IsFirst);
	return;
}

void MainWindow::addRecentlyOpenedFile(const QString &fn, QStringList &lst)
{
	QFileInfo fi(fn);

	if (lst.contains(fi.absoluteFilePath()))
		return;

	if (lst.count() >= maxRecentlyOpenedFile)
		lst.removeLast();

	lst.prepend(fi.absoluteFilePath());
}

void MainWindow::setupRecentFilesMenu()
{
	ui.menuRecent_Files->clear();

	if (m_recentFiles.count() > 0)
	{
		ui.menuRecent_Files->setEnabled(true);
		QStringList::Iterator it = m_recentFiles.begin();

		for (; it != m_recentFiles.end(); ++it)
			ui.menuRecent_Files->addAction(*it);
	}
	else
	{
		ui.menuRecent_Files->setEnabled(false);
	}
}

void MainWindow::recentFileActivated(QAction *action)
{
	if (!action->text().isEmpty())
	{
		if (!loadFile(action->text()))
			m_recentFiles.removeAt(m_recentFiles.indexOf(action->text()));
	}
}

// enable/disable actions depending on a valid loaded file
void MainWindow::enableActions(bool val)
{
	//ui->actionSnapShot->setEnabled(val);
	ui.actionReset_View->setEnabled(val);
	ui.actionTexture->setEnabled(val);
	ui.actionLight->setEnabled(false);
	ui.actionHighLight->setEnabled(val);
	ui.actionBackFace->setEnabled(val);
	ui.actionOptimize->setEnabled(val);
	ui.actionUnload->setEnabled(val);

	ui.actionEnableNode->setEnabled(val);

	ui.actionQuit->setEnabled(false);

	//ui->actionIncreaseLOD->setEnabled(val);
	//ui->actionDecreaseLOD->setEnabled(val);
	//ui->actionResetLOD->setEnabled(val);
	//ui->actionNewBookmark->setEnabled(val);

	//ui->widgetBookmarkManager->setEnabled(val);
	//ui->widgetStats->setEnabled(val);
	ui.dockWidget_Model->setEnabled(val);
	ui.dockWidget_Properties->setEnabled(val);
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData* mimeData = event->mimeData();

	if (!mimeData->hasUrls())
	{
		event->ignore();
		return;
	}

	QList<QUrl> urls = mimeData->urls();

	if(urls.count() != 1)
	{
		event->ignore();
		return;
	}

	QUrl url = urls.at(0);
	QString filename = url.toLocalFile();

	// We don't test extension
	if (!QFileInfo(filename).exists())
	{
		event->ignore();
		return;
	}

	if (!QFileInfo(filename).isFile())
	{
		event->ignore();
		return;
	}

	event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->source() == this)
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
	else
	{
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
	const QMimeData* mimeData = event->mimeData();
	QList<QUrl> urls = mimeData->urls();
	QUrl url = urls.at(0);

	QString filename = url.toLocalFile();
	loadFile(filename);
}

//-------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
	//if (!saveIfNeeded())
	//{
	//    statusBar()->showMessage(tr("Quit application aborted !!"), 5000);
	//    event->ignore();
	//    return; // cancel triggered !!
	//}

	//just in case => restore the dockWidgets
	//on_actionLight_triggered(true);

	if (m_pCollisionDlg!=NULL)
		m_pCollisionDlg->close();

	if (m_pNormalUserWidget!=NULL)
		m_pNormalUserWidget->close();

	//Sleep(3000);
	//removeCallBack();

	//showDockWidgets();
}

void MainWindow::updateApplicationIdentity()
{
	QString strTitle = PACKAGE_NAME;
	strTitle += " ";
	strTitle += PACKAGE_VERSION;
	setWindowTitle(strTitle);
	QApplication::setApplicationName(PACKAGE_NAME);
	QApplication::setApplicationVersion(PACKAGE_VERSION);
	QApplication::setOrganizationName(PACKAGE_ORGANIZATION);
}

void MainWindow::on_actionUnload_triggered()//播放
{
	SetViewLookAtByFileValue();

	ResumeAllModelColor();

#if 1
	//执行读取加工值文件
	QString strtmpName;
	QString strtmp;
	std::string stmp;
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, PRODUCTION_VALUES_FILES_PATH, 3, production_values_files_path);
	GetWholePath(production_values_files_path);
	stmp = production_values_files_path;
	strtmp = QString::fromLocal8Bit(stmp.c_str());
	strtmpName = QDir::fromNativeSeparators(strtmp);//"\"转"/"
	strtmpName = strtmpName + QString::fromLocal8Bit("/加工值.csv");	
	ReadProductionValues(strtmpName);

	//更新加工值表格数据
	RebuildProductionTable();

	//读取运动位置文件
	QString strMovePosFile = need_filePath + QString::fromLocal8Bit("/运动位置.csv");
	ReadMovePosFile(strMovePosFile);

	QString groupSetting = need_filePath + QString::fromLocal8Bit("/GroupToModels.csv");
	ReadGroupToSTLsFile(groupSetting);
	SetGroupToSTLS();

	//读取组设置文件
	QString strSettingGroupFile = need_filePath + QString::fromLocal8Bit("/代码定义.csv");
	ReadGroupSettingFile(strSettingGroupFile);

	QString strCenterFile = need_filePath + QString::fromLocal8Bit("/JT1/JT1_LMValues.csv");
	std::vector<LMR> vValues = ReadJT1CenterFileValues(strCenterFile);
	GetLayerToR(vValues);

	char max_r[32];
	GetNthValueFromTxt(MODEL_CONFFIG_FILE, CONFIG, MAX_R, 3, max_r);
	m_fmax_r = atof(max_r);

	for (size_t k=0;k<m_param.LmRadius.size();k++)
	{
		if(m_param.LmRadius[k]>m_fmax_r)
		{
			m_fmax_r = m_param.LmRadius[k];
		}
	}

	//读流程文件
	OnlyReadProcessFileNew();

	//设置碰撞检测对象
	SetCollisionDetectParts();

	// 添加需要做碰撞检测的模型
	//AddModelsToCollision();
	//GetLoopStartAndEnd();
	AllStepsNew();
	//SetPlayNew();
	//SetPlayNewNew();

	SetPlayNew2();

#endif
}

void MainWindow::saveSettings()
{
	QSettings settings(PACKAGE_ORGANIZATION, PACKAGE_NAME);

	settings.beginGroup("MainWindow");
	settings.setValue("MainWindowState",saveState(0));
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.setValue("fullScreen", isFullScreen());
	settings.endGroup();

	settings.beginGroup("Application");

	settings.setValue("lastDirectory", m_lastDirectory);
	//settings.setValue("lastDirectorySnapshot", m_lastDirectorySnapshot);

	//settings.setValue("currentLanguage", m_currentLanguage);

	//settings.setValue("resetConfig", m_resetConfig);
	//settings.setValue("watermark", m_watermark);
	//settings.setValue("inverseMouseWheel", m_inverseMouseWheel);
	//settings.setValue("displayTrackballHelper", m_displayTrackballHelper);

	//settings.setValue("splashscreenAtStartup", m_splashscreenAtStartup);
	//settings.setValue("splashscreenTransparentBackground", m_splashscreenTransparentBackground);

	// recent files
	settings.setValue("recentlyOpenedFiles", m_recentFiles);

	// scene background
	//settings.setValue("bgcolor", ui->widgetSceneView->getBgColor());

	settings.endGroup();
}

void MainWindow::loadSettings()
{
	xAppSettings settings;

	settings.beginGroup("MainWindow");

	restoreState(settings.value("MainWindowState").toByteArray(), 0);

	resize(settings.value("size", QSize(600, 600)).toSize());
	move(settings.value("pos", QPoint(200, 200)).toPoint());

	bool fullScreen = settings.value("fullScreen",false).toBool();
	if (fullScreen)
		showFullScreen();
	settings.endGroup();

	settings.beginGroup("Application");
	m_lastDirectory = settings.value("lastDirectory","/home").toString();
	//m_lastDirectorySnapshot = settings.value("lastDirectorySnapshot","/home").toString();

	// recent files
	m_recentFiles = settings.value("recentlyOpenedFiles").toStringList();

	//m_splashscreenAtStartup = settings.value("splashscreenAtStartup", true).toBool();
	//m_splashscreenTransparentBackground = settings.value("splashscreenTransparentBackground",true).toBool();

	//m_resetConfig = settings.value("resetConfig", false).toBool();
	//m_watermark = settings.value("watermark", true).toBool();
	//m_inverseMouseWheel = settings.value("inverseMouseWheel", false).toBool();
	//ui->widgetSceneView->setEnabledInverseMouseWheel(m_inverseMouseWheel);

	//m_displayTrackballHelper = settings.value("displayTrackballHelper", false).toBool();
	//ui->widgetSceneView->setEnabledTrackbalHelper(m_displayTrackballHelper);

	//m_currentLanguage = settings.value("currentLanguage","").toString();

	//if (m_currentLanguage.isEmpty())
	//	m_currentLanguage = QLocale::system().name().left(2);

	//// scene background
	//QColor color = settings.value("bgcolor",QColor(50,50,50)).value<QColor>();
	//ui->widgetSceneView->setBgColor(color);

	settings.endGroup();
}

void MainWindow::on_actionReset_View_triggered()
{
	if (m_pCollisionDlg!=NULL)
	{
		if (m_pCollisionDlg->isMinimized())
		{
			m_pCollisionDlg->showNormal();
		}
		else
		{
			ReadCollisionDectionLanguageFile();
			ReadCollisionDectionFile();
			//m_pCollisionDlg->InitAllChildrenCombox(m_vAllNodeName);
		    m_pCollisionDlg->show();
			//m_pCollisionDlg->showMaximized();
		}
	}
}
void MainWindow::on_actionFull_Screen_triggered(bool on)
{
	//ui.actionFull_Screen->setEnabled(false);
	ui.actionFull_Screen->setChecked(false);

	if(m_pSlightAdjustDlg != NULL)
	{
		if (m_pSlightAdjustDlg->isMinimized())
		{
			m_pSlightAdjustDlg->showNormal();
		}
		else
		{
			m_pSlightAdjustDlg->Before();
			m_pSlightAdjustDlg->InitModelCombox(m_vAllNodeName);

			//QString strPosFile = need_filePath + QString::fromLocal8Bit("/chushiweizhi.csv");
			QString strPosFile = need_filePath + QString::fromLocal8Bit("/InitPosition.csv");
			ReadInitModelCenterFile(strPosFile);
			m_pSlightAdjustDlg->SetCurFileValue(m_vInitModelCenterFileValues);

			m_pSlightAdjustDlg->show();
			m_isFirstOpenAdjustDlg = false;
		}
	}

	//m_ShowTableDlg->SortTable();

	//m_ShowTableDlg->show();

	//if (on)
	//{
	//	setWindowState(windowState() | Qt::WindowFullScreen);
	//	hideDockWidgets();
	//}
	//else
	//{
	//	setWindowState(windowState() & ~Qt::WindowFullScreen);
	//	showDockWidgets();
	//}
}

void MainWindow::on_actionShadow_triggered(bool val)
{
	ui.actionTexture->setChecked(false);

	if(m_pReplaceWheelModelDlg !=  NULL)
	{

		if (m_pReplaceWheelModelDlg->isMinimized())
		{
			m_pReplaceWheelModelDlg->showNormal();
		}
		else
		{
			m_pReplaceWheelModelDlg->InitModelCombox(m_vAllNodeName);
			OnlyReadLMFile();
			OnlyReadJMFile();
			m_pReplaceWheelModelDlg->SetLMFileValues(m_vLMRFileValues);
			m_pReplaceWheelModelDlg->SetJMFileValues(m_vJMFileValues);
			m_pReplaceWheelModelDlg->show();
			m_isFirstOpenMoveRangeDlg = false;

		}



	}

}

void MainWindow::on_actionTexture_triggered(bool val)
{

	ui.actionTexture->setChecked(false);

	if(m_pAddModelDlg!=NULL)
	{

		if (m_pAddModelDlg->isMinimized())
		{
			m_pAddModelDlg->showNormal();
		}
		else
		{
			m_pAddModelDlg->InitModelCombox(m_vAllNodeName);
			m_pAddModelDlg->show();
		}

	}


	//	LoadAccumulateModelsFromTheFile();
}

void MainWindow::removeCallBack()
{
	//for (int i = 0; i < m_vMatrixHideCallback.size(); ++i)
	//{
	//	m_vMatrixHideCallback[i].first->removeUpdateCallback(m_vMatrixHideCallback[i].second);
	//}
	//m_vMatrixHideCallback.clear();

	//for (int i = 0; i < m_vMatrixInfoCallback.size(); ++i)
	//{
	//	m_vMatrixInfoCallback[i].first->removeUpdateCallback(m_vMatrixInfoCallback[i].second);
	//}
	//m_vMatrixInfoCallback.clear();

	//Sleep(2000);
}

void MainWindow::BeforeSettingNodeDlg(QString name)
{


	if(m_bIsFirstSTL == true)//添加根节点到父节点下拉框中
	{
		NodeDlg->setIsDel(false);
		NodeDlg->setCurNodeName(name);
		NodeDlg->InitFatherCombox();
		NodeDlg->initCombox();
		STL *scene = new STL(m_sceneModel->GetNodeScene(),m_sceneModel->GetNodeScene()->getName());
		m_vSTLs.push_back(scene);
		NodeDlg->show();
	}
	else
	{
		NodeDlg->setIsDel(false);
		NodeDlg->setCurNodeName(name);
		NodeDlg->setWindowTitle(name);
		NodeDlg->initCombox();
	}
}

void MainWindow::on_actionLight_triggered(bool val)//暂停
{
	ui.actionLight->setChecked(val);

	//管子与加工值显示动画
	for (size_t i= 0; i < m_vMatrixAnimation.size();++i)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vMatrixAnimation[i].first;
		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(matrix->getUpdateCallback());

		if (callback!=NULL)
		{
			callback->setPause(val);
		}
	}

	osg::ref_ptr<osg::PositionAttitudeTransform> LongStraightpipefatherTrans = m_sceneModel->getLongStraightPipeTrans();
	osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(LongStraightpipefatherTrans->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);
	}

	//模型
	for(size_t i=0;i<m_vNodes.size();i++)
	{
		osg::AnimationPathCallback* Movecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vNodes[i]->GetModelMoveMatrix()->getUpdateCallback());
		osg::AnimationPathCallback* Rotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vNodes[i]->GetModelRotateMatrix()->getUpdateCallback());

		osg::AnimationPathCallback* MidRotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vNodes[i]->GetModelMidRotateMatrix()->getUpdateCallback());
		osg::AnimationPathCallback* LeftRotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vNodes[i]->GetModelLeftRotateMatrix()->getUpdateCallback());

		std::vector<osg::ref_ptr<osg::MatrixTransform>>& trans = m_vNodes[i]->GetTransMatrix();
		if (!trans.empty())
		{
			for (size_t i = 0; i < trans.size();++i)
			{
				osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(trans[i]->getUpdateCallback());
				if (callback!=NULL)
				{
					callback->setPause(val);
				}
			}
		}

		osg::AnimationPathCallback* SecondRotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vNodes[i]->GetModelSecondRotateMatrix()->getUpdateCallback());

		if (Movecallback!=NULL)
		{
			Movecallback->setPause(val);
		}

		if (Rotatecallback!=NULL)
		{
			Rotatecallback->setPause(val);
		}

		if (MidRotatecallback!=NULL)
		{
			MidRotatecallback->setPause(val);
		}

		if (LeftRotatecallback!=NULL)
		{
			LeftRotatecallback->setPause(val);
		}

		if (SecondRotatecallback!=NULL)
		{
			SecondRotatecallback->setPause(val);
		}
	}

	if (val == true)//暫停
	{
		closeTimer();
	}
	else if (val == false)
	{
		// startTimer(dtime);
		ResumeAllModelColor();
	}


#if 0


	////左右辅推增加导夹
	//if(m_param.LmValueNum == 2)
	//{
	//	if(m_vRAddDaoJiaMatrix[0]!= NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[0]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[0] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[0]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//}
	//else if(m_param.LmValueNum == 3)
	//{
	//	if(m_vRAddDaoJiaMatrix[0] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[0]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vRAddDaoJiaMatrix[1] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[1]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[0] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[0]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[1] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[1]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}


	//}
	//else if(m_param.LmValueNum == 4)
	//{
	//	if(m_vRAddDaoJiaMatrix[0] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[0]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vRAddDaoJiaMatrix[1] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[1]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vRAddDaoJiaMatrix[2] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[2]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[0] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[0]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[1] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[1]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[2] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[2]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}


	//}
	//else if(m_param.LmValueNum == 5)
	//{
	//	if(m_vRAddDaoJiaMatrix[0] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[0]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vRAddDaoJiaMatrix[1] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[1]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vRAddDaoJiaMatrix[2] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[2]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vRAddDaoJiaMatrix[3] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vRAddDaoJiaMatrix[3]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[0] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[0]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[1] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[1]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[2] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[2]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}

	//	if(m_vLAddDaoJiaMatrix[3] != NULL)
	//	{
	//		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(m_vLAddDaoJiaMatrix[3]->getUpdateCallback());
	//		if (callback!=NULL)
	//		{
	//			callback->setPause(val);
	//		}
	//	}


	//}

	osg::ref_ptr<osg::MatrixTransform> xiaochematrix = m_sceneModel->getXiaoCheFatherMatrix();
	osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(xiaochematrix->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);

		double time = callback->getAnimationTime();
	}

	osg::ref_ptr<osg::MatrixTransform> chetoumatrix = m_sceneModel->getCheTouFatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(chetoumatrix->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);

		double time = callback->getAnimationTime();
	}

	/*osg::ref_ptr<osg::MatrixTransform> wanqumo2matrix = m_sceneModel->getWanQuMo2PosFatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(wanqumo2matrix->getUpdateCallback());
	if (callback!=NULL)
	{
	callback->setPause(val);
	}
	*/

	osg::ref_ptr<osg::MatrixTransform> jitouwanguan2matrix = m_sceneModel->getJiTouWanGuan2FatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(jitouwanguan2matrix->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);
	}

	osg::ref_ptr<osg::MatrixTransform> jitouwanguan1matrix = m_sceneModel->getJiTouWanGuan1FatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(jitouwanguan1matrix->getUpdateCallback());

	if (callback!=NULL)
	{
		callback->setPause(val);
	}


	osg::ref_ptr<osg::MatrixTransform> zhualiaomomatrix = m_sceneModel->getZhuLiaoMoFatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(zhualiaomomatrix->getUpdateCallback());	
	if (callback!=NULL)
	{
		callback->setPause(val);
	}

	//osg::ref_ptr<osg::MatrixTransform> allzhujia2matrix = m_sceneModel->getAllZhuJia2Matrix();
	//callback = dynamic_cast<osg::AnimationPathCallback*>(allzhujia2matrix->getUpdateCallback());
	//if (callback!=NULL)
	//{
	//	callback->setPause(val);
	//}

	osg::ref_ptr<osg::MatrixTransform> zhujia1matrix = m_sceneModel->getZhuJia1FatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(zhujia1matrix->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);
	}

	osg::ref_ptr<osg::MatrixTransform> daolun1matrix = m_sceneModel->getDaoLun1FatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(daolun1matrix->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);
	}

	osg::ref_ptr<osg::MatrixTransform> daolun2matrix = m_sceneModel->getDaoLun2FatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(daolun2matrix->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);
	}

	osg::ref_ptr<osg::MatrixTransform> zhunlunmatrix = m_sceneModel->getZhuLunFatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(zhunlunmatrix->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);
	}


	osg::ref_ptr<osg::MatrixTransform> jitoumozuo = m_sceneModel->getJiTouMoZuoFatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(jitoumozuo->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);
	}

	osg::ref_ptr<osg::MatrixTransform> jitoudizuo = m_sceneModel->getJiTouDiZuoFatherMatrix();
	callback = dynamic_cast<osg::AnimationPathCallback*>(jitoudizuo->getUpdateCallback());
	if (callback!=NULL)
	{
		callback->setPause(val);
	}

#endif
}

void MainWindow::on_actionLight_triggered2(bool val)
{
	//管子与加工值显示动画
	for (size_t i= 0; i < m_vMatrixAnimation.size();++i)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> matrix = m_vMatrixAnimation[i].first;
		osg::AnimationPathCallback* callback = dynamic_cast<osg::AnimationPathCallback*>(matrix->getUpdateCallback());

		if (callback!=NULL)
		{
			callback->setPause(val);
		}
	}


	//模型

	for(size_t i=0;i<m_vSTLs.size();i++)
	{
		osg::AnimationPathCallback* Movecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vSTLs[i]->GetModelMoveMatrix()->getUpdateCallback());
		osg::AnimationPathCallback* Rotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vSTLs[i]->GetModelRotateMatrix()->getUpdateCallback());
		osg::AnimationPathCallback* SecondRotatecallback = dynamic_cast<osg::AnimationPathCallback*>(m_vSTLs[i]->GetModelSecondRotateMatrix()->getUpdateCallback());

		if (Movecallback!=NULL)
		{
			Movecallback->setPause(val);
		}

		if (Rotatecallback!=NULL)
		{
			Rotatecallback->setPause(val);
		}


		if (SecondRotatecallback!=NULL)
		{
			SecondRotatecallback->setPause(val);
		}
	}
}

void MainWindow::on_actionHighLight_triggered(bool val)
{
	ui.actionHighLight->setChecked(false);

	if(m_pRotateTestDlg !=  NULL)
	{
		if (m_pRotateTestDlg->isMinimized())
		{
			m_pRotateTestDlg->showNormal();
		}
		else
		{
			m_pRotateTestDlg->InitModelCombox(m_vAllNodeName);
			m_pRotateTestDlg->show();
			m_isFirstOpenRotateTestDlg = false;
		}

	}

	/*m_pSettingAnimationDlg->SetTWLC1(m_vTwlc1values);
	m_pSettingAnimationDlg->SetTWLC2(m_vTwlc2values);
	m_pSettingAnimationDlg->SetParamValues(m_param);

	m_pSettingAnimationDlg->SetFileValues(m_vproductionvalues);
	m_pSettingAnimationDlg->InitModelCombox(name);
	m_pSettingAnimationDlg->show();
	m_pSettingAnimationDlg->setWindowModality(Qt::WindowModal);*/

}
void MainWindow::on_pushButtonClearLog_pressed()
{
	ui.textBrowserLog->clear();
}

void MainWindow::on_pushButtonSaveLog_pressed()
{
	QString logFile = QFileDialog::getSaveFileName(
		this,
		tr("Log file"),
		m_lastDirectory,
		"Log file (*.log)");

	if (logFile.isEmpty())
		return;

	m_lastDirectory = QFileInfo(logFile).absolutePath(); // save current directory

	QFile file(logFile);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		xLogHandler::getInstance()->slotReportError(tr("Can't save log file !!: %1").arg(logFile));
	}

	QTextStream out(&file);
	out << ui.textBrowserLog->document()->toPlainText();
	file.close();
}
void MainWindow::on_actionBackFace_triggered(bool val)
{
	//m_sceneView->setBackfaceEnabled(!val);
	ui.actionBackFace->setChecked(val);

	if (val)
	{
		if(m_pAdvanceUserDlg)
		{
			m_pAdvanceUserDlg->clearPassword();
			m_pAdvanceUserDlg->show();
		}
	}
	else
	{
		if(m_pAdvanceUserDlg)
		{
			m_pAdvanceUserDlg->clearPassword();
		}
		HideAction(false);
	}
}

void MainWindow::on_actionOptimize_triggered(bool val)
{
	m_isUseSoftKeyBoard = val;
	ui.actionOptimize->setChecked(val);

	m_pRotateTestDlg->SetIsUseSoftKeyboard(!m_isUseSoftKeyBoard);
	m_pSlightAdjustDlg->SetIsUseSoftKeyboard(!m_isUseSoftKeyBoard);
	m_pSettingNodeGroupDlg->SetIsUseSoftKeyboard(!m_isUseSoftKeyBoard);
	m_pReplaceWheelModelDlg->SetIsUseSoftKeyboard(!m_isUseSoftKeyBoard);
	m_pAddModelDlg->SetIsUseSoftKeyboard(!m_isUseSoftKeyBoard);
	m_pNormalUserWidget->SetIsUseSoftKeyboard(!m_isUseSoftKeyBoard);

	//if (m_objectLoader != NULL)
	//{
	//	m_objectLoader->slotSetOptimization(m_isOptimized);
	//}
}

void MainWindow::on_actionEnableNode_triggered()
{
	//qDebug("on_actionEnableNode_activated");
	m_pSpeedDlg->show();

	return;
	QModelIndex index = m_treeView->currentIndex();

	if ( !index.isValid() )
		return;

	m_treeModel->setEnableIndex(index,true);
	m_treeView->update();
}

void MainWindow::on_actionDisableNode_triggered()
{
	bool detectedBak = m_IsDetected;
	m_IsDetected = true;
	m_bIsAutoPlan = true;

	on_actionUnload_triggered();

	m_IsDetected = detectedBak;
	m_bIsAutoPlan = false;  // 判断是否是自动优化

	return;

	QModelIndex index = m_treeView->currentIndex();

	if ( !index.isValid() )
		return;

	m_treeModel->setEnableIndex(index,false);
	m_treeView->update();
}

void MainWindow::on_actionExpandTree_triggered()
{
	QModelIndex index = m_treeView->currentIndex();

	if ( !index.isValid() )
		return;

	expandReccursively(index);
	m_treeView->resizeColumnToContents(0);
}

void MainWindow::on_actionCollapseTree_triggered()
{
	QModelIndex index = m_treeView->currentIndex();

	if ( !index.isValid() )
		return;

	collapseReccursively(index);
}

void MainWindow::expandReccursively(const QModelIndex &index)
{
	if ( !index.isValid() )
		return;

	for (int i = 0; i < m_treeModel->rowCount(index); i++)
	{
		QModelIndex ind = index.child(i,0);
		expandReccursively(ind);
	}

	m_treeView->expand(index);
}

void MainWindow::collapseReccursively(const QModelIndex &index)
{
	if ( !index.isValid() )
		return;

	m_treeView->collapse(index);

	for (int i = 0; i < m_treeModel->rowCount(index); i++)
	{
		QModelIndex ind = index.child(i,0);
		collapseReccursively(ind);
	}
}
void MainWindow::on_actionCenterOnSelection_triggered()
{
	QModelIndex index = m_treeView->currentIndex();

	if ( !index.isValid() )
		return;

	m_sceneView->centerOnNode( reinterpret_cast<osg::Node*>( index.internalPointer() ) );

	// TODO !!!!
}

void MainWindow::slotTreeViewCustomContextMenuRequested(const QPoint & pos)
{
	QModelIndex index = m_treeView->indexAt(pos);

	if ( !index.isValid() )
		return;

	QMenu menu(this);

	menu.addAction(ui.actionEnableNode);
	menu.addAction(ui.actionDisableNode);
	menu.addAction(ui.actionCenterOnSelection);
	menu.insertSeparator(0);
	//   menu.addAction(ui.actionExpandTree);
	//  menu.addAction(ui.actionCollapseTree);

	menu.exec( m_treeView->viewport()->mapToGlobal(pos) );
}

void MainWindow::showDockWidgets()
{
	QMainWindow::menuBar()->show();
	//QMainWindow::statusBar()->show();

	// restore the dock widget state
	foreach(QWidget * widget, m_dockWidgets)
	{
		widget->show();
	}
	m_dockWidgets.clear();
}

void MainWindow::hideDockWidgets()
{
	QMainWindow::statusBar()->hide();
	QMainWindow::menuBar()->hide();

	// save the visible dock widget
	foreach(QWidget *widget, findChildren<QDockWidget*>())
	{
		if (widget->isVisible())
		{
			m_dockWidgets << widget;
			widget->hide();
		}
	}
}

void MainWindow::slotTreeNodeSelected(const QModelIndex &index)
{
	if (index.isValid())
	{
		m_sceneView->highlight((osg::Node *)index.internalPointer());
		// display Properties
		//m_pSceneView->displayProperties((osg::Node *)index.internalPointer());
		m_propertyWidget->displayProperties((osg::Node *)index.internalPointer());
	}
}

void MainWindow::slotSelectTreeItem(osg::Drawable *pDrawable)
{
	QModelIndex parentIndex = m_treeModel->searchForNode(pDrawable->getParent(0));

	if (parentIndex.isValid())
	{
		m_treeView->setCurrentIndex(parentIndex);
		m_treeView->scrollTo(parentIndex, QAbstractItemView::EnsureVisible);
		m_treeView->resizeColumnToContents(0);
		slotTreeNodeSelected(parentIndex);
	}
}

void MainWindow::resetViews(bool allClear)
{
	// clear current bookmark
	//ui->widgetBookmarkManager->clearBookmark();

	m_sceneView->resetSelection();

	// reset 3d view
	if (allClear)
	{
		m_treeModel->setNode(NULL);
		m_sceneModel->setData(NULL);
	}
}

void MainWindow::slotSettingNodeFinishedMy(osg::Node* node,const QString& name,bool IsFirst)
{
	m_sceneModel->setData(node);

	// update the window title
	setWindowTitle(m_appName + " " + m_version + " - " + QFileInfo(name).fileName());
	addRecentlyOpenedFile(name, m_recentFiles);

	// reset the cursor
	QApplication::restoreOverrideCursor();

	// enable actions
	enableActions(true);

	m_treeModel->setNode(node);

	// arrange display
	//QModelIndex i = m_treeModel->index(0,0, m_treeView->rootIndex());
	//m_treeView->setExpanded(i,true);
	//m_treeView->resizeColumnToContents(0);//自动显示水平滚动条

	// update completer for find
	delete m_completerSearch;

	xFindNameListVisitor visit;
	node->accept(visit);

	QStringList nameList = visit.getNameList();
	nameList.removeDuplicates();

	m_completerSearch = new QCompleter(nameList, this);
	m_lineEditSearch->setCompleter(m_completerSearch);
	if (m_isCaseSensitive)
		m_completerSearch->setCaseSensitivity(Qt::CaseSensitive);
	else
		m_completerSearch->setCaseSensitivity(Qt::CaseInsensitive);

	// display stats
	m_propertyWidget->displayProperties(node);

	// init current index
	m_currSearchIndex = QModelIndex();
	ui.actionQuit->setEnabled(true);
}

void MainWindow::slotPrintToLog(const QString & mess)
{
	ui.textBrowserLog->append(mess);
}

void MainWindow::slotPrintToLog(const QStringList & mess)
{
	ui.textBrowserLog->append(mess.join("<br>"));
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == m_sceneView)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		keyPressEvent(keyEvent);

		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		mousePressEvent(mouseEvent);
	}

	return false;
}

bool MainWindow::CheckAdminCode(std::string code)
{
	char filename[256] = "";  
	QString fileName = QCoreApplication::applicationDirPath();
	strcpy(filename,fileName.toStdString().c_str());

	strcat(filename, "\\dind77844plde~ni3dffdb163ef");
	BOOL isexist = ExistFile(filename);
	FILE *m_file = NULL;
	if(!isexist)
	{
		//m_file = fopen(filename,"w");
		//fputs("0",m_file);
		//fclose(m_file);
		//SetFileAttributesA(filename,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
		return false;
	}
	else
	{
		SetFileAttributesA(filename,FILE_ATTRIBUTE_NORMAL);
		char data[256];
		m_file = fopen(filename,"r");
		fgets(data, 256, m_file);
		std::string readCode(data);
		if(readCode != code) 
		{
			fclose(m_file);
			SetFileAttributesA(filename,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
			return false;
		}
		else
		{
			fclose(m_file);
			SetFileAttributesA(filename,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
			return true;
		}
	}

	return false;
}

BOOL  ExistFile(char   *szName)
{
	HANDLE   hFile;
	hFile   =   CreateFileA(szName,   0,   0,   0,   OPEN_EXISTING,   0,   NULL);
	if(GetLastError()   ==   ERROR_FILE_NOT_FOUND)
	{
		CloseHandle(hFile);
		return   FALSE;
	}
	CloseHandle(hFile);
	return   TRUE;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	// 设置用户权限快捷键
	if (event->key() == Qt::Key_Q  && event->modifiers() == (Qt::AltModifier | Qt::ControlModifier))
	{
		m_pAdvanceUserDlg->clearPassword();
		m_pAdvanceUserDlg->show();
		std::string password;
		if (m_pAdvanceUserDlg->exec()==QDialog::Accepted)
		{
			password = m_pAdvanceUserDlg->GetPassword().toStdString();
		}

		if(CheckAdminCode(password))
		{
			HideAction(true);
			//	QMessageBox::information(NULL, "info", "this is admin !", QMessageBox::Yes);
			LogInfo("this is admin !");
		}
		else
		{
			HideAction(false);
			//	QMessageBox::information(NULL, "info", "this is normal !", QMessageBox::Yes);
			LogInfo("this is normal !");
		}

		return;
	}

	//if(event->key() == Qt::Key_Escape)
	//{
	//	if (bool(windowState() & Qt::WindowFullScreen) == true) // fullscreen mode => quit fullscreen
	//	{
	//		on_actionFull_Screen_triggered(false);
	//		ui.actionFull_Screen->setChecked(false);
	//	}

	//	return;
	//}

	bool shift = event->modifiers() & Qt::ShiftModifier;
	bool ctrl = event->modifiers() & Qt::ControlModifier;
	bool alt = event->modifiers() & Qt::AltModifier;

	if (shift)
	{
		//QMainWindow::statusBar()->showMessage(tr("Use Shift+Left Click to recenter view"), 5000);
	}
	else if (ctrl)
	{
		//QMainWindow::statusBar()->showMessage(tr("Use Ctrl+Left Click to select element"), 5000);
	}
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	//setKeyboardModifiers( event );
	//this->getEventQueue()->mouseMotion(event->x(), event->y());
	////if(nullptr != m_pMainWnd)
	////{
	if (event->button() == Qt::LeftButton)
	{
		auto x = event->x();
		auto y = event->y();
		int iWindowHeight = this->height();
		//坐标值转换
		y = iWindowHeight - y;
		int z = 0;
		osg::Camera* camera = m_sceneView->getOsgView()->getCamera();

		auto matViewMatrix = camera->getViewMatrix();
		auto matProjectionMatrix = camera->getProjectionMatrix();
		auto wndMatrix = camera->getViewport()->computeWindowMatrix();
		osg::Matrix MVPW = matViewMatrix * matProjectionMatrix * wndMatrix;
		osg::Matrix inverseMVPW = osg::Matrix::inverse(MVPW);
		osg::Vec3 mouseWorld = osg::Vec3(x,y, 0) * inverseMVPW;

		QString s;
		s = s.fromLocal8Bit("当前位置：");
		s += QString("%1,%2,%3").arg(mouseWorld.x()).arg(mouseWorld.y()).arg(mouseWorld.z());
		QMainWindow::statusBar()->clearMessage();
		//QMainWindow::statusBar()->showMessage(s, 6000);
	}
	else if (event->button() == Qt::RightButton)
	{
		return;
	}


	//auto pEvent = MessageEvent::CreateInstance(1);
	//pEvent->m_sMsg = s;
	// qApp->postEvent(m_pMainWnd, pEvent);
	//}
}

void MainWindow::slotChangeAspectRatio(const QSize & sz)
{
	if (sz.height())
	{
		float ratio = sz.width() / (float)sz.height();
		m_aspectRatioLabel->setText(tr("View aspect ratio: %1").arg(QString::number(ratio,'g',2)));
	}
}

void MainWindow::slotSettingNodeFinished(osg::Node *node,const QString& name)
{
	/*m_sceneModel->setNodeType(m_setNodeDlg->GetNodeType());
	m_sceneModel->setNodeOpeType(m_setNodeDlg->GetOpeType());
	m_sceneModel->serFatherNodeName(m_setNodeDlg->getFatherNodeName());
	m_sceneModel->setAddNodeName(m_setNodeDlg->getAddNodeName());*/

	//if(node->getName() == "xiaoche.stl")
	//{
	//	v3Center = node->getBound().center()*osg::computeLocalToWorld((node)->getParentalNodePaths()[0]);
	//	v3Center -= osg::Vec3(0.0,45.0,60.0);

	//}

	/*std::string stdstrFileName = m_currFile.toStdString(); 
	node = createQuade(stdstrFileName);*/

	m_sceneModel->setData(node);

	// update the window title
	setWindowTitle(m_appName + " " + m_version + " - " + QFileInfo(name).fileName());
	addRecentlyOpenedFile(name, m_recentFiles);

	// reset the cursor
	QApplication::restoreOverrideCursor();

	// enable actions
	enableActions(true);

	m_treeModel->setNode(node);

	// arrange display
	//QModelIndex i = m_treeModel->index(0,0, m_treeView->rootIndex());
	//m_treeView->setExpanded(i,true);
	//m_treeView->resizeColumnToContents(0);//自动显示水平滚动条

	// update completer for find
	delete m_completerSearch;

	xFindNameListVisitor visit;
	node->accept(visit);

	QStringList nameList = visit.getNameList();
	nameList.removeDuplicates();

	m_completerSearch = new QCompleter(nameList, this);
	m_lineEditSearch->setCompleter(m_completerSearch);
	if (m_isCaseSensitive)
		m_completerSearch->setCaseSensitivity(Qt::CaseSensitive);
	else
		m_completerSearch->setCaseSensitivity(Qt::CaseInsensitive);

	// display stats
	m_propertyWidget->displayProperties(node);

	// init current index
	m_currSearchIndex = QModelIndex();
	ui.actionQuit->setEnabled(true);
}

void MainWindow::slotSettingNodeFinishedNew(int type0,int type1, const QString& name0,
	const QString& name1,
	osg::ref_ptr<osg::Node> node,osg::ref_ptr<osg::Material> material,const QString& name)
{
	//m_sceneModel->setNodeType(type0);
	//m_sceneModel->setNodeOpeType(type1);
	//m_sceneModel->serFatherNodeName(name0);
	//m_sceneModel->setAddNodeName(name1);

	/*std::string stdstrFileName = m_currFile.toStdString(); 
	node = createQuade(stdstrFileName);*/
	/*std::string tmps = name.toStdString();
	const char* resultName = tmps.c_str();
	node->setName(resultName);*/
	//m_sceneModel->setData(node);
	osg::Vec3 dcenter = node->getBound().center()*osg::computeLocalToWorld(node->getParentalNodePaths()[0]);
	STL *curstl = new STL(node,material,node->getName());

	curstl->SetSTLColor(m_objectLoader->GetLoadModelColor());
	curstl->SetModelCenter(dcenter);//随模型运动的中心
	curstl->SetModelOriginalCenter(dcenter);//原始中心
	curstl->SetModelCopyFatherCenter(dcenter);
	curstl->SetIsNew(false);
	curstl->SetIsAtLoadModel(true);
	curstl->SetLMShow(true);
	curstl->SetCanChanged(0);
	curstl->SetAsSpecialReplaceModel(0);

	InitSTLSide(curstl);

	// 由各模型平行结构改成父子节点结构，先解除挂在根节点下的模型
	//m_sceneModel->GetNodeScene()->addChild(curstl->GetModelMatrix());

	m_vSTLs.push_back(curstl);

	//深拷贝一份，储存一份文件的节点指针
	osg::ref_ptr<osg::Node> deepCopy = dynamic_cast<osg::Node*>(node->clone(osg::CopyOp::DEEP_COPY_ALL));
	STL *clonecurstl = new STL(deepCopy,material,deepCopy->getName());
	m_vOriginalSTLs.push_back(clonecurstl);

	// update the window title
	setWindowTitle(m_appName + " " + m_version + " - " + QFileInfo(name).fileName());
	addRecentlyOpenedFile(name, m_recentFiles);

	// reset the cursor
	QApplication::restoreOverrideCursor();

	// enable actions
	enableActions(true);

	m_treeModel->setNode(node);

	// arrange display
	QModelIndex i = m_treeModel->index(0,0, m_treeView->rootIndex());
	m_treeView->setExpanded(i,true);
	//m_treeView->resizeColumnToContents(0);//自动显示水平滚动条

	// update completer for find
	delete m_completerSearch;

	xFindNameListVisitor visit;
	node->accept(visit);

	QStringList nameList = visit.getNameList();
	nameList.removeDuplicates();

	m_completerSearch = new QCompleter(nameList, this);
	m_lineEditSearch->setCompleter(m_completerSearch);
	if (m_isCaseSensitive)
		m_completerSearch->setCaseSensitivity(Qt::CaseSensitive);
	else
		m_completerSearch->setCaseSensitivity(Qt::CaseInsensitive);

	// display stats
	m_propertyWidget->displayProperties(node);

	// init current index
	m_currSearchIndex = QModelIndex();
	ui.actionQuit->setEnabled(true);
}

void MainWindow::slotSearchCaseSensitiveToggled(bool checked)
{
	m_isCaseSensitive = checked;

	if (!m_completerSearch)
		return;

	if (m_isCaseSensitive)
		m_completerSearch->setCaseSensitivity(Qt::CaseSensitive);
	else
		m_completerSearch->setCaseSensitivity(Qt::CaseInsensitive);
}

void MainWindow::slotEditSearchReturnPressed()
{
	QString searchText = m_lineEditSearch->text();

	if  (!m_currSearchIndex.isValid())
	{
		m_currSearchIndex = m_treeModel->index(0, 0, QModelIndex());
		search(m_currSearchIndex,searchText);
	}
	else
	{
		// search for next valid sibling or root
		QModelIndex node = m_currSearchIndex;
		QModelIndex sibling;
		bool found = false;
		while (!found)
		{
			sibling = node.sibling(node.row() + 1,node.column());
			while (!sibling.isValid())
			{
				node = node.parent();
				if (!node.isValid())
				{
					found = true;
					break;
				}
				if (node != m_treeModel->index(0, 0, QModelIndex()))
					sibling = node.sibling(node.row() + 1,node.column());
				else
					sibling = m_treeModel->index(0, 0, QModelIndex());
			}
			if (found)
				break;
			found = search(sibling,searchText);
			node = sibling;
		}
	}
}

bool MainWindow::search(const QModelIndex &index,const QString &name)
{
	m_currSearchIndex = m_treeModel->searchForName(name,index);
	if (m_currSearchIndex.isValid())
	{
		m_treeView->setCurrentIndex(m_currSearchIndex);
		m_treeView->scrollTo(m_currSearchIndex,QAbstractItemView::EnsureVisible);
		m_treeView->resizeColumnToContents(0);

		//widgetProperties->displayProperties(curritf);

		slotTreeNodeSelected(m_currSearchIndex);
		m_sceneView->centerOnNode(reinterpret_cast<osg::Node*>(m_currSearchIndex.internalPointer()));
		return true;
	}
	return false;
}

