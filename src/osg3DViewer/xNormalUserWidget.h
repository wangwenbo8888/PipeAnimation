#ifndef XNORMALUSERWIDGET_H
#define XNORMALUSERWIDGET_H

#include "qdialog.h"
#include "Novadef.h"
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <iostream>
#include <osg/Node>
#include <QTabWidget>
#include "xModifyLMTable.h"
#include "xSettingAnimation.h"
#include "NumberKeyboard.h"
#include "STL.h"
#include "xMyLabel.h"

#include "xModelWidget.h"
#include <QTableWidget>

class MainWindow;
class xNormalUserWidget :public QWidget
{
	Q_OBJECT
public:
	xNormalUserWidget(MainWindow *p,bool keyboard=true);
	~xNormalUserWidget();

	void SetHideLMAsChosenSpecialModel();
	void SetHideJMAsChosenSpecialModel();
	void ReSetJT1SideToModelsBySortFile();
	void bubble_sort(std::vector<JM> &values,int size);
	void LMbubble_sort(std::vector<LMR> &values,int size);

	int GetPicTableRow(QString types,std::vector<std::pair<QString,QString>> PairTypeModelNameToPicName);
	void SetNewSideToModels(std::map<QString,std::vector<QString>>& SideToModels);//把原来按照左中右分类，变成夹模和轮模分类
	void DeleteAllItems();
	
	void ReadJT1JMSortFile(QString m_strJT1Dir);
	void ReadModelPictureNameMap(QString m_strJT1Dir);
	
	std::vector<std::pair<QString,QString>> GetCurTypeNamePic(std::vector<QString> ModelNames,std::vector<std::pair<QString,QString>> vTypeName);
	void SetPicTableBtnFrame(xMyLabel *label);
	void RefreshPicTable(std::vector<QString> values);
	void ShowOrHideTable();
	void InitDlg();
	void SetJT1();
	void InitTable();
	void InitTab1(std::vector<LMR> values);
	void InitJT1Table();
	void InitJT2Table();

	void ReFreshJT1LeftTable();
	void ReFreshJT1RightTable();
	void ReFreshJT1CenterTable();

	void GetAimJMValues(std::vector<JM> values,QString aimName,JM& aim);
	void GetAimLMValues(std::vector<LMR> values,QString aimName,LMR& aim);

	void GetJMTableValues(std::vector<JM>& values,Table *table);

	void ReadJT1ModelToLayerFileValues();
	void ReadJT1LeftFileValues();
	void ReadJT1RightFileValues();

	//
	std::vector<JM> SetLayerByFileValues(std::vector<JM> JmFileValues,std::map<QString,int> MapNameToLayerFileValues);
	std::vector<JM> AddSortNameToValues(std::vector<JM> JmFileValues,std::map<QString,std::vector<QString>> MapSortNameTovJT1JMNames);//模型结构体数据增加类型
	std::vector<JM> ReSortJMValues(std::vector<JM> JmFileValues,std::map<int,std::vector<QString>> Mapsort);//重新排列数据
	std::vector<LMR> SetLMLayerByFileValues(std::vector<LMR> JmFileValues,std::map<QString,int> MapNameToLayerFileValues);
	std::vector<LMR> AddSortNameToLMValues(std::vector<LMR> LmFileValues,std::map<QString,std::vector<QString>> MapSortNameTovJT1LMNames);//模型结构体数据增加类型
	std::vector<LMR> ReSortLMValues(std::vector<LMR> LmFileValues,std::map<int,std::vector<QString>> Mapsort);//重新排列数据

	void ResetColorByJT1SortFile(Table * table,std::vector<int> vColorRow);
	void ReadJT1JMFileValues();
	void ReadJT1CenterFileValues();

	//显示隐藏
	void ResetJT1LeftModelVector();
	void ReBuildJT1LeftModel();//显示隐藏
	void ResetJT1RightModelVector();
	void ReBuildJT1RightModel();//显示隐藏
	void ResetJT1CenterModelVector();
	void ReBuildJT1CenterModel();//显示隐藏


	void SetNormalUserWidgetLanguage(std::vector<QString> vCurWords);
	void SetIsUseSoftKeyboard(bool var);

	void SetChooseLabel(std::vector<xMyLabel*> vLabels,xMyLabel * label,std::vector<xMyLabel*> vOtherLabels);

