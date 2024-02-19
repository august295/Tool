#include <QtWidgets/QMessageBox>

#include "core/tcp_client/tcp_client.h"

#include "ClientWidget.h"

struct ClientWidget::ClientWidgetPrivate
{
    std::shared_ptr<TcpClient> m_tcp_client;
};

ClientWidget::ClientWidget(QString protocol, QString ip, QString port, QWidget* parent /*= nullptr*/)
    : QWidget(parent)
    , ui(new Ui::ClientWidgetClass())
    , m_impl(new ClientWidgetPrivate)
{
    ui->setupUi(this);

    this->Init();

    ui->comboBox_protocol->setCurrentText(protocol);
    ui->lineEdit_ip->setText(ip);
    ui->lineEdit_port->setText(port);
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::Init()
{
    this->InitNetworkSetting();
    this->InitSendArea();
    this->InitRecvArea();
}

void ClientWidget::InitNetworkSetting()
{
    QStringList protocol_list = {"TCP", "UDP"};
    ui->comboBox_protocol->addItems(protocol_list);

    connect(ui->pushButton_connect, &QPushButton::clicked, this, &ClientWidget::SlotConnect);

    // 创建一个正则表达式验证器，用于验证输入的文本是否符合IP地址格式
    QRegExpValidator* validator = new QRegExpValidator(QRegExp(
        "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"));
    ui->lineEdit_ip->setValidator(validator);
    // 用于占位
    ui->lineEdit_ip->setInputMask("000.000.000.000; ");
}

void ClientWidget::InitSendArea()
{
    connect(ui->pushButton_send, &QPushButton::clicked, this, &ClientWidget::SlotSendData);
    connect(ui->pushButton_send_clear, &QPushButton::clicked, this, [=] {
        ui->textEdit_send->clear();
    });
}

void ClientWidget::InitRecvArea()
{
    connect(ui->pushButton_recv_clear, &QPushButton::clicked, this, [=] {
        ui->textEdit_recv->clear();
    });
}

void ClientWidget::SlotConnect()
{
    QString protocol = ui->comboBox_protocol->currentText();
    QString ip       = ui->lineEdit_ip->text();
    QString port     = ui->lineEdit_port->text();
    QString name     = ui->pushButton_connect->text();
    if (name == tr("连接"))
    {
        if (protocol == "TCP")
        {
            m_impl->m_tcp_client = std::make_shared<TcpClient>();
            if (m_impl->m_tcp_client->connect_server(ip, port.toInt()))
            {
                ui->pushButton_connect->setText(tr("断开"));
                emit SignalConnect(NetworkState::CONNECT);
                connect(m_impl->m_tcp_client.get(), &TcpClient::signal_recv, this, &ClientWidget::SlotRecvData);
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr("连接失败"), QMessageBox::Yes, QMessageBox::Yes);
            }
        }
    }
    else if (name == tr("断开"))
    {
        if (protocol == "TCP")
        {
            m_impl->m_tcp_client->disconnect_server();
            ui->pushButton_connect->setText(tr("连接"));
            emit SignalConnect(NetworkState::DISCONNECT);
            m_impl->m_tcp_client = nullptr;
        }
    }
}

void ClientWidget::SlotRecvData(const QByteArray& data)
{
    auto socket = m_impl->m_tcp_client->get_tcp_socket();
    // 获取对端IP地址和端口号
    QHostAddress peerAddress = socket->peerAddress();
    quint16      peerPort    = socket->peerPort();
    ui->textEdit_recv->append(QString("[%1:%2] %3")
                                  .arg(peerAddress.toString(), 15)
                                  .arg(QString::number(peerPort), 5)
                                  .arg(QString::fromUtf8(data)));
}

void ClientWidget::SlotSendData()
{
    QString protocol = ui->comboBox_protocol->currentText();

    Qt::CheckState state = ui->checkBox_send_file->checkState();
    if (state == Qt::Checked)
    {
    }
    else
    {
        if (protocol == "TCP")
        {
            QString msg = ui->textEdit_send->toPlainText();
            m_impl->m_tcp_client->send_message_string(msg.toUtf8());
        }
    }
}
