#ifndef XOPTIMIZEDLG_H
#define XOPTIMIZEDLG_H

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

class xOptimizeDlg :public QDialog
{
	Q_OBJECT

public:
	xOptimizeDlg();
	~xOptimizeDlg();
	void InitDlg();

	int GetType(){return m_OptimizeType;}

public slots:
	void SlotOk();

	void RefreshLabel(int);

	void On_Curve_To_Straight();
	void On_ModifyYBC();
	void On_Exit();

signals:
	void SigSettingSpeedFinished();

private:
	//QLabel * m_SpeedLabel;
	int m_OptimizeType;
	QSlider* m_SpeedSlider;

	QPushButton *m_PushButtonCurveToStraight;
	QPushButton *m_PushButtonModifyYBC;
	QPushButton *m_PushButtonExit;
	//QPushButton *m_okBtn;
};

#endif  // XOPTIMIZEDLG_H