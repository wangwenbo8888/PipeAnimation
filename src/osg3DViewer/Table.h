#ifndef TABLE_H
#define TABLE_H
#include <QtGui>

#include "DTableWidgetItem.h"
class Table : public QTableWidget
{
	Q_OBJECT
public:
	Table(QWidget *parent = 0);
	~Table();
	void setColumnValue(const int &columnSum, const QStringList &header);   //set header value
	void setHeaderWidth(const int &index, const int &width);    //set header and column widhth for each index
	void setHeaderHeight(const int &height);                    //set header height

	void addRowValue(const int &height, const QStringList &value, const QIcon &fileIcon = QIcon());
	void addRowValueToLMTable(const int &height, const QStringList &value, const QIcon &fileIcon = QIcon());
	void addRowValueToCollideTable(const int &height, const QStringList &value, const QIcon &fileIcon = QIcon());

	void addMyRowValue(const int &height,const QString &strname,const double &dST,const double &dET,const QString &strAnimationType,const QString &strvalue,const QString &strAxis);
	void setRowH(const int &index, const int &height);
	void setItemFixed(bool flag);
	bool getSelectedRow(QList<int> &rowList);
	void UpdateRow(std::vector<QString> vName);
	void SetRowItemsColor(int row);
	int  GetRow(){return m_iCurrentRow;}
	void ResetAllColor();
	void AddiText(int i);
	std::vector<int> GetviText(){return m_viText;}
	void DeleteAllValueRow();
	void SetColumnUnEdit(int row,int column);
protected:
	void contextMenuEvent(QContextMenuEvent *event);
	QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
	void keyPressEvent(QKeyEvent *event);

private:
	void createActions();

	private slots:
		void slotItemEntered(QTableWidgetItem *item);
		void slotActionRename();
		void slotItemSelectionChanged();

Q_SIGNALS:
	  //  void cellChanged(int row, int column);

private:
	std::vector<int> m_viText;
	int tableWidth;
	int tableHeight;
	QList<int>rowHeghtList;
	QList<int>rowWidthList;

	QMenu *popMenu;
	QAction *actionName;
	QAction *actionSize;
	QAction *actionType;
	QAction *actionDate;
	QAction *actionOpen;
	QAction *actionDownload;
	QAction *actionFlush;
	QAction *actionDelete;
	QAction *actionRename;
	QAction *actionCreateFolder;
	QTableWidgetItem *rightClickedItem;
	QMap<QTableWidgetItem *, QString>fileMap;
	bool dupFlag;

	int m_iCurrentRow;  // 记录当前运行的行
};

// custom item delegate class
class NoFocusDelegate : public QStyledItemDelegate
{
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};
#endif // TABLE_H