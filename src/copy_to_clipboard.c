#include "common.h"
#include "os.h"
#include <stdio.h>
#include <stdlib.h>

void copy_to_clipboard(const char *text, int max_size) {
    const char *os = get_platform_name();
    char command[max_size + 56];

    if (are_equal_strings((char *)os, OS_LINUX)) {
        if (getenv("WAYLAND_DISPLAY")) {
            snprintf(command, sizeof(command), "echo \"%s\" | wl-copy", text);
        } else if (getenv("DISPLAY")) {
            snprintf(command, sizeof(command),
                     "echo \"%s\" | xclip -selection clipboard", text);
        }
    }
    if (are_equal_strings((char *)os, OS_MAC)) {
        snprintf(command, sizeof(command), "echo \"%s\" | pbcopy", text);
    }

    system(command);
}
