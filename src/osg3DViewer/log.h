
#ifndef LOG_H_
#define LOG_H_

#include <QString>

#define FILE_NAME __FILE__
#define LINE_COUNT __LINE__

#define getCodeLocate() QString("line %1 in file %2").arg(LINE_COUNT).arg(FILE_NAME)

void Log(QString message);

#define outputInfo() Log(getCodeLocate());

//#define LogInfo(message) outputInfo() Log(message);

#define LogInfo(message) Log(message);

#endif //LOG_H_