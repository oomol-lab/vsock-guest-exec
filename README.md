# Vsock Guest Exec

`vsock_guest_exec` is a tool that allows you to execute commands in a guest VM from the host. It uses the vsock protocol to communicate with the guest.

The `vsock_guest_exec` receives commands of up to 2048 bytes at a time. Before running, please ensure that the host is listening on the corresponding socket file.

## EXAMPLES:

### Disposable

Simulating [ignition] to some extent, the program immediately exits after receiving and executing a command.
Suitable for running in the initrd stage.

```shell
$ ./vsock_guest_exec 1:2049 # cid is 2, vsock port is 2049
$ ./vsock_guest_exec -1: # cid set to -1 (VMADDR_CID_ANY), vsock port is default(1024)
$ ./vsock_guest_exec :1025 # vsock port set to 1025, cid is default(2)

# Invalid Params
$ ./vsock_guest_exec foo:bar # cid is default(2), vsock port is default(1024)
```

### Persistent

The program will run persistently, receiving and executing commands, and will wait for the next request until the host disconnects the socket connection.

If running in persistent mode, two bytes (in hexadecimal) need to be added in front of the command to describe the length of the entire command (the maximum length of the command is still 2048). This approach is primarily used to solve the TCP sticky packet problem. The following is the diagram of this protocol:

![protocol](https://github.com/oomol-lab/vsock-guest-exec/assets/8198408/64c34918-0e4b-47fa-8761-ce31211f7796)

```shell
$ ./vsock_guest_exec cid:port persistent
```

If you are using golang as the server, you can generate a protocol-compliant message with the following code.

```golang
func run(conn net.Conn) {
    msg := []byte("echo hello world")
	msg_length := len(msg)
	length_bytes := make([]byte, 2)
	binary.LittleEndian.PutUint16(length_bytes, uint16(msg_length))
	
	_ = write(conn, lengthBytes)
	_ = write(conn, msg)
}

func write(conn net.Conn, data []byte) error {
	total := 0
	for {
		now, err := conn.Write(data[total:])
		if err != nil {
			return err
		}
		total += now
		if total == len(data) {
			return nil
		}
	}
}
```

[ignition]: https://coreos.github.io/ignition/