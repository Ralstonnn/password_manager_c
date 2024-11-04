#include "common.h"
#include "os.h"
#include <stdio.h>
#include <stdlib.h>

void escape_for_shell(const char *input, char *output, size_t output_size) {
    size_t j = 0;
    for (size_t i = 0; input[i] != '\0' && j < output_size - 1; i++) {
        switch (input[i]) {
        case '$':
            // Escape special shell characters
            if (j + 2 < output_size) {
                output[j++] = '\\'; // Escape the special character
                output[j++] = input[i];
            }
            break;
        default:
            // Copy normal characters
            if (j < output_size - 1) {
                output[j++] = input[i];
            }
            break;
        }
    }
    output[j] = '\0'; // Null-terminate the output string
}

int copy_to_clipboard(const char *text, int max_size) {
    const int escaped_max_size = max_size * 2;
    const char *os = get_platform_name();
    char command[escaped_max_size + 256];
    char text_escaped[escaped_max_size];

    make_string_empty(text_escaped, escaped_max_size);
    escape_for_shell(text, text_escaped, escaped_max_size);

    if (are_equal_strings((char *)os, OS_LINUX)) {
        if (is_wsl()) {
            snprintf(command, sizeof(command), "echo \"%s\" | clip.exe", text);
        } else if (getenv("WAYLAND_DISPLAY")) {
            snprintf(command, sizeof(command), "echo \"%s\" | wl-copy", text_escaped);
        } else if (getenv("DISPLAY")) {
            snprintf(command, sizeof(command),
                     "echo \"%s\" | xclip -selection clipboard", text_escaped);
        }
    }
    if (are_equal_strings((char *)os, OS_MAC)) {
        snprintf(command, sizeof(command), "echo \"%s\" | pbcopy", text_escaped);
    }

    return system(command);
}
