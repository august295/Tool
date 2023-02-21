#include <Manager/DataManager.h>
#include <ParseHeader/TypeParser.h>

#include "DataShowSubWidget.h"
#include "ui_DataShowSubWidget.h"

struct DataShowSubWidget::DataShowSubWidgetPrivate {
    QString m_TypeName; // 结构名称
};

DataShowSubWidget::DataShowSubWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DataShowSubWidget)
    , m_p(new DataShowSubWidgetPrivate)
{
    ui->setupUi(this);
    setMinimumSize(800, 600);
    // 设置关闭销毁
    setAttribute(Qt::WA_DeleteOnClose);
    // 设置界面属性
    setWindowFlags(Qt::Window);
    // 设置模态
    setWindowModality(Qt::NonModal);

    this->InitWidget();
}

DataShowSubWidget::~DataShowSubWidget()
{
    delete ui;
}

void DataShowSubWidget::InitWidget()
{
    QStringList headerComment = { tr("注释") };
    ui->tableWidget_Comment->setColumnCount(1);
    ui->tableWidget_Comment->setRowCount(1);
    ui->tableWidget_Comment->setVerticalHeaderLabels(headerComment);
    ui->tableWidget_Comment->setMinimumWidth(100);
    ui->tableWidget_Comment->horizontalHeader()->hide();
    ui->tableWidget_Comment->horizontalHeader()->setStretchLastSection(true);

    QStringList headerStruct = { tr("成员属性"), tr("是否指针"), tr("成员名称"), tr("备注") };
    ui->tableWidget_Struct->setColumnCount(headerStruct.size());
    ui->tableWidget_Struct->setHorizontalHeaderLabels(headerStruct);
    ui->tableWidget_Struct->verticalHeader()->hide();                           // 隐藏行号
    ui->tableWidget_Struct->horizontalHeader()->setStretchLastSection(true);    // 最后一列自动扩展
    ui->tableWidget_Struct->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可修改
    ui->tableWidget_Struct->setAlternatingRowColors(true);                      // 隔行变色

    QStringList headerEnum = { tr("成员名称"), tr("成员数值"), tr("备注") };
    ui->tableWidget_Enum->setColumnCount(headerEnum.size());
    ui->tableWidget_Enum->setHorizontalHeaderLabels(headerEnum);
    ui->tableWidget_Enum->verticalHeader()->hide();                           // 隐藏行号
    ui->tableWidget_Enum->horizontalHeader()->setStretchLastSection(true);    // 最后一列自动扩展
    ui->tableWidget_Enum->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可修改
    ui->tableWidget_Enum->setAlternatingRowColors(true);                      // 隔行变色

    ui->verticalLayout->setStretchFactor(ui->tableWidget_Comment, 1);
    ui->verticalLayout->setStretchFactor(ui->tableWidget_Struct, 9);
    ui->verticalLayout->setStretchFactor(ui->tableWidget_Enum, 9);
    ui->tableWidget_Comment->hide();
    ui->tableWidget_Struct->hide();
    ui->tableWidget_Enum->hide();
}

void DataShowSubWidget::ShowTableStruct(const std::string& fileName, const std::string& typeName)
{
    auto          typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    TypeParser    parser        = typeParserMap[fileName];
    QTableWidget* tableWidget   = ui->tableWidget_Struct;

    tableWidget->setRowCount(0);
    ui->tableWidget_Comment->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(parser.comments_[typeName])));
    for (auto var : parser.struct_defs_[typeName]) {
        if (kPaddingFieldName != var.m_name) {
            int rowCount = tableWidget->rowCount();
            tableWidget->insertRow(rowCount);
            tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::fromStdString(var.m_type)));
            QTableWidgetItem* checkBox = new QTableWidgetItem();
            var.m_is_pointer ? checkBox->setCheckState(Qt::Checked) : checkBox->setCheckState(Qt::Unchecked);
            checkBox->setFlags(Qt::ItemIsEnabled);
            tableWidget->setItem(rowCount, 1, checkBox);
            tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::fromStdString(var.m_name)));
            tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::fromStdString(var.m_comment)));
        }
    }

    ui->tableWidget_Enum->hide();
    ui->tableWidget_Struct->show();
    ui->tableWidget_Comment->show();
}

void DataShowSubWidget::ShowTableEnum(const std::string& fileName, const std::string& typeName)
{
    auto          typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    TypeParser    parser        = typeParserMap[fileName];
    QTableWidget* tableWidget   = ui->tableWidget_Enum;

    tableWidget->setRowCount(0);
    ui->tableWidget_Comment->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(parser.comments_[typeName])));
    for (auto var : parser.enum_defs_[typeName]) {
        int rowCount = tableWidget->rowCount();
        tableWidget->insertRow(rowCount);
        tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::fromStdString(var.m_name)));
        tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(var.m_value)));
        tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::fromStdString(var.m_comment)));
    }

    ui->tableWidget_Struct->hide();
    ui->tableWidget_Enum->show();
    ui->tableWidget_Comment->show();
}

void DataShowSubWidget::AdjustTableComment()
{
    ui->tableWidget_Comment->setRowHeight(0, ui->tableWidget_Comment->height());
}

void DataShowSubWidget::closeEvent(QCloseEvent* event)
{
    emit SignalClose(windowTitle());
    QWidget::closeEvent(event);
}
