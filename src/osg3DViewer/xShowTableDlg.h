#ifndef XSHOWTABLEDLG_H
#define XSHOWTABLEDLG_H


#include "Table.h"

class ShowTableDlg : public QWidget
{
	Q_OBJECT

public:
	ShowTableDlg();
	~ShowTableDlg();
	void AddTableRow(const int &height, const QStringList &value);
	void UpdateTable(const std::vector<QString> &vName);
	void SetRowandColumn(int row,int column);
	void SortTable();
	void closeEvent(QCloseEvent *event);


signals:

	void SigCloseThisWindow();

private:
	Table *m_table;
};


#endif