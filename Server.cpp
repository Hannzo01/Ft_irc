#include <sys/socket.h>
#include <iostream>
#include <exception>
#include <unistd.h> // pour close function
#include <arpa/inet.h>  // For htons(), htonl(), etc.
#include <netinet/in.h> // For struct sockaddr_in
#include <cstring>
#include <string>
#include <poll.h>
#include <vector>
#include <ctype.h>
#include <algorithm>
#include <map>
#include <fcntl.h>
#include <signal.h>

using namespace std;

int keep_running = true;

void    ft_handler(int num)
{
    if (num == SIGINT){
        keep_running = false;
    }
}

void parse_port(std::string a1)
{
    for (int i = 0; i < a1.size(); i++)
    {
        if (!isdigit(a1[i]))
            throw std::logic_error("Valid ports are between 1024 and 65535");
    }
    if (atoi(a1.c_str()) < 1024 || atoi(a1.c_str()) > 65535)
        throw std::logic_error("Invalid port num");
    
}

int main(int argc, char *argv[])
{
   if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    parse_port(argv[1]);
    try
    {
        signal(SIGINT,ft_handler);
        if (strlen(argv[2]) == 0)
            throw std::logic_error("Enter a password");
        struct sockaddr_in localaddr;
        memset(&localaddr, 0, sizeof(localaddr));
        localaddr.sin_family = AF_INET;
        localaddr.sin_port = htons(atoi(argv[1])); //convert int to 
        localaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        // localaddr.sin_zero = ach rnder f hada
        //ss scoket server
        int ss = socket(AF_INET, SOCK_STREAM ,0);
        if (ss < 0)
            throw std::exception();
        fcntl(ss, F_SETFL, O_NONBLOCK);
        int opt = 1;
        setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (bind(ss, (sockaddr *)&localaddr, sizeof(localaddr)) < 0)
            throw std::runtime_error("Bind failed");
        
        struct pollfd spfd;
        spfd.fd = ss;
        spfd.events = POLLIN;

        std::map<int, std::string> fds_buffer;
        std::vector<struct pollfd> v;
        v.push_back(spfd);
        if (listen(ss, 4) < 0)
            throw std::runtime_error("Listen failed");// taille dyal file dattente < 5 si 0 on laisse le system decide 

        while (keep_running)
        {
            if (poll(&v[0], v.size(), -1) < 0)
                throw std::runtime_error("Poll failed");
            for (int i = 0; i < v.size(); i++)
            {
                if (v[i].revents & POLLIN)
                {
                    int current_fd = v[i].fd;
                    if (current_fd == ss)
                    {
                        struct sockaddr_in listenaddr;
                        socklen_t len = sizeof(listenaddr); // KERNAL qui va ecrire accept() remplira la structure avec la vraie adresse IP du client
                        int ns = accept(ss, (sockaddr *)&listenaddr, &len);
                        if (ns < 0)
                        {
                            std::cerr << "Accept failed" << std::endl;
                            continue;
                        }
                        fcntl(ns, F_SETFL, O_NONBLOCK);
                        std::cout << "Client connecté ! Son FD est : " << ns << std::endl;
                        struct pollfd spf;
                        spf.fd = ns;
                        spf.events = POLLIN;
                        v.push_back(spf);
                        fds_buffer[ns];
                        continue;
                    }
                    else
                    {
                        char buf[1024];
                        int byteread = recv(current_fd , buf, 1023, 0);
                        if (byteread <= 0)
                        {
                            fds_buffer.erase(current_fd);
                            close(current_fd);
                            v.erase(v.begin() + i); // n7ydha mn vector
                            i--; //v kyn9slo size n9si bch mtn9zich chi client
                            continue;
                        }
                        else
                        {
                            std::cout << "dd" << std::endl;
                            fds_buffer[current_fd].append(buf, byteread);
                            size_t delfound = fds_buffer[current_fd].find("\r\n");
                            while (delfound != std::string::npos)
                            {
                                std::cout << "elch mdkhltch" << std::endl;
                                std::string command = fds_buffer[current_fd].substr(0, delfound);
                                std::cout << "This is the cmd  :" << command << std::endl;
    
                                fds_buffer[current_fd].erase(0,delfound + 2);
                                std::cout << "This is the msg after erasing :" << fds_buffer[current_fd] << std::endl;
                                delfound = fds_buffer[current_fd].find("\r\n");
                            } 
                            //check for /r/n if found send it to my partner if not keep filling the ma;
                            // std::cout << buf << std::endl;
                            // send(v[i].fd, "kenza", 6, 0);
                            // close(v[i].fd);
                        }
                    }
                }
            }
        }
        for (int i = 0; i < v.size(); i++)
        {
            close(v[i].fd);
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }


}
/* ========================================================================== */
/*                               SOCKET CREATION                              */
/* ========================================================================== */
/*
** WHAT IT IS:
** The socket() function asks the Kernel to create an endpoint for network 
** communication. It returns a File Descriptor (an integer) representing this connection.
**
** WHY WE NEED IT:
** This is like buying a physical telephone. You have the hardware to talk, 
** but you don't have a phone number yet. We need this FD to listen for clients.
**
** HOW IT IS USED (The Parameters):
** 1. domain:   AF_INET      -> Tells the kernel we are using IPv4 addresses (like 127.0.0.1).
** 2. type:     SOCK_STREAM  -> Tells the kernel we want a TCP connection (reliable, in-order, stream).
** 3. protocol: 0            -> Automatically chooses the default protocol for TCP.
**
** RETURN:
** Returns the Socket FD (e.g., 3) on success, or -1 if the kernel failed to create it.
*/
// server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);



/* ========================================================================== */
/*                                SOCKET BINDING                              */
/* ========================================================================== */
/*
** WHAT IT IS:terminate called after throwing an instance of 'std::out_of_range'
  what():  map::at
Aborted (core dumpe
** The bind() function associates a specific IP address and Port number to our 
** previously created socket.
**
** WHY WE NEED IT:
** Now that we have a "telephone" (the socket), bind() gives it a "phone number" 
** (Port 6667). Without this, clients wouldn't know where to connect to our server.
** It claims this specific port on the machine exclusively for our process.
**
** HOW IT IS USED (The Parameters):
** 1. sockfd:  Our server socket FD created by socket().
** 2. addr:    A pointer to our filled-out 'sockaddr_in' structure. We MUST cast it 
**             to a generic (struct sockaddr*) because bind() is an old C function 
**             that accepts many different types of network structures.
** 3. addrlen: The size of our structure in bytes (sizeof(my_address)).
**
** RETURN:
** Returns 0 on success, or -1 on error (usually meaning the port is already in use).
*/
// bind(server_socket_fd, (struct sockaddr *)&my_address, sizeof(my_address));
