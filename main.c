#include "cheat_engine.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <process_name>\n", argv[0]);
        return 1;
    }

    return run(argv);
}