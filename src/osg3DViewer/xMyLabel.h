#ifndef XMYLABEL_H
#define XMYLABEL_H

#include <QLabel>


class xMyLabel : public QLabel
{
	Q_OBJECT
public:
	  explicit xMyLabel(QWidget *parent = 0);
	  ~xMyLabel();
	  void SetCurRowToModelName(QString str);
	  void SetType(QString str);
	  void SetName(QString str);
	  void SetJTNth(int num);
	  void SetSide(QString side);
	  void SetModifiable(int value);
	  void SetPicTableRow(int irow);

	  QString GetCurRowToModelName();
	  QString GetType();
	  QString GetName();
	  QString GetSide();
	  int GetModifiable();
	  int GetJTNth();
	  int GetPicTableRow();
		  

private:
	int m_PicTableRow;
	int m_strModifiable;
	QString m_strType;
	QString m_strModelName;
	int m_iJTNth;
	QString m_strSide;
	QString m_strCurRowToModelName;
};



#endif