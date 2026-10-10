#ifndef XSLIGHTADJUST_H
#define XSLIGHTADJUST_H

#include "qdialog.h"

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

#include "SoftKeylineEdit.h"

class MainWindow;
class xSlightAdjust :public QDialog
{
	Q_OBJECT

public:

	xSlightAdjust(MainWindow *pParent=NULL);
	~xSlightAdjust();
	void InitDlg();
	void SetCurFileValue(std::vector<struct InitModelCenter> vInitModelCenterFileValues);
	void InitModelCombox(std::vector<QString> vModelName);
	int GetAxis(); //x1,y2,z3
	QString GetChosenModelName();

	// 获取模型当前中心
	osg::Vec3 GetAdjustValue();
	osg::Vec3 GetModelCenter();

	void Before();
	void SetSlightAdjustLanguage(std::vector<QString> vCurWords);

	void SetIsUseSoftKeyboard(bool var);//更具按钮设置是否启用r软键盘

	void SetCurrentItem(const QString& name);

	void SetCurrentPos(osg::Vec3 pos);

public slots:
	void showEvent(QShowEvent *event);

	void SlotOk();
	void On_Item_changed(int index);
	void SlotSetCenter(int state);
	void closeEvent(QCloseEvent *event);

	//void SlotCancel();

signals:
	void SigSettingSlightAdjustFinished();

private:
	void SetDefaultValue();

private:
	std::vector<struct InitModelCenter> m_vInitPosFileValues;
	MainWindow* m_pMainWindow;
	QComboBox *m_ModelCombox;
	QLabel *m_ModlLabel;

	QLabel *m_AxisLabel;
	QComboBox *m_AxisCombox;

	QLabel *m_ValueLabel;
	SoftKeyLineEdit *m_XValueLEdit;
	SoftKeyLineEdit *m_YValueLEdit;
	SoftKeyLineEdit *m_ZValueLEdit;

	QPushButton *m_OkBtn;
	QPushButton *m_CancelBtn;

	QLabel *m_originalcenterLabel;
	SoftKeyLineEdit *m_XCenterValueLEdit;
	SoftKeyLineEdit *m_YCenterValueLEdit;
	SoftKeyLineEdit *m_ZCenterValueLEdit;

	std::vector<QString> m_vCurLanguageWords;
};

#endif