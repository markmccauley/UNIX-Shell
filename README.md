# My UNIX Shell

This is a self-made UNIX shell created in C++ to better understand pipelining and UNIX processes.

# Shell Features

- **Environment**:  This shell is able to easily navigate the filesystem. This is done by keeping track of the current working directory and the PATH. 
	> The program can handle basic **cd** and **pwd** operations.

- **Pipelining**: A pipe inbetween two commands causes the standard output of one to be redirected into the standard input of another. The program has been tested to handle up to **4 separate pipes**.
- **Input/Output Redirection**:   The program implements input redirection by redirecting the standard input of a program to an file opened for reading. Output redirection is implemented by changing the standard output to point to a file opened for writing.

## Basic Operations
![enter image description here](https://i.imgur.com/FV24gOe.png)

## How to Run Program
>Simply use the make file by entering **make** then **./shell** the execute the UNIX Shell.
