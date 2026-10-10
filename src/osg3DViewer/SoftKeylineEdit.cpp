#include "SoftKeylineEdit.h"
 
SoftKeyLineEdit::SoftKeyLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
	m_bUseKeyboard = true;
    numkeyboard = new NumKeyboard();
}

void SoftKeyLineEdit::setUseKeyboard(bool use)
{
	m_bUseKeyboard = use;
}
 
void SoftKeyLineEdit::mousePressEvent(QMouseEvent *e)
{
	if (m_bUseKeyboard)
	{
		return;
	}

    if(e->button() == Qt::LeftButton)
    {
        numkeyboard->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        numkeyboard->setStyleSheet("border:2px solid white;");
		//numkeyboard->setGeometry(0,300);
		numkeyboard->move(80,80);

        numkeyboard->setText(this->text());  //当前的文本框的内容，this->指向的是SoftKeyLineEdit        
        numkeyboard->exec();
        if(numkeyboard->valid)
        {
            this->setText(numkeyboard->getText());  //此处的setText是继承QLineEdit的，numkeyboard->setTex是自定义的，目的是获取当前的文本
        }

    }
}