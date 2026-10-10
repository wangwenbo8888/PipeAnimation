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
#include <QTextEdit>
#include <iostream>
#include <osg/Node>
#include <QCloseEvent>

#include "SoftKeylineEdit.h"

class MainWindow;
class xRotateTest :public QDialog
{
	Q_OBJECT
public:
	xRotateTest(MainWindow*);
	~xRotateTest();
	void InitDlg();
	void InitModelCombox(std::vector<QString> vModelName);
	double GetRotateAngle();
	int GetAxis(); //x1,y2,z3
	QString GetChosenModelName();
	osg::Vec3 GetRotateCenter();

	void SetRotateTestDlgLanguage(std::vector<QString> vCurWords);

	//更具按钮设置是否启用r软键盘
	void SetIsUseSoftKeyboard(bool var);
public slots:
	void SlotOk();
	void closeEvent(QCloseEvent *event);
	void On_Item_changed(int index);

signals:
	void SigSettingRotateTestFinished();

private:
	QComboBox *m_ModelCombox;
	QLabel *m_ModlLabel;

	QLabel *m_AxisLabel;
	QComboBox *m_AxisCombox;

	QLabel *m_RotateCenterLabel;
	SoftKeyLineEdit *m_XRotateCenterLEdit;
	SoftKeyLineEdit *m_YRotateCenterLEdit;
	SoftKeyLineEdit *m_ZRotateCenterLEdit;

	QLabel *m_RotateValueLabel;
	SoftKeyLineEdit *m_RotateValueLEdit;

	QPushButton *m_OkBtn;
	QPushButton *m_CancelBtn;

	double m_curRotateAngle;
	double m_sumRotateAngle;

	MainWindow *m_MainWindow;
	std::vector<QString> m_vCurLanguageWords;
};

