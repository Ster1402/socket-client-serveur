#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serveur.h"
#include "client.h"
#include <QMainWindow>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void launchServer(QObject* sender = nullptr);
    void connectToServer(QObject* sender = nullptr);

private:
    Ui::MainWindow *ui;
    Client* client;
    Serveur* serveur;
};

#endif // MAINWINDOW_H
