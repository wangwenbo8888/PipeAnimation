#ifndef XMOVERANGE_H
#define XMOVERANGE_H

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

class xMoveRange :public QDialog
{
	Q_OBJECT
public:
	xMoveRange();
	~xMoveRange();
	void InitDlg();
	void InitModelCombox(std::vector<QString> vModelName);
	double GetRangeMax();
	double GetRangeMin();
	QString GetChosenModelName();
	QString GetHighLightModelName();
public slots:
	void SlotOk();
	void SlotHighLightModel();
	//void SlotCancel();

signals:
	void SigSettingMoveRangeFinished();
	void SigSettingMoveRangeModelHighLight();

private:
	QLabel *m_ModelLabel;
	QComboBox *m_ModelComBox;

	QLabel *m_MinLabel;
	QLineEdit *m_MinLEdit;

	QLabel *m_MaxLabel;
	QLineEdit *m_MaxLEdit;

	QPushButton *m_OkBtn;
	QPushButton *m_CancelBtn;

	QString m_curModelName;
};

#endif
