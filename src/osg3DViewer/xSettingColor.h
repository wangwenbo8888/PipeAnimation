#ifndef XSETTINGCOLOR_H
#define XSETTINGCOLOR_H


#include "qdialog.h"
#include "Novadef.h"
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
#include <QTabWidget>
#include "xModifyLMTable.h"
#include "xSettingAnimation.h"
#include "NumberKeyboard.h"
#include "STL.h"


struct RGB_COLOE
{
	int r;
	int g;
	int b;
};

class MainWindow;
class xSettingColor :public QDialog
{
	Q_OBJECT
public:
	xSettingColor(MainWindow *p);
	~xSettingColor();

	void InitDlg();
	void InitModelComBox(std::vector<QString> v);

	void SetColorDlgLanguage(std::vector<QString> vCurWords);
	void RebuildModelColorFile();
	void SetMapValue(std::map<QString,RGB_COLOE> value);

public slots:
	void closeEvent(QCloseEvent *event);		
	void SlotChangeColor();
	void On_Item_changed( int index);

private:
	MainWindow *m_pMainWindow;

	QLabel *m_ModelLabel;
	QComboBox *m_ModelCombox;
	QColor  m_color;

	QPushButton *m_pColorBtn;

	std::map<QString,RGB_COLOE> m_MapModelNameToRgb;
};

#endif
