#include "mainwindow.h"
#include "ui_mainwindow.h"

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::string out, err, username;
    redi::ipstream user("whoami", redi::pstreams::pstdout);
    std::getline(user.out(), username);
    redi::ipstream curl("which curl", redi::pstreams::pstdout);
    std::getline(curl.out(), out);
    if(out.empty()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Curl");
        msgBox.setText("curl command not found and it's important to use this application !.\nplease download it and try again.");
        msgBox.exec();
        exit(1);
    }
    redi::ipstream ngrok("which ngrok", redi::pstreams::pstdout);
    std::getline(ngrok.out(), out);
    if(out.empty()){
        if(QFile("./ngrok").exists()){
            if(QDir(QDir::homePath() + "/.local/bin").exists()){system("cp -f ./ngrok  ~/.local/bin/");}
            else{system("mkdir ~/.local/bin");system("cp -f ./ngrok  ~/.local/bin/");}
        }else{
            QMessageBox msgBox;
            msgBox.setWindowTitle("Ngrok");
            msgBox.setText("Ngrok command not found (locale & globale) and it's important to use this application !.\nplease download it and try again.");
            msgBox.exec();
            exit(1);
        }
    }
    QFile File("/home/" + QString::fromStdString(username) + "/.ngrok2/ngrok.yml");
    if(File.exists()){
        File.open(QIODevice::ReadOnly);
        QString token = File.readAll();
        token.replace(QString("\n"),QString(""));
        token.replace(QString("authtoken: "),QString(""));
        ui->ng_token->clear();
        ui->ng_token->setText(token);
        ui->ng_token->setEnabled(false);
        File.close();
    }
    redi::ipstream link("echo $(curl -s -N http://127.0.0.1:4040/api/tunnels | grep -o \"https://[0-9a-z]*\\.ngrok.io\")", redi::pstreams::pstdout);
    std::getline(link.out(), out);
    ui->ngurl->setText(out.c_str());
    if(out.empty()){ui->btn_stop->setEnabled(false);}
    else{ui->btn_start->setEnabled(false);ui->btn_stop->setEnabled(true);}
}

