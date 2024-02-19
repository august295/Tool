#pragma once

#include <QWidget>

#include "ui_ClientWidget.h"

#include "Common.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class ClientWidgetClass;
};
QT_END_NAMESPACE

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    ClientWidget(QString protocol, QString ip, QString port, QWidget* parent = nullptr);
    ~ClientWidget();

    /**
     * @brief 初始化
     */
    void Init();
    void InitNetworkSetting();
    void InitSendArea();
    void InitRecvArea();

public slots:
    void SlotConnect();

	/**
     * @brief 接收信息
     */
    void SlotRecvData(const QByteArray& data);

	/**
     * @brief 发送信息
     */
    void SlotSendData();

signals:
    void SignalConnect(NetworkState state);

private:
    Ui::ClientWidgetClass* ui;

    struct ClientWidgetPrivate;
    ClientWidgetPrivate* m_impl;
};
