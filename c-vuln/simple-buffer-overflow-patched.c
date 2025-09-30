#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Hardened variant of the vulnerable example. It demonstrates safer input
 * handling by bounding reads, validating lengths, and clearing sensitive data.
 */

#define NAME_LIMIT 31

static void secure_greet(void) {
    char buffer[NAME_LIMIT + 1]; /* Reserve space for the terminator. */

    puts("What is your name?");

    if (!fgets(buffer, sizeof(buffer), stdin)) {
        fputs("[-] Failed to read input\n", stderr);
        return;
    }

    /* Strip the trailing newline added by fgets(). */
    buffer[strcspn(buffer, "\n")] = '\0';

    if (strlen(buffer) > NAME_LIMIT) {
        fputs("[-] Input too long\n", stderr);
        return;
    }

    printf("Hello, %s!\n", buffer);

    /* Clear the buffer before returning in case sensitive data was entered. */
    memset(buffer, 0, sizeof(buffer));
}

int main(void) {
    secure_greet();
    puts("Program finished normally.");
    return 0;
}
