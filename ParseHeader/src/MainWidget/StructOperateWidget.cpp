#include "StructOperateWidget.h"
#include "DelegateComboBox.h"

StructOperateWidget::StructOperateWidget(QWidget* parent /*= Q_NULLPTR*/)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle(QString("结构体操作"));
    setMinimumSize(800, 800);

    // 初始化界面
    InitTableWidget();

    // 绑定按钮
    connect(ui.pushButton_Add, &QPushButton::clicked, this, &StructOperateWidget::on_pushButton_Add);
    connect(ui.pushButton_Delete, &QPushButton::clicked, this, &StructOperateWidget::on_pushButton_Delete);
    connect(ui.pushButton_Up, &QPushButton::clicked, this, &StructOperateWidget::on_pushButton_Up);
    connect(ui.pushButton_Down, &QPushButton::clicked, this, &StructOperateWidget::on_pushButton_Down);
    connect(ui.pushButton_Clear, &QPushButton::clicked, this, &StructOperateWidget::on_pushButton_Clear);
}

StructOperateWidget::~StructOperateWidget()
{
}

void StructOperateWidget::InitTableWidget()
{
    // 清空
    ui.lineEdit_Name->clear();
    ui.tableWidget->clearContents();
    ui.tableWidget->setRowCount(0);

    QStringList header = {QString("成员属性"), QString("是否指针"), QString("成员名称"), QString("备注")};
    ui.tableWidget->setColumnCount(header.size());
    ui.tableWidget->setHorizontalHeaderLabels(header);
    ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:gray;}");
    ui.tableWidget->verticalHeader()->hide();                        // 隐藏行号
    ui.tableWidget->horizontalHeader()->setStretchLastSection(true); // 最后一列自动扩展
    ui.tableWidget->setAlternatingRowColors(true);                   // 隔行变色
    ui.tableWidget->setPalette(QPalette(Qt::gray));                  // 设置隔行变色的颜色

    // 设置代理
    QStringList strList;
    ui.tableWidget->setItemDelegateForColumn(0, new DelegateComboBox(this, _StructTypeList));
}

void StructOperateWidget::UpdateStructTypeList(const QStringList& structTypeList)
{
    _StructTypeList = structTypeList;
}

void StructOperateWidget::on_pushButton_Add()
{
    ui.tableWidget->insertRow(ui.tableWidget->rowCount());
    QTableWidgetItem* checkBox = new QTableWidgetItem();
    checkBox->setCheckState(Qt::Unchecked);
    ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1, 1, checkBox);
}

void StructOperateWidget::on_pushButton_Delete()
{
    int row = ui.tableWidget->currentRow();
    if (row >= 0)
    {
        ui.tableWidget->removeRow(row);
    }
}

void StructOperateWidget::on_pushButton_Up()
{
    int row = ui.tableWidget->currentRow();
    MoveRow(ui.tableWidget, row, row - 1);
}

void StructOperateWidget::on_pushButton_Down()
{
    int row = ui.tableWidget->currentRow();
    MoveRow(ui.tableWidget, row, row + 1);
}

void StructOperateWidget::on_pushButton_Clear()
{
    ui.tableWidget->clearContents();
    ui.tableWidget->setRowCount(0);
}

void StructOperateWidget::MoveRow(QTableWidget* pTable, int nFrom, int nTo)
{
    if (pTable == nullptr)
    {
        return;
    }
    if (nFrom == nTo)
    {
        return;
    }
    if (nFrom < 0 || nTo < 0)
    {
        return;
    }
    int nRowCount = pTable->rowCount();
    if (nFrom >= nRowCount || nTo >= nRowCount)
        return;

    int nColCur             = 0;
    nColCur                 = pTable->currentColumn();
    QTableWidgetItem* itCur = pTable->currentItem();
    if (nullptr != itCur)
    {
        nColCur = itCur->column();
    }
    int nFromRow   = nFrom;
    int nInsertRow = nTo;
    // 小于 Up 大于 Down
    if (nTo < nFrom)
    {
        nFromRow = nFrom + 1;
        pTable->insertRow(nTo);
    }
    else
    { // Down
        nInsertRow = nTo + 1;
        pTable->insertRow(nInsertRow);
    }
    this->CopyRow(pTable, nFromRow, nInsertRow);
    // 删除旧行信息
    pTable->removeRow(nFromRow);

    // 选择之前移动的行
    pTable->selectRow(nInsertRow);
    pTable->setCurrentCell(nTo, nColCur);
}

void StructOperateWidget::CopyRow(QTableWidget* pTable, int nFrom, int nTo)
{
    int nColCount = pTable->columnCount();
    for (int col = 0; col < nColCount; col++)
    {
        if (QTableWidgetItem* item = pTable->item(nFrom, col))
        {
            QString text = item->text();
            pTable->setItem(nTo, col, new QTableWidgetItem(text));
        }
    }
}