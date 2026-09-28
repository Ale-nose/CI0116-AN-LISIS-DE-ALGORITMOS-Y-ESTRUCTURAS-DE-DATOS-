// Copyright 2026 Ashley Solano, Alejandro Cubero y Kevin Velásquez
#include <QApplication>
#include <iostream>
#include "Replay.hpp"
#include "Cliargs.hpp"
#include "headlessRunner.hpp"
#include "MainWindow.hpp"

int main(int argc, char** argv) {
  CliArgs args;
  if (!parseCliArgs(argc, argv, args)) {
    return 1;
  }

  if (args.headless && !args.replayPath.empty()) {
    std::cerr << "--replay cannot be used with --headless\n";
    return 1;
  }

  if (args.challenge && !args.replayPath.empty()) {
    std::cerr << "--replay cannot be used with --challenge\n";
    return 1;
  }

  if (args.headless) {
    return runHeadless(args);
  }

  ReplayData replayData;
  const ReplayData* replayPtr = nullptr;

  if (!args.replayPath.empty()) {
    if (!loadReplay(args.replayPath, replayData)) {
      std::cerr << "Could not load replay: "
                << args.replayPath << '\n';
      return 1;
    }

    replayPtr = &replayData;
  }

  QApplication app(argc, argv);
  MainWindow window(args.challenge, replayPtr);
  window.show();
  return app.exec();
}
