#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Windows.h>
#include <TlHelp32.h>
#include "cygnus.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(250,100);
    Attach(EXE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    suspend();
}

void MainWindow::on_pushButton_2_clicked()
{
    resume();
}

void Hack()
{
    /*Attach();
    char buf[100];
    sprintf(buf, "%d", ProcId);
    int val = 1000;
    int i = WriteProcessMemory(hProcess, (void*)0x0167C20, &val, 4, NULL);*/
}

void MainWindow::on_pushButton_3_clicked()
{
    Attach(EXE);
    char b = 0x30;
    WriteProcessMemory(hProcess, (LPVOID)0x00C54E2F, &b, sizeof(b), NULL);
    DWORD err = GetLastError();
    char buf[100];
    sprintf(buf, "%d", ProcId);
    QMessageBox::information(this, tr("Cygnus"), buf);
    sprintf(buf, "%d", err);
    QMessageBox::information(this, tr("Cygnus"), buf);
    CloseHandle(hProcess);
}
