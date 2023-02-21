#pragma once

#include <list>
#include <string>
#include <vector>

#include <QtWidgets/QWidget>
#include <QtWidgets/QTableWidgetItem>

#include "ui_ParseHeaderWidget.h"

struct StructDeclaration;

class ParseHeaderWidget : public QWidget {
    Q_OBJECT

public:
    enum NodeType {
        ROOT = QTreeWidgetItem::UserType,
        NODE_STRUCT,
        NODE_ENUM,
        NODE_CONST,
        LEAF_STRUCT,
        LEAF_ENUM,
        LEAF_CONST,
        ALIAS_STRUCT,
    };

public:
    ParseHeaderWidget(QWidget* parent = Q_NULLPTR);
    ~ParseHeaderWidget();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	初始化界面. </summary>
    ///
    /// <remarks>	August295, 2022/9/6. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void Init();
    void InitTreeWidget();
    void InitTableWidget();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	加载样式. </summary>
    ///
    /// <remarks>	August295, 2023/02/13. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadStyle(const QString& qssFile);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	树形图显示数据. </summary>
    ///     1. 显示树形图
    ///     2. 判断结构体是否包含
    ///     3. 判断枚举是否包含
    /// <remarks>	August295, 2022/11/24. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void ShowTreeWidget(const std::string& selectStr);
    bool ContainStruct(const std::string&                             structName,
                       const std::multimap<std::string, std::string>& structAliases,
                       const std::string&                             selectStr);
    bool ContainEnum(const std::string& enumName,
                     const std::string& selectStr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	表格显示数据. </summary>
    ///
    /// <remarks>	August295, 2022/11/24. </remarks>
    ///
    /// <param name="fileName">     所在文件名称. </param>
    /// <param name="typeName">     结构名称. </param>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void ShowTableStruct(const std::string& fileName, const std::string& typeName);
    void ShowTableEnum(const std::string& fileName, const std::string& typeName);

public slots:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	数据类型数操作. </summary>
    ///		1. 刷新结构体
    ///		2. 查询结构体
    ///     3. 保存到文件
    ///     4. 表格显示详细信息
    ///     5. 导入头文件
    /// <remarks>	August295, 2022/9/6. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SlotDataTypeExpand();
    void SlotDataTypeSelect();
    void SlotDataTypeSave();
    void SlotDataTypeShow(QTreeWidgetItem* item, int column);
    void SlotDataTypeImport();

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
    /// <summary>	展开、折叠. </summary>
    ///
    /// <remarks>	August295, 2023/02/13. </remarks>
    ///
    /// <param name="item">     树节点. </param>
    /// <param name="depth">	深度，0本身结束. </param>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SlotExpandByDepth(const QTreeWidgetItem* item, int depth);
    void SlotcollapseByDepth(const QTreeWidgetItem* item, int depth);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	表格双击事件. </summary>
    ///
    /// <remarks>	August295, 2023/02/13. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SlotStructTableDoubleClicked(QTableWidgetItem *item);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	组装结构体字符串. </summary>
    ///
    /// <remarks>	August295, 2022/9/8. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void PackageStruct(const std::string& structName, const std::list<StructDeclaration>& varList, std::vector<std::string>& lineVec);

private:
    Ui::ParseHeaderClass ui;

    struct ParseHeaderWidgetPrivate;
    ParseHeaderWidgetPrivate* m_p;
};
