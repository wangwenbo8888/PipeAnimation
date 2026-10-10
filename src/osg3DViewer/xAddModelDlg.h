#ifndef ADDMODEL_H
#define ADDMODEL_H

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
#include "SoftKeylineEdit.h"

class MainWindow;
class xAddModel :public QDialog
{
	Q_OBJECT
public:
	xAddModel(MainWindow *p);
	~xAddModel();
	void InitDlg();
	void InitModelCombox(std::vector<QString> vModelName);

	int GetNthJT(){return m_iNthJT;}
	int GetUpUnits(){return m_curUpUnits;}
	QString GetNewModelName(){return m_NewModelName;}
	QString GetChosenModelName();
	QString GetHighLightModelName();

	void SetAccumulateDlgLanguage(std::vector<QString> vCurWords0);
	void SetIsUseSoftKeyboard(bool var);//更具按钮设置是否启用r软键盘

public slots:
	void SlotOk();
	void SlotHighLightModel();
	void closeEvent(QCloseEvent *event);


signals:
	void SigSettingAddModelFinished();
	void SigSettingAddModelHighLight();

private:
	QLabel *m_ModelLabel;
	QComboBox *m_ModelComBox;
	
	QLabel *m_NewModelNameLabel;
	SoftKeyLineEdit *m_NewModelNameLEdit;

	QLabel *m_NewUpUnitsLabel;
	SoftKeyLineEdit *m_NewUpUnitsLEdit;

	QPushButton *m_OkBtn;
	QPushButton *m_CancelBtn;

	MainWindow *m_MainWindow;

	QString m_NewModelName;
	QString m_CopyModelName;
	int m_curUpUnits;
	int m_iNthJT;

	QLabel *m_JTNthLabel;
	SoftKeyLineEdit *m_JTNthLEdit;
	
	std::vector<QString> m_vCurLanguageWords;
};

#endif
