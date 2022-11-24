#pragma once

#include <map>
#include <set>
#include <string>

#include <QtWidgets/QWidget>

#include "ui_ParseHeaderWidget.h"

struct StructDeclaration;

class ParseHeaderWidget : public QWidget
{
    Q_OBJECT

public:
    ParseHeaderWidget(QWidget* parent = Q_NULLPTR);
    ~ParseHeaderWidget();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	初始化界面. </summary>
    ///
    /// <remarks>	August295, 2022/9/6. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void InitWidget();
    void InitTreeWidget();
    void InitTableWidget();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	数据显示. </summary>
    ///
    /// <remarks>	August295, 2022/11/24. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void ShowTreeWidget(const std::string& selectStr = "");
    void ShowTableStruct(const std::string& fileName, const std::string& typeName);
    void ShowTableEnum(const std::string& fileName, const std::string& typeName);

public slots:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	数据类型数操作. </summary>
    ///		1. 刷新结构体
    ///		2. 查询结构体
    ///     3. 保存到文件
    /// <remarks>	August295, 2022/9/6. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SlotDataTypeFlush();
    void SlotDataTypeSelect();
    void SlotDataTypeSave();
    void SlotDataTypeShow(QTreeWidgetItem* item, int column);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	右键菜单. </summary>
    ///		1. 显示菜单
    ///     2. 添加结构体
    ///     3. 完成结构体添加
    /// <remarks>	August295, 2022/9/7. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SlotTreeRightMenu(const QPoint& pos);
    void SlotAddStruct();
    void SlotAddStructFinish();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	组装结构体字符串. </summary>
    ///
    /// <remarks>	August295, 2022/9/8. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void PackageStruct(const std::string& structName, const std::list<StructDeclaration>& varList, std::vector<std::string>& lineVec);

public:
    enum NodeType
    {
        ROOT = QTreeWidgetItem::UserType,
        NODE_STRUCT,
        NODE_ENUM,
        NODE_CONST,
        LEAF_STRUCT,
        LEAF_ENUM,
        LEAF_CONST,
    };

private:
    Ui::ParseHeaderClass ui;

    struct ParseHeaderWidgetPrivate;
    ParseHeaderWidgetPrivate* m_p;
};
