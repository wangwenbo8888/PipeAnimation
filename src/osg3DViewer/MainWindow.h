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

#ifndef _OSGVIEWER_MAINWINDOW_H_
#define _OSGVIEWER_MAINWINDOW_H_

#include <QMouseEvent>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QModelIndex>
#include <osg/PositionAttitudeTransform>
#include <osg/Node>
#include <osg/ClipNode>
#include <QTimer>
#include <osgText/Font>
#include <osgText/Text>
#include <osgFX/Outline>
#include <osg/Material>
#include <osg/ClipPlane>
#include <QPushButton>

//#include "mmtimer.h"

class xSceneView;
class xSceneModel;
class xTreeModel;
class xTreeView;
class xPropertyWidget;
class xObjectLoader;
class QCompleter;
class xSearchLineEdit;

#include "ui_MainWindow.h"
#include "xSettingNode.h"

#include "NormalTree.h"

#include "Vector3.h"
#include "NodeTransFormCallBack.h"
#include <string.h>
#include "xCollisionDetectionDlg.h"
#include "xSettingAnimation.h"
#include "Pipe.h"
#include "CreatePipe.h"
#include "xShowTableDlg.h"
#include "xSettingNodeGroup.h"
#include "xMoveRange.h"
#include "xSlightAdjust.h"
#include "xHUDAxis.h"
#include "xRotateTest.h"
#include "xModelGroup.h"
#include "Table.h"
#include "xReplaceWheelModel.h"
#include "xAddModelDlg.h"
#include "String2Formula.h"
#include "xModifyLMTable.h"
#include "xNormalUserWidget.h"
#include "xAdvanceUserDlg.h"
#include "xSettingColor.h"
//#include "language.h"

#include "xLanguageDlg.h"
#include "xSpeedDlg.h"
#include "xOptimizeDlg.h"

#include <osgDragger/DraggerManager>
#include <osgDragger/ClearDepth>
#include <osgDragger/HudCamera>
#include <osgDragger/MyDragger>
#include <osgDragger/ShowBound>
#include <Utils/Utils>
#include <osgVirtualAssembly/CustomDraggerManager>
#include <osgCollisionDetection/CollisionInterface>
#include <osgCollisionDetection/DebugDraw>

//无用,实现可以删除
struct YBCExt
{
	float myb;
	float mbb;
	float mcb;
	float myt;
	float mz;
	float myth;
	int lc;
	int mmd;
	int mty;
	int mtb;
	float jy;
	float twy;
	float twbj;
	float twc;

	//getType();
};

struct YBCExt2Base//参数
{
	float zgp;
	float dywp;
	float wqmr;
	float mjl;
	float zxmr;
	float dmal;
	float wtwp;
};

struct YBCExt2
{
	float yd;
	std::vector<float> zm;
	std::vector<float> dm;
	std::vector<YBCExt2Base> ptvalue;
};

struct TWLC
{
	float ftwy;
	float ftwbj;
	float ftwc;
	float ftwb;
};

struct AccumulateModel
{
	QString strNewModelName;
	QString strCopyModelName;
	int iUpUints;
	int iJTNth;
};

//struct LMR
//{
//	QString strModelName;
//	float fLmr;
//	float fupLen;
//	float fdownLen;
//	float ftuLen;  
//	int isShow;
//
//};

struct InitModelCenter
{
	QString ModelName;  // 模型名
	int ModelLayer;  //模层
	osg::Vec3 ModelCenter;   // 初始位置
};

struct OtherFileValue
{
	QString strFileName;
	size_t irow;
	int icolumn;
	QString _operator;
	float fcondionvalue;
	QString strIndexVarName;

	OtherFileValue()
	{
		strFileName = " ";
		irow = -1;
		icolumn = -1;
		_operator = " ";
		fcondionvalue = -10000;
		strIndexVarName = " ";
	}
};

struct Gcode
{
	QString strGroupName;
	QString strGCode;
	int icode;
	int iSpecialMove; // 1 机头弯管相对转动，2 机头弯管绝对转动，3 相对机头弯管运动
	int iMoveType;//平移0，旋转1，2置管
	int iAxis;//x0,y1,z2,沿机头弯管运动
	int irelative; //相对1,决对0
	osg::Vec3 vecRotateCenter;
	int iDirectly;
	osg::Vec3 vecMoveToPos;//旋转的时候取对应轴的坐标值
	OtherFileValue other;
	int iDir;//指定与默认的正方向（x,y,z轴）相同还是相反
};
struct OtherFileWithCondition
{
	QString strFileName;
	int irow;
	int icolumn;
	QString _operator;
	float fcondionvalue;
	OtherFileWithCondition()
	{
		strFileName = " ";
		irow = -1;
		icolumn = -1;
		_operator = " ";
		fcondionvalue = -10000;
	}
};

struct OtherFileValueNew
{
	QString strFileName;
	QString strRow;
	QString strColumn;
	QString strOperator;

	OtherFileValueNew()
	{
		strFileName = " ";
		strRow = " ";
		strColumn = " ";
		strOperator = " ";
	}
};

struct OnlyReadProcessNew
{
	QString strStep;
	QString strCode;
	QString strP1;
	QString strP2;
	QString strRoll;
	QString strPipe;
	QString strShape;
	QString strCurveR;

	OnlyReadProcessNew()
	{
		strStep = " ";
		strCode = " ";
		strP1 = " ";
		strP2 = " ";
		strRoll = " ";
		strPipe = " ";
		strShape = " ";
		strCurveR = " ";
	}
};

struct OnlyReadProcess
{
	int istep;//当前行索引
	QString strCode;
	int iDirectly;//直接间接指定、其他行索引
	OtherFileValue _valueFile;
	float _fvalue;
	//OtherFileWithCondition _valueFileCondition;
	int iPipeRotate;//0,1
	int iCreatPipe;//0,1
	int iCurveOrStraight;//0,1
	OtherFileValue _curveRFile;
	float fcurveR;
	int iNegative;//0,1
	bool isInLoop;
	QString _strOperator;
	QString _strIndexVar;
	QString _strColorValue;

	OnlyReadProcess()//初始化
	{
		istep = -1;
		strCode = " ";
		iDirectly = -1;
		_fvalue = -10000;
		iPipeRotate = -1;
		iCreatPipe = -1;
		iCurveOrStraight = -1;
		fcurveR = -10000;
		iNegative = -1;
		isInLoop = false;
		_strColorValue = " ";
	}
};

struct Process
{
	int istep;//当前行索引
	QString strCode;
	float fvalue;//c单元格的数据
	int iDirectly;//直接间接指定、其他行索引	
	int iPipeRotate;
	int iCreatPipe;
	int iCurveOrStraight;
	float fcurveR;
	int iHeader;  // 机头编号
	int iTip;     // 长直管的左端还是右端加工
	int iNegative;
	int iNthCureve;
	int iColor;
	int iTwColor;
	float fDir;   // 判断当前是左侧加工还是右侧加工，0 右侧加工，1 左侧加工
	
	int iCJstep;
	bool bIsLoop;
	OtherFileValue _other;
	std::string strProcess;

	Process()//初始化
	{
		bIsLoop = false;
		istep = -1;
		strCode = " ";
		iDirectly = -1;
		fvalue = -10000;
		iPipeRotate = -1;
		iCreatPipe = -1;
		iCurveOrStraight = -1;
		fcurveR = -10000;
		iHeader = 1; // 默认为一号机头,机头编号从1开始
		iTip = 0;    // 默认为长直管右端加工，视线顺着坐标轴正向（x,y,z轴）
		iNegative = -1;
		iCJstep = -1;
		iNthCureve = -1;
		iColor = -1;
		iTwColor = -1;
		fDir = -1.0;
		strProcess = "";
	}
};

struct MovePos
{
	osg::Vec3 vecMovePos;
};

struct MultiLmLoopData
{
	// 是否已经初始化
	bool isInitied;
	int index; // 弯索引

	std::vector<float> vfMyt;
	std::vector<float> vfMyth;
    std::vector<float> vfBtm;
	std::vector<float> vfCtm;
	std::vector<float> vfMz;
	std::vector<int> vDz;
	std::vector<int> vLc;
	std::vector<int> vMmd;
	std::vector<std::pair<int,double>> vLr;
};

enum columnType
{
	Data,
	File,
	Var,    //变量
    Assign, // 变量赋值
	Formula
};

