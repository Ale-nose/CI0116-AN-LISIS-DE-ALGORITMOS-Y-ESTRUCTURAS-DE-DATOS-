#include <QApplication>

#include "Cliargs.hpp"
#include "headlessRunner.hpp"
#include "MainWindow.hpp"

int main(int argc, char** argv) {
  CliArgs args;
  if (!parseCliArgs(argc, argv, args)) {
    return 1;
  }

  if (args.headless) {
    return runHeadless(args);
  }

  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  return app.exec();
}