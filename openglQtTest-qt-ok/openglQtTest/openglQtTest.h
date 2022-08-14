#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_openglQtTest.h"

class openglQtTest : public QMainWindow
{
    Q_OBJECT

public:
    openglQtTest(QWidget *parent = Q_NULLPTR);

private:
    Ui::openglQtTestClass ui;

public slots:
    void StartRender();

public:
   // ShowVideoRender* m_pVideoRender = NULL;
};