struct FileColumn
{
	std::string str;
	columnType type;
};



//对象高亮颜色
const osg::Vec4 HIGHTLIGHT_COLOR = osg::Vec4(0.8, 0.0, 0.0, 1.0);

#define MYEPSILON 1.1920929e-007
#define MYDEG_TO_RAD 0.017453292
#define TOL  0.001
using namespace std;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void SetSTLNodeHideByName(QString str);
	// 把原来的模型结点连接新的特殊模型结点，这类特殊模型软件里只有一个实例
	// 这类特殊模型位置和尺寸都不会发生改变
	void ReplaceBySpecialModel(QString strCurName,QString strSpecialName);

	// 把原来的模型节点连接新的特殊模型节点，这类特殊模型软件里可以有多个实例
	// 这类特殊模型位置会发生改变，尺寸不会发生改变
	void ReplaceByNewInstance(QString strCurName,QString strSpecialName);

	void ReadAsSpecialReplaceModelFile();
	int GetTypeModelIsReplaceSpecialByName(QString strName);
	void RebuildParamFile(std::vector<LMR> vNormalUserLMValues);
	void RefreshLMrByParamFile(std::vector<LMR>& vLMValues);

	// 打开软件的时候刷新机头1轮模文件里轮模半径
	void RefreshJT1LMValueFile();

	void Trim(std::string &str);

	void GetWholePath(char* path);

	// 软件开始运行的时候刷新普通用户设置，就是显示普通用户界面，然后关闭
	void RefreshNormalUserSetting();

	// 模型位置微调的时候设置当前激活的模型
	void SetCurrentModel(const QString& name);

	// 设置模型当前位置
	void SetCurrentPos(osg::Vec3 pos);

	// 获取被替换模型初始位置
	osg::Vec3 GetReplacedModelInitPos(QString name,int layer);

	int GetLMLayerByName(QString str);
	std::map<QString,std::vector<QString>> GetSortNameToJT1LMNamesMap();
	std::map<QString,std::vector<QString>> GetSortNameToJT1JMNamesMap();
	std::map<int,std::vector<QString>> ReadSortFile(QString strPathFile);
	std::map<int,std::vector<QString>> ReadLMSortFile(QString strPathFile);
	
	void RebuildProductionTable();
	std::map<QString,QString> GetModelToType();
	void ReadModelColorFileValue();
	void ReBuildLanguageDefaultFile();
	QString GetDefaultLanguage();
	void SetQStringLen(QString &str,int len);
	void ReadModelDiplayNameFile();
	void SetViewLookAtByFileValue();
	void SetDefaultLanguage();
	void SetQToolBarSize(int wide,int height);
	float GetBiggestLMRadius(int iNthJT);
	void BuildLayerToRadiusFile(std::map<int,float> MapJTLayerToR,int iNth);
	void BuildSumOfTwoRadius(std::map<int,float> MapJTLayerToR,int iNth);
	void BuildSemidiameterFile(std::map<int,float> MapJTLayerToR,int iNth);//半径差文件
	void SetAllJTLayerToRadiusAtLoad(std::map<int,std::map<QString,std::vector<QString>>> values);
	void SetJT1LayerValues(std::vector<STL*> stl);
	void SetJT2LayerValues(std::vector<STL*> stl);
	float GetJT1CurLayerLMRadius(int layer);
	std::vector<STL*> GetAllSTLs();
	Param GetParamFileValues();
	void GetLayerToR(std::vector<LMR> values);

	void InitNormalUserPadWidget();
	void HideAction(bool var);
		
	void setisStart(bool val){ m_isStart = val;}
	bool GetIsStart(){return m_isStart;}

	void ReadCollisionDectionFile();
	void ReadAlterModelName();
	void SetAlterSTL();
	void ReadCollisionDectionLanguageFile();
		
	void ReadLanguageFile();
	void SetLanguageInMainWindow(std::vector<QString> vCurWords);

	void ResetAllModelAbsPos();

	void SetAdjusting(bool b)
	{
		m_bIsAdjusting = b;
	}

	bool isAdjusting()
	{
		return m_bIsAdjusting;
	}

	// 重新设置虚拟节点初始位置
	void ResetAllNodeInitPos();

	void SetGroupToSTLS();
	void ReadLMRFILE();//未用
	void AddvNeedRebuildLMName(QString str);
	void ReBuildLm();

	void UpdateTWLCINDEX(QString &Twlc1,QString stroperator,float fvalue);

	void UpdateTWLCINDEX3Value(QString &Twlc1,QString stroperator,QString v0,QString v1);

	void UpdateIndexValue(QString& index,int value);

	int GetTWLCNUM(int index);
	void GetValueInGroupSettingFileByGcode(const QString& strGcode,float& fvalue);
	std::map<int,std::vector<OnlyReadProcess>>::iterator GetAimReadProcessByAimStep(int istep);
	std::vector<Process> GetAimStepReadProcess(int istep,int index);
	void AllSteps();
	//void AllStepsNew();
	void OnlyReadProcessFile();
	//void OnlyReadProcessFileNew();
	void AddTheEedProcess();
	bool IsParamInInIFile(std::string str,float& fvalue);
	//
	bool isDivideZero(float fvalue);
	columnType GetStringType(std::string str);//1 为数值，2为文件，3为公式，4变量
	bool GetIsDoCJ(std::string symbol,float fFormer,float fLaster);//返回是否跳转
	void GetTwoFormulaBySymbol(std::string& former,std::string& laster,std::string symbol,std::string str);//比较符两侧的公式
	void GetCellValue(FileColumn fc,float& fvalue);//获取单元格数据，单元格可以为  数值，文件，公式
	void GetValueByTWLCNUMFileRowAndColumnNew(OtherFileValue other,float& fvalue,int twlcIndex); 
	void GetValueBySumOfTwoRadiusFile(OtherFileValue other,float& fvalue,int iNthJT);//两模层和
	void GetValueByMinusOfTwoRadiusFile(OtherFileValue other,float& fvalue,int iNthJT);
	void GetResultValueByAllFile(NodeNew& nodenew);
	bool GetFileValue(std::string str,float& tmp);//通过文件行列获取值，行可以为指定变量
	std::string GetSymbol(std::string str);//获取比较符
	std::string GetOperator(std::string str);////获取比较符
	void DivideString(std::string str,std::vector<std::string>& res);//按运算符分隔
	void DivideStringByDouHao(std::string str,std::vector<std::string>& res);//按逗号分隔
	bool IsNumber(std::string num);

	bool isCSVFileName(std::string str);
	bool isHasKuoHao1(std::string str);
	std::map<int,std::vector<std::vector<FileColumn>>>::iterator GetStartIt(int index);
	std::map<int,std::vector<std::vector<FileColumn>>>::iterator GetAimItByAimStep(int istep);
	void GetResultValue(NodeNew& nodenew);


	void AnalyzeCurRow(std::vector<std::string> res);//res当前行的字符串，返回按列数存的数组
	std::vector<std::pair<int,std::string> > GetFormulaString(std::vector<pair<bool,std::string> >& vres);
	std::vector<std::pair<int,std::string> > GetFileNameString(std::vector<pair<bool,std::string> >& vres);
    std::vector<std::pair<int,std::string> > GetVarString(std::vector<pair<bool,std::string> >& vres);

	void AnalyzeGMCode(std::vector<string>	res);
	void AnalyzeCJCode(std::vector<string>	res);
	void AnalyzeLOOP_SorE(std::vector<string>	res);

	void BuildNormalUserFile(std::map<int,std::map<QString,std::vector<QString>>> values);
	void BuildOneJTFile(int num,std::map<QString,std::vector<QString>> SideToModels);
	void BuildOneSideFile(QString strFileName,std::vector<QString> vValues,QString strType,int num);
	void SetMapToMapTypeToSTLs(std::map<int,std::map<QString,std::vector<QString>>> values);
	void BuildNormalUserJMFile(QString strFileName,int num);
	void BuildNormalUserLMFile(QString strFileName,int num);

	// 根据用户配置，将一个机头对应的左，中，右模型写进配置文件
	void BuildSideConfigFile(int num,const std::map<QString,std::vector<QString>>& SideToModels);

	// 读边模型设置文件
	void ReadSideConfigFile(int num,std::map<QString,std::vector<QString>>& SideToModels);

	void ReadAllJTSideConfigFile();

	std::map<QString,int> ReadModelToLayerFileValues(QString filepath);
	std::vector<JM> ReadJT1LeftFileValues(QString filepath);
	std::vector<JM> ReadJT1RightFileValues(QString filepath);
	std::vector<LMR> ReadJT1CenterFileValues(QString filepath);


	void BuildModifyLMTableFile(std::vector<AccumulateModel> vAccumulateNewModelFileValues);//无用

	void ReplaceModelByLmTable(LMR lmValue,STL* replaceSTL);
	void ReplaceModelByJmTable(JM lmValue,STL* replaceSTL);
	void GetLoopStartAndEnd();
	void ResumeAllModelColor();
	void SetColor(QString name,osg::Vec4f color);
	void ChangeMutiModelColor(std::vector<QString> vmodelname);
	// void On_modelColor_change(const QString& name);
	void InitProductionTable();
	void InitTwTable();
	void SetTwValue();
	void UpdateModelColor(osg::Vec4 color,STL *curSTL);
	void UpdataTableRow();
	void CheTouFun(int i,double STime,double ETime,float frotate_z);
	void JiTouMoZuoAnimationFun(int i,double STime,double ETime,float fmove_x);
	void JiTouWanGuan2AnimationFun(int i,double STime,double ETime,float rotate_y,float dir = -1);//-1:顺时针
	void JiTouWanGuan1AnimationFun(int i,double STime,double ETime,float rotate_y);
	void JiTouDiZuoAnimationFun(int i,double STime,double ETime,float move_y);
	void XiaoCheAnimation(int i,double STime,double ETime,float move_z);
	void PrePare(AnimationInfo& ZhuaLiaoMoInfo,float fzlm,AnimationInfo& CheTouInfo,float fct,AnimationInfo& JiTouMoZuo,float fjtmz,AnimationInfo& JiTouDiZuo,float fjtdz,float fjtwg1Zore,float fjtwg2Zore);
	osg::Vec3 GetCurZhuaLiaoMoCenter(float angle,int dir);//dir 0:右 1：左
	
	void SetStringLength(std::string& str,size_t len);
	void LoadAllModel();
	void LoadOneModel(QString str ,std::string s);
	void LoadOneModel(QString strPath,QString stlName);
	bool loadFile(const QString &file);
	void InitSTLSide(STL *stl);

	//初始位置
	void ReBuildGroupToSTLsFile(std::map<QString,GroupChildren> m_MapGroupToGroupChildren);//生成组和成员关系文件
	void ReBuildInitPosFile(std::vector<InitModelCenter> vtmp);	//生成初始位置文件

	//上移模型
	void UpdateAccumulateNewModelFile(QString copyName,QString newName,int upUnits);//新操作并更新数据结构
	void ReBuildAccumulateNewModelFile(std::vector<AccumulateModel> vtmp);//更新文件
	void ReBuildAccumulateNewModelVector();//vector,上移模型
	void ReBuildAccumulateNewModelVectorNew();//vector,上移模型,未用
	void LoadAccumulateModelsFromTheFile();//打开软件 导入累加模型


	//重构模型
	void UpdateJMFile(QString ModelName);//响应界面，更新文件数据结构
	void ReBuildJMFile(std::vector<JM> vtmp);//写文件 更新文件
	void ReBuildJMVector();//重读文件,更新文件数据结构
	void OnlyReadJMFile();
	void ReBuildJMModel();//重构模型
	void ReBuildJMVectorAtLoad();//打开软件重读文件,更新文件数据结构
	void ReBuildJMModelAtLoad();//打开软件重构模型

	void RebuildJT1JMModel(std::vector<JM> vtmp);
	void RebuildJT1LMModel(std::vector<LMR> vtmp);
	void ReadNormalUserModels();

	// 读模型类型和图片名映射文件
	std::vector<pair<QString,QString>> ReadTypeNameMap(QString filepath);

	void UpdateLmrFile(QString ModelName,double LMr);//响应界面，更新文件数据结构
	void ReBuildLMRFile(std::vector<LMR> vtmp);//写文件 更新文件
	void ReBuildLmVector();//重读文件,更新文件数据结构
	void OnlyReadLMFile();
	void ReBuildLMModel();//重构
	void ReBuildLmVectorAtLoad();//打开软件 vector
	void ReBuildLMModelAtLoad();//打开软件重构模型

	void UpdateInitPosFile(QString ModelName,osg::Vec3 center); 
	void ReadInitModelCenterFile(QString strPosFile);
	void ResetModelPosByInitPosFile();//设置模型初始位置

	// 夾子模因轮模半径不一样导致的偏移
	double GetJMShift(QString modelName);

	void GetValueByProductionFileRowAndColumn(OtherFileValue other,float& fvalue);
	void GetValueByParamFileRowAndColumn(OtherFileValue other,float& fvalue);
	void GetValueByTWLC1FileRowAndColumn(OtherFileValue other,float& fvalue);
	void GetValueByTWLC2FileRowAndColumn(OtherFileValue other,float& fvalue);
	void GetValueByGroupSettingFileRowAndColumn(OtherFileValue other,float& fvalue);
	void GetValueByTWLCNUMFileRowAndColumn(OtherFileValue other,float& fvalue,int twlcIndex); 

	void UpdataSettingNodeGroupDlg();//更新设置界面数据
	void ReadGroupToSTLsFile(QString filepath);
	void ReadMovePosFile(QString filepath);
	void ReadGroupSettingFile(QString filepath);
	void ReadAnimationStepFile(QString filepath,int iCurmn);
	void ReadFilesAtLoad();

	void AddModelsToCollision();

	void AddPipesToCollision();

	void PlanCollisionGroupAndMasker(std::map<QString,std::pair<int,int>>& mapGroupMask);

	void HighLightHUDText(int i);
	bool sma(double a, double b, double tol = TOL);
	bool isFileExist(QString fullFileName);
	//读文件 推弯流程1/2 、 参数 、 加工文件
	void ReadOneTWLCFile(QString pathName);
	void ReadBendProcess1(QString);
	void ReadBendProcess2(QString);
	void ReadParams(QString);
	void ReadAllTWLCFiles();
	void ReadProductionValues(QString);	

	// 写新的加工值文件
	void WriteProductionFile(QString);

	float GetPosValueByString(std::string str);
	QString GetStrPosValueByString(std::string str);

	void split(string& s,vector<string>&res,string& delema1);//按逗号读取数据

	void CreateAnimation();//创建动画 
	void PlayAnimation();
	void SetPlayNew();//设置动画数据

	void SetPlayNew2();//设置动画数据

	// 为了修复相同的模型在不同的组里面，导致计算模型相对运动错误的问题
	// 王文波 20230322
	void SetPlayNewNew();

	void SetPlay();
	void Play();//设置回调，播放动画

	// 将模型节点重新设置成树形结构后设置动画
	void PlayNew();

	// 建立各模型及节点之间的树形关系
	void BuildNodeTree();

	void AddNodeToTree(osg::ref_ptr<osg::MatrixTransform> matrix,QString name);

    void AddNodeToTree(QString father,QString son);

    void SetSegStartTimeMap();

	void OnlyReadProcessFileNew();
	void AllStepsNew();

	void InfoClear(AnimationInfo& info);
	void GetZhuaLiaoMoCenter();
	void JiTouDiZuoAnimation(float& fcurModelLayer,float fmm,double& dtime,AnimationInfo& info,float& curlayer);
	void AdvanceZhuanJiao(AnimationInfo& info,double& dtime);
	void AdvanceTuiWan(AnimationInfo& info,double& dtime);
	void CarReturn(int i,AnimationInfo& info,double& dtime);

    // 在下一段直管向前之前，先让整个管子控制点回到轮模原点
    void ReturnPipeToZeroAndCreate(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,
        AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth,const std::string& process);

	// 检查，处理直管长度是否和回退长度一只，不一直，则处理
	void RefreshBackPipe(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth);

    void RefreshBackTwoPipe(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth);
	// 根据my,myt,myth值重新编写直管生成函数

	// tip 此段属于从长直管左侧还是右侧加工
	void ShowStraightPipeNew(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,AnimationInfo& XiaoCheinfo,
        AnimationInfo PipeInfo,int i,float flength,int iNth,const std::string& process);//iNth:文字高亮显示行 

	// tip 此段是属于从长直管左侧还是右侧加工
	void ShowStraightPipe(int tip,int header,osg::Vec3 vec3Center,osg::Vec3 dir,
		                  AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth);//iNth:文字高亮显示行 

	void AddPipeAndInfos(osg::Vec3 vec3Center,osg::Vec3 dir,AnimationInfo& XiaoCheinfo,AnimationInfo PipeInfo,int i,float flength,int iNth,
		std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>>& infos,PIPE& pipe);

	// tip 此段管子是从长直管左侧还是右侧加工管子
	void ShowCurvePipe(int i,int tip,int header,osg::Vec3 center,osg::Vec3 dir,AnimationInfo& JiTouWanGuan1info,
		               AnimationInfo PipeInfo, float fcurveR ,float fcurveAngle, bool bIsTuiWan = true/*, bool bIsAngleChange = false*/);

	void PipeRotateX(AnimationInfo& ZhuaLiaoMoinfo,AnimationInfo PipeInfo,float frotateX,int i);
	void InitModel();//初始化模型
	void CreatLongStraightPipe();

	void MeshInitialPosition();
	void LeftFutuiDown(double& fTimeStart);
	void RightFutuiDown(double& fTimeStart);
	void LeftFutuiUp(double& fTimeStart);
	void RightFutuiUp(double& fTimeStart);
	void DaoJiaMoveForward(float fmove,AnimationInfo info,int i);
	void DaoJiaMoveBack(float fmove,AnimationInfo info,int i);
	void startTimer(double time);
	void GetSize();
	const char* QStringToCharptr(QString str);
	template<typename T> string toString(const T& t);

	void DoProduction(QString);//旧 读数据
	void BuidCurvePipe();//动画
	char* QStringToChar(QString );
	void QStringToChar(QString,char *);
	STL *GetCurSTLByName(QString strName);
	std::vector<STL*> GetGroupNodeByGroupName(QString strGName);

	std::vector<STL*> GetSTLsByGroupName(QString strGName);

	// 根据节点名获取此节点下所有节点
	std::vector<GroupChildren*> GetGroupNodesByGroupName(QString strGName);

	std::vector<STL*> GetSideSTLsBySideName(std::vector<QString> strSName);

	void ReadYBCExt(std::wstring fileName);
	void ReadYBCExt2(std::wstring fileName);
	void PostReadYBCExt2();
	int GetNthValueFromTxt(char *filename, char *key1, char *key2, int n, char *rvalue);
	int GetMachineValueFromTxt(char *filename, char *key, char *rvalue);
	int CF_TXT_SplitToStrs(char *instr, char* key, char **strs, int &numStr);
	int removeStrSpace(char * str);
	int  CF_TXT_RemoveSpaceHead(char * instr);
	int  CF_TXT_RemoveSpaceTail(char * str);
	int  CF_TXT_RemoveTailFileEndSign(char * str);
	int GetThirdStr(char *data, char* key, char *third);
	std::wstring qToStdWString(const QString &str);
	QString stdWToQString(const std::wstring &str);

	osg::ref_ptr<osg::Node> createQuade(std::string strFileName);

	void JitouRotate(double& fTimeStart,double angle);

	void JiTouMoZuoPos(Point3D fTrn, double& fTimeStart);
	void JiTouDiZuoPos(Point3D fTrn, double& fTimeStart);

	void removeCallBack();
	void BeforeSettingNodeDlg(QString name);
		
	bool CheckAdminCode(std::string code);

	void MoveCenter(double leftMove,double rightMove,  // 左右夹紧的量
		const std::vector<AnimationInfo>& move,
		const std::vector<AnimationInfo>& rotate,
		std::vector<AnimationInfo>& left,std::vector<AnimationInfo>& mid,
		std::vector<AnimationInfo>& right,
		std::vector<AnimationInfo>& test);

	void SetTransMatrix(STL* stl,
		const std::vector<AnimationInfo>& move,
		const std::vector<AnimationInfo>& rotate);

    // 合并相同矩阵的操作，同一段管子的提前转角和转角操作
    void CombineSameMatrix(const std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>>& pairs);

	// 合并所有管子动画，包括管子前进，后退，旋转动作
	void CombinePipesAnimations();

	void DevideInfo(std::vector<AnimationInfo>& infos);

    void SetTransMatrixNew(STL* stl,
		const std::vector<AnimationInfo>& move,
		const std::vector<AnimationInfo>& rotate);

