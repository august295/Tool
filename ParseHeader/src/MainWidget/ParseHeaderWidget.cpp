#include <QtCore/QString>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMenu>

#include <DataManager/DataManager.h>

#include "FileManage.h"
#include "ParseHeaderWidget.h"
#include "StructOperateWidget.h"

struct ParseHeaderWidget::ParseHeaderWidgetPrivate
{
    ParseHeaderWidgetPrivate()
    {
        m_StructOperateWidget = nullptr;
        m_TreeRightMenu       = nullptr;
    }
    ~ParseHeaderWidgetPrivate()
    {
        delete m_StructOperateWidget;
        delete m_TreeRightMenu;
    }

    std::string          m_CurrFile;            // 当前操作文件
    StructOperateWidget* m_StructOperateWidget; // 结构体操作界面

    QMenu* m_TreeRightMenu; // 树右键菜单
};

ParseHeaderWidget::ParseHeaderWidget(QWidget* parent)
    : QWidget(parent)
    , m_p(new ParseHeaderWidgetPrivate)
{
    ui.setupUi(this);
    setWindowTitle(QString("文件解析"));
    setMinimumSize(1200, 800);

    // 优先解析文件
    std::set<std::string> includePaths({"../../../test"});
    DataManager::GetInstance()->ParseFiles(includePaths);

    // 初始化界面
    InitTreeWidget();
    InitTableWidget();

    // treeWidget 选择右键用户自定义菜单
    ui.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.treeWidget, &QTreeWidget::customContextMenuRequested, this, &ParseHeaderWidget::ShowTreeRightMenu);

    // 绑定按钮
    connect(ui.pushButton_Flush, &QPushButton::clicked, this, &ParseHeaderWidget::DataFlush);
    connect(ui.pushButton_Select, &QPushButton::clicked, this, &ParseHeaderWidget::DataSelect);
    connect(ui.pushButton_Save, &QPushButton::clicked, this, &ParseHeaderWidget::DataSave);
}

ParseHeaderWidget::~ParseHeaderWidget()
{
    if (m_p)
    {
        delete m_p;
        m_p = nullptr;
    }
}

void ParseHeaderWidget::InitTreeWidget(const std::string& selectStr /*= ""*/)
{
    // 设置树头
    ui.treeWidget->setHeaderLabel(QString("结构体树形图"));
    ui.treeWidget->clear();

    // 显示到界面
    auto typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    for (auto parser : typeParserMap)
    {
        std::string      filename = parser.first;
        QTreeWidgetItem* item     = new QTreeWidgetItem(NODE);
        item->setText(0, QString::fromStdString(filename.substr(filename.find_last_of('/') + 1)));
        item->setData(0, Qt::UserRole, QString::fromStdString(filename));
        ui.treeWidget->addTopLevelItem(item);
        for (auto iter : parser.second.struct_defs_)
        {
            if (iter.first.find(selectStr) != std::string::npos)
            {
                QTreeWidgetItem* itemStruct = new QTreeWidgetItem(LEAF);
                itemStruct->setText(0, QString::fromStdString(iter.first));
                itemStruct->setData(0, Qt::UserRole, QString::fromStdString(filename));
                item->addChild(itemStruct);
            }
        }
    }
    ui.treeWidget->expandAll();

    // 触发器
    connect(ui.treeWidget, &QTreeWidget::itemClicked, this, &ParseHeaderWidget::DataShow);
}

void ParseHeaderWidget::InitTableWidget()
{
    QStringList header = {QString("成员属性"), QString("是否指针"), QString("成员名称"), QString("备注")};
    ui.tableWidget->setColumnCount(header.size());
    ui.tableWidget->setHorizontalHeaderLabels(header);
    ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:gray;}");
    ui.tableWidget->verticalHeader()->hide();                           // 隐藏行号
    ui.tableWidget->horizontalHeader()->setStretchLastSection(true);    // 最后一列自动扩展
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可修改
    ui.tableWidget->setAlternatingRowColors(true);                      // 隔行变色
    ui.tableWidget->setPalette(QPalette(Qt::gray));                     // 设置隔行变色的颜色
}

QStringList ParseHeaderWidget::GetStructTypeList()
{
    QStringList structTypeList;
    // 基础类型
    for (auto basic : data_types)
    {
        structTypeList.push_back(QString::fromStdString(basic));
    }
    auto typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    for (auto parser : typeParserMap)
    {
        // 结构体类型
        for (auto structDef : parser.second.struct_defs_)
        {
            structTypeList.push_back(QString::fromStdString(structDef.first));
        }
        // 枚举类型
        for (auto enumDef : parser.second.enum_defs_)
        {
            structTypeList.push_back(QString::fromStdString(enumDef.first));
        }
    }
    return structTypeList;
}

void ParseHeaderWidget::DataFlush()
{
    InitTreeWidget();
}

void ParseHeaderWidget::DataSelect()
{
    std::string selectStr = ui.lineEdit->text().toStdString();
    InitTreeWidget(selectStr);
}

void ParseHeaderWidget::DataSave()
{
    auto fileMap = DataManager::GetInstance()->GetFileMap();
    for (auto& file : fileMap)
    {
        std::string                  filename  = file.first;
        std::map<std::string, bool>& structVec = file.second;
        if (structVec.empty())
        {
            continue;
        }
        auto       typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
        TypeParser parser        = typeParserMap[filename];
        auto       fileLineVec   = FileManage::GetInstance()->GetFileContent(filename);
        for (size_t i = 0; i < fileLineVec.size(); i++)
        {
            for (auto& stru : structVec)
            {
                if (fileLineVec[i].find(stru.first) != std::string::npos && false == stru.second)
                {
                    std::vector<std::string> lineVec;
                    PackageStruct(stru.first, parser.struct_defs_[stru.first], lineVec);
                    size_t j = i;
                    while (fileLineVec[j].find("}") == std::string::npos)
                    {
                        j++;
                    };
                    fileLineVec.erase(fileLineVec.begin() + i, fileLineVec.begin() + j);
                    fileLineVec.insert(fileLineVec.begin() + i, lineVec.begin(), lineVec.end());
                    i += lineVec.size();
                    stru.second = true;
                }
            }
        }
        for (size_t i = fileLineVec.size() - 1; i > fileLineVec.size() - 10; i--)
        {
            if (fileLineVec[i].find("#endif") != std::string::npos)
            {
                for (auto& stru : structVec)
                {
                    if (false == stru.second)
                    {
                        std::vector<std::string> lineVec;
                        PackageStruct(stru.first, parser.struct_defs_[stru.first], lineVec);
                        fileLineVec.insert(fileLineVec.begin() + i, lineVec.begin(), lineVec.end());
                        i += lineVec.size();
                        stru.second = true;
                    }
                }
            }
        }
        FileManage::GetInstance()->SaveFile(filename, fileLineVec);
    }
}

void ParseHeaderWidget::DataShow(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    std::string file = item->data(0, Qt::UserRole).toByteArray().data();
    std::string stru = item->text(0).toStdString();
    ShowTableStruct(file, stru);
}

void ParseHeaderWidget::ShowTableStruct(const std::string& filename, const std::string& stru)
{
    auto       typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    TypeParser parser        = typeParserMap[filename];

    ui.tableWidget->setRowCount(0);
    int i = 0;
    for (auto var : parser.struct_defs_[stru])
    {
        if (kPaddingFieldName != var.m_name)
        {
            ui.tableWidget->insertRow(ui.tableWidget->rowCount());
            ui.tableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(var.m_type)));
            QTableWidgetItem* checkBox = new QTableWidgetItem();
            var.m_is_pointer ? checkBox->setCheckState(Qt::Checked) : checkBox->setCheckState(Qt::Unchecked);
            checkBox->setFlags(Qt::ItemIsEnabled);
            ui.tableWidget->setItem(i, 1, checkBox);
            ui.tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(var.m_name)));
            ui.tableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(var.m_comment)));
            i++;
        }
    }
}

void ParseHeaderWidget::ShowTreeRightMenu(const QPoint& pos)
{
    m_p->m_TreeRightMenu = new QMenu;
    QAction* addAction   = new QAction(QString("增加"));
    connect(addAction, &QAction::triggered, this, &ParseHeaderWidget::AddStruct);

    QTreeWidgetItem* item = ui.treeWidget->itemAt(pos); //关键
    if (item)
    {
        switch (item->type())
        {
        case NODE:
            m_p->m_TreeRightMenu->addAction(addAction);
            break;
        default:
            break;
        }
    }
    //移动菜单出现在鼠标点击的位置
    m_p->m_TreeRightMenu->move(ui.treeWidget->cursor().pos());
    m_p->m_TreeRightMenu->show();
}

void ParseHeaderWidget::AddStruct()
{
    QTreeWidgetItem* item = ui.treeWidget->currentItem();
    m_p->m_CurrFile       = item->data(0, Qt::UserRole).toByteArray().data();

    if (nullptr == m_p->m_StructOperateWidget)
    {
        m_p->m_StructOperateWidget = new StructOperateWidget;
        connect(m_p->m_StructOperateWidget->ui.pushButton_Finish, &QPushButton::clicked, this, &ParseHeaderWidget::AddStructFinish);
    }
    QStringList structTypeList = this->GetStructTypeList();
    m_p->m_StructOperateWidget->UpdateStructTypeList(structTypeList);
    m_p->m_StructOperateWidget->InitTableWidget();
    m_p->m_StructOperateWidget->show();
}

void ParseHeaderWidget::AddStructFinish()
{
    // 获取界面数据并保存
    std::string                  structName = m_p->m_StructOperateWidget->ui.lineEdit_Name->text().toStdString();
    std::list<StructDeclaration> varList;
    for (int i = 0; i < m_p->m_StructOperateWidget->ui.tableWidget->rowCount(); i++)
    {
        QTableWidget*     tableWidget = m_p->m_StructOperateWidget->ui.tableWidget;
        StructDeclaration var;
        var.m_type       = tableWidget->item(i, 0)->text().toLocal8Bit().data();
        var.m_is_pointer = tableWidget->item(i, 1)->checkState();
        var.m_name       = tableWidget->item(i, 2)->text().toLocal8Bit().data();
        var.m_comment    = tableWidget->item(i, 3)->text().toLocal8Bit().data();
        varList.push_back(var);
    }
    auto typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    auto iter          = typeParserMap.find(m_p->m_CurrFile);
    if (iter != typeParserMap.end())
    {
        iter->second.struct_defs_.emplace(structName, varList);
    }

    // 表示文件修改
    DataManager::GetInstance()->SetFileMap(m_p->m_CurrFile, structName, false);
    m_p->m_StructOperateWidget->hide();
    InitTreeWidget();
    ShowTableStruct(m_p->m_CurrFile, structName);
}

void ParseHeaderWidget::PackageStruct(const std::string& structName, const std::list<StructDeclaration>& varList, std::vector<std::string>& lineVec)
{
    std::string temp;
    temp = "struct " + structName;
    lineVec.push_back(temp);
    temp.clear();

    lineVec.push_back("{");
    for (auto var : varList)
    {
        temp += "\t" + var.m_type;
        if (var.m_is_pointer)
        {
            temp += "*";
        }
        temp += " " + var.m_name;
        temp += ";";
        temp += " // " + var.m_comment;
        lineVec.push_back(temp);
        temp.clear();
    }

    lineVec.push_back("};\n");
}
