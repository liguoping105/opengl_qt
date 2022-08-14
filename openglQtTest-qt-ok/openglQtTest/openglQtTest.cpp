#include "openglQtTest.h"
#include <windows.h>
#include <iostream>
#include <thread>
openglQtTest::openglQtTest(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButtonRender, SIGNAL(clicked()), this, SLOT(StartRender()));

    //m_pVideoRender = ui.openGLWidgetVideo
}

void openglQtTest::StartRender()
{
   std::thread([](auto self) {
        
        unsigned char* pBuffer = new unsigned char[640 * 480 * 3 / 2];
        FILE* pFile = fopen("test.yuv", "rb");

        while (true) {
            int ret = fread(pBuffer, 1, 640 * 480 * 3 / 2, pFile);
            self->SetYUVData((unsigned char*)pBuffer, (unsigned char*)(pBuffer + 640 * 480),
                (unsigned char*)(pBuffer + 640 * 480 + 640 * 480 / 4), 640, 480);
            self->SetIsShowFlag(true);
            self->UpdataVideo();
            Sleep(200);
        }
      
        
        }, ui.openGLWidgetVideo).detach();

}