#include <chrono>
#include <ctime>
#include <cassert>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Tokenizer.h"

class Shell {

private:
    // parsing variables
    Tokenizer generator;
    string usr_input;
    vector<string> tokens;
    
    // important for operations
    int size;
    char** cmd;
    int cmd_num;
    int in;
    int out;

    // variables used for directory changes
    string usr;
    string current_dir;
    char curr_dir[256];

    // pid and pipe information
    int pipefd[2];
    pid_t pid;
    int status;

    // check if bg or redirect
    bool run;
    bool ispipe;
    bool bg_process;
    bool redirect;

public:
    // constructor
    Shell() : usr(getenv("USER")), ispipe(false)
            ,bg_process(false),redirect(false), run(true) {}

    // command processes
    void execute(bool usr_prompt);
    void custom_prompt();
    void piping();
    void redirect_input(int i);
    void redirect_output(int i);
    void change_dir(vector<string> cmd);
    void bg_handler();

    // methods to manipulate cmd arrays
    void update(char** &cmd, int &i);
    char* char_convert(string line);
};