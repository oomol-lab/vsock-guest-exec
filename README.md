# Vsock Guest Exec

`vsock_guest_exec` is a tool that allows you to execute commands in a guest VM from the host. It uses the vsock protocol to communicate with the guest.

The program is designed to be executed during the initrd stage in order to simulate the functions of [ignition] to some extent. This allows the virtual machine to handle tasks such as disk mounting and formatting before entering the rootfs.

The `vsock_guest_exec` accepts commands composed of up to 2048 bytes at a time and automatically terminates after the command ends. Before running, please ensure that the host is listening to the corresponding socket file.

EXAMPLES:

```shell
$ ./vsock_guest_exec 1:2049 # cid is 2, vsock port is 2049
$ ./vsock_guest_exec -1: # cid set to -1 (VMADDR_CID_ANY), vsock port is
default(1024)
$ ./vsock_guest_exec :1025 # vsock port set to 1025, cid is default(2)

# Invalid Params
$ ./vsock_guest_exec foo:bar # cid is default(2), vsock port is default(1024)
```

[ignition]: https://coreos.github.io/ignition/
