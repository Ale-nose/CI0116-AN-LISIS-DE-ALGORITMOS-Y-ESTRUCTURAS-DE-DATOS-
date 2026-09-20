#include <cstdio>
#include "Cliargs.hpp"
#include "headlessRunner.hpp"

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