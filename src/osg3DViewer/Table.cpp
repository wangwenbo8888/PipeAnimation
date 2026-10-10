

#include "Table.h"

#include "log.h"

Table::Table(QWidget *parent)
	: QTableWidget(parent)
	, rightClickedItem(NULL)
	, dupFlag(false)
{
	
	rowHeghtList.clear();
	rowWidthList.clear();
	fileMap.clear();
	this->setMouseTracking(false);
	//setWindowTitle(tr("table"));
	horizontalHeader()->setDefaultSectionSize(60);
	verticalHeader()->setDefaultSectionSize(18);    //设置默认行高
	tableWidth = 100;
	tableHeight = 18;
	horizontalHeader()->setClickable(false);    //设置表头不可点击（默认点击后进行排序

	QFont font = horizontalHeader()->font();    //设置表头字体加粗
	font.setBold(true);
	horizontalHeader()->setFont(font);
	horizontalHeader()->setStretchLastSection(true);    //设置充满表宽度
	horizontalHeader()->setMovable(false);              //表头左右互换
	//verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	setFrameShape(QFrame::NoFrame);      //设置无边框
	//setShowGrid(false);                //设置不显示格子线
	verticalHeader()->setVisible(false); //设置垂直头不可见
	setSelectionMode(QAbstractItemView::ExtendedSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
	setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
	setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑

	setStyleSheet("selection-background-color:lightblue;");  //设置选中背景色
	//horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
	//setStyleSheet("background: rgb(56,56,56);alternate-background-color:rgb(48,51,55);selection-background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(56,56,56),stop:1 rgb(76,76,76));"); //设置选中背景色
	//horizontalHeader()->setStyleSheet("QHeaderView::section{background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(134, 245, 99, 255),stop:0.5 rgba(134, 148, 99, 255),stop:1 rgba(115, 87, 128, 255));color:rgb(25, 70, 100);padding-left: 1px;border: 1px solid #FFFF00;}"); //设置表头背景色
	horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(215,216,225);color: black;}");

	setAlternatingRowColors(true);//隔行变色

	//setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//设置水平、垂直滚动条样式
	horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:12px;}"
		"QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
		"QScrollBar::handle:hover{background:gray;}"
		"QScrollBar::sub-line{background:transparent;}"
		"QScrollBar::add-line{background:transparent;}");

	verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width:12px;}"
		"QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
		"QScrollBar::handle:hover{background:gray;}"
		"QScrollBar::sub-line{background:transparent;}"
		"QScrollBar::add-line{background:transparent;}");

	// set the item delegate to your table widget
	setItemDelegate(new NoFocusDelegate());             //虚线边框去除
	//setFocusPolicy(Qt::NoFocus);   //去除选中虚线框
	horizontalHeader()->setHighlightSections(false);    //点击表时不对表头行光亮（获取焦点）

	m_iCurrentRow = -1;
	//createActions();
	setItemFixed(false);
	//connect(this, SIGNAL(itemEntered(QTableWidgetItem*)), this , SLOT(slotItemEntered(QTableWidgetItem*)));
	//connect(this, SIGNAL(itemSelectionChanged()), this , SLOT(slotItemSelectionChanged()));
	//this->resize(600, 600);
	setEditTriggers(QAbstractItemView::NoEditTriggers); //不允许编辑

	horizontalHeader()->setResizeMode(QHeaderView::Stretch);//列自适应宽度
}

Table::~Table()
{

}

void Table::setColumnValue(const int &columnSum, const QStringList &header)
{
	setColumnCount(columnSum);                //设置列数
	this->setHorizontalHeaderLabels(header);  //设置列的标签
}

void Table::setHeaderWidth(const int &index, const int &width)
{
	horizontalHeader()->resizeSection(index,width);
	if (rowWidthList.count() <= index + 1) {
		rowWidthList.append(width);
	}
	else {
		rowWidthList[index+1] = width;
	}
	tableWidth = 0;
	for(int index = 0; index < rowWidthList.count(); index++)
		tableWidth += rowWidthList.at(index);
	resize(tableWidth, tableHeight);
}

void Table::setHeaderHeight(const int &height)
{
	horizontalHeader()->setFixedHeight(height);        //设置表头的高度
	if (rowHeghtList.isEmpty()) {
		rowHeghtList.append(height);
	}
	else {
		rowHeghtList[0] = height;
	}
	tableHeight = 0;
	for(int index = 0; index < rowHeghtList.count(); index++)
		tableHeight += rowHeghtList.at(index);
	resize(tableWidth, tableHeight);
}


