#pragma once

#include <QMainWindow>

#include "Common.hpp"
#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindowClass;
};
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void Init();
    void InitMenuBar();
    void InitView();
    void InitTabWidget();

public slots:
    void SlotShowRightMenu(QPoint point);
    void SlotCreateClient();
    void SlotCreateServer();
    void SlotDelete();

private:
    Ui::MainWindowClass* ui;

    struct MainWindowPrivate;
    MainWindowPrivate* m_impl;
};
