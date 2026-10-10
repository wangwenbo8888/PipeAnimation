#pragma once
#include "qdialog.h"

#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QColor>
#include <QCheckBox>

#include <iostream>
#include <osg/Node>
#include <QTableWidget>
#include <QWidget>
#include "Table.h"
using namespace std;

class MainWindow;
class xCollisionDetectionDlg :public QDialog
{
	Q_OBJECT
public:
	xCollisionDetectionDlg(MainWindow *pParent);
	~xCollisionDetectionDlg(void);

	void InitDlg();
	bool GetIsDetected();
	void ReBuildCollisionDetectionFile();
	std::map<QString,std::vector<std::pair<QString,int>>> GetCollisonTableValues(){return m_FileValues;}
	void SetTable(std::map<QString,std::vector<std::pair<QString,int>>> filevalues,
		          std::map<QString,QString> SymToCurLanguage,
		          std::map<std::pair<QString,QString>,int>* CollPair,
				  std::map<QString,int>* models);

	std::vector<QString> GetChosenDetectedItem(){return m_vCurCollideItemsName;}
	float GetAnimatiuonSpeed(){return m_fAnimationSpeed;}
	void InitAllChildrenCombox(std::vector<QString> strAllName);
	void ReSetTable();
	void SetCollisionDlgLanguage(std::vector<QString> vCurWords);

	public slots:
		void SlotOk();
		void SlotCancel();
		void SlotAddModel();
		void SlotDelModel();
		void SlotSelectTableItems();
		void SlotChooseAllDisplayName();
		void SlotsettingTableChanged(int,int);
		void SlotSelected(int);
		void closeEvent(QCloseEvent *event);
		void SlotSetAllCheckbox(int);
signals:
		void SigSetDetectedFinished();
		void SigSetAllCheckbox(int);

private:

	void RefreshCollisionPair();

	QHBoxLayout *m_pJiTouMoZuoHLayout;
	QCheckBox *m_pJiTouMoZuoChBox;
	QLabel *m_pJiTouMoZuoLabel;

	QHBoxLayout *m_pJiTouWanGuanHLayout;
	QCheckBox *m_pJiTouWanGuanChBox;
	QLabel *m_pJiTouWanGuanLabel;

	QHBoxLayout *m_pZhuJiaHLayout;
	QCheckBox *m_pZhuJiaChBox;
	QLabel *m_pZhuJiaLabel;

	QHBoxLayout *m_pDaoJiaHLayout;
	QCheckBox *m_pDaoJiaChBox;
	QLabel *m_pDaoJiaLabel;

	QHBoxLayout *m_pJiTouDiZuoHLayout;
	QCheckBox *m_pJiTouDiZuoChBox;
	QLabel *m_pJiTouDiZuoLabel;

	QHBoxLayout *m_pXiaoCheHLayout;
	QCheckBox *m_pXiaoCheChBox;
	QLabel *m_pXiaoCheLabel;

	QHBoxLayout *m_pDiMianHLayout;
	QCheckBox *m_pDiMianChBox;
	QLabel *m_pDiMianLabel;

	QHBoxLayout *m_pJiShenHLayout;
	QCheckBox *m_pJiShenChBox;
	QLabel *m_pJiShenLabel;

	QComboBox *m_pAnimationSpeedComBox;
	QLabel *m_pSpeedLabel;

	QLabel *m_pModelNameLabel;
	QComboBox *m_pModelComBox;

	QPushButton *m_addModelBtn;
	QPushButton *m_delModelBtn;

	QPushButton *m_pOk;
	QPushButton *m_pCancel;
	std::vector<QString> m_vDetectiveItemName;
	float m_fAnimationSpeed;

	std::vector<QString> m_vCurCollideItemsName;

	MainWindow *m_MainWindow;
	QTableWidget *m_pCollideItemsTable;
	Table *m_table;

	std::vector<QString> m_vCurLanguageWords;
	std::map<QString,std::vector<std::pair<QString,int>>> m_FileValues;

	QCheckBox *m_ChooseAllCheckBox;
	QLabel *m_ChooseAllLabel;

	QCheckBox *m_IsDetectedCheckBox;
	QLabel *m_IsDetectedLabel;

	std::map<std::pair<int,int>,QCheckBox*> m_IndexToCheckBox;
	std::map<std::pair<int,int>,QString> m_MapNameToPos;

	std::map<std::pair<QString,QString>,int>* m_pMapCollisionPair;
	std::map<QString,int>* m_pMapCollisionModel;
};

