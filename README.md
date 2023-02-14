# terminal
A light-weight C-based custom shell implementation. Supports basic inbuilt shell commands like ls, cd etc, as well as more complicated operators like pipe ```|```, I/O redirection. Implements custom signal handler for some signal. 
## Installation
Tested on a linux environment but should be easily portable to Unix or Windows. 

1. Clone the repository.
2. Install Docker Desktop and use the image ```ytang\os``` as the base image for terminal.
3. Alternatively, use the command ```docker run -i --name terminal --privileged --rm -t -v PATH_TO_YOUR_REPO:/terminal -w /terminal ytang/os bash``` from terminal to install the image ytang/os and mount your repository to the docker environment.
4. After this, you should be inside the docker environment in your terminal. run ```make``` to compile the C code.
5. Run ```./nyush```

That's it! You have now complied and launched the custom C shell. Play around with the code and implement additional functionality!
