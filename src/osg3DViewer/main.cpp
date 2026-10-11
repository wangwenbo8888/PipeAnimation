/******************************************************************************
   osg3DViewer: a 3D file viewer
   Copyright(C) 2015-2015  ronggenmiao@163.com

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software

 *******************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QPlastiqueStyle>
#include <QtGui/QDesktopWidget>
#include <QtCore/QTextStream>

#include <QMessageBox>

//#include <QApplication>
#include <QFile>
#include <QDebug>

#include <osgDB/Registry>

#include "MainWindow.h"
#include "VerificationCodeDlg.h"

#include "Encode.h"

//#include <QTextCodec>

#include <string.h>
#include <Windows.h>    
#include "CPUID.h"
#include "MacAddress.h"
//#include "pch.h"//vs控制台项目自动加入的头文件
#include "HardDriveSerialNumer.h"
//#include "language.h"

#include "log.h"

char sBuf[1024];
char *ptr;

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

void usage()
{
    QTextStream out(stdout);
    out << endl;
    out << QObject::tr("OSG viewer") << endl;
    out << QObject::tr("Usage: ") << QCoreApplication::arguments().at(0) << " " << QObject::tr("[options]") << " " << QObject::tr("filename") << endl;
    out << endl;
    out << "Following options are known:" << endl;
    out << QObject::tr(" --help                 - displays this help.") << endl;
    out << QObject::tr(" --reset-config         - clear the saved preference parameters.") << endl;
    out << endl;
}

WCHAR* CHARTOWCHAR(const char * pchar)
{
	WCHAR *m_pWchar = NULL;

	int outlen = MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0);
	m_pWchar = new WCHAR[outlen];
	MultiByteToWideChar(CP_ACP,0,pchar,-1,m_pWchar,outlen);

	return m_pWchar;
}

int GetRegValue(std::string str)
{
	char filename[256] = "";  
	QString fileName = QCoreApplication::applicationDirPath();
	strcpy(filename,fileName.toStdString().c_str());

	strcat(filename, "\\dind99844plde~ni3dffdb163ef");
	BOOL isexist = ExistFile(filename);
	FILE *m_file = NULL;
	if(!isexist)
	{
		m_file = fopen(filename,"w");
		fputs("0",m_file);
		fclose(m_file);
		SetFileAttributesA(filename,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
		return -2;
	}
	else
	{
		SetFileAttributesA(filename,FILE_ATTRIBUTE_NORMAL);
		char data[256];
		m_file = fopen(filename,"r");
		fgets(data, 256, m_file);
		std::string readCode(data);
		if(readCode != str) 
		{
			fclose(m_file);
			SetFileAttributesA(filename,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
			return -1;
		}
		else
		{
			fclose(m_file);
			SetFileAttributesA(filename,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
			return 1;
		}
	}
}

void RegistOK(std::string code)
{
	char filename[256] = "";
	//char   MyDocumentDir[MAX_PATH];  
	QString fileName = QCoreApplication::applicationDirPath();
	strcpy(filename,fileName.toStdString().c_str());

	strcat(filename, "\\dind99844plde~ni3dffdb163ef");
	FILE *m_file = NULL;
	SetFileAttributesA(filename,FILE_ATTRIBUTE_NORMAL);
	m_file = fopen(filename,"w");
	fputs(code.c_str(),m_file);  
	fclose(m_file);
	SetFileAttributesA(filename,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);

	return;
}

void SetAdminCode(std::string code)
{
	char filename[256] = "";
	//char   MyDocumentDir[MAX_PATH];  
	QString fileName = QCoreApplication::applicationDirPath();
	strcpy(filename,fileName.toStdString().c_str());

	strcat(filename, "\\dind77844plde~ni3dffdb163ef");
	FILE *m_file = NULL;
	SetFileAttributesA(filename,FILE_ATTRIBUTE_NORMAL);
	m_file = fopen(filename,"w");
	fputs(code.c_str(),m_file);  
	fclose(m_file);
	SetFileAttributesA(filename,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);

	return;
}

//void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
//{
//	QString txt = str;
//
//	QString file = QDateTime::currentDateTime().toString(QString("yyyy-MM-dd"));
//	file.append(".log");
//	QFile outFile(file.toStdString().c_str());
//	outFile.open(QIODevice::WriteOnly | QIODevice::Append);
//	QTextStream ts(&outFile);
//	ts << txt << endl;
//}

int main(int argc, char *argv[])
{
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));

	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    Q_INIT_RESOURCE(osg3DViewer);

    QApplication app(argc, argv);

	LogInfo("=== APP START (QApplication created) ===");

	QSharedMemory singleton(app.applicationName());
	if (!singleton.create(1))
	{    //已经存在的
		return -1;
	}

	/*if (GetModuleFileNameA(NULL, sBuf, sizeof(sBuf)))
	{
	ptr = strrchr(sBuf, '\\');
	if (ptr)
	*ptr = '\0';
	SetCurrentDirectoryA(sBuf);
	}*/

    // add the current path
    osgDB::Registry::instance()->getDataFilePathList().push_back(qApp->applicationDirPath().toStdString ());
    osgDB::Registry::instance()->getDataFilePathList().push_back(QString(qApp->applicationDirPath() + "/data/").toStdString ());
    osgDB::Registry::instance()->getDataFilePathList().push_back(QString(qApp->applicationDirPath() + "/../data/").toStdString ());
    osgDB::Registry::instance()->getDataFilePathList().push_back(QString(qApp->applicationDirPath() + "/../../data/").toStdString ());
    osgDB::Registry::instance()->getDataFilePathList().push_back("/usr/local/share/osg3DViewer/data/");
    osgDB::Registry::instance()->getDataFilePathList().push_back("/usr/share/osg3DViewer/data/");

    // in order to speed up the picking process ...
    osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::ReaderWriter::Options::BUILD_KDTREES);

    //MiscFunctions::setDefaultLanguage();

    QApplication::setStyle(new QPlastiqueStyle);

    // check for special argument
    bool forceResetConfig = false;
    QString filename;
    QStringList args = QApplication::arguments();
    for (int i = 1; i < args.count(); ++i)
    {
        const QString arg = args.at(i);

        if (arg == "--reset-config")
        {
            forceResetConfig = true;
        }
        else if (arg == "--help")
        {
            usage();
            return 0;
        }
        else
        {
            filename = arg;
        }
    }

    //AppSettings settings;

    //// in order to display splashscreen on the same screen than application ...
    //settings.beginGroup("MainWindow");
    //int screenNumber = settings.value("screenNumber",0).toInt();
    //settings.endGroup();

    //settings.beginGroup("Application");

    //// reset the saved configuration if needed
    //bool resetConfig = settings.value("resetConfig",false).toBool();
    //if (resetConfig || forceResetConfig)
    //{
    //    settings.clear();
    //    settings.sync();
    //}

    //bool splashscreenAtStartup = settings.value("splashscreenAtStartup",true).toBool();
    //bool splashscreenTransparentBackground = settings.value("splashscreenTransparentBackground",true).toBool();
    //settings.endGroup();

    //// splash screen
    //if (splashscreenAtStartup)
    //{
    //    SplashScreen sScreen(QPixmap(":/osGraphX/splashscreen.png"), 3000, screenNumber,splashscreenTransparentBackground);
    //    sScreen.show();
    //}

	LogInfo("[perf] MainWindow ctor begin");
	MainWindow w;
	LogInfo("[perf] MainWindow ctor end");

#if 0

	MD5 algo1;
	MD5 algo2;
	std::string strCpuID = GetCPUID();

	std::string user = "Normal";
	std::string strCpuIDNormal = strCpuID + user; 
	std::string strNormal = algo1.encode(strCpuIDNormal);

	user = "Admin";
	std::string strCpuIDAdmin = strCpuID + user; 
	std::string strAdmin = algo2.encode(strCpuIDAdmin);


	if (GetRegValue(strNormal) > 0 )
	{
		// 普通
		//w.HideAction(false);
		
	}
	else if (GetRegValue(strAdmin) > 0 )
	{
		// 高级
		//w.HideAction(true);

	}

	else /*(GetRegValue(str) < 0)*///判断加密码是否存在于文件中
	{
		//验证码
		VerificationCodeDlg CodeDlg;
		CodeDlg.show();

		if (CodeDlg.exec()==QDialog::Accepted)
		{
			//软件加密
			std::string tmp;
			//GetMacByGetAdaptersInfo(tmp);
			//std::string strMacAddress = tmp;
			
			if (CodeDlg.GetCode().toStdString() == strAdmin)
			{
				RegistOK(strAdmin);
				// 高级
				//w.HideAction(true);

			}
			else if (CodeDlg.GetCode().toStdString() == strNormal)
			{
				RegistOK(strNormal);
				// 普通
				//w.HideAction(false);

			}
			else 
			{
				QMessageBox::warning(NULL, "Regist failed !", " Regist code error !",QMessageBox::Ok);
				return 0;
			}

		}
		else
		{
			return 0;
		}
	}
#endif

		//int out = 0,i=10;
		//MasterHardDiskSerial a;
		//char SerialNumber[1024] = {""};
		//std::cout << "i" << i-- << std::endl;
		//memset(&SerialNumber,0,sizeof(SerialNumber));
		//out = a.GetSerialNo(SerialNumber);
		//std::cout << "SN " << SerialNumber << std::endl;
		//原文链接：https://blog.csdn.net/yanchenyu365/article/details/85061879
	QProcess p;
	QString cmd("wmic diskdrive where index=0 get serialnumber");
	p.start(cmd);
	p.waitForFinished();
	QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
	QStringList list = cmd.split(" ");
	result = result.remove(list.last(), Qt::CaseInsensitive);
	result = result.replace("\r", "");
	result = result.replace("\n", "");
	result = result.simplified();

	std::string diskID = result.toStdString();
	std::string strCpuID = GetCPUID();
	std::string resultID = strCpuID + diskID;

	MD5 algo;
	std::string strID = algo.encode(resultID);
	MD5 algo2;
	std::string str = strID + "88";
	str = algo2.encode(str);

	if (GetRegValue(str) < 0)
	{
		//验证码
		VerificationCodeDlg CodeDlg;
		CodeDlg.setUserNum(strID);
		CodeDlg.show();

		if (CodeDlg.exec()==QDialog::Accepted)
		{
			//软件加密
			std::string tmp;
			//GetMacByGetAdaptersInfo(tmp);
			//std::string strMacAddress = tmp;

			if (CodeDlg.GetCode().toStdString() == str)
			{
				RegistOK(str);
			}
			else
			{
				QString needcode = QString::fromStdString(strID);
				QMessageBox::warning(NULL, "Regist failed !", QString::fromLocal8Bit(" Regist code error !")/*.arg(needcode)*/,QMessageBox::Ok);
				return 0;
			}

		}
		else
		{
			return 0;
		}
	}
	LogInfo("Regist success !");

	// 设置高级账户密码
	SetAdminCode("666666");
    w.HideAction(false);

	LogInfo("step 1 success !");

    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    w.showMaximized();
	LogInfo("step 2 success !");

	w.LoadAllModel();
	LogInfo("step 3 success !");

	w.ReadFilesAtLoad();//读取加工文件
	LogInfo("step 4 success !");

	w.InitProductionTable();
	LogInfo("step 5 success !");

	w.InitTwTable();
	LogInfo("step 6 success !");

	w.ReadLanguageFile();
	LogInfo("step 7 success !");

	//导入累加模型   1
	w.LoadAccumulateModelsFromTheFile();
	LogInfo("step 8 success !");

	//变换模型       2
	w.ReBuildLmVectorAtLoad();
	LogInfo("step 9 success !");

	w.ReBuildJMVectorAtLoad();
	LogInfo("step 10 success !");

	w.ReadAsSpecialReplaceModelFile();

	//设置组和STLs关系
	w.SetGroupToSTLS();
	LogInfo("step 11 success !");

	//更新模型位置   3
	w.ResetModelPosByInitPosFile();
	LogInfo("step 12 success !");

	//加载普通用户文件模型
	w.ReadNormalUserModels();
	LogInfo("step 13 success !");

	w.ResumeAllModelColor();
	LogInfo("step 14 success !");

	w.ReadModelDiplayNameFile();
	LogInfo("step 15 success !");

	w.SetViewLookAtByFileValue();
	LogInfo("step 16 success !");

	w.ReadModelColorFileValue();
	LogInfo("step 17 success !");

	// 刷新普通用户界面设置
	w.RefreshNormalUserSetting();

    return app.exec();
}
