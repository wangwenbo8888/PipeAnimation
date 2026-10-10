#ifndef DTABLEWIDGETITEM_H
#define DTABLEWIDGETITEM_H

#include <QWidget>
#include <QTableWidgetItem>

class DTableWidgetItem : public QTableWidgetItem
{
	//Q_OBJECT
	//Q_INTERFACES(QTableWidgetItem)
public:
	explicit DTableWidgetItem(const QString txt = QString("0.000"));
	~DTableWidgetItem();

	bool operator < (const QTableWidgetItem &other) const;


};

#endif // DTABLEWIDGETITEM_H