void Table::addRowValue(const int &height, const QStringList &value, const QIcon &fileIcon /*= QIcon()*/)
{
	int row_count = rowCount();    //获取表单行数
	insertRow(row_count);          //插入新行
	setRowHeight(row_count, height);
	for (int index = 0; index < columnCount(); index++)
	{
		QTableWidgetItem *item = new QTableWidgetItem;
		if (index == 0)
		{
			item->setIcon(fileIcon);
			item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
			fileMap.insert(item, value.at(index));
		}
		else
		{
			item->setTextAlignment(Qt::AlignCenter);
		}
		if(index == 1|| index == 2)//开始时间 结束时间
		{

			QTableWidgetItem *item2 = new DTableWidgetItem(QString::number(value.at(index).toDouble(),'f',3));
			item2->setData(Qt::DisplayRole,value.at(index).toDouble());
			setItem(row_count, index, item2);

		}
		else
		{
			item->setText(value.at(index));
			setItem(row_count, index, item);
		}
			
	}
	rowHeghtList.append(height);
	tableHeight += height; 
	//resize(tableWidth, tableHeight);
}

void Table::addRowValueToLMTable(const int &height, const QStringList &value, const QIcon &fileIcon /*= QIcon()*/)
{

	int canchange = value.at(7).toInt();
	if (canchange == 0)
	{
#if 1


		int row_count = rowCount();    //获取表单行数
		insertRow(row_count);          //插入新行
		setRowHeight(row_count, height);
		for (int index = 0; index < columnCount()-1; index++) {
			QTableWidgetItem *item = new QTableWidgetItem;
			if (index == 0) {
				item->setIcon(fileIcon);
				item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
				fileMap.insert(item, value.at(index));
			}
			else {
				item->setTextAlignment(Qt::AlignCenter);
			}
			
			{
				if (index<value.size() && index!=7)
				{			
					item->setText(value.at(index));
					setItem(row_count, index, item);
					//if (index == 0|| index == 1||index == 2)
					//{
						item->setFlags(item->flags() & (~Qt::ItemIsEditable));

					//}
				}
				else if (index == 7)
				{
					item->setText(value.at(index));
					setItem(row_count, index+2, item);
					//if (index == 0|| index == 1||index == 2)
					//{
					item->setFlags(item->flags() & (~Qt::ItemIsEditable));

					//}
				}

			}

		}
		rowHeghtList.append(height);
		tableHeight += height; 
#endif		
	}
	else if (canchange == 1)
	{
		int row_count = rowCount();    //获取表单行数
		insertRow(row_count);          //插入新行
		setRowHeight(row_count, height);
		for (int index = 0; index < columnCount()-1; index++) 
		{
			QTableWidgetItem *item = new QTableWidgetItem;
			if (index == 0) 
			{
				item->setIcon(fileIcon);
				item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
				fileMap.insert(item, value.at(index));
			}
			else 
			{
				item->setTextAlignment(Qt::AlignCenter);
			}

			if (index<value.size())
			{	

				if (index != 7)
				{
					item->setText(value.at(index));
					setItem(row_count, index, item);
					if (index == 0|| index == 1||index == 2||index == 3)
					{
						item->setFlags(item->flags() & (~Qt::ItemIsEditable));

					}
				}
				else if (index == 7)
				{
					item->setText(value.at(index));
					setItem(row_count, index+2, item);
					if (index == 0||index == 1|| index == 2||index == 3)
					{
						item->setFlags(item->flags() & (~Qt::ItemIsEditable));

					}
				}

				
			}
		}
		rowHeghtList.append(height);
		tableHeight += height; 
		//resize(tableWidth, tableHeight);
	}
}

void Table::addRowValueToCollideTable(const int &height, const QStringList &value, const QIcon &fileIcon /*= QIcon()*/)
{
	int row_count = rowCount();    //获取表单行数
	int columncount = columnCount();

	insertRow(row_count);          //插入新行
	setRowHeight(row_count, height);
	for (int index = 0; index < columnCount()-1; index++) 
	{
		QTableWidgetItem *item = new QTableWidgetItem;
		if (index == 0) 
		{
			item->setIcon(fileIcon);
			item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
			fileMap.insert(item, value.at(index));
		}
		else 
		{
			item->setTextAlignment(Qt::AlignCenter);
		}

		if (index<value.size())
		{	
			item->setText(value.at(index));
			setItem(row_count, index, item);
			if (index == 0|| index == 1||index == 2)
			{
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));

			}

		}
	}
	rowHeghtList.append(height);
	tableHeight += height; 
	resize(tableWidth, tableHeight);

}

