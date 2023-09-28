#include "Window.hpp"
#include <QApplication>
#include <cstdlib>
#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    QGuiApplication app(__argc, __argv);
    Window window(960,540);
    window.show();

    return app.exec();
}