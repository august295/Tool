#include <QtWidgets/QMessageBox>

#include "DebuggerWidget.h"

DebuggerWidget::DebuggerWidget(NetworkType type, QString protocol, QString ip, QString port, QWidget* parent /*= nullptr*/)
    : QWidget(parent)
    , ui(new Ui::DebuggerWidgetClass())
{
    ui->setupUi(this);

    this->Init();

    m_type = type;
    ui->comboBox_protocol->setCurrentText(protocol);
    ui->lineEdit_ip->setText(ip);
    ui->lineEdit_port->setText(port);
}

DebuggerWidget::~DebuggerWidget()
{
    delete ui;
}

void DebuggerWidget::Init()
{
    this->InitNetworkSetting();
    this->InitSendArea();
    this->InitRecvArea();
}

void DebuggerWidget::InitNetworkSetting()
{
    QStringList protocol_list = {"TCP", "UDP"};
    ui->comboBox_protocol->addItems(protocol_list);

    connect(ui->pushButton_connect, &QPushButton::clicked, this, &DebuggerWidget::SlotConnect);

    // 创建一个正则表达式验证器，用于验证输入的文本是否符合IP地址格式
    QRegExpValidator* validator = new QRegExpValidator(QRegExp(
        "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"));
    ui->lineEdit_ip->setValidator(validator);
    // 用于占位
    ui->lineEdit_ip->setInputMask("000.000.000.000; ");
}

void DebuggerWidget::InitSendArea()
{
}

void DebuggerWidget::InitRecvArea()
{
}

void DebuggerWidget::SlotConnect()
{
    QString protocol = ui->comboBox_protocol->currentText();
    QString ip       = ui->lineEdit_ip->text();
    QString port     = ui->lineEdit_port->text();
    QString name     = ui->pushButton_connect->text();
    if (name == tr("连接"))
    {
        if (m_type == NetworkType::LEAF_CLIENT && protocol == "TCP")
        {
            m_tcp_client = std::make_shared<TcpClient>();
            if (m_tcp_client->connect_server(ip, port.toInt()))
            {
                ui->pushButton_connect->setText(tr("断开"));
                emit SignalConnect(NetworkState::CONNECT);
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr("连接失败"), QMessageBox::Yes, QMessageBox::Yes);
            }
        }
    }
    else if (name == tr("断开"))
    {
        if (m_type == NetworkType::LEAF_CLIENT && protocol == "TCP")
        {
            m_tcp_client->disconnect_server();
            ui->pushButton_connect->setText(tr("连接"));
            emit SignalConnect(NetworkState::DISCONNECT);
            m_tcp_client = nullptr;
        }
    }
}
