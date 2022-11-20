#pragma once

#include <QtWidgets/QWidget>

#include "ui_StructOperateWidget.h"

class StructOperateWidget : public QWidget
{
    Q_OBJECT

public:
    StructOperateWidget(QWidget* parent = Q_NULLPTR);
    ~StructOperateWidget();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	初始化界面. </summary>
    ///
    /// <remarks>	August295, 2022/9/6. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void InitTableWidget();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	更新结构体类型列表. </summary>
    ///
    /// <remarks>	August295, 2022/9/8. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateStructTypeList(const QStringList& structTypeList);

public slots:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	结构体变量编辑. </summary>
    ///		1. 添加变量
    ///		2. 删除变量
    ///		3. 上移变量
    ///		4. 下移变量
    ///		5. 清空
    ///		6. 完成编辑
    /// <remarks>	August295, 2022/9/7. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void on_pushButton_Add();
    void on_pushButton_Delete();
    void on_pushButton_Up();
    void on_pushButton_Down();
    void on_pushButton_Clear();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	行操作. </summary>
    ///		1. 移动行
    ///		2. 拷贝行
    /// <remarks>	August295, 2022/9/7. </remarks>
    ///
    /// <param name="pTable">	[in,out] 表格指针. </param>
    /// <param name="nFrom"> 	当前行. </param>
    /// <param name="nTo">   	目标行. </param>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void MoveRow(QTableWidget* pTable, int nFrom, int nTo);
    void CopyRow(QTableWidget* pTable, int nFrom, int nTo);

public:
    Ui::StructOperateWidget ui;

    QStringList _StructTypeList; // 结构体类型
};