	void WriteToCenterFile(QString fileName,const std::vector<LMR>& values,int num);

public slots:

	void JT1ModelLeftTypeChanged(xMyLabel * label);

	void ModelLeftTypeChanged(int);//無用
	void ModelLeftShowChanged(int);
	void LeftTableChanged(int i,int j);//键盘修改
	void LeftItemDoubleClicked(int i,int j);//软键盘修改

	void ModelCenterTypeChanged(int);//無用
	void ModelCenterShowChanged(int);
	void CenterTableChanged(int i,int j);
	void CenterItemDoubleClicked(int i,int j);

	void ModelRightTypeChanged(int);
	void ModelRightShowChanged(int);
	void RightTableChanged(int i, int j);
	void RightItemDoubleClicked(int i,int j);
	

	void SlotSelectJT1LTableItems();
	void SlotSelectJT1LRightItems();
	void SlotSelectJT1LCenterItems();

	void closeEvent(QCloseEvent *event);
	void Slot_Type_Selected();

signals:
	void SigChangeType(xMyLabel*);

private:
	
	void WriteToSideFile(QString fileName,const std::vector<JM>& values);

private:
	xMyLabel *m_curLabel;
	std::map<QString,QPushButton*> m_mapTypeButtons;
	QGridLayout *gLayout;
	QHBoxLayout *hLayout1;
	MainWindow *m_pMainWindow;
	QTabWidget *m_pTabWidget;
	//Table *m_table;
	NumKeyboard *numkeyboard;

	QWidget *tab1;
	QWidget *tab2;

	QTableWidget *m_JT1PicTable;
	xModelWidget* m_table1Pictures;
	xModelWidget* m_table2Pictures;

	QString m_strJT1Dir;
	QString m_strJT2Dir;

	//Table *m_JT1_LTable;
	//Table *m_JT1_RTable;
	Table *m_JT1_CTable;
	Table *m_JT1_JMTable;

	std::vector<xMyLabel*> m_JT1LeftLabels;
	std::vector<xMyLabel*> m_JT1RightLabels;

	std::vector<xMyLabel*> m_JT1JMLabels;
	std::vector<xMyLabel*> m_JT1CenterLabels;
	
	std::vector<STL*> m_AllSTLs;
	Param m_NorWidget_param;

	std::map<QString,std::vector<QString>> m_JT1SideToModels;//表格对应模型名
	std::vector<LMR> m_JT1_CFileValues;
	std::vector<JM> m_JT1_LFileValues;
	std::vector<JM> m_JT1_RFileValues;
	std::vector<JM> m_JT1_JMFileValues;

	std::vector<STL*> m_JT1_CFileSTLs;
	std::vector<STL*> m_JT1_LFileSTLs;
	std::vector<STL*> m_JT1_RFileSTLs;
	std::vector<STL*> m_JT1_JMFileSTLs;

	Table *m_JT2_LTable;
	Table *m_JT2_RTable;
	Table *m_JT2_CTable;

	std::map<QString,std::vector<QString>> m_JT2SideToModels;

	int m_iJTNum;
	bool m_bIsUseKeyboard;

	std::vector<QString> m_vCurLanguageWords;

	int m_iJT1LTablePos;
	int m_iJT1CTablePos;
	int m_iJT1RTablePos;

	std::map<QString,QString> m_MapModelToType;//文件第一列和第二列
	std::vector<std::pair<QString,QString>> m_vTypeName;//文件第一列和第三列
	std::vector<std::pair<QString,QString>> m_vCurTypeNamePic;//文件第一列和第三列  模型名对应图片名

	// 图片名与模型组名之间对应关系
	std::vector<std::pair<QString,std::vector<QString>>> modelsPicMap;

	std::map<QString,std::vector<QString>> m_MapSortNameTovJT1LMNames;
	std::map<QString,std::vector<QString>> m_MapSortNameTovJT1JMNames;
	std::map<int,std::vector<QString>> m_MapTovJT1JMSort;
	std::map<int,std::vector<QString>> m_MapTovJT1LMSort;
	std::vector<int> m_vJT1JMTableColorRow;//无用
	std::vector<int> m_vJT1LMTableColorRow;

	std::map<QString,int> m_MapFileSetModelToLayer;
};
#endif

