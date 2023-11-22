# Vsock Guest Exec

`vsock_guest_exec` is a tool that allows you to execute commands in a guest VM from the host. It uses the vsock protocol to communicate with the guest.

The program is designed to be executed during the initrd stage in order to simulate the functions of [ignition] to some extent. This allows the virtual machine to handle tasks such as disk mounting and formatting before entering the rootfs.

The `vsock_guest_exec` accepts commands composed of up to 2048 bytes at a time and automatically terminates after the command ends. Before running, please ensure that the host is listening to the corresponding socket file.

[ignition]: https://coreos.github.io/ignition/
