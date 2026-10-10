#ifndef XSPEEDDLG_H
#define XSPEEDDLG_H

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

class xSpeedDlg :public QDialog
{
	Q_OBJECT

public:
	xSpeedDlg();
	~xSpeedDlg();
	void InitDlg();

	int GetCurSpeed(){return m_SpeedSlider->value();}

public slots:
	void SlotOk();

	void RefreshLabel(int);

signals:
	void SigSettingSpeedFinished();

private:
	QLabel * m_SpeedLabel;
	QLabel * m_SpeedValue;
	QSlider* m_SpeedSlider;

	//QPushButton *m_okBtn;
};

#endif  // XSPEEDDLG_H