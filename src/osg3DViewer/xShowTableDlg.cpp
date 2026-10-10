#include "xShowTableDlg.h"

ShowTableDlg::ShowTableDlg()
{
	m_table = new Table(this);

	QStringList header;
	header<<QString::fromLocal8Bit("模型名")
		<<QString::fromLocal8Bit("开始时间")
		<<QString::fromLocal8Bit("结束时间")
		<<QString::fromLocal8Bit("运动方式")
		<<QString::fromLocal8Bit("数值")
		<<QString::fromLocal8Bit("轴");
	m_table->setColumnValue(6, header);

	m_table->setHeaderWidth(0, 150);
	m_table->setHeaderWidth(1, 100);
	m_table->setHeaderWidth(2, 100);
	m_table->setHeaderWidth(3, 100);
	m_table->setHeaderWidth(4, 100);
	m_table->setHeaderWidth(5, 100);
	m_table->setHeaderHeight(30);

	m_table->sortItems(1, Qt::AscendingOrder);

	resize(670, 800);


}

ShowTableDlg::~ShowTableDlg()
{

}

void ShowTableDlg::AddTableRow(const int &height, const QStringList &value)
{
	m_table->addRowValue(height,value);
}

void ShowTableDlg::UpdateTable(const std::vector<QString> &vName)
{
	m_table->UpdateRow(vName);
}

void ShowTableDlg::SetRowandColumn(int row,int column)
{
	m_table->setColumnCount(column);
	m_table->setRowCount(row);
}

void ShowTableDlg::SortTable()
{
	m_table->sortByColumn(1);
}

void ShowTableDlg::closeEvent(QCloseEvent *event)
{
	emit SigCloseThisWindow();
}


