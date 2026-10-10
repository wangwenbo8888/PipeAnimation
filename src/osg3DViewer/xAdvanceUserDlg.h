#ifndef XADVANCEUSERDLG_H
#define XADVANCEUSERDLG_H 

#include <QLabel>
#include "SoftKeylineEdit.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

class MainWindow;
class xAdvanceUserDlg :public QDialog
{
	Q_OBJECT
public:
	xAdvanceUserDlg(MainWindow *p);
	~xAdvanceUserDlg();
	void InitDlg();
	void clearPassword();
	QString GetPassword(){return m_passwordLEdit->text();}
	void SetIsUseSoftKeyboard(bool var);//更具按钮设置是否启用r软键盘

public slots:
	void SlotOk();


signals:
	void SigInputAdvancePasswordFinished();

private:
	QLabel *m_AdvancePasswordLabel;
	SoftKeyLineEdit *m_passwordLEdit;
	MainWindow *m_MainWindow;
	QPushButton *m_OkBtn;

};

#endif
