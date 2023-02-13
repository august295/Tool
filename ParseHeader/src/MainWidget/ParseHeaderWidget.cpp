#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtWidgets/QAction>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

#include <Manager/DataManager.h>
#include <Manager/FileManager.h>
#include <ParseHeader/TypeParser.h>

#include "ParseHeaderWidget.h"
#include "StructOperateWidget.h"

struct ParseHeaderWidget::ParseHeaderWidgetPrivate {
    std::string          m_CurrFile;            // 当前操作文件
    std::string          m_CurrType;            // 当前结构名称
    StructOperateWidget* m_StructOperateWidget; // 结构体操作界面

    QMenu*                  m_TreeRightMenu; // 树右键菜单
    QMap<QString, QAction*> m_ActionMap;     // 右键按钮

    QTableWidget*    m_TableWidgetStruct;  // 结构体表格
    QTableWidget*    m_TableWidgetEnum;    // 枚举表格
    QTableWidget*    m_TableWidgetConst;   // 常量表格
    QTableWidget*    m_TableWidgetComment; // 注释表格
    QTreeWidgetItem* m_CurrTreeItem;       // 当前操作节点

    ParseHeaderWidgetPrivate()
    {
        m_StructOperateWidget = nullptr;
        m_TreeRightMenu       = nullptr;
        m_TableWidgetStruct   = nullptr;
        m_TableWidgetEnum     = nullptr;
        m_TableWidgetConst    = nullptr;
        m_TableWidgetComment  = nullptr;
    }
    ~ParseHeaderWidgetPrivate()
    {
        delete m_StructOperateWidget;
        delete m_TreeRightMenu;
        for (auto action : m_ActionMap) {
            delete action;
        }
        m_ActionMap.clear();
        delete m_TableWidgetStruct;
        delete m_TableWidgetEnum;
        delete m_TableWidgetConst;
        delete m_TableWidgetComment;
    }
};

ParseHeaderWidget::ParseHeaderWidget(QWidget* parent)
    : QWidget(parent)
    , m_p(new ParseHeaderWidgetPrivate)
{
    ui.setupUi(this);
    setMinimumSize(1920, 1080);
    setWindowTitle(tr("文件解析"));
    setWindowIcon(QIcon(":/icon/main.ico"));

    // 优先解析文件
    std::set<std::string> includePaths({ DataManager::GetInstance()->GetResourcesPath() });
    DataManager::GetInstance()->ParseFiles(includePaths);

    // 初始化界面
    this->Init();
}

ParseHeaderWidget::~ParseHeaderWidget()
{
    if (m_p) {
        delete m_p;
        m_p = nullptr;
    }
}

void ParseHeaderWidget::Init()
{
    this->InitTreeWidget();
    this->InitTableWidget();

    // 设置样式
    this->LoadStyle(":/qss/blacksoft.css");

    // 绑定按钮
    connect(ui.pushButton_Expand, &QPushButton::clicked, this, &ParseHeaderWidget::SlotDataTypeExpand);
    connect(ui.pushButton_Select, &QPushButton::clicked, this, &ParseHeaderWidget::SlotDataTypeSelect);
    connect(ui.pushButton_Save, &QPushButton::clicked, this, &ParseHeaderWidget::SlotDataTypeSave);
}

void ParseHeaderWidget::InitTreeWidget()
{
    this->ShowTreeWidget();

    // treeWidget 选择右键用户自定义菜单
    ui.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.treeWidget, &QTreeWidget::customContextMenuRequested, this, &ParseHeaderWidget::SlotTreeRightMenu);

    m_p->m_TreeRightMenu     = new QMenu;
    QAction* actionAddStruct = new QAction(tr("增加结构体"));
    QAction* actionAddEnum   = new QAction(tr("增加枚举"));
    QAction* actionAddConst  = new QAction(tr("增加常量"));
    QAction* actionExpand    = new QAction(tr("一键展开"));
    QAction* actionCollapse  = new QAction(tr("一键折叠"));
    m_p->m_ActionMap.insert("actionAddStruct", actionAddStruct);
    m_p->m_ActionMap.insert("actionAddEnum", actionAddEnum);
    m_p->m_ActionMap.insert("actionAddConst", actionAddConst);
    m_p->m_ActionMap.insert("actionExpand", actionExpand);
    m_p->m_ActionMap.insert("actionCollapse", actionCollapse);

    connect(actionAddStruct, &QAction::triggered, this, &ParseHeaderWidget::SlotAddStruct);
    connect(actionAddEnum, &QAction::triggered, this, &ParseHeaderWidget::SlotAddStruct);
    connect(actionAddConst, &QAction::triggered, this, &ParseHeaderWidget::SlotAddStruct);
    connect(actionExpand, &QAction::triggered, this, [&]() {
        this->SlotExpandByDepth(m_p->m_CurrTreeItem, 1);
        ui.pushButton_Expand->setText("折叠");
    });
    connect(actionCollapse, &QAction::triggered, this, [&]() {
        this->SlotcollapseByDepth(m_p->m_CurrTreeItem, 1);
    });
}

