#include <cstdio>

#include "Cliargs.hpp"
#include "headlessRunner.hpp"

/*
TODO(Topic 5.2 / build system): windowed mode is implemented in
MainWindow.hpp. To enable it here, the Makefile needs
Qt5Widgets includes/libs added (check if it does it). 
  #include <QApplication>
  #include "MainWindow.hpp"
  ...
instead of the printf:
  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  return app.exec();
*/

int main(int argc, char** argv) {
  CliArgs args;
  if (!parseCliArgs(argc, argv, args)) {
    return 1;
  }

  if (args.headless) {
    return runHeadless(args);
  }

  std::printf("Windowed mode not implemented yet -- use --headless.\n");
  return 1;
}
