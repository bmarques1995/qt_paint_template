#include "Window.hpp"
#include "Painter.hpp"
#include <QApplication>
#include <cstdlib>
#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    QGuiApplication app(__argc, __argv);
    Window* window = new Window(960,540);
    window->show();
    Painter* painter = new Painter(window);
    painter->start();

    auto result = app.exec();

    painter->terminate();
    delete painter;
    delete window;

    return result;
}