MainWindow::~MainWindow(){delete ui;}
void MainWindow::on_ng_copy_clicked(){clipboard->setText(ui->ngurl->text());}
void MainWindow::on_btn_start_clicked(){
    redi::ipstream proc("ping -c 1 www.google.com", redi::pstreams::pstdout | redi::pstreams::pstderr);
    std::string stder;
    std::getline(proc.err(), stder);
    if(stder == "ping: www.google.com: Name or service not known") {
        QMessageBox msgBox;
        msgBox.setWindowTitle("InterNet");
        msgBox.setText("Check your network Connection..!");
        msgBox.exec();
        return;
    }
    if(ui->ng_token->text() == nullptr){
        QMessageBox msgBox;
        msgBox.setText("the token fiddle is Empty");
        msgBox.exec();
        return;
    }
    if(ui->ng_port->text() == nullptr){
        QMessageBox msgBox;
        msgBox.setText("the port fiddle is Empty");
        msgBox.exec();
        return;
    }
    ui->btn_start->setEnabled(false);
    std::string work;
    if(ui->ng_port_http->isChecked()){
        work = "http ";
        if(ui->ng_subdomain_enable->isChecked()){
            work += "-subdomain=" + ui->ng_subdomain->text().toStdString();
        }
        work += ui->ng_port->text().toStdString() + " &> /dev/null &";
        if(!ui->ng_token->isEnabled()){
            std::string out("ngrok " + work);
            system(out.c_str());
        }else{
            std::string tok("ngrok authtoken " + ui->ng_token->text().toStdString());
            system(tok.c_str());
            std::string out("ngrok " + work);
            system(out.c_str());
        }
        std::string out;
        for(int i=0;i<4;i++) {
            redi::ipstream link("echo $(curl -s -N http://127.0.0.1:4040/api/tunnels | grep -o \"https://[0-9a-z]*\\.ngrok.io\")", redi::pstreams::pstdout);
            std::getline(link.out(), out);
            sleep(int(1));
            if(!out.empty()) break;
        }
        if(out.empty()){
            ui->btn_start->setEnabled(true);
            QMessageBox msgBox;
            msgBox.setWindowTitle("Error");
            msgBox.setText("Your key might be invalid or the program crashed, no one knows");
            msgBox.exec();
            return;
        }else{
            ui->btn_stop->setEnabled(true);
            ui->ngurl->setText(out.c_str());
            QString TYPE = ui->ng_port_http->isChecked()?"HTTP":"TCP";
            QString SUBD = ui->ng_subdomain_enable->isChecked()?"Yes":"No";
            ui->ng_log->insertHtml("<head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
                                   "p, li { white-space: pre-wrap; }"
                                   "</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">"
                                   "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-weight:600; color:#ffaa00;\">Host</span><span style=\"color:#ffaa00;\"> </span><span style=\"color:#ffffff;\">: </span><span style=\"font-style:italic; color:#ffffff;\">" + QString::fromStdString(out) + "</span></p>"
                                   "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-weight:600; color:#ffaa00;\">Type</span><span style=\"color:#ffffff;\"> : </span><span style=\"font-style:italic; color:#ffffff;\">" + TYPE + "</span></p>"
                                   "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-weight:600; color:#ffaa00;\">Port </span><span style=\"color:#ffffff;\"> : </span><span style=\"font-style:italic; color:#ffffff;\">" + ui->ng_port->text() + "</span></p>"
                                   "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-weight:600; color:#ffaa00;\">Sub</span><span style=\"color:#ffaa00;\"> </span><span style=\"color:#ffffff;\">  : </span><span style=\"font-weight:600; font-style:italic; color:#ffffff;\">" + SUBD + "</span></p>");
        }
    }else if(ui->ng_port_tcp->isChecked()){
        work = "tcp ";
        work += ui->ng_port->text().toStdString() + " &> /dev/null &";
        if(!ui->ng_token->isEnabled()){
            std::string out("ngrok " + work);
            system(out.c_str());
        }else{
            std::string tok("ngrok authtoken " + ui->ng_token->text().toStdString());
            system(tok.c_str());
            std::string out("ngrok " + work);
            system(out.c_str());
        }
        std::string out;
        for(int i=0;i<4;i++) {
            redi::ipstream link("echo $(curl -s -N http://127.0.0.1:4040/api/tunnels | grep -o \"[0-9a-z]*\\.ngrok.io:[0-9]*\")", redi::pstreams::pstdout);
            std::getline(link.out(), out);
            sleep(int(1));
            if(!out.empty()) break;
        }
        if(out.empty()){
            ui->btn_start->setEnabled(true);
            QMessageBox msgBox;
            msgBox.setWindowTitle("Error");
            msgBox.setText("Your key might be invalid or the program crashed, no one knows");
            msgBox.exec();
            return;
        }else{
            ui->btn_stop->setEnabled(true);
            ui->ngurl->setText("tcp://" + QString::fromStdString(out));
            QString TYPE = ui->ng_port_http->isChecked()?"HTTP":"TCP";
            QString SUBD = ui->ng_subdomain_enable->isChecked()?"Yes":"No";
            ui->ng_log->insertHtml("<head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
                                   "p, li { white-space: pre-wrap; }"
                                   "</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">"
                                   "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-weight:600; color:#ffaa00;\">Host</span><span style=\"color:#ffaa00;\"> </span><span style=\"color:#ffffff;\">: </span><span style=\"font-style:italic; color:#ffffff;\">" + QString::fromStdString(out) + "</span></p>"
                                   "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-weight:600; color:#ffaa00;\">Type</span><span style=\"color:#ffffff;\"> : </span><span style=\"font-style:italic; color:#ffffff;\">" + TYPE + "</span></p>"
                                   "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-weight:600; color:#ffaa00;\">Port </span><span style=\"color:#ffffff;\"> : </span><span style=\"font-style:italic; color:#ffffff;\">" + ui->ng_port->text() + "</span></p>"
                                   "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-weight:600; color:#ffaa00;\">Sub</span><span style=\"color:#ffaa00;\"> </span><span style=\"color:#ffffff;\">  : </span><span style=\"font-weight:600; font-style:italic; color:#ffffff;\">" + SUBD + "</span></p>");
        }
    }
}
void MainWindow::on_btn_stop_clicked(){
    system("pkill -f ngrok");
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(!ui->btn_start->isEnabled()){
        std::cout << event << std::endl;
        QMessageBox msgBox;
        msgBox.setWindowTitle("Quit");
        msgBox.setText("Do you want to keep the app active in the background?");
        QPushButton *yep = msgBox.addButton(tr("Yes"), QMessageBox::ActionRole);
        QPushButton *no = msgBox.addButton(tr("No"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == yep) {exit(0);}
        else if (msgBox.clickedButton() == no) {system("pkill -f ngrok");exit(0);}
    }
}
