#include "log.h"

#include <QDebug>
#include <QDataStream>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <qdatetime.h>

void Log(QString message)
{
	// Perf 2026-10: keep the file handle open across calls.
	// The old implementation did open/write/close on EVERY log line,
	// which throttled the whole application (collision timer logs a lot).
	static QFile s_file;
	static QTextStream s_stream(&s_file);
	static QString s_curDate;

	QString data = QDate::currentDate().toString("yyyy-MM-dd");
	if (s_curDate != data || !s_file.isOpen())
	{
		if (s_file.isOpen())
		{
			s_stream.flush();
			s_file.close();
		}
		QString path = QCoreApplication::applicationDirPath();
		path.append("//");
		QString fileName = path + data;
		fileName.append(".log");
		s_file.setFileName(fileName);
		if (!s_file.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			return;
		}
		s_stream.setDevice(&s_file);
		s_curDate = data;
	}

	QDateTime datetime;
	s_stream << datetime.currentDateTime().toString("hh:mm:ss.zzz") << ": " << message << "\r\n";
	s_stream.flush();
}
