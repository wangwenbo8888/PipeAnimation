#ifndef XREPLACEWHEELMODEL_H
#define XREPLACEWHEELMODEL_H

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
#include <QCheckBox>
#include "xModifyLMTable.h"
#include "SoftKeylineEdit.h"

class MainWindow;
class xReplaceWheelModel :public QDialog
{
	Q_OBJECT
public:
	xReplaceWheelModel(MainWindow *p);
	~xReplaceWheelModel();
	void SetLMFileValues(std::vector<LMR> valus);
	void SetJMFileValues(std::vector<JM> values);
	void InitDlg();
	void InitModelCombox(std::vector<QString> vModelName);

	double GetUpLen();
	double GetDownLen();

	//double GetJMOriginalLen(){return m_originalJMLen;}
	//double GetJMCurLen(){return m_curJMLen;}

	double GetLmR();
	double GetTuLen(){return m_TuLen;}
	int GetTuSide(){return m_TuSide;}
	int GetAoSide(){return m_AoSide;}
	int GetJTNth(){return m_JTNth;}
	int GetJMAid(){return m_AidJM;}

	double GetJMLen(){return m_JmLen;}
	double GetJMWide(){return m_JmWide;}

	bool GetIsHaveTu(){return m_isHaveTu;}

	QString GetChosenModelName();
	QString GetHighLightModelName();

	void setReplaceDlgLanguage(std::vector<QString> vCurWords);
	void SetIsUseSoftKeyboard(bool var);//∏¸æﬂ∞¥≈•…Ë÷√ «∑Ò∆Ù”√r»Ìº¸≈Ã

public slots:
	void SlotOk();
	void SlotHighLightModel();
	void closeEvent(QCloseEvent *event);
	void SlotChangeUIByModelType();
	//void SlotCancel();

signals:
	void SigSettingJMFinished();
	void SigSettingLMrFinished();
	void SigSettingLMrHighLight();

private:

	QComboBox *m_ModelTypeCombox;
	QLabel *m_ModelTypeLabel;

	QLabel *m_ModelLabel;
	QComboBox *m_ModelComBox;


	QLabel *m_LMrLabel;
	SoftKeyLineEdit *m_LMrLEdit;

	QLabel *m_UpLenLabel;
	SoftKeyLineEdit *m_UpLenLEdit;

	QLabel *m_DownLenLabel;
	SoftKeyLineEdit *m_DownLenLEdit;

	QLabel *m_TuLabel;
	SoftKeyLineEdit *m_TuLenLEdit;
	QCheckBox *m_TuCheckBox;

	QLabel *m_OriginalLenLabel;
	SoftKeyLineEdit *m_OriginalLenLEdit;

	QLabel *m_CurLenLabel;
	SoftKeyLineEdit *m_CurLenLEdit;

	QLabel *m_JmLenLabel;
	SoftKeyLineEdit *m_JmLenLEdit;

	QLabel *m_JmWideLabel;
	SoftKeyLineEdit *m_JmWideLEdit;

	QPushButton *m_OkBtn;
	QPushButton *m_CancelBtn;



	QString m_curModelName;
	double m_curLMr;
	double m_upLen;
	double m_downLen;
	double m_TuLen;
	int m_TuSide;
	int m_JTNth;

	double m_originalJMLen;
	double m_curJMLen; 

	double m_JmLen;
	double m_JmWide;
	int m_AoSide;
	int m_AidJM;//0zhu,1fu
	
	bool m_isHaveTu;
	
	std::vector<QString> m_vCurLanguageWords;

	MainWindow *m_MainWindow;
	std::vector<LMR> m_vCurLMFileValues;
	std::vector<JM> m_vCurJMFileValues;

	QGridLayout *gLayout;
	QHBoxLayout *hModelLayout;
	QHBoxLayout *hRLayout;
	QHBoxLayout *hUpLayout;
	QHBoxLayout *hDownLayout;
	QHBoxLayout *hTuLayout;
	QHBoxLayout *hBtnLayout;
	QHBoxLayout *hModelTypeLayout;
	QHBoxLayout *hOriginalJMLenLayout;
	QHBoxLayout *hCurJMLenLayout;
	QHBoxLayout *hJMLenLayout;
	QHBoxLayout *hJMWideLayout;
	QHBoxLayout *hJTNthLayout;


	QHBoxLayout *hNoneLayout;


	QLabel *m_MinLabel;
	SoftKeyLineEdit *m_MinLEdit;

	QLabel *m_MaxLabel;
	SoftKeyLineEdit *m_MaxLEdit;

	QComboBox *m_TuSideCombox;//0:◊Û£¨1”“
	QComboBox *m_AoSideCombox;//0:◊Û£¨1”“
	QComboBox *m_AidJMCombox;

	QLabel *m_JiTouNthTable;
	SoftKeyLineEdit *m_JiTouNthLEdit;

};

#endif
