#include "Shell.h"

vector<pid_t> pids;

void handle(int signals) {
    int status;
    pid_t pid;
    assert (signals == SIGCHLD);
    pid = waitpid(-1, &status, WNOHANG);
    pids.erase(remove(pids.begin(), pids.end(), pid), pids.end());
}

void Shell::execute(bool usr_prompt) {
    while(run) {
        size = 8; // reinstantiate key variables
        cmd = new char* [size];
        cmd_num = 0;
        in = dup(0);
        out = dup(1);
        ispipe = false;
        redirect = false;
        bg_process = false;

        for (int i = 0; i < size; i++) { // set pointers to null
            cmd[i] = nullptr;
        }
        if (usr_prompt) { // check for -t flag
            custom_prompt();
        }
        getline(cin, usr_input);
        tokens = generator.tokenize(usr_input);  // place tokens into vector
        if (tokens[0] == "exit") { // exit shell
            cout << "Quitting shell...\n";
            run = false;
        }
        else if (tokens[0] == "cd") { // check for change dir command
            change_dir(tokens);
        }
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i] == "|") { // check for redirect
                piping();
            }
            else if (tokens[i] == "<") { // check to redirect input
                redirect_input(i);
            }
            else if (tokens[i] == ">") { // check to redirect output
                redirect_output(i);
            }
            else if (tokens[i] == "&") { // check if bg procress
                bg_process = true;
            }
            else if (tokens[i] == "jobs") { // show running progress of bg processes
                for (int i = 0; i < pids.size(); i++) {
                    cout << "[" << i+1 << "]+ Running    &";
            }
            } else {  // store commands
                char* seg = char_convert(tokens[i]);
                cmd[cmd_num++] = seg;
                if (tokens[i] == "ls") { // check to list files
                    cmd[cmd_num++] = char_convert("--color=auto"); // (BONUS) set color scheme for ls
                }
            }
        }
        cmd[cmd_num] = nullptr;
        dup2(out, 1); // overwrite pipe write
        if(close(out) < 0) {  // close pipe
            perror("close");
        }
        pid = fork();
        if (pid == 0) { // check for child process
            execvp(cmd[0], cmd); // execute commands in child
        }
        else if (pid < 0) { // check for forking error
            perror("fork");
        } else {
            bg_handler();
            if (ispipe || redirect) { // check for pipes or redirect
                if (close(pipefd[0]) < 0) { // close pipe in
                    perror("close");
                }
            }
            cmd_num = 0;
            delete [] cmd;
        }
        dup2(in, 0); // duplicate in pipe
        if (close(in) < 0) { // close pipe
            perror("close");
        }
    }
}

void Shell::custom_prompt() { // customize the command prompt
    time_t curr_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string dir = getcwd(curr_dir, 255);

    cout << "\n" << "\033[1;36m"+usr+"\033[0m" << ":\033[1;35m~"+dir+"\033[0m" << " " << ctime(&curr_time) << ":: "; 
}

void Shell::piping() {
    if (!redirect) { // split commands into pipes
        pipe(pipefd);
        pid = fork();
        if (pid == 0) { // check for child
            dup2(pipefd[1], 1); // overwrite to pipe write
            if (close(pipefd[0]) < 0) { // close unused pipe
                perror("close");
            }
            cmd[cmd_num] = nullptr;
            execvp(cmd[0], cmd); // execute process
        }
        else if (pid < 0) {
            perror("fork");
        } else {
            status = waitpid(pid, nullptr, WUNTRACED); // wait for child processes
            if (close(pipefd[1]) < 0) { // close unused pipe
                perror("close");  
            }
            dup2(pipefd[0], 0); // overwrite pipe read
        }  
    } else {
        redirect = false;
    }
    ispipe = true;
    update(cmd, cmd_num); // restore cmd array
}

void Shell::redirect_input(int i) {
    cmd[cmd_num] = nullptr;
    if (cmd_num < tokens.size() - 2) { // pipe if number of commands is less than commands
        pipe(pipefd);
    }
    pid = fork();
    if (pid == 0) { // check for child
        int fds = open(tokens[++i].c_str(), O_RDONLY, 0);
        dup2(fds, STDIN_FILENO); 
        if (close(fds) < 0) { // close pipe
            perror("close");
        }
        if (i < tokens.size() - 2) {
            dup2(pipefd[1], 1); // overwrite to pipe write
            if (close(pipefd[0]) < 0) {  // close read pipe
                perror("close");
            }
        }
        execvp(cmd[0], cmd);
    }
    else if(pid < 0) { // if fork was unsuccessful
        perror("fork");
    } else {
        status = waitpid(pid, nullptr, WUNTRACED); // wait for children
        if (i < tokens.size() - 2) {
            if (close(pipefd[1]) < 0) {
                perror("close");
            }
            dup2(pipefd[0], 0); // overrite pipe read
        }
    }
    redirect = true;
    update(cmd, cmd_num); // restore cmd array
}

void Shell::redirect_output(int i) {
    if (close(1) < 0) { 
        perror("close");
    }
    cmd[cmd_num] = nullptr;
    FILE *file;
    file = fopen(tokens[++i].c_str(), "w"); // given write access
    pid = fork();
    if (pid == 0) { // check for children
        execvp(cmd[0], cmd); // execute processes in children
    }
    else if (pid < 0) { // unsuccessful fork
        perror("fork");
    } else {
        bg_handler();
    }
    fclose(file);
    update(cmd, cmd_num);
}

void Shell::change_dir(vector<string> command) {
    current_dir = getcwd(curr_dir, 255); // find current working directory
    if (command[1] == "..") {
        if (current_dir.find_last_of("/") != '\0') {
            current_dir = string(curr_dir).substr(0, current_dir.find_last_of("/"));
        }
    } else if (command[1].find('/') == 0) {
        current_dir = command[1];
    } else {
        current_dir = current_dir + "/" + command[1];
    }
    chdir(current_dir.c_str());
}

void Shell::bg_handler() {
    if (!bg_process) { 
        status = waitpid(pid, nullptr, WUNTRACED); // wait for children
    } else { // handling background processes
        pids.push_back(pid); // push pid into bg vector
        signal(SIGCHLD, handle); // send signal
    }
}

void Shell::update(char**& cmd, int& cmd_num) { // clean commands
    char** temp = new char* [size];
    delete [] cmd;
    cmd_num = 0;
    cmd = temp;
}

char* Shell::char_convert(string line) { // convert string to array of characters
    char* arr = new char [line.size()+1];
    for (int i = 0; i < line.size(); i++) {
        arr[i] = line[i];
    }
    arr[line.size()] = '\0';
    return arr;
}