
#ifndef XMODELWIDGET_H_
#define XMODELWIDGET_H_

#include "qdialog.h"
#include <QTableWidget>
#include <QLabel>
#include <map>
#include "xMyLabel.h"

class MainWindow;
class xNormalUserWidget;

class xModelWidget :public QWidget
{
	Q_OBJECT

public:
	xModelWidget(xNormalUserWidget *p);
	~xModelWidget();
	void SetMainWindow(MainWindow *p);
	std::map<QString,QString> GetModelToType();
	void LoadModelPicture();

	void ReadModelPictureNameMap();

	// 设置图像和映射文件路径
	void SetFilesDir(QString dir);

	void setLabel(xMyLabel* label);
	void RefreshTypeToBtnMap(std::vector<QString> ModelNames);
	void DeleteAllItems();
	std::vector<std::pair<QString,QString>> GetCurTypeNamePic(std::vector<QString> ModelNames,std::vector<std::pair<QString,QString>> vTypeName);

public slots:

	void Slot_Type_Selected();

signals:
	void SigChangeType(xMyLabel*);

private:
	xMyLabel* m_curLabel;  // 当前操作对应表格的label
	QTableWidget* m_Widget;

	// 图像文件和类型图像名映射文件保存路径
	QString m_sDir;
	MainWindow *m_pMainWindow;
	std::map<QString,QString> m_MapModelToType;//文件第一列和第二列
	std::vector<std::pair<QString,QString>> m_vTypeName;
	std::vector<std::pair<QString,QString>> m_vCurTypeNamePic;//文件第二列和第三列

	std::map<QString,QPushButton*> m_mapTypeButtons;
};

#endif // XMODELWIDGET_H_