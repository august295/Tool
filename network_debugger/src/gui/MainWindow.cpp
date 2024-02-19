#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

#include "ClientWidget.h"
#include "MainWindow.h"

struct MainWindow::MainWindowPrivate
{
    QMap<QString, QMenu*>   m_MenuMap;
    QDockWidget*            m_DockWidgetPropertyBar;
    QMenu*                  m_RightMenu;
    QMap<QString, QAction*> m_ActionMap;
    QTreeWidgetItem*        m_CurrTreeWidgetItem;
    QTabWidget*             m_TabWidget;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClass)
    , m_impl(new MainWindowPrivate)
{
    ui->setupUi(this);

    this->Init();
}

MainWindow::~MainWindow()
{
    for (auto menu : m_impl->m_MenuMap)
    {
        delete menu;
    }
    m_impl->m_MenuMap.clear();
    delete m_impl->m_DockWidgetPropertyBar;
    delete m_impl->m_RightMenu;
    for (auto action : m_impl->m_ActionMap)
    {
        delete action;
    }
    m_impl->m_ActionMap.clear();
    m_impl->m_CurrTreeWidgetItem = nullptr;
    delete m_impl->m_TabWidget;
    delete m_impl;
    delete ui;
}

void MainWindow::Init()
{
    setWindowTitle(tr("网络调试助手"));
    showMaximized();
    setWindowIcon(QIcon(":/icons/network.png"));

    this->InitMenuBar();
    this->InitView();
    this->InitTabWidget();
}

void MainWindow::InitMenuBar()
{
    // 创建菜单栏
    QMenuBar* menuBar   = this->menuBar();
    QMenu*    menu_view = menuBar->addMenu(tr("查看"));
    QMenu*    menu_help = menuBar->addMenu(tr("帮助"));

    m_impl->m_MenuMap["menu_view"] = menu_view;
    m_impl->m_MenuMap["menu_help"] = menu_help;
}

void MainWindow::InitView()
{
    // 悬浮窗
    m_impl->m_DockWidgetPropertyBar = new QDockWidget(this);
    m_impl->m_DockWidgetPropertyBar->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_impl->m_DockWidgetPropertyBar->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_impl->m_DockWidgetPropertyBar->setWindowTitle(tr("属性栏"));
    this->addDockWidget(Qt::LeftDockWidgetArea, m_impl->m_DockWidgetPropertyBar);

    // 属性树
    QTreeWidget*     tree        = new QTreeWidget;
    QTreeWidgetItem* item_client = new QTreeWidgetItem(tree, NetworkType::ROOT_CLIENT);
    item_client->setText(0, tr("客户端"));
    item_client->setIcon(0, QIcon(":/icons/client.png"));
    QTreeWidgetItem* item_server = new QTreeWidgetItem(tree, NetworkType::ROOT_SERVER);
    item_server->setText(0, tr("服务端"));
    item_server->setIcon(0, QIcon(":/icons/server.png"));
    tree->setHeaderHidden(true);
    tree->expandAll();
    m_impl->m_DockWidgetPropertyBar->setWidget(tree);

    // 右键
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree, &QTreeWidget::customContextMenuRequested, this, &MainWindow::SlotShowRightMenu);
    m_impl->m_RightMenu                      = new QMenu;
    QAction* action_add_client               = new QAction(tr("创建客户端"));
    QAction* action_add_server               = new QAction(tr("创建服务端"));
    QAction* action_del                      = new QAction(tr("删除"));
    m_impl->m_ActionMap["action_add_client"] = action_add_client;
    m_impl->m_ActionMap["action_add_server"] = action_add_server;
    m_impl->m_ActionMap["action_del"]        = action_del;
    connect(action_add_client, &QAction::triggered, this, &MainWindow::SlotCreateClient);
    connect(action_add_server, &QAction::triggered, this, &MainWindow::SlotCreateServer);
    connect(action_del, &QAction::triggered, this, &MainWindow::SlotDelete);

    // 菜单栏显示
    QAction* action_property_bar = new QAction(tr("属性栏"));
    m_impl->m_MenuMap["menu_view"]->addAction(action_property_bar);
    connect(action_property_bar, &QAction::triggered, this, [=] {
        if (m_impl->m_DockWidgetPropertyBar->isHidden() == true)
        {
            m_impl->m_DockWidgetPropertyBar->show();
        }
        else
        {
            m_impl->m_DockWidgetPropertyBar->hide();
        }
    });
}

void MainWindow::InitTabWidget()
{
    QTabWidget* tab_widget = new QTabWidget;
    tab_widget->setTabsClosable(true);
    tab_widget->hide();
    QObject::connect(tab_widget, &QTabWidget::tabCloseRequested, [=](int index) {
        QWidget* widgetToRemove = tab_widget->widget(index);
        tab_widget->removeTab(index);
        delete widgetToRemove;
        if (tab_widget->count() == 0)
        {
            tab_widget->hide();
        }
    });

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(tab_widget);
    ui->centralWidget->setLayout(layout);

    m_impl->m_TabWidget = tab_widget;
}

void MainWindow::SlotShowRightMenu(QPoint point)
{
    QMenu* menu = m_impl->m_RightMenu;
    menu->clear();
    // 获取右键条目
    QTreeWidget*     widget = static_cast<QTreeWidget*>(m_impl->m_DockWidgetPropertyBar->widget());
    QTreeWidgetItem* item   = widget->itemAt(point);
    if (item == nullptr)
    {
        return;
    }

    switch (item->type())
    {
    case NetworkType::ROOT_CLIENT:
        menu->addAction(m_impl->m_ActionMap["action_add_client"]);
        break;
    case NetworkType::ROOT_SERVER:
        menu->addAction(m_impl->m_ActionMap["action_add_server"]);
        break;
    case NetworkType::LEAF_CLIENT:
    case NetworkType::LEAF_SERVER:
    default:
        menu->addAction(m_impl->m_ActionMap["action_del"]);
        break;
    }
    menu->exec(QCursor::pos());
    m_impl->m_CurrTreeWidgetItem = item;
}

void MainWindow::SlotCreateClient()
{
    QString       protocol = "TCP";
    QString       ip       = "127.0.0.1";
    QString       port     = "10000";
    ClientWidget* tab      = new ClientWidget(protocol, ip, port);
    m_impl->m_TabWidget->addTab(tab, ip + ":" + port);
    m_impl->m_TabWidget->setCurrentWidget(tab);
    m_impl->m_TabWidget->show();

    QTreeWidget*     tree_widget = static_cast<QTreeWidget*>(m_impl->m_DockWidgetPropertyBar->widget());
    QTreeWidgetItem* item_client = tree_widget->topLevelItem(0);
    QTreeWidgetItem* item_child  = new QTreeWidgetItem(item_client, NetworkType::LEAF_CLIENT);
    item_child->setText(0, ip + ":" + port);
    item_child->setIcon(0, QIcon(":/icons/state_gray.png"));

    connect(tab, &ClientWidget::SignalConnect, this, [=](NetworkState state) {
        if (state == NetworkState::CONNECT)
        {
            item_child->setIcon(0, QIcon(":/icons/state_green.png"));
        }
        else if (state == NetworkState::DISCONNECT)
        {
            item_child->setIcon(0, QIcon(":/icons/state_gray.png"));
        }
    });
}

void MainWindow::SlotCreateServer()
{
}

void MainWindow::SlotDelete()
{
    QTreeWidget* widget = static_cast<QTreeWidget*>(m_impl->m_DockWidgetPropertyBar->widget());
    widget->removeItemWidget(m_impl->m_CurrTreeWidgetItem, 0);
}
