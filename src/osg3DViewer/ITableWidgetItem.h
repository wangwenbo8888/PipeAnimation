#ifndef ITABLEWIDGETITEM_H
#define ITABLEWIDGETITEM_H

#include <QWidget>
#include <QTableWidgetItem>


class ITableWidgetItem : public QTableWidgetItem
{

public:
	explicit ITableWidgetItem(const QString txt = QString("0"));
	~ITableWidgetItem();

	bool operator < (const QTableWidgetItem &other) const
	{
		QString str1 = text();
		QString str2 = other.text();
		return str1.toInt() < str2.toInt();
	}

};

#endif // ITABLEWIDGETITEM_H