#include "DTableWidgetItem.h"

bool DTableWidgetItem::operator<(const QTableWidgetItem &other) const
{
	QString str1 = text();
	QString str2 = other.text();
	return str1.toDouble() < str2.toDouble();
}

DTableWidgetItem::DTableWidgetItem(const QString txt /*= QString("0.000")*/)
{
	
}

DTableWidgetItem::~DTableWidgetItem()
{

}
