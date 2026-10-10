#ifndef NUMBER_KEYBOARD_H
#define NUMBER_KEYBOARD_H
 
#include <QDialog>
#include <QtDebug>
#include <QKeyEvent>
#include <QEvent>
#include <QLineEdit>
#include <QPushButton>

//QPushButton#keyboard{		
//	/* 控件最小宽度 */  
//	width:50px;
//	height:50px;	
//	/* 字体 加粗 大小 字体种类  PS: qss不支持中文设置字体*/  
//	font: bold 30px "Arial Black,Microsoft YaHei";  	
//	border-style: outset;
//	border-width: 2px;
//	border-color: white;
//	color: rgb(255, 255, 255);	
//	background-color: rgba(33, 131, 128, 255);	
//}

//QLineEdit#keyboard {
//	/* 最小宽度 */
//	min-width:200px;	
//	/* 边框样式 */
//	border: 2px solid white;		
//	border-style: inset;	
//	/* 内边框:上下和左右的距离 */
//	padding: 0 8px;		
//	/* 背景色 */
//	background: rgb(33, 131, 128);	
//}
 
class NumKeyboard : public QDialog
{
    Q_OBJECT
 
public:
    explicit NumKeyboard(QWidget *parent = 0);
    ~NumKeyboard();
    bool valid;
 
    void setText(QString str);
    QString getText();
 
    QPushButton *num6Button;
    QPushButton *backspaceButton;
    QPushButton *num4Button;
    QPushButton *okButton;
    QPushButton *leftButton;
    QPushButton *num1Button;
    QPushButton *cancelButton;
    QPushButton *rightButton;
    QPushButton *num9Button;
    QPushButton *num8Button;
    QPushButton *num2Button;
    QPushButton *num7Button;
    QPushButton *dotButton;
    QPushButton *num3Button;
    QPushButton *num0Button;
    QPushButton *num5Button;
    QPushButton *signButton;
    QLineEdit *lineEdit;
 
protected:
    void changeEvent(QEvent *e);
//    void keyPressEvent(QKeyEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);
 
private slots:
    void buttonClickResponse(int gemfield);
 
private:
    QString strContent;
};
 
#endif // NUMKEYBOARD_H