public slots:
	// 微调时，选中的模型改变颜色
	void On_modelColor_change(const QString& name);

    void on_actionQuit_triggered();//分页显隐表格 
    void on_actionOpen_triggered();//打开文件
    void on_actionUnload_triggered();//播放
    void on_actionSave_As_triggered();
    void recentFileActivated(QAction *action);
    void setupRecentFilesMenu();

	void on_actionReset_View_triggered();//碰撞检测
	void on_actionFull_Screen_triggered(bool on);//微调
	void on_actionTexture_triggered(bool val);
	void on_actionLight_triggered(bool val);//暫停
	void on_actionLight_triggered2(bool val);//暫停

	void on_actionHighLight_triggered(bool val);//调试旋转中心    // 设置节点动画
	void on_actionShadow_triggered(bool val);//replacewheelmodel，修改
	void on_actionOptimize_triggered(bool val);//keyboard
	void on_actionBackFace_triggered(bool val);//高级用户
	
	void on_actionLoad_Shader_triggered();
	void on_actionRemove_Shader_triggered();
	
    void on_actionEnableNode_triggered();
    void on_actionDisableNode_triggered();
    void on_actionExpandTree_triggered();
    void on_actionCollapseTree_triggered();
    void on_actionCenterOnSelection_triggered();
    void slotTreeViewCustomContextMenuRequested(const QPoint& pos);

	void on_pushButtonClearLog_pressed();
	void on_pushButtonSaveLog_pressed();

	// search
	void slotSearchCaseSensitiveToggled(bool checked);
	void slotEditSearchReturnPressed();

	void slotTreeNodeSelected(const QModelIndex &index);
	void slotSelectTreeItem(osg::Drawable *);
	void slotNewLoadedFile(osg::ref_ptr<osg::Node> node,
		osg::ref_ptr<osg::Material> material,
		const QString& name);

	void slotNewLoadedFileNew(osg::Node * node,const QString& name,bool IsFirst);
	void slotSettingNodeFinishedMy(osg::Node*,const QString& ,bool IsFirst);

	void slotPrintToLog(const QString & mess);
	void slotPrintToLog(const QStringList & mess);
	void slotChangeAspectRatio(const QSize & sz);

	void slotSettingNodeFinished(osg::Node*,const QString& );

	void slotSettingNodeFinishedNew(int , int ,const QString& ,const QString& ,osg::ref_ptr<osg::Node> node,
		osg::ref_ptr<osg::Material> material,const QString&);
	void closeTimer();
	void collisionDetect();

	// 利用回调函数检测到了碰撞 wangwenbo 20240927
	void collisionDetectedNew(std::string n0,std::string n1);

	void exportCollisionPairs(std::string n0,std::string n1);

	void writePipeStl(osg::PositionAttitudeTransform* pMatrix);

	void AddChildToFatherMatrix(QString strChildName,QString strFatherName);
	void DelChildToFatherMatrix(QString strChildName,QString strFatherName);
	void SlotRightMenu(const QPoint& pos);
	void SlotMenuTablecheckbox_clicked(bool);
	void SlotMenuBtnscheckbox_clicked(bool);
	void SlotMenuLookAt_clicked(bool);
	void SlotSetAnimation();
	void SlotChangeTableBtnState();
	void SlotSetGroupToChildren();
	void SlotSetJTSideModels();

	void SlotSlightAdjust();
	void SlotReplaceWheelModel();//轮模
	void SlotReplaceJMModel();//夹模
	void SlotAddModels();
	void SlotSetMoveRange();
	void SlotSetMoveModelHighLight();
	void SlotPlayRotateTest();
	void SlotSetLanguage();
	void SlotReplaceByModifyLMTableModel();
	void SlotShowModifyLMTableModel();
	void SlotModifyLMTableFinished();
	void SlotSetDetectedFinished();

	void SlotZoomIn();
	void SlotZoomOut();
	void SlotUpMove();
	void SlotDownMove();
	void SlotLeftMove();
	void SlotRightMove();
	void SlotCWRotate();	
	void SlotCCWRotate();
	void SlotResume();
	void SlotSignInAdvanceUser();

