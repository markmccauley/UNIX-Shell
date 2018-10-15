#include "Shell.h"

int main(int argc, char** argv) {

    Shell my_shell;
    bool usr_prompt = true;
    int i = 0;
    while (argv[i] != nullptr) {
        if (strcmp(argv[i], "-t") == 0) {
            usr_prompt = false;
        }
        i++;
    }
    my_shell.execute(usr_prompt);
}