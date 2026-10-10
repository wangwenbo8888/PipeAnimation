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

#include <iostream>
#include "SoftKeylineEdit.h"

using namespace std;



class MainWindow;
class VerificationCodeDlg :public QDialog
{
	Q_OBJECT
public:
	VerificationCodeDlg();
	~VerificationCodeDlg(void);
	void setUserNum(std::string strnum);
	void InitDialog();
	QString GetCode(){return m_pCodeLineEdit->text();}
	void SetIsUseSoftKeyboard(bool var);//更具按钮设置是否启用r软键盘


public slots:
	void SlotOk();
	
signals:
	void sigGetCode(const QString &code);


private:
	QPushButton *m_pOk;
	QPushButton *m_pCancel;
	QHBoxLayout *m_pBtnHLayout;
	QGridLayout* m_pBtnGLayout;
	QLabel *m_pCodeLabel;
	QLineEdit *m_pCodeLineEdit;
	QLabel *m_userLabel;
	QComboBox *m_userCombox;


	QLabel *m_userNumLabel;
	QLineEdit *m_UserNumLEdit;
};