void ParseHeaderWidget::InitTableWidget()
{
    m_p->m_TableWidgetStruct = new QTableWidget;
    QStringList headerStruct = { tr("成员属性"), tr("是否指针"), tr("成员名称"), tr("备注") };
    m_p->m_TableWidgetStruct->setColumnCount(headerStruct.size());
    m_p->m_TableWidgetStruct->setHorizontalHeaderLabels(headerStruct);
    m_p->m_TableWidgetStruct->verticalHeader()->hide();                           // 隐藏行号
    m_p->m_TableWidgetStruct->horizontalHeader()->setStretchLastSection(true);    // 最后一列自动扩展
    m_p->m_TableWidgetStruct->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可修改
    m_p->m_TableWidgetStruct->setAlternatingRowColors(true);                      // 隔行变色
    m_p->m_TableWidgetStruct->setPalette(QPalette(Qt::gray));                     // 设置隔行变色的颜色

    m_p->m_TableWidgetEnum = new QTableWidget;
    QStringList headerEnum = { tr("成员名称"), tr("成员数值"), tr("备注") };
    m_p->m_TableWidgetEnum->setColumnCount(headerEnum.size());
    m_p->m_TableWidgetEnum->setHorizontalHeaderLabels(headerEnum);
    m_p->m_TableWidgetEnum->verticalHeader()->hide();                           // 隐藏行号
    m_p->m_TableWidgetEnum->horizontalHeader()->setStretchLastSection(true);    // 最后一列自动扩展
    m_p->m_TableWidgetEnum->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可修改
    m_p->m_TableWidgetEnum->setAlternatingRowColors(true);                      // 隔行变色
    m_p->m_TableWidgetEnum->setPalette(QPalette(Qt::gray));                     // 设置隔行变色的颜色

    m_p->m_TableWidgetConst = new QTableWidget;
    QStringList headerConst = { tr("成员名称"), tr("成员数值"), tr("备注") };
    m_p->m_TableWidgetConst->setColumnCount(headerConst.size());
    m_p->m_TableWidgetConst->setHorizontalHeaderLabels(headerConst);
    m_p->m_TableWidgetConst->verticalHeader()->hide();                           // 隐藏行号
    m_p->m_TableWidgetConst->horizontalHeader()->setStretchLastSection(true);    // 最后一列自动扩展
    m_p->m_TableWidgetConst->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可修改
    m_p->m_TableWidgetConst->setAlternatingRowColors(true);                      // 隔行变色
    m_p->m_TableWidgetConst->setPalette(QPalette(Qt::gray));                     // 设置隔行变色的颜色

    m_p->m_TableWidgetComment = new QTableWidget;
    QStringList headerComment = { tr("注释") };
    m_p->m_TableWidgetComment->setColumnCount(1);
    m_p->m_TableWidgetComment->setRowCount(1);
    m_p->m_TableWidgetComment->setVerticalHeaderLabels(headerComment);
    m_p->m_TableWidgetComment->setMinimumWidth(100);
    m_p->m_TableWidgetComment->horizontalHeader()->hide();
    m_p->m_TableWidgetComment->horizontalHeader()->setStretchLastSection(true);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_p->m_TableWidgetComment, 1);
    layout->addWidget(m_p->m_TableWidgetStruct, 9);
    layout->addWidget(m_p->m_TableWidgetEnum, 9);
    layout->addWidget(m_p->m_TableWidgetConst, 9);
    ui.groupBox_Member->setLayout(layout);
    m_p->m_TableWidgetStruct->close();
    m_p->m_TableWidgetEnum->close();
    m_p->m_TableWidgetConst->close();
    m_p->m_TableWidgetComment->close();
}

void ParseHeaderWidget::LoadStyle(const QString& qssFile)
{
    // 加载样式表
    QString qss;
    QFile   file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        // 用 QTextStream 读取样式文件不用区分文件编码，带 bom 也行
        QStringList list;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }

        file.close();
        qss                  = list.join("\n");
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(paletteColor));
        // 用时主要在下面这句
        qApp->setStyleSheet(qss);
    }
}

void ParseHeaderWidget::ShowTreeWidget(const std::string& selectStr)
{
    // 设置树头
    ui.treeWidget->setHeaderLabel(tr("结构体树形图"));
    ui.treeWidget->clear();

    // 显示到界面
    auto& typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    for (auto& parser : typeParserMap) {
        std::string      filename = parser.first;
        QTreeWidgetItem* item     = new QTreeWidgetItem(ROOT);
        item->setText(0, QString::fromStdString(filename.substr(filename.find_last_of('/') + 1)));
        item->setData(0, Qt::UserRole, QString::fromStdString(filename));
        ui.treeWidget->addTopLevelItem(item);

        // 结构体
        auto& structDefs = parser.second.struct_defs_;
        if (structDefs.size() > 0) {
            QTreeWidgetItem* itemStruct = new QTreeWidgetItem(NODE_STRUCT);
            itemStruct->setText(0, QString::fromStdString("Struct"));
            item->addChild(itemStruct);
            for (auto& iter : structDefs) {
                if (iter.first.find(selectStr) != std::string::npos) {
                    QTreeWidgetItem* itemStructChild = new QTreeWidgetItem(LEAF_STRUCT);
                    itemStructChild->setText(0, QString::fromStdString(iter.first));
                    itemStructChild->setData(0, Qt::UserRole, QString::fromStdString(filename));
                    itemStruct->addChild(itemStructChild);
                    auto& structAliases = parser.second.struct_aliases_;
                    for (auto alias = structAliases.lower_bound(iter.first); alias != structAliases.upper_bound(iter.first); ++alias) {
                        QTreeWidgetItem* itemStructAlias = new QTreeWidgetItem(ALIAS_STRUCT);
                        itemStructAlias->setText(0, QString::fromStdString(alias->second));
                        itemStructChild->addChild(itemStructAlias);
                    }
                }
            }
        }

        // 枚举
        auto& enumDefs = parser.second.enum_defs_;
        if (enumDefs.size() > 0) {
            QTreeWidgetItem* itemEnum = new QTreeWidgetItem(NODE_ENUM);
            itemEnum->setText(0, QString::fromStdString("Enum"));
            item->addChild(itemEnum);
            for (auto& iter : enumDefs) {
                if (iter.first.find(selectStr) != std::string::npos) {
                    QTreeWidgetItem* itemEnumChild = new QTreeWidgetItem(LEAF_ENUM);
                    itemEnumChild->setText(0, QString::fromStdString(iter.first));
                    itemEnumChild->setData(0, Qt::UserRole, QString::fromStdString(filename));
                    itemEnum->addChild(itemEnumChild);
                }
            }
        }

        // 常量
    }

    // 触发器
    connect(ui.treeWidget, &QTreeWidget::itemClicked, this, &ParseHeaderWidget::SlotDataTypeShow);
}

void ParseHeaderWidget::ShowTableStruct(const std::string& fileName, const std::string& typeName)
{
    auto          typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    TypeParser    parser        = typeParserMap[fileName];
    QTableWidget* tableWidget   = m_p->m_TableWidgetStruct;

    tableWidget->setRowCount(0);
    m_p->m_TableWidgetComment->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(parser.comments_[typeName])));
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

    m_p->m_TableWidgetEnum->close();
    m_p->m_TableWidgetConst->close();
    m_p->m_TableWidgetStruct->show();
}

