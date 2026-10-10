#pragma once
#include "qdialog.h"
#include <QCheckBox>
#include <QRadioButton>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QColor>
#include <QTextEdit>
#include <iostream>
#include <osg/Node>
#include <QTableWidget>
#include "STL.h"
#include "SoftKeylineEdit.h"


struct GroupChildren
{
	// 初始位置
	float fx;
	float fy;
	float fz;
	std::vector<QString> vChildrenNames;
	// 此节点下的节点
	std::vector<QString> vNodes;
	osg::Vec3d currentPos;
	float m_fAngle;
	STL* node;

	GroupChildren()
	{
		fx = -10000.0;
		fy = -10000.0;
		fz = -10000.0;

		currentPos = osg::Vec3d(fx,fy,fz);
		m_fAngle = 0.0;
		node = NULL;
	}
};

using namespace std;
class MainWindow;

class xSettingNodeGroupDlg: public QDialog
{
	Q_OBJECT
public:
	xSettingNodeGroupDlg(MainWindow *parent);
	~xSettingNodeGroupDlg();

	void InitDlg();
	void InitJTNum(int num);
	void InitAllChildrenCombox(std::vector<QString> vChildrenName);
	std::map<QString,GroupChildren> GetGroupToGroupChildrenMap(){return m_GroupToGroupChildrenMap;}
	std::map<int,std::map<QString,std::vector<QString>>> GetJTSideModelNames(){return m_JTNumMapToSideToModelNames;}
	void DivideSide();

	std::map<QString,int> GetGroupToAxisMap();
	void DelGroupToAxis(QString str);
	void SetGroupToChildrenMap(std::map<QString,GroupChildren> GroupToGroupChildrenMap);
	void SetJTSideModelsMap(std::map<int,std::map<QString,std::vector<QString>>> map);
	void UpdataDlg();
	bool GetIsRebuildLM(){return m_isRebuildLM;}
	osg::Vec3 GetGroupPos(){return m_GroupPos;}
	void setGroupNodeDlgLanguage(std::vector<QString> lang);

	void SetIsUseSoftKeyboard(bool var);//更具按钮设置是否启用r软键盘

public slots:
	void SlotChangeChildrenCombox();
	void SlotSetSideUI();
	void SlotSetGroupUI();
	void SlotPosBtn();
	void SlotOk();
	void SlotCancel();
	void SlotAddGroup();//新建组
	void SlotDelGroup();
	void SlotDelChildToGroup();//删除组成员
	void SlotAddChildToGroup();
	void SlotResetChildrenText();//更新显示框  组
	void SlotResetSideChildrenText();//边
	void SlotSelectTableItems();
	void closeEvent(QCloseEvent *event);

signals:
	void SigSettingGroupFinished();
	void SigSettingJTSideFinished();

private:
	// 删除边分组成员
	void DeleteSideMember();

private:
	QButtonGroup *m_UiTypeBtnGroup;
	QRadioButton *m_isGroupRBtn;
	QRadioButton *m_isSideRBtn;
	QLabel *m_sideLabel;
	QComboBox *m_sideCombox;

	QLabel *m_JTNumLabel;
	QComboBox *m_JTNumComBox;

	QLabel *m_GroupNameLabel;
	SoftKeyLineEdit *m_GroupNameLEdit;
	QPushButton *m_addGroupBtn;
	QPushButton *m_delGroupBtn;

	QLabel *m_allGroupNameLabel;
	QComboBox *m_allGroupNameCombox;
	QLabel *m_allchildrenNameLabel;
	QComboBox *m_allchildrenNameCombox;
	QPushButton *m_addChildrenBtn;
	QPushButton *m_delChildrenBtn;

	QLabel *m_AxisLabel;
	QComboBox *m_AxisComBox;

	QLabel *m_RebuildLMLabel;
	QCheckBox *m_RebuildLMCheckBox;

	QLabel *m_PosLabel;
	SoftKeyLineEdit *m_XPosLEdit;
	SoftKeyLineEdit *m_YPosLEdit;
	SoftKeyLineEdit *m_ZPosLEdit;
	QPushButton *m_PosBtn;

	osg::Vec3 m_GroupPos;

	QPushButton *m_okBtn;
	QPushButton *m_CancelBtn;

	QTextEdit *m_ChildenNameText;

	std::map<std::pair<int,QString>,std::vector<QString>> m_PairTovModelNames;

	std::map<int,std::map<QString,std::vector<QString>>> m_JTNumMapToSideToModelNames;
	std::map<QString,GroupChildren> m_GroupToGroupChildrenMap;//组名与组结构体
	std::map<QString,std::vector<QString>> m_GroupToChildrenMap;//无用
	std::map<QString,int> m_GroupToAxisMap;

	MainWindow *m_MainWindow;
	QTableWidget *m_pChildItemsTable;

	
	bool m_isRebuildLM;
	std::vector<QString> m_vlanguage;

	std::vector<QString> m_vCurLanguageWords;



	QHBoxLayout *hSideLayout;
	QHBoxLayout *hJTNumLayout;
	QHBoxLayout *gNLayout;
	QHBoxLayout *hgroupLayout;
	QHBoxLayout *hchildLayout;
	QHBoxLayout *rebuildLayout;
	QHBoxLayout *posLayout;
	QGridLayout *glayout;
};