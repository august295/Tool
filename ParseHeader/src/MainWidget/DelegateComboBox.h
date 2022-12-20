#pragma once

#include <QtCore/QStringList>
#include <QtCore/QStringListModel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStyledItemDelegate>

class DelegateComboBox : public QStyledItemDelegate {
    Q_OBJECT

public:
    DelegateComboBox(QObject* parent, QStringList strList);
    ~DelegateComboBox();

    // 创建委托控件
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    // 设置控件数据
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    // 设置模型数据
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    // 设置控件位置
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    QStringList _StructTypeList; // 下拉框内容
};
