#include "DelegateComboBox.h"

DelegateComboBox::DelegateComboBox(QObject* parent, QStringList structTypeList)
    : QStyledItemDelegate(parent)
{
    _StructTypeList = structTypeList;
}

DelegateComboBox::~DelegateComboBox()
{
}

QWidget* DelegateComboBox::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index); // 提示编译器，参数未使用
    Q_UNUSED(option);

    // 实现可编辑并模糊查询
    QComboBox*        comboBox  = new QComboBox(parent);
    QLineEdit*        lineEdit  = new QLineEdit;
    QCompleter*       completer = new QCompleter;
    QStringListModel* lisModel  = new QStringListModel;
    lisModel->setStringList(_StructTypeList);
    completer->setModel(lisModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive); // 大小写不敏感
    completer->setFilterMode(Qt::MatchRecursive);       // 匹配规则：包含则匹配
    comboBox->addItems(_StructTypeList);                // 添加列表
    comboBox->setEditable(true);                        // 设置可编辑
    comboBox->setCompleter(completer);                  // 模糊匹配
    comboBox->setLineEdit(lineEdit);                    // 与编辑绑定

    return comboBox;
}

void DelegateComboBox::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    // 获取单元格内容
    QString    str          = index.data().toString();
    QComboBox* box          = static_cast<QComboBox*>(editor);
    int        currentIndex = box->findText(str);
    if (currentIndex >= 0) {
        box->setCurrentIndex(currentIndex);
    }
}

void DelegateComboBox::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    // 获取编辑控件的内容
    QString str = static_cast<QComboBox*>(editor)->currentText();
    // 设置模型数据
    model->setData(index, str);
}

void DelegateComboBox::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    // 更新控件位置大小
    editor->setGeometry(option.rect);
}
