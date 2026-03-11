#include "../cli/cli.hpp"

int main(int argc, char** argv)
{
    sysp::cli::CLI cli;
    return cli.run(argc, argv);
}
