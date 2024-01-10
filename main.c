#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/vm_sockets.h>

#define MAX_COMMAND_LENGTH 2048
#define SIZE_OF_MSG_LENGTH 2

void setCID(char params[], int *cid) {
    if (strlen(params) > 0) {
        int _cid = atoi(params);
        if (_cid == VMADDR_CID_ANY || _cid == VMADDR_CID_HYPERVISOR ||
            _cid == VMADDR_CID_LOCAL || _cid == VMADDR_CID_HOST) {
            *cid = _cid;
        }
    }
}

void setPort(char params[], int *port) {
    if (strlen(params) > 0) {
        int _port = atoi(params);
        if (_port >= 0 && _port <= 65535) {
            *port = _port;
        }
    }
}

int recv_all(int socket, void *buffer, size_t length) {
    size_t to_read = length;
    char *buffer_char = (char *) buffer;

    while (to_read > 0) {
        ssize_t read_now = recv(socket, buffer_char, to_read, 0);
        if (read_now <= 0) {
            if (read_now < 0)
                perror("recv failed");
            return -1;
        }

        to_read -= read_now;
        buffer_char += read_now;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int cid = VMADDR_CID_HOST;
    int vsockPort = 1024;
    int persistent = 0;
    int sockfd;
    struct sockaddr_vm sa_vm;
    char command[MAX_COMMAND_LENGTH];
    unsigned short msg_length;

    if (argc > 1) {
        char *param = argv[1];
        char *delim = strchr(param, ':');
        if (delim != NULL) {
            *delim = '\0';
            char *cidString = param;
            char *portString = delim + 1;

            setCID(cidString, &cid);
            setPort(portString, &vsockPort);
        } else {
            setCID(param, &cid);
        }
    }

    if (argc > 2) {
        char *param = argv[2];
        if (strcmp(param, "persistent") == 0) {
            persistent = 1;
        }
    }

    sockfd = socket(AF_VSOCK, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    memset(&sa_vm, 0, sizeof(struct sockaddr_vm));
    sa_vm.svm_family = AF_VSOCK;
    sa_vm.svm_cid = cid;
    sa_vm.svm_port = vsockPort;

    if (connect(sockfd, (struct sockaddr *) &sa_vm, sizeof(struct sockaddr_vm)) < 0) {
        perror("Failed to connect");
        close(sockfd);
        exit(1);
    }

    printf("persistent: %d\n", persistent);

    if (!persistent) {
        if (recv(sockfd, command, sizeof(command), 0) < 0) {
            perror("Failed to receive command");
            close(sockfd);
            exit(1);
        }

        printf("Received message: %s\n", command);

        system(command);
    } else {
        while (1) {
            if (recv_all(sockfd, &msg_length, SIZE_OF_MSG_LENGTH) == -1) {
                perror("Server closed connection");
                break;
            }
            if (recv_all(sockfd, command, msg_length) == -1) {
                perror("Server closed connection");
                break;
            }

            command[msg_length] = '\0';
            printf("Received message: %s\n", command);

            system(command);
        }
    }

    close(sockfd);

    return 0;
}
