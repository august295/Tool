#pragma once

#include <QWidget>

#include "core/tcp_client/tcp_client.h"

#include "Common.hpp"
#include "ui_DebuggerWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class DebuggerWidgetClass;
};
QT_END_NAMESPACE

class DebuggerWidget : public QWidget
{
    Q_OBJECT

public:
    DebuggerWidget(NetworkType type, QString protocol, QString ip, QString port, QWidget* parent = nullptr);
    ~DebuggerWidget();

    void Init();
    void InitNetworkSetting();
    void InitSendArea();
    void InitRecvArea();

public slots:
    void SlotConnect();

signals:
    void SignalConnect(NetworkState state);

private:
    Ui::DebuggerWidgetClass* ui;

    NetworkType m_type;
    std::shared_ptr<TcpClient>  m_tcp_client;
};
