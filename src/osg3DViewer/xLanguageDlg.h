#ifndef XLANGUAGEDLG_H_
#define XLANGUAGEDLG_H_

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

class xLanguageDlg :public QDialog
{
	Q_OBJECT
public:
	xLanguageDlg();
	~xLanguageDlg();

	void InitDlg();
	void InitLanguageCombox(std::vector<std::vector<QString>> vModelName,QString str);
	QString GetCurLanguage(){return m_curLanguage;}

	void SetLanguageDlgLanguage(std::vector<QString> vCurWords);

public slots:
	void SlotOk();

signals:
		void SigSettingLanguageFinished();

private:
	QLabel *m_LanguageLabel;
	QComboBox *m_LanguageComBox;

	QPushButton *m_okBtn;
	QString m_curLanguage;
	std::vector<QString> m_vCurLanguageWords;

};

#endif