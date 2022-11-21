#include <QtCore/QString>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMenu>

#include "ParseHeader/utility.h"

#include "FileManage.h"
#include "ParseHeaderWidget.h"
#include "StructOperateWidget.h"

struct ParseHeaderWidget::ParseHeaderWidgetPrivate
{
    ParseHeaderWidgetPrivate()
        : _StructOperateWidget(nullptr)
    {
    }

    std::map<std::string, TypeParser>                  _TypeParserMap; // 头文件解析 map<文件名, 解析类>
    std::map<std::string, std::map<std::string, bool>> _FileMap;       // 需解析文件 map<文件名, map<修改结构体, 是否保存>>

    std::string          _CurrFile;            // 当前操作文件
    StructOperateWidget* _StructOperateWidget; // 结构体操作界面

    QMenu* TreeRightMenu; // 树右键菜单
};

ParseHeaderWidget::ParseHeaderWidget(QWidget* parent)
    : QWidget(parent)
    , _p(new ParseHeaderWidgetPrivate)
{
    ui.setupUi(this);
    setWindowTitle(QString("文件转换"));
    setMinimumSize(1200, 800);

    // 初始化界面
    ParseFiles();
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
    if (_p)
    {
        delete _p;
        _p = nullptr;
    }
}

void ParseHeaderWidget::ParseFiles()
{
    // 解析指定文件夹下头文件
    std::set<std::string> inc_paths;
    inc_paths.insert("../../../src/resources");

    for (auto path : inc_paths)
    {
        FindFiles(path, _p->_FileMap);
        for (auto file : _p->_FileMap)
        {
            std::string filename = file.first;
            TypeParser  parser;
            parser.ParseFile(filename);
            _p->_TypeParserMap.emplace(filename, parser);
        }
    }
}

void ParseHeaderWidget::InitTreeWidget(const std::string& selectStr /*= ""*/)
{
    // 设置树头
    ui.treeWidget->setHeaderLabel(QString("结构体树形图"));
    ui.treeWidget->clear();

    // 显示到界面
    for (auto parser : _p->_TypeParserMap)
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

void ParseHeaderWidget::GetStructTypeList(QStringList& structTypeList)
{
    // 基础类型
    for (auto basic : data_types)
    {
        structTypeList.push_back(QString::fromStdString(basic));
    }
    for (auto parser : _p->_TypeParserMap)
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
    for (auto& file : _p->_FileMap)
    {
        std::string                  filename  = file.first;
        std::map<std::string, bool>& structVec = file.second;
        if (structVec.empty())
        {
            continue;
        }
        TypeParser               parser      = _p->_TypeParserMap[filename];
        std::vector<std::string> fileLineVec = FileManage::GetInstance()->GetFileContent(filename);
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

void ParseHeaderWidget::ShowTableStruct(const std::string& file, const std::string& stru)
{
    TypeParser parser = _p->_TypeParserMap[file];

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
    _p->TreeRightMenu  = new QMenu;
    QAction* addAction = new QAction(QString("增加"));
    connect(addAction, &QAction::triggered, this, &ParseHeaderWidget::AddStruct);

    QTreeWidgetItem* item = ui.treeWidget->itemAt(pos); //关键
    if (item)
    {
        switch (item->type())
        {
        case NODE:
            _p->TreeRightMenu->addAction(addAction);
            break;
        default:
            break;
        }
    }
    //移动菜单出现在鼠标点击的位置
    _p->TreeRightMenu->move(ui.treeWidget->cursor().pos());
    _p->TreeRightMenu->show();
}

void ParseHeaderWidget::AddStruct()
{
    QTreeWidgetItem* item = ui.treeWidget->currentItem();
    _p->_CurrFile         = item->data(0, Qt::UserRole).toByteArray().data();

    if (nullptr == _p->_StructOperateWidget)
    {
        _p->_StructOperateWidget = new StructOperateWidget;
        connect(_p->_StructOperateWidget->ui.pushButton_Finish, &QPushButton::clicked, this, &ParseHeaderWidget::AddStructFinish);
    }
    QStringList structTypeList;
    GetStructTypeList(structTypeList);
    _p->_StructOperateWidget->UpdateStructTypeList(structTypeList);
    _p->_StructOperateWidget->InitTableWidget();
    _p->_StructOperateWidget->show();
}

void ParseHeaderWidget::AddStructFinish()
{
    // 获取界面数据并保存
    std::string                  structName = _p->_StructOperateWidget->ui.lineEdit_Name->text().toStdString();
    std::list<StructDeclaration> varList;
    for (int i = 0; i < _p->_StructOperateWidget->ui.tableWidget->rowCount(); i++)
    {
        QTableWidget*     tableWidget = _p->_StructOperateWidget->ui.tableWidget;
        StructDeclaration var;
        var.m_type       = tableWidget->item(i, 0)->text().toLocal8Bit().data();
        var.m_is_pointer = tableWidget->item(i, 1)->checkState();
        var.m_name       = tableWidget->item(i, 2)->text().toLocal8Bit().data();
        var.m_comment    = tableWidget->item(i, 3)->text().toLocal8Bit().data();
        varList.push_back(var);
    }
    auto iter = _p->_TypeParserMap.find(_p->_CurrFile);
    if (iter != _p->_TypeParserMap.end())
    {
        iter->second.struct_defs_.emplace(structName, varList);
    }

    // 表示文件修改
    _p->_FileMap[_p->_CurrFile].emplace(structName, false);
    _p->_StructOperateWidget->hide();
    InitTreeWidget();
    ShowTableStruct(_p->_CurrFile, structName);
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