void ParseHeaderWidget::ShowTableEnum(const std::string& fileName, const std::string& typeName)
{
    auto          typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
    TypeParser    parser        = typeParserMap[fileName];
    QTableWidget* tableWidget   = m_p->m_TableWidgetEnum;

    tableWidget->setRowCount(0);
    m_p->m_TableWidgetComment->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(parser.comments_[typeName])));
    for (auto var : parser.enum_defs_[typeName]) {
        int rowCount = tableWidget->rowCount();
        tableWidget->insertRow(rowCount);
        tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::fromStdString(var.m_name)));
        tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(var.m_value)));
        tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::fromStdString(var.m_comment)));
    }

    m_p->m_TableWidgetStruct->close();
    m_p->m_TableWidgetConst->close();
    m_p->m_TableWidgetEnum->show();
}

void ParseHeaderWidget::SlotDataTypeExpand()
{
    QString type = ui.pushButton_Expand->text();
    if (type == "展开") {
        ui.pushButton_Expand->setText("折叠");
        ui.treeWidget->expandToDepth(1);
    } else if (type == "折叠") {
        ui.pushButton_Expand->setText("展开");
        ui.treeWidget->collapseAll();
    }
}

void ParseHeaderWidget::SlotDataTypeSelect()
{
    std::string selectStr = ui.lineEdit->text().toStdString();
    ShowTreeWidget(selectStr);
    ui.treeWidget->expandToDepth(1);
}

void ParseHeaderWidget::SlotDataTypeSave()
{
    auto fileMap = DataManager::GetInstance()->GetFileMap();
    for (auto& file : fileMap) {
        std::string                  filename  = file.first;
        std::map<std::string, bool>& structVec = file.second;
        if (structVec.empty()) {
            continue;
        }
        auto       typeParserMap = DataManager::GetInstance()->GetTypeParserMap();
        TypeParser parser        = typeParserMap[filename];
        auto       fileLineVec   = FileManager::GetInstance()->GetFileContent(filename);
        for (size_t i = 0; i < fileLineVec.size(); i++) {
            for (auto& stru : structVec) {
                if (fileLineVec[i].find(stru.first) != std::string::npos && false == stru.second) {
                    std::vector<std::string> lineVec;
                    this->PackageStruct(stru.first, parser.struct_defs_[stru.first], lineVec);
                    size_t j = i;
                    while (fileLineVec[j].find("}") == std::string::npos) {
                        j++;
                    }
                    fileLineVec.erase(fileLineVec.begin() + i, fileLineVec.begin() + j);
                    fileLineVec.insert(fileLineVec.begin() + i, lineVec.begin(), lineVec.end());
                    i += lineVec.size();
                    stru.second = true;
                }
            }
        }
        for (size_t i = fileLineVec.size() - 1; i > fileLineVec.size() - 10; i--) {
            if (fileLineVec[i].find("#endif") != std::string::npos) {
                for (auto& stru : structVec) {
                    if (false == stru.second) {
                        std::vector<std::string> lineVec;
                        this->PackageStruct(stru.first, parser.struct_defs_[stru.first], lineVec);
                        fileLineVec.insert(fileLineVec.begin() + i, lineVec.begin(), lineVec.end());
                        i += lineVec.size();
                        stru.second = true;
                    }
                }
            }
        }
        FileManager::GetInstance()->SaveFile(filename, fileLineVec);
    }
}

void ParseHeaderWidget::SlotDataTypeShow(QTreeWidgetItem* item, int column)
{
    std::string fileName = item->data(column, Qt::UserRole).toByteArray().data();
    std::string typeName = item->text(column).toStdString();
    switch (item->type()) {
    case LEAF_STRUCT:
        ShowTableStruct(fileName, typeName);
        break;
    case LEAF_ENUM:
        ShowTableEnum(fileName, typeName);
        break;
    case LEAF_CONST:
        break;
    case ALIAS_STRUCT:
        SlotDataTypeShow(item->parent(), 0);
    default:
        break;
    }
    m_p->m_TableWidgetComment->show();
    m_p->m_TableWidgetComment->setRowHeight(0, m_p->m_TableWidgetComment->height());
}