protected slots:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void keyPressEvent(QKeyEvent *event);
	void mousePressEvent( QMouseEvent *event );

signals:
    void sigNewFileToLoad(const QString &);

	void sigPartsPause(bool val);

private:
	std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> GetAnmiPipeSeg();

	void CombineSamePeriodAnimation(std::vector<AnimationInfo>& anims);

	void OptimizeMachinePara(size_t i);

	void InitiMultiLmLoopParam(int i);

	void SetTimeOffset(double time);

	// 判断模型是否需要做碰撞检测
	bool IsModelCollision(QString name);

	void saveSettings();
	void loadSettings();
    void addRecentlyOpenedFile(const QString &fn, QStringList &lst);
    void updateUi();
    void enableActions(bool);
    void updateApplicationIdentity();
    void expandReccursively(const QModelIndex &index);
    void collapseReccursively(const QModelIndex &index);

	void hideDockWidgets();
	void showDockWidgets();
	bool search(const QModelIndex &index,const QString &name);
	void resetViews(bool allClear);

	void SetCollisionDetectParts();

	void CreateSameModel(osg::Node* pNode,osg::ref_ptr<osg::Geode>& geode,osg::ref_ptr<osg::Material>& matrial);
private:
	Ui::MainWindow ui;
	osg::ref_ptr<osg::Node> m_rootNode;
	xSceneView* m_sceneView;
	xSceneModel* m_sceneModel;

	// 读取配置文件中各机头参数，生成弯管段中心，弯管段中止角，遮挡平面中心，遮挡平面法向量
	void ReadHeaderPara();

	QList<QWidget *> m_dockWidgets;
	xTreeModel *m_treeModel;
	xTreeView *m_treeView;

	QString m_appName;
	QString m_version;
    // variables
	QString m_currFile;
    QString m_lastDirectory;

    // recent files
    QMenu *m_pRecentFilesMenu;
    QStringList m_recentFiles;

	// for Lod factor manipulation
	QLabel *m_LODFactorLabel;
	float m_LODFactor;
	
	bool m_isOptimized;
	bool m_isUseSoftKeyBoard;


	xObjectLoader *m_objectLoader;
    // display aspect ratio of the central windows
    QLabel *m_aspectRatioLabel;

	// for search function
	bool m_isCaseSensitive;
	QCompleter *m_completerSearch;
	QModelIndex m_currSearchIndex;
	xSearchLineEdit *m_lineEditSearch;
	
	xPropertyWidget *m_propertyWidget;
	//xSettingNodeDlg *m_setNodeDlg;

	int nodecount;
	bool m_bIsFirstSTL;

	// 纯左侧加工
	bool m_bOnlyLeft;

	int m_nCurrentMoCheng;
	int m_nPreviousMoCheng;
	int m_dmMoveWhich;
	bool m_nCurrentMoChengIsRight;

	std::vector<float> m_heightArray; // 模程高度
	std::vector<float> m_radiusArray; // 模程半径
	std::vector<float> m_radiusArrayLeft; // 模程半径
	std::vector<int>   m_radiusIndexArray;// 模程半径索引，对于每一XYZ/YBC都有一个模程索引
	std::vector<bool>   m_isRightArray;// 如果是右边则为1 左边则为0
	std::vector<InputData> m_InputDataArray;

	std::vector<AnimationInfo> m_vWanQuMo2Animation;  // 换边时，弯曲模2向后移动
	std::vector<AnimationInfo> m_vJiTouWanGuan3Animation;    // 换边时，机头弯管3向前移动
	std::vector<AnimationInfo> m_vXiaoCheAnimation;  // 小车动画路径
	std::vector<AnimationInfo> m_vJiTouWanGuan1Animation;  // 机头弯管1动画路径
	std::vector<AnimationInfo> m_vJiTouWanGuan2Animation;  // 机头弯管1动画路径

	std::vector<AnimationInfo> m_vJiTouWanGuan1Animation_back;  // 机头弯管1动画路径
	std::vector<AnimationInfo> m_vZhuaLiaoMoAnimation;  // 抓料模动画路径
	std::vector<AnimationInfo> m_vZhuJia2Animation;  // 抓料模动画路径
	std::vector<AnimationInfo> m_vZhuJia1Animation;  // 抓料模动画路径
	std::vector<AnimationInfo> m_vDaoJia2Animation;  // 抓料模动画路径
	std::vector<AnimationInfo> m_vDaoJia1Animation;  // 抓料模动画路径
	std::vector<AnimationInfo> m_vDaoJia2_1Animation;
	std::vector<AnimationInfo> m_vDaoJia1_1Animation;

	std::vector<AnimationInfo> m_vDaoJiaTmpAnimation;
	std::vector<AnimationInfo> m_vLFuiTuiDaoJia0Animation;
	std::vector<AnimationInfo> m_vLFuiTuiDaoJia1Animation;
	std::vector<AnimationInfo> m_vLFuiTuiDaoJia2Animation;
	std::vector<AnimationInfo> m_vLFuiTuiDaoJia3Animation;
	std::vector<AnimationInfo> m_vRFuiTuiDaoJia0Animation;
	std::vector<AnimationInfo> m_vRFuiTuiDaoJia1Animation;
	std::vector<AnimationInfo> m_vRFuiTuiDaoJia2Animation;
	std::vector<AnimationInfo> m_vRFuiTuiDaoJia3Animation;

	std::vector<AnimationInfo> m_vPipeAnimation;  // 管件动画
	std::vector<AnimationInfo> m_vZuoFuTuiAnimation;
	std::vector<AnimationInfo> m_vYouFuTuiAnimation;
	std::vector<AnimationInfo> m_vJiTouMoZuoAnimation;
	std::vector<AnimationInfo> m_vJiTouDiZuoAnimation;
	std::vector<AnimationInfo> m_vDaoLun1Animation;
	std::vector<AnimationInfo> m_vDaoLun2Animation;
	std::vector<AnimationInfo> m_vZhuLunAnimation;
	std::vector<AnimationInfo> m_vCheTouAnimation;

	std::vector<AnimationInfo> m_vRotateTestAnimation;
	std::vector<std::string> m_vMachineInfo;


	std::vector<YBCExt> m_YBCExtArray;
	YBCExt2 m_YBCExt2;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_straiPipePosTrans;
	//osg::Group * m_pstraiPipe;
	osg::ref_ptr<osg::Geode> m_pstraiPipe;

	osg::Group* m_pLunMo;

	float m_fPipeLength;
	float m_fPipeRadius;
	float m_fmax_r;	

	osg::ref_ptr<osg::MatrixTransform> transPipePos;//Y的平移矩陣
	osg::Vec3 v3Center;
	osg::Group* m_pCarFatherNode;
	osg::Group* m_pJiTouWanGuan1FatherNode;
	
	double m_fAnimationTime; // 每个动画的时间
	double m_fAnimationStopTime; // 动画之间暂停的时间
	double m_dXiaoCheStartTime;  //保存小车最开始移动时间
	double m_dXiaoCheEndTime; 
	std::vector<JiaGongInfo> m_jiaGongInfo;
	float m_fOverSet;
	double m_dAniTotalTime;

	std::map<QString,int> m_MapModelNameToIsAsSpecial;
	//std::map<QString,int> m_MapSpecialModelNameToIsAsSpecial;

	Param m_param;
	std::vector<std::vector<TWLC>> m_allvTWLCValues;
	std::vector<struct TWLC> m_vTWLCValues;
	std::map<int,std::vector<struct TWLC>> m_MapFileTovTWLCValues;
	std::vector<struct ProductionValue>	m_vproductionvalues;

	std::vector<int> m_iLoopRecord;

	std::vector<struct TWLC1> m_vTwlc1values;	 
	std::vector<struct TWLC2> m_vTwlc2values;	 
	std::vector<struct InitModelCenter> m_vInitModelCenterFileValues;
	std::vector<InitModelCenter> m_vtmpPosFileValues;
	std::vector<Gcode> m_vGcodeFileValues;//代码定义文件

	// GCode与是否是特殊移动对应关系
	std::map<QString,int> m_mapGcodeSpecialMove;

	std::vector<Process> m_vProcessFileValues;
	std::map<int,std::vector<Process>> m_MapStepTovProcessFileValues;//流程文件
	std::map<int,std::map<int,std::vector<Process>>> m_MapLoopToMapStepTovProcessFileValues;
	std::vector<int> m_vCurMaptoNextMap;//当前弯跳转对应的下一弯
	std::vector<std::vector<Process>> m_vLoopvProcess;
	std::map<int,std::vector<std::vector<Process>>> m_MapLoopTovLoopvProcess;
	std::vector<MovePos> m_vMovePosFileValues;
	std::map<int,std::vector<OnlyReadProcess>> m_MapStepTovOnlyReadProcess;
	std::vector<std::vector<Process>>  m_vAllStepvProcess;
	std::vector<int> m_vLoopStartIndexs;
	std::vector<int> m_vLoopEndIndexs;
	std::vector<int> m_vLoopTimes;
	int m_LoopTimes;
	int m_LoopStartIndex;
	std::vector<WheelModel*> m_vNewWheelModel;
	std::vector<AnimationInfo> m_vColorInfo;
	std::vector<QString> m_vNeedRebuildLmName;//ini文件设定的重构模型名
	float m_fhole_r;
	float m_flm_h;
	bool m_IsDetected; // 判断是否需要碰撞检测

	bool m_bIsAutoPlan;  // 判断是否是自动规划
	bool m_bAutoPlaning;  // 判断是否在自动规划持续时间内

	float m_safeDis;
	float m_pipe_z;

	std::map<int,std::vector<QString>> m_MapTovJT1LMSort;
	std::map<int,std::vector<QString>> m_MapTovJT1JMSort;
	std::map<QString,std::vector<QString>> m_MapSortNameTovJT1JMNames;
	std::map<QString,std::vector<QString>> m_MapSortNameTovJT1LMNames;

	std::vector<JM> m_JT1_JMFileValues;//JM
	std::vector<LMR> m_JT1_LMFileValues;//LM
	std::vector<LMR> m_JT1_CFileValues;
	std::vector<JM> m_JT1_LFileValues;
	std::vector<JM> m_JT1_RFileValues;
	
	std::vector<JM> m_JT1_RJMValues;
	std::vector<JM> m_JT1_LJMValues;
	std::vector<LMR> m_JT1_CLMValues;//LM
	std::vector<JM> m_JT1_JMValues;//JM
	
	std::vector<STL*> m_vAlterModelSTL;

	std::vector<JM> m_vJMFileValues;
	std::vector<LMR> m_vLMRFileValues;
	std::vector<AccumulateModel> m_vAccumulateNewModelFileValues;
	std::vector<STL*> m_vJMFileSTLs;
	std::vector<STL*> m_vLMRFileSTLs;
	std::vector<STL*> m_vAccumulateFileSTLs;
	std::vector<QString> m_vCurLanguageWords;

	std::map<int,FileColumn> m_ColumnToString;//一行的列数据
	std::map<int,std::vector<std::map<int,FileColumn>>> m_MapStepTovColumnToString;//步号对应多行
	std::map<int,std::vector<std::vector<FileColumn>>> m_MapStepTovRowString;//步号对应多行

	std::vector<QString> m_vLaguages;
	std::vector<std::vector<QString>> m_vLanguagevWords;
	std::map<QString,std::vector<std::pair<QString,int>>> m_MapDisplayNameToPairModelNames;
	std::map<QString,std::vector<QString>> m_MapCollisonDectionFileDisplayNameToLanguage;
	std::map<QString,QString> m_MapModelNameToAlterModelName;
	std::map<QString,std::vector<QString>> m_MapSymbolToCollisionLanguage;
	std::map<QString,QString> m_MapSymbolToCurLanguage;
	std::map<int,std::map<QString,std::vector<QString>>> m_JTNumToSideModelNamesMap;

	std::map<int,float> m_MapJT1LayerToR;//模层对应中心模半径
	std::map<int,float> m_MapJT2LayerToR;
	int m_LayerNum;//模层数
	std::map<QString,QString> m_ModelNameToDisplayName;
	std::map<int,std::map<QString,std::vector<STL*>>> m_MapNumToMapTypeToSTLs;

	double m_dJMLen;
	double m_dJMWide;

	double m_dOriginalJMLen;
	double m_dCurJMLen;
	double m_LMupLen;
	double m_LMdownLen;
	bool m_isHaveTu;
	double m_LMtuLen;
	double m_LMTuSide;
	int m_LMJTNth;
	int m_LMLayer;
	int m_JMJTNth;
	int m_JMAoSide;
	int m_JMAid;
	int m_iJTNthInAccumulate;
	bool m_isStart;

	int m_iTwlc1;
	int m_iTwlc2;
	int m_iTwlc3;
	int m_iTwlc4;
	int m_iTwlc5;
	int m_iTwlc6;
	int m_iTwlc7;
	int m_iTwlc8;
	int m_iTwlc9;
	int m_iTwlc10;
    int m_iTwlc11;
    int m_iTwlc12;
	int m_iTwlc13;
	int m_iTwlc14;
	int m_iTwlc15;
	int m_iTwlc16;
	int m_iTwlc17;
	int m_iTwlc18;
	int m_iTwlc19;
	int m_iTwlc20;

	size_t m_index;
	int m_iN;//推弯流程号

	QMenu *cmenu;

	float m_fcurModelLayer;
	float m_LmHeight;
	Point3D m_3dZhuaLiaoMoCenter;
	osg::ref_ptr<osg::Node> m_CurZlmNode;
	float m_lastangle;//当前处于向左转了多少度 +x
	float m_Rlastangle;//当前处于向右转了多少度 -x
	float m_zlm_X;//夹料磨x
	float m_ZhongXinMoHeight;
	float m_fNeedRightmove;
	float m_fNeedRightmove2;
	int m_changesidedown;//换边时模型下降预留的中心模个数
	float m_fAdvanceTuiWan;
	bool m_bIsLeft;
	int ilastmmdR;
	float flastmz;
	float m_fzhuanjiao;
	std::vector<osg::ref_ptr<PIPE>> m_vPipes;
	// 因为有删除管子，这个是存储新矩阵代替原来的旧矩阵，后面生成动画的时候替换
	std::map<osg::ref_ptr<osg::PositionAttitudeTransform>,
		     osg::ref_ptr<osg::PositionAttitudeTransform>> m_mapMatrixInstead;

	// 变换矩阵与机头编号映射
	std::map<osg::ref_ptr<osg::PositionAttitudeTransform>,int> m_mapMatrixHeader;

	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> m_vMatrixAnimation;
    std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,std::vector<AnimationInfo>>> m_vMatrixAnimations;

	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> m_vPipeBackMatrixAnimation;

	// 按照时序排列的矩阵节点，包括前进和后退的所有动作
	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> m_vPipesMatrixAnimation;

	std::vector<pair<osg::ref_ptr<osg::PositionAttitudeTransform>,AnimationInfo>> m_vLongStraightPipeMaxtrixAnimation;
	std::vector<AnimationInfo> m_vLongStraightPipAnimation;

	// 弯数和起始时间对应关系
	std::map<int,double> m_mSegStartTime;

	int m_linkedNum;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_vRotateMatrix;
	std::vector<float> m_vfzoremove;
	osg::Vec3 m_vec3RotateCenter; 
	float m_fTimeOff;
	float m_fTimeOffC;

	double dtime;//时间轴
	bool m_IsLeftFuTuiFlat;
	bool m_IsRightFuTuiFlat;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_vRAddDaoJiaMatrix;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_vLAddDaoJiaMatrix;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_vAddZhuJiaMatrix;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_vAddJiTouWanGuan2Matrix;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_vAddJiTouWanGuan3Matrix;

	float m_fFuTuiDaoJiaDis;
	float m_fZhuJia2ChangeMove;
	float m_fLastTwc;
	QTimer* m_collisionTimer;
	QTimer* m_closeTimer;

    //MMTimer *SetTimer;

	float m_v;

    // 管子参考点位置
    double m_dPipePos;

	xCollisionDetectionDlg *m_pCollisionDlg;
	xSettingNodeDlg *NodeDlg;
	xSettingAnimation *m_pSettingAnimationDlg;
	xSettingNodeGroupDlg *m_pSettingNodeGroupDlg;
	xSlightAdjust *m_pSlightAdjustDlg;
	xSettingColor *m_pSettingColor;

	xOptimizeDlg* m_pOptimizeDlg;
	
	//xMoveRange *m_pMoveRangeDlg;
	xAddModel *m_pAddModelDlg;
	xReplaceWheelModel *m_pReplaceWheelModelDlg;
	xRotateTest *m_pRotateTestDlg;
	Table *m_productionTable;
	Table *m_twTable;

	xLanguageDlg *m_pLanguageDlg;
	xSpeedDlg* m_pSpeedDlg;
	xModifyLMTable *m_pModifyLMTable;
	xNormalUserWidget *m_pNormalUserWidget;//分页
	xAdvanceUserDlg *m_pAdvanceUserDlg;
	QWidget *m_NormalPadWidget;

	QAction *TableAction;
	QAction *BtnsAction;
	QAction *SetViewAsLookAt;
	QString m_strDefaultLanguageType;
	std::pair<int,QString> m_NumToDefalutLanguagePair;

	QPushButton *m_PadUpMoveBtn;
	QPushButton *m_PadDownMoveBtn;
	QPushButton *m_PadLeftMoveBtn;
	QPushButton *m_PadRightMoveBtn;
	QPushButton *m_PadCWRotateBtn;
	QPushButton *m_PadCCWRotateBtn;
	QPushButton *m_PadZoomInBtn;
	QPushButton *m_PadZoomOutBtn;
	QPushButton *m_PadResumeBtn;

	bool m_isFirstOpenGroupDlg;
	bool m_isFirstOpenAdjustDlg;
	bool m_isFirstOpenMoveRangeDlg;
	bool m_isFirstOpenRotateTestDlg;
	bool m_isFirstOpenCollisionDlg;

	// 碰撞检测需要检测的碰撞对 wangwenbo 20241210
	std::map<std::pair<QString,QString>,int> m_MapCollisionPair;

	// 碰撞检测需要添加的模型
	std::map<QString,int> m_MapCollisionModel;

	StringList m_CollisionNameList;

	std::pair<int,int> m_pairPipeGroupMask;

	std::map<QString,QString> m_MapModelNameToType;
	std::map<QString,RGB_COLOE> m_MapModelNameToRgbColor;
	std::vector<QString> m_vAllNodeName;
	std::map<QString,GroupChildren> m_MapGroupToGroupChildren;//组和位置对应关系

	std::map<int,std::map<QString,std::vector<QString>>> m_MapJTSideModels;

	std::map<QString,std::vector<QString>> m_MapGroupToChildren;//组和子的对应关系
	std::map<QString,std::vector<STL*>> m_MapGroupToSTL;//组和子的对应关系,由上面的生成
	std::map<QString,int> m_MapGroupToAxis;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_PipeInfoMatrix;

	std::vector<osg::ref_ptr<osg::Group>> m_vCollisionParts;
	std::vector<std::pair<STL*,STL*>> m_vPairCollisionPartsToAlter;
	int m_iSize;

	std::vector<osg::ref_ptr<osgText::Text>> m_vOsgText;
	std::vector<float> m_fallR;
	float m_foriginalR;
	float m_fOriginalLmAc;//轮模中间的半径
	float m_fcurZxmAc;//中心莫中間的半径

	osg::Vec3 m_OriginalPipeCenter;//长直管center
	osg::Vec3 m_OriginalCheTouCenter;//车头中心
	osg::Vec3 m_CurZhuaLiaoMoCenter;
	osg::Vec3 m_vec3ZxmCenter; //中心模center
	osg::Vec3 m_CurZhuLunCenter;

	// 不同机头对应的机头中心
	std::vector<osg::Vec3> m_vHeaderCenters;

	std::vector<osg::Vec3> m_vPipeNormals;//不同机头生成直管时的方向
	std::vector<osg::Vec3> m_vPipeCenters;//生成弯管时的圆心

	osg::Vec3 m_LongPipeCenter;
	osg::Vec3 m_LongPipeNormal;

	// 各机头遮挡平面中心
	std::vector<osg::Vec3> m_vPlaneCenters;
	// 各机头遮挡平面法向量
	std::vector<osg::Vec3> m_vPlaneNormals;

	// 各机头生成弯管中止角
	std::vector<double> m_vPipeRotateAngles;

	float m_zlmTozlDis;//抓料模和主轮的距离
	float m_fRdlastangle;//dl在右边 
	float m_fLdlastangle;//dl在左边 
	std::vector<STL*> m_vSTLs;//存当前所有的模型对象

	std::vector<STL*> m_vNodes;  // 保存所有父节点
	std::vector<STL*> m_vOriginalSTLs;//存文件导入的模型对象

	// 判断当前是否处于调整模型位置的操作
	bool m_bIsAdjusting;

	osg::Node *m_curNode;
	int m_mark;
	osg::Vec3 m_LongStraightPipeCenter;
	int m_showLongStraightPipe;
    ShowTableDlg *m_ShowTableDlg;
	osg::ref_ptr<HUDAxis> m_HudAxis;
	std::vector<xModelGroup*> m_Groups;//组对应模型成员
	QString need_filePath;//动画文件路径
	bool  m_bIsDivideTwoTimes;
	AnimationInfo m_DivideInfo1;
	AnimationInfo m_DivideInfo2;

	AnimationInfo JiTouMoZuoinfo;
	AnimationInfo DaoJia2info;
	AnimationInfo DaoJia1info;
	AnimationInfo DaoJia1_1info;
	AnimationInfo ZhuJia1info;
	AnimationInfo ZhuJia2info;
	AnimationInfo WanQuMo2info;
	AnimationInfo JiTouWanGuan3info;
	AnimationInfo JiTouWanGuan2info;
	AnimationInfo JiTouWanGuan1info;
	AnimationInfo ZhuaLiaoMoinfo;
	AnimationInfo PipeInfo;
	AnimationInfo CheTouInfo;
	AnimationInfo DaoLun1Info;
	AnimationInfo DaoLun2Info;
	AnimationInfo ZhuLunInfo;
	AnimationInfo JiTouDiZuoinfo;
	AnimationInfo XiaoCheinfo;

    CTree* m_pTree;

	// 模型平移/旋转/缩放操作记录
    double m_dRat;    // 缩放
	double m_dX;      // X方向平移
	double m_dY;      // Y方向平移
	double m_dDegree;  // 旋转角度
	bool m_bFirstSet;
	osg::Vec3f m_eye;
	osg::Vec3f m_center;
	osg::Vec3f m_up;

	// 记录机头弯管的角度
	double m_dJitouAngle;

    QString m_strLunmoName;

	std::vector<AnimationInfo> m_vJitouwanguanAnims;
    std::vector<AnimationInfo> m_vLunMoAnims;
	osg::Vec3 m_vCenter;

	MultiLmLoopData m_sMultiLmLoop;

	// 判断机头沿着弯管移动的距离
	double m_dRDistance;

	// 判断主夹垂直机头弯管移动的距离
	double m_dTDistance;

	// 模型位置ID，区分普通用户操作模型尺寸，类型变化后是否还是属于同一初始位置的模型
	//int m_id;

