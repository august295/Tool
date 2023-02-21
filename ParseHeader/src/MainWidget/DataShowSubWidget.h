#ifndef DATASHOWSUBWIDGET_H
#define DATASHOWSUBWIDGET_H

#include <QWidget>

namespace Ui {
class DataShowSubWidget;
}

class DataShowSubWidget : public QWidget {
    Q_OBJECT

public:
    explicit DataShowSubWidget(QWidget* parent = nullptr);
    ~DataShowSubWidget();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	初始化界面. </summary>
    ///
    /// <remarks>	August295, 2023/02/20. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void InitWidget();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	表格显示数据. </summary>
    ///
    /// <remarks>	August295, 2023/02/20. </remarks>
    ///
    /// <param name="fileName">     所在文件名称. </param>
    /// <param name="typeName">     结构名称. </param>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void ShowTableStruct(const std::string& fileName, const std::string& typeName);
    void ShowTableEnum(const std::string& fileName, const std::string& typeName);
    void AdjustTableComment();

signals:
    void SignalClose(const QString& typeName);

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	重写事件. </summary>
    ///
    /// <remarks>	August295, 2023/02/20. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::DataShowSubWidget* ui;

    struct DataShowSubWidgetPrivate;
    DataShowSubWidgetPrivate* m_p;
};

#endif // DATASHOWSUBWIDGET_H
