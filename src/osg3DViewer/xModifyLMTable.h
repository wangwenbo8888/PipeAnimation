#ifndef XMODIFYLMTABLE_H
#define XMODIFYLMTABLE_H


#include "Table.h"

#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <iostream>
#include <osg/Material>
#include <osg/Node>
#include <QCloseEvent>
#include <QWidget>
struct LMR
{
	QString strModelName;
	QString strType;
	float fLmr;
	float fupLen;
	float fdownLen;
	float ftuLen;  
	int isShow;
	int iSide;
	int iJTNth;
	int iLayer;
	int iModifiable;
	QString strSortName;
	LMR()
	{
		strModelName ="";
		strType = "";
		fLmr = 0;
		fupLen = 0;
		fdownLen = 0;
		ftuLen = 0;
		isShow = 1;
		iSide = 0;
		iJTNth = 1;
		iLayer = 1;
		iModifiable = 0;
		strSortName = "";

	}
	//int id;

};

struct JM
{
	QString strModelName;
	QString strType;
	float fUpHeight;
	float fDownHeight;
	float fLength;
	float fWide;
	int iShow;
	 // 凹槽是否朝右，朝右，则是左边夹子，凹槽朝左，是右边夹子
	int iSide; 
	int iJTNth;
	int iAid;
	int iLayer;
	int iModifiable;
	QString strSortName;

	JM()
	{
		strModelName = "";
		strType = "";
		fUpHeight = 0;
		fDownHeight = 0;
		fLength = 0;
		fWide = 0;
		iShow = 1;
		iSide = 0;
		iJTNth = 1;
		iAid = 0;
		iLayer = 1;
		iModifiable = 0;
		strSortName = "";
	}

	//int id;  // 模型位置ID，在模型参数和类型变化的时候区分是否是固定的初始位置的模型
};

class MainWindow;
class xModifyLMTable : public QDialog
{
	Q_OBJECT

public:
	xModifyLMTable(MainWindow *pParent=NULL);
	~xModifyLMTable();
	std::map<QString,int> GetModelIsShow();
	std::vector<LMR> GetCurTableLMValues();
	void InitDlg(std::vector<LMR> v);//初始化LM table


	std::map<QString,int> GetJMModelIsShow();
	std::vector<JM> GetCurTableJMValues();
	void InitJMTable(std::vector<JM> v);


public slots:
	void SlotIsShow(int irow, int icoloum);
	void SlotSelectTableItems();
	void SlotSelectJMTableItems();
	void SlotOk();
signals:

	void SigCloseThisWindow();
	void SigSetLMTableFinished();
	void SigLMTableModelIsShow();
	void SigLMTableCurValues();
private:
	Table *m_table;
	Table *m_JmTable;


	MainWindow *m_MainWindow;
	QPushButton *m_okBtn;
	std::map<QString,int> m_MapModelNameToIsShow;
	std::map<QString,int> m_MapJmModelNameToIsShow;

	QString m_strIsShowModelName;

	std::vector<LMR> m_CurTableLmValues;
	std::vector<JM> m_CurTableJmValues;

	std::vector<QString> m_vCurSelectItemNames;
	std::vector<QString> m_vCurSelectJMItemNames;

	//QCheckBox


	Table *m_pMainTable;
	Table *m_pSubTable;
};


#endif