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

int main() {
    int cid, sockfd;
    struct sockaddr_vm sa_vm;
    char command[MAX_COMMAND_LENGTH];

    sockfd = socket(AF_VSOCK, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    memset(&sa_vm, 0, sizeof(struct sockaddr_vm));
    sa_vm.svm_family = AF_VSOCK;
    sa_vm.svm_cid = VMADDR_CID_HOST;
    sa_vm.svm_port = 1024;

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
