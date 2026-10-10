#include "xMyLabel.h"

xMyLabel::xMyLabel(QWidget *parent /*= 0*/)
{

}

xMyLabel::~xMyLabel()
{

}

void xMyLabel::SetCurRowToModelName(QString str)
{
	m_strCurRowToModelName = str;
}

void xMyLabel::SetType(QString str)
{
	m_strType = str;
}

void xMyLabel::SetName(QString str)
{
	m_strModelName = str;
}

void xMyLabel::SetJTNth(int num)
{
	m_iJTNth = num;
}

void xMyLabel::SetSide(QString side)
{
	m_strSide = side;
}

void xMyLabel::SetModifiable(int value)
{
	m_strModifiable = value;
}

void xMyLabel::SetPicTableRow(int irow)
{
	m_PicTableRow = irow;
}

QString xMyLabel::GetCurRowToModelName()
{
	return m_strCurRowToModelName;
}

QString xMyLabel::GetType()
{
	return m_strType;
}

QString xMyLabel::GetName()
{
	return m_strModelName;
}

QString xMyLabel::GetSide()
{
	return m_strSide;
}

int xMyLabel::GetModifiable()
{
	return m_strModifiable;
}

int xMyLabel::GetJTNth()
{
	return m_iJTNth;
}

int xMyLabel::GetPicTableRow()
{
	return m_PicTableRow;

}
