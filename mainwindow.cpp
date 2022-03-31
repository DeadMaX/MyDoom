#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->welcome);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog::getOpenFileContent("Doom Files (*.wad)", [this](const QString& fileName, const QByteArray& data) {
       // try
        {
            if (!fileName.isEmpty())
            {
                auto new_game = std::make_shared<Doom>(ui->game, data);
                doom_game = new_game;
            }
        //} catch(...)
        //{
        }

        if (doom_game)
            ui->stackedWidget->setCurrentWidget(ui->game);
        else
            ui->stackedWidget->setCurrentWidget(ui->welcome);
       });
    ui->stackedWidget->setCurrentWidget(ui->loading);
}