void Table::addMyRowValue(const int &height,const QString &strname,const double &dST,const double &dET,const QString &strAnimationType,const QString &strvalue,const QString &strAxis)
{
	int row_count = rowCount();    //获取表单行数
	insertRow(row_count);          //插入新行
	setRowHeight(row_count, height);
	for (int index = 0; index < columnCount(); index++) 
	{
		QTableWidgetItem *item = new QTableWidgetItem;
		if (index == 0) 
		{
			item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		}
		else
			item->setTextAlignment(Qt::AlignCenter);
	}
}

void NoFocusDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
	QStyleOptionViewItem itemOption(option);
	if (itemOption.state & QStyle::State_HasFocus)
		itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
	QStyledItemDelegate::paint(painter, itemOption, index);
}

void Table::setRowH(const int &index, const int &height)
{
	setRowHeight(index, height);
	if (rowHeghtList.count() <= index + 1) {
		rowHeghtList.append(height);
	}
	else {
		rowHeghtList[index+1] = height;
	}
	tableHeight = 0;
	for(int index = 0; index < rowHeghtList.count(); index++)
		tableHeight += rowHeghtList.at(index);
	resize(tableWidth, tableHeight);
}

void Table::createActions()
{
	popMenu = new QMenu();
	actionName = new QAction(this);
	actionSize = new QAction(this);
	actionType = new QAction(this);
	actionDate = new QAction(this);
	actionOpen = new QAction(this);   
	actionDownload = new QAction(this);
	actionFlush = new QAction(this);
	actionDelete = new QAction(this);
	actionRename = new QAction(this);
	actionCreateFolder = new QAction(this);

	actionOpen->setText(tr("打开"));
	actionDownload->setText(tr("下载"));
	actionFlush->setText(tr("刷新"));
	actionDelete->setText(tr("删除"));
	actionRename->setText(tr("重命名"));
	actionCreateFolder->setText(tr("新建文件夹"));
	actionName->setText(tr("名称"));
	actionSize->setText(tr("大小"));
	actionType->setText(tr("项目类型"));
	actionDate->setText(tr("修改日期"));

	actionFlush->setShortcut(QKeySequence::Refresh);
	connect(actionRename, SIGNAL(triggered()), this, SLOT(slotActionRename()));
}

void Table::contextMenuEvent(QContextMenuEvent *event)
{
	//popMenu->clear();
	//QPoint point = event->pos();
	//rightClickedItem = this->itemAt(point);
	//if(rightClickedItem != NULL) {
	//	popMenu->addAction(actionDownload);
	//	popMenu->addAction(actionFlush);
	//	popMenu->addSeparator();
	//	popMenu->addAction(actionDelete);
	//	popMenu->addAction(actionRename);
	//	popMenu->addSeparator();
	//	popMenu->addAction(actionCreateFolder);
	//	QMenu *sortStyle = popMenu->addMenu(tr("排序"));
	//	sortStyle->addAction(actionName);
	//	sortStyle->addAction(actionSize);
	//	sortStyle->addAction(actionType);
	//	sortStyle->addAction(actionDate);

	//	popMenu->exec(QCursor::pos());
	//	event->accept();
	//}
}

QModelIndex Table::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	//重写移动光标事件，当存在编辑项的时候，让光标永远位于当前项（编辑项）,否则返回父类
	if(rightClickedItem && rightClickedItem->row() >= 0) {
		return currentIndex();
	}
	else {
		return QTableWidget::moveCursor(cursorAction, modifiers);
	}
}


void Table::keyPressEvent(QKeyEvent *event)
{
	//if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
	//	QTableWidgetItem *item = currentItem();
	//	if (item) {
	//		closePersistentEditor(item);
	//		openPersistentEditor(item);
	//		slotItemSelectionChanged();
	//		dupFlag = false;
	//	}
	//}
}