void ParseHeaderWidget::SlotTreeRightMenu(const QPoint& pos)
{
    m_p->m_CurrTreeItem = ui.treeWidget->itemAt(pos); // 关键
    if (m_p->m_CurrTreeItem) {
        m_p->m_TreeRightMenu->clear();
        switch (m_p->m_CurrTreeItem->type()) {
        case ROOT:
            m_p->m_TreeRightMenu->addAction(m_p->m_ActionMap["actionAddStruct"]);
            m_p->m_TreeRightMenu->addAction(m_p->m_ActionMap["actionAddEnum"]);
            m_p->m_TreeRightMenu->addAction(m_p->m_ActionMap["actionAddConst"]);
            m_p->m_TreeRightMenu->addAction(m_p->m_ActionMap["actionExpand"]);
            m_p->m_TreeRightMenu->addAction(m_p->m_ActionMap["actionCollapse"]);
            break;
        case NODE_STRUCT:
            m_p->m_TreeRightMenu->addAction(m_p->m_ActionMap["actionAddStruct"]);
            break;
        case NODE_ENUM:
            m_p->m_TreeRightMenu->addAction(m_p->m_ActionMap["actionAddEnum"]);
            break;
        case NODE_CONST:
            m_p->m_TreeRightMenu->addAction(m_p->m_ActionMap["actionAddConst"]);
            break;
        default:
            break;
        }
    }
    // 移动菜单出现在鼠标点击的位置
    m_p->m_TreeRightMenu->move(ui.treeWidget->cursor().pos());
    m_p->m_TreeRightMenu->show();
}

void ParseHeaderWidget::SlotAddStruct()
{
    QTreeWidgetItem* item = ui.treeWidget->currentItem();
    m_p->m_CurrFile       = item->data(0, Qt::UserRole).toByteArray().data();

    if (nullptr == m_p->m_StructOperateWidget) {
        m_p->m_StructOperateWidget = new StructOperateWidget(this);
        connect(m_p->m_StructOperateWidget->ui.pushButton_Finish, &QPushButton::clicked, this, &ParseHeaderWidget::SlotAddStructFinish);
    }
    std::list<std::string> typeList = DataManager::GetInstance()->GetTypeList();
    QStringList            typeQList;
    for (auto type : typeList) {
        typeQList << QString::fromStdString(type);
    }
    m_p->m_StructOperateWidget->UpdateStructTypeList(typeQList);
    m_p->m_StructOperateWidget->InitTableWidget();
    m_p->m_StructOperateWidget->show();
}

void ParseHeaderWidget::SlotAddStructFinish()
{
    // 获取界面数据并保存
    std::string                  structName = m_p->m_StructOperateWidget->ui.lineEdit_Name->text().toStdString();
    std::list<StructDeclaration> varList;
    for (int i = 0; i < m_p->m_StructOperateWidget->ui.tableWidget->rowCount(); i++) {
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
    if (iter != typeParserMap.end()) {
        iter->second.struct_defs_.emplace(structName, varList);
    }

    // 表示文件修改
    DataManager::GetInstance()->SetFileMap(m_p->m_CurrFile, structName, false);
    m_p->m_StructOperateWidget->hide();
    ShowTreeWidget();
    ShowTableStruct(m_p->m_CurrFile, structName);
}

void ParseHeaderWidget::SlotExpandByDepth(const QTreeWidgetItem* item, int depth)
{
    if (depth < 0) {
        return;
    }
    ui.treeWidget->expandItem(item);
    for (int i = 0; i < item->childCount(); i++) {
        this->SlotExpandByDepth(item->child(i), depth - 1);
    }
}

void ParseHeaderWidget::SlotcollapseByDepth(const QTreeWidgetItem* item, int depth)
{
    if (depth < 0) {
        return;
    }
    ui.treeWidget->collapseItem(item);
    for (int i = 0; i < item->childCount(); i++) {
        this->SlotcollapseByDepth(item->child(i), depth - 1);
    }
}

void ParseHeaderWidget::PackageStruct(const std::string& structName, const std::list<StructDeclaration>& varList, std::vector<std::string>& lineVec)
{
    std::string temp;
    temp = "struct " + structName;
    lineVec.push_back(temp);
    temp.clear();

    lineVec.push_back("{");
    for (auto var : varList) {
        temp += "\t" + var.m_type;
        if (var.m_is_pointer) {
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
