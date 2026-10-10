
#include "log.h"

#include <QDebug>
#include <QDataStream>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <qdatetime.h>

void Log(QString message)
{
	QString path = QCoreApplication::applicationDirPath();
	QDate currentdate = QDate::currentDate();
	QString data = currentdate.toString("yyyy-MM-dd");
	path.append("//");
	QString fileName = path+data;
	fileName.append(".log");

	QFile file(fileName);
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	if(!file.isOpen())
	{
		qDebug() << "open file error !";
		qDebug() << "filename: " <<  __FILE__ << "line: " <<  __LINE__ ;
		return;
	}

	QTextStream stream(&file);
	QString str;

	QDateTime datatime;
	str += datatime.currentDateTime().toString("hh:mm:ss.zzz");
	str += ": ";
	str += message;
	str += "\r\n";
	stream << str  ;

	file.close();
}