void Table::slotItemSelectionChanged()
{
	//关闭编辑项
	if (rightClickedItem && dupFlag == false) {
		int editRow = rightClickedItem->row();
		QTableWidgetItem *item = this->item(editRow, 0);
		QMap<QTableWidgetItem *, QString>::iterator it;
		for (it = fileMap.begin(); it != fileMap.end(); ++it) {
			if (it.key() != item) {
				if (it.value() == item->text()) {
					dupFlag = true;
				}
			}
		}
		if (dupFlag == false) {
			this->closePersistentEditor(item);
			rightClickedItem = NULL;
		}
		else {
			QMessageBox::critical(this,tr("错误提示"),tr("文件重名"), tr("确定"));
			LogInfo("文件重名");
			setCurrentItem(item);
		}
	}
	else {
		dupFlag = false;
	}
}

void Table::setItemFixed(bool flag)
{
	if (flag == true)
		horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	else
		horizontalHeader()->setResizeMode(QHeaderView::Interactive);
}

bool Table::getSelectedRow(QList<int> &rowList)
{
	//多选并获取所选行
	QList<QTableWidgetItem *> items = this->selectedItems();
	int itemCount = items.count();
	if(itemCount <= 0) {
		return false;
	}
	for (int index = 0; index < itemCount; index++) {
		int itemRow = this->row(items.at(index));
		rowList.append(itemRow);
	}
	return  true;
}


void Table::UpdateRow( std::vector<QString> vName)
{
	std::vector<QString> vneedDel;
	//获取当前表格里的内容
	std::vector<QString> vcurItemNames;
	for(int j=1;j<this->rowCount();j++)
	{
		vcurItemNames.push_back(this->item(j,0)->text());
	}

	//取出表格里多余的内容
	for(size_t i=0;i<vcurItemNames.size();i++)
	{
	    std::vector<QString>::iterator iter = vName.begin();
		for(;iter!=vName.end();iter++)
		{
			if(vcurItemNames[i] == *iter)
			{
				break;
			}

		}

		if(iter == vcurItemNames.end())//保存當前表格需要删除的内容
		{
			vneedDel.push_back(vcurItemNames[i]);
			
			
		}
	}

	//删除多余的项
	std::vector<int> vdelRows;
	for(size_t k=0;k<vneedDel.size();k++)
	{
		for(int j=1;j<this->rowCount();j++)
		{
			if(item(j,0)->text() == vneedDel[k])
			{
				vdelRows.push_back(j);
			}
		}
	}
	//删除
	for(size_t i=0;i<vdelRows.size();i++)
	{
		this->removeRow(i);	
	}
}

void Table::SetRowItemsColor(int row)
{
	//row = row -1;
	for(int j=0;j<this->rowCount();j++)
	{
		if(row == j)
		{
			for(int i=0;i<this->columnCount();i++)
			{
				this->item(j,i)->setForeground(Qt::red);
			}

		}
		else
		{
			for(int i=0;i<this->columnCount();i++)
			{
				this->item(j,i)->setForeground(Qt::black);
			}
		}
		
	}

	m_iCurrentRow = row;

}

void Table::ResetAllColor()
{
	for(int j=0;j<this->rowCount();j++)
	{
		for(int i=0;i<this->columnCount();i++)
		{
			this->item(j,i)->setForeground(Qt::black);
		}
	}
}

void Table::AddiText(int i)
{
	m_viText.push_back(i);
}

void Table::DeleteAllValueRow()
{
	int a = rowCount();
	if(a==0)
		return;
	else
	{
		
		clearContents();
		for (int i = rowCount(); i > 0; i--)
		{
			removeRow(0);
		}
	}
	
	
	a = rowCount();

}

void Table::SetColumnUnEdit(int irow, int icolumn)
{	
	QTableWidgetItem *item = new QTableWidgetItem();
	//item->setBackground(QBrush(QColor(Qt::lightGray)));
	item->setFlags(/*item->flags() & */Qt::ItemIsEnabled);
	setItem(irow, icolumn, item);
	
}

void Table::slotItemEntered(QTableWidgetItem *item)
{
	if(!item)
		return;
	QString name = item->text();
	if (name.isEmpty())
		return;
	QToolTip::showText(QCursor::pos(), name);
}

void Table::slotActionRename()
{
	//获得当前节点并获取编辑名称
	if (rightClickedItem) {
		int editRow = rightClickedItem->row();
		QTableWidgetItem *item = this->item(editRow, 0);   //编辑的行号及第一列
		this->setCurrentCell(editRow, 0);
		this->openPersistentEditor(item);                  //打开编辑项
		this->editItem(item);
	}
}