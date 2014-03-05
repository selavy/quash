quash
=====

"Quite a Shell" - A really simple shell for the *nix environment.

1. To compile quash
> make

2. To run quash
> ./quash

3. To run quash with commands inside a file instead
> ./quash < commands.txt

4. Features in quash
4.1: Run executables
     4.1.a: Run executables with an absolute path
     	    > /bin/ls
     4.1.b: Run executables in PATH
     	    Quash will search for the executable in the directories list in the PATH variable
     	    (assuming /bin is in PATH)
     	    > ls
4.2: Run executables with arguments
     run an executable as specified in 4.1 and add arguments:
     e.g. > ls -la
4.3 Set HOME and PATH
    	set HOME=/home/myhomedir
    separate directories with a ':'
    	set PATH=/usr/local/bin:/bin:
4.4: To exit Quash
     > quit
     > exit
4.5 Change directory:
    > cd <new directory>
    e.g. > cd ..
4.6 Execute a process in the background
    Add a '&' at the end of the command
    e.g. emacs &
4.7 List background processes
    > jobs
4.8 Redirection
     4.8.a Input Redirection
     	    Use the '<' character followed by a filename to redirect
	    stdin from the given file.
     	    e.g. cat < input.txt
     4.8.b Output Redirection
     	    use the '>' character followed by a filename to redirect
	    stdout to the given file.
     	    e.g. ls > out.txt
4.9 Pipe
     Connect commands together with a pipe
     e.g. ls | more
4.10 Kill
     Send a signal to a background process using kill
     kill <signum> <jobid>
     To stop a process use:
     	> kill 9 1
     
Background execution does not work for jobs, kill, set, or cd because it doesn't
make any sense to do them in the background (though you can in a bash shell).