protected:
	osg::ref_ptr<osgDragger::HudCamera> _hudCamera;
	osg::observer_ptr<osg::Geode> _debugGeode;
};

/*class CustomDraggerManager : public osgDragger::DraggerManager
{
public:
	CustomDraggerManager(){}

	static CustomDraggerManager* Instence()
	{
		static osg::ref_ptr<CustomDraggerManager> cdm = new CustomDraggerManager;

		return cdm.get();
	}

protected:
	virtual ~CustomDraggerManager(){}
};*/

#if 1
class MouseHandle : public osgGA::GUIEventHandler
{

private:

	osg::Vec3 screenToWorld(osgViewer::View* viewer,double dx,double dy)
	{
		osg::Camera *camera = viewer->getCamera();
		osg::Matrix viewMat = camera->getViewMatrix(); //获取当前视图矩阵
		osg::Matrix projMat = camera->getProjectionMatrix();//获取投影矩阵
		osg::Matrix windMat = camera->getViewport()->computeWindowMatrix();//获取窗口矩阵
		osg::Matrix MVPW = viewMat * projMat *windMat;  //视图-》投影-》窗口变换

		osg::Matrix inverseMVPW = osg::Matrix::inverse(MVPW);  
		osg::Vec3 mouseWorld = osg::Vec3(dx, dy, 0) * inverseMVPW;  
		return mouseWorld;
	}

public:
	MouseHandle(MainWindow* window)
	: m_pMainWindow(window)
	{
	}

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		if (ea.getHandled() == true)
		{
			return true;
		}

		osgViewer::View *viewer = dynamic_cast<osgViewer::View*>(&aa);
		if (!viewer)
		{
			return false;
		}

		float x = ea.getX ();
		float y = ea.getY ();

		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
			{
				if (ea.getKey() == osgGA::GUIEventAdapter::KEY_1)
				{
					osgVirtualAssembly::CustomDraggerManager::Instence()->setActiveDragger("move");
				}

				if (ea.getKey() == osgGA::GUIEventAdapter::KEY_2)
				{
					osgVirtualAssembly::CustomDraggerManager::Instence()->setActiveDragger("rotate");
				}
			}
			break;

		case osgGA::GUIEventAdapter::PUSH:
			{
				//更改与模型相关联的Selection
				if(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON & ea.getButtonMask())
				{
					osg::MatrixTransform* mt= pick(viewer, x, y);

					if (mt)
					{
						if ( (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) == 0)
						{
							_pickSelections.clear();
						}
						
						_pickSelections.push_back(mt);

						OSG_WARN<<mt->getName()<<std::endl;
						if (m_pMainWindow && m_pMainWindow->isAdjusting())
						{
							// 设置主窗口钟被选择模型颜色变色
							//m_pMainWindow->On_modelColor_change(QString(mt->getName().c_str()));
							m_pMainWindow->SetCurrentModel(QString(mt->getName().c_str()));
							std::cout <<  "pick "<< mt->getName();
							osgVirtualAssembly::CustomDraggerManager::Instence()->setSelections(_pickSelections);
							osgDragger::ShowBound::Instence()->setSelections(_pickSelections);
						}
						else
						{
							osgVirtualAssembly::CustomDraggerManager::Instence()->reset();
							osgDragger::ShowBound::Instence()->reset();
						}
					}
				}
			}
			break;

		case osgGA::GUIEventAdapter::DRAG:
			if(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON /*& ea.getButtonMask()*/)
			{
				osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
					new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);

				osgUtil::IntersectionVisitor iv( intersector.get() );
				iv.setTraversalMask(0x1);
				viewer->getCamera()->accept(iv);
		
				if ( intersector->containsIntersections() )
				{
					const osgUtil::LineSegmentIntersector::Intersection& result =
						*(intersector->getIntersections().begin());
					int size = result.nodePath.size ();

					osg::Vec3d pickPoint = result.getWorldIntersectPoint();
					osg::ref_ptr<osg::Node> node = new osg::Node;
					node = result.nodePath.at(result.nodePath.size() - 1);
					osg::Vec3 curPos;
					if (node!=nullptr)
					{
						//获取当前的摄像机的位置
						//osg::Vec3 eye = viewer->getCamera()->getInverseViewMatrix().getTrans();
						////计算当前摄像机与pick到的模型之间的距离是多少
						//osg::Vec3 offset = pickPoint - eye;
						//int dist = offset.length();
						////计算当前的鼠标屏幕点映射到三维中的值
						//osg::Vec3 mouseWorldPos = screenToWorld(viewer,x,y);
						////计算当前鼠标三维点与摄像机的方向
						//osg::Vec3 rayDir = mouseWorldPos - eye;
						//rayDir.normalize();
						////最后计算物体拖拽时最终的世界位置
						//curPos = eye + rayDir*dist;

						if (m_pMainWindow && m_pMainWindow->isAdjusting())
						{
							curPos = osgVirtualAssembly::CustomDraggerManager::Instence()->GetCenter();
							// 设置模型位置
							m_pMainWindow->SetCurrentPos(curPos);
							std::cout <<  "Move pos x "<< curPos.x() << " y "<< curPos.y() << " z "<< curPos.z() << endl;
							// osgVirtualAssembly::CustomDraggerManager::Instence()->setSelections(_pickSelections);
							// osgDragger::ShowBound::Instence()->setSelections(_pickSelections);
						}
					}
				}
			}
			break;

		case osgGA::GUIEventAdapter::MOVE:
			if(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON /*& ea.getButtonMask()*/)
			{
				osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
					new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);

				osgUtil::IntersectionVisitor iv( intersector.get() );
				iv.setTraversalMask(0x1);
				viewer->getCamera()->accept(iv);
		
				if ( intersector->containsIntersections() )
				{
					const osgUtil::LineSegmentIntersector::Intersection& result =
						*(intersector->getIntersections().begin());
					int size = result.nodePath.size ();

					osg::Vec3d pickPoint = result.getWorldIntersectPoint();
					osg::ref_ptr<osg::Node> node = new osg::Node;
					node = result.nodePath.at(result.nodePath.size() - 1);
					osg::Vec3 curPos;
					if (node!=nullptr)
					{
						//获取当前的摄像机的位置
						osg::Vec3 eye = viewer->getCamera()->getInverseViewMatrix().getTrans();
						//计算当前摄像机与pick到的模型之间的距离是多少
						osg::Vec3 offset = pickPoint - eye;
						int dist = offset.length();
						//计算当前的鼠标屏幕点映射到三维中的值
						osg::Vec3 mouseWorldPos = screenToWorld(viewer,x,y);
						//计算当前鼠标三维点与摄像机的方向
						osg::Vec3 rayDir = mouseWorldPos - eye;
						rayDir.normalize();
						//最后计算物体拖拽时最终的世界位置
						curPos = eye + rayDir*dist;

						if (m_pMainWindow && m_pMainWindow->isAdjusting())
						{
							curPos = osgVirtualAssembly::CustomDraggerManager::Instence()->GetCenter();
							// 设置模型位置
							m_pMainWindow->SetCurrentPos(curPos);
							std::cout <<  "Move pos x "<< curPos.x() << " y "<< curPos.y() << " z "<< curPos.z() << endl;
							// osgVirtualAssembly::CustomDraggerManager::Instence()->setSelections(_pickSelections);
							// osgDragger::ShowBound::Instence()->setSelections(_pickSelections);
						}
					}
				}
			}

			break;

	    case osgGA::GUIEventAdapter::RELEASE:
			{
				int a = 0;
			}
			break;

		case osgGA::GUIEventAdapter::FRAME:
			break;

		default:
			LogInfo(QString(("operator is %1 !")).arg(ea.getEventType()));

			break;
		}

		return false;
	}

    // 参数说明：firstPos:是当前摄像机的位置。endPos:为偏移坐标值，eye + curRayLineDir*100
	// curRayLineDir = mouseWorldPos（屏幕点转三维点使用上面的函数） - eye;
	// curRayLineDir.normallize();

	osg::MatrixTransform* pick( osgViewer::View* viewer, float x, float y )
	{
		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
			new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);

		osgUtil::IntersectionVisitor iv( intersector.get() );
		iv.setTraversalMask(0x1);
		viewer->getCamera()->accept( iv );
		
		if ( intersector->containsIntersections() )
		{
			const osgUtil::LineSegmentIntersector::Intersection& result =
				*(intersector->getIntersections().begin());
			int size = result.nodePath.size ();

			osg::Vec3d pickPoint = result.getWorldIntersectPoint();
			osg::ref_ptr<osg::Node> node = new osg::Node;
			node = result.nodePath.at(result.nodePath.size() - 1);
			osg::Vec3 curPos;
			if (node!=nullptr)
			{
				//获取当前的摄像机的位置
				osg::Vec3 eye = viewer->getCamera()->getInverseViewMatrix().getTrans();
				//计算当前摄像机与pick到的模型之间的距离是多少
				osg::Vec3 offset = pickPoint - eye;
				int dist = offset.length();
				//计算当前的鼠标屏幕点映射到三维中的值
				osg::Vec3 mouseWorldPos = screenToWorld(viewer,x,y);
				//计算当前鼠标三维点与摄像机的方向
				osg::Vec3 rayDir = mouseWorldPos - eye;
				rayDir.normalize();
				//最后计算物体拖拽时最终的世界位置
				curPos = eye + rayDir*dist;
			}

			osg::MatrixTransform* selection = 0;
			for(int index=size-1; index > 0; index--)
			{
				selection = dynamic_cast<osg::MatrixTransform*>(result.nodePath[index]);
				if (selection)
				{
					std::string str = selection->getName();
					if (!str.empty())
					{
						return selection;
					}
				}
			}
		}

		return nullptr;
	}

	private:
		MainWindow* m_pMainWindow;

		SelectionList _pickSelections;

};
#endif

template<typename T>
string MainWindow::toString(const T& t)
{
	ostringstream oss;  //创建一个格式化输出流
	oss.precision(5);
	oss.setf(std::ios::fixed);
	oss<< t ;             //把值传递如流中
	return oss.str();  
}

BOOL ExistFile(char   *szName);

#endif 
