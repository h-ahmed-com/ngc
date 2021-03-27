#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <iostream>
#include <custom/pstream.h>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QClipboard>

/**
 ** This file is part of the Gui-ngrok project.
 ** Copyright 2021 anasybal <anasybal@protonmail.com>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

QT_BEGIN_NAMESPACE
namespace Ui{class MainWindow;}
QT_END_NAMESPACE

class MainWindow:public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QClipboard *clipboard = QGuiApplication::clipboard();
private slots:
    void on_btn_start_clicked();
    void on_btn_stop_clicked();
    void closeEvent(QCloseEvent *event);
    void on_ng_copy_clicked();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
