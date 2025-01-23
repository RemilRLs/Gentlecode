# GentleCode Worm

**GentleCode** is a C project that simulates the behavior of a worm. It is designed to:

- Determine if the machine is the C2 server or an infected machine.
- Scan a network to identify machines that can be infected.
- Check authorization via a flag file present on the target.
- Download an update from the C2 server if one is available, saving it with a random name to remain hidden.
- Execute the payload and create a cron job to periodically execute the worm.
- Send information via a `datav` file from the infected machine to the C2 server.

---

## Features

Refer to the **diagram** (`.pdf`).

---

## Compilation

To compile the project, you need `gcc`. Use the following command:

```bash
gcc -o gentlecode gentlecode.c
```

---

## Usage

The behavior depends on the machine's role.

### On the C2 Server (Host)

- The program detects the machine as the C2 server (by comparing its IP with the global variable `ip_host`).
- The C2 server scans the network to identify target machines to infect via port `:22`.

### On a Target Machine (Infected)

1. The program first checks if an update is available on the C2 server by checking the flag file at `/tmp/.update`.
2. If an update is available:
   - The program deletes the old payload.
   - It generates a random name for the payload script, downloads the new script `instruction.sh` from the C2 server, and saves its name in `/tmp/.virus`.
3. If no update is available:
   - The program reads the file `/tmp/.virus` to retrieve the name of the script to use.
4. The payload is then executed, and information is sent to the C2 server via the `datav` file.

### Example Usage

```bash
sudo ./gentlecode <file_to_send>
```

---

## Configuration

- **ip_host**: Modify the global variable `ip_host` in the code to set your C2 server's IP address.
- **IP Ranges**: The function `scanNetwork()` scans a defined IP range for faster operation (e.g., from `192.168.100.100` to `192.168.100.105`).

