
#ifndef SOFTKEYLINEEDIT_H
#define SOFTKEYLINEEDIT_H
 
#include <QLineEdit>
#include <QMouseEvent>
#include "NumberKeyboard.h"
 
class SoftKeyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SoftKeyLineEdit(QWidget *parent = 0);

	void setUseKeyboard(bool use);

protected:
    void mousePressEvent(QMouseEvent *e);
 
private:
    NumKeyboard *numkeyboard;
	bool m_bUseKeyboard;
 
signals:
 
public slots:
 
};
 
#endif // SOFTKEYLINEEDIT_H
