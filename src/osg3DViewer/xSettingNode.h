#pragma once
#include "qdialog.h"

#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QColor>
#include <QTextEdit>
#include <iostream>
#include <osg/Node>

#include "STL.h"
using namespace std;



class MainWindow;
class xSettingNodeDlg :public QDialog
{
	Q_OBJECT
public:
	xSettingNodeDlg(MainWindow *);
	~xSettingNodeDlg(void);
	
	void InitDialog();
	void setNodeValue(osg::Node* node);
	int GetOpeType();
	int GetNodeType();
	QColor GetNodeColor();
	void SetVecGNodes(std::vector<osg::Group *>);
	QString getFatherNodeName();
	QString getAddNodeName();
	QComboBox* getNodeTypeCombox();
	void setCurLineEditFileName(QString str);
	void setCurNodeName(QString name);
	void initCombox();
	void ResetFatherCombox(QString strFather);
	void AddFatherCombox(QString strFather,int i);
	void DelFatherCombox(std::vector<QString> strFather,int i);

	void InitFatherCombox();
	void setIsDel(bool isDel){m_IsDel = isDel; m_IsAdd = isDel;}
	bool GetIsDel(){return m_IsDel;}
	bool GetIsAdd(){return m_IsAdd;}
	const QString GetCurFatherName();
	const QString GetCurChildName();
	void GetAllGChildrenByFather(QString strFa);
	std::vector<QString> GetAllDelNode();

	void AddFatherToChildrenMap(QString strFa,QString strChild);
	std::pair<QString,QString> GetAddChildToFatherPair(){return m_AddChildToFatherPair;}
	std::pair<QString,QString> GetDelChildToFatherPair(){return m_DelChildToFatherPair;}


public slots:
	void SlotColor();
	void SlotOpeType();
	void SlotNodeType();
	void SlotOk();
	void SlotCancel();
	void AddChoics(int);
	void SlotAddChildToFather();
	void SlotDelChildToFather();
	void ResetChildrenText();

	
signals:
	void sigSetiingNodeFinished(osg::Node *,const QString& name);
	void sigAddChildToFather(QString strChildName,QString strFatherName);
	void sigDelChildToFather(QString strChildName,QString strFatherName);

private:
	QPushButton *m_pAddNodeBtn;
	QPushButton *m_pDelNodeBtn;
	QTextEdit *m_ChildenNameText;

	QPushButton *m_pOk;
	QPushButton *m_pCancel;
	QPushButton *m_pColorBtn;
	QHBoxLayout *m_pBtnHLayout;
	QGridLayout* m_pBtnGLayout;
	QLabel *m_pFatherNodeLabel;
	QLabel *m_pNodeTypeLabel;
	QLabel *m_pOperaTypeLabel;
	QLabel *m_pNodeNameLabel;
	QLineEdit *m_pNodeNameLE;
	QComboBox *m_pNodeTypeCombox;
	QComboBox *m_pOperaTypeCombox;
	QComboBox *m_pChooseFatherNode;
	QColor m_color;

	int m_iOpeType; //1平,2转,3无 
	int m_iNodeType; //1父，2子
	QColor m_NodeColor;
	osg::Node *m_Node;

	MainWindow *m_pMainWin;

	std::vector<osg::Group *> m_vNodes;
	QString m_strFatherNodeName;
	QString m_strAddNodeName;
	QString m_strcurFileName;
	QString m_strCurNodeName;
	QString m_strCurFatherName;
	QString m_strDelCurFatherName;
	QString m_strDelCurChildName;


	std::map<QString,std::vector<QString>>m_FatherToChildrenNameMap;
	std::vector<QString> m_vFatherNodesName;

	osg::Node *m_curNode;
	STL* m_curSTL;
	bool m_IsDel;
	bool m_IsAdd;
	std::pair<QString,QString> m_AddChildToFatherPair;
	std::pair<QString,QString> m_DelChildToFatherPair;


	std::vector<QString> vtmpChildren;//父节点下的所有子节点和子节点的子节点
	std::vector<QString> m_vChildren;
};

