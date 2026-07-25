#define TB_IMPL
#include "termbox2.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdbool.h>
struct dictionary {
    int x;
    int y;
    char key;   
};
struct input_event iv;
static const char ascii_map_unshifted[] = {
    [KEY_1] = '1', [KEY_2] = '2', [KEY_3] = '3', [KEY_4] = '4', [KEY_5] = '5',
    [KEY_6] = '6', [KEY_7] = '7', [KEY_8] = '8', [KEY_9] = '9', [KEY_0] = '0',
    [KEY_MINUS] = '-', [KEY_EQUAL] = '=',
    [KEY_Q] = 'q', [KEY_W] = 'w', [KEY_E] = 'e', [KEY_R] = 'r', [KEY_T] = 't',
    [KEY_Y] = 'y', [KEY_U] = 'u', [KEY_I] = 'i', [KEY_O] = 'o', [KEY_P] = 'p',
    [KEY_A] = 'a', [KEY_S] = 's', [KEY_D] = 'd', [KEY_F] = 'f', [KEY_G] = 'g',
    [KEY_H] = 'h', [KEY_J] = 'j', [KEY_K] = 'k', [KEY_L] = 'l',
    [KEY_Z] = 'z', [KEY_X] = 'x', [KEY_C] = 'c', [KEY_V] = 'v', [KEY_B] = 'b',
    [KEY_N] = 'n', [KEY_M] = 'm', [KEY_SPACE] = ' '
};

static struct dictionary keyboard_pos[] = {
    // --- Row 1: Number Row (y = 1) --- Spaced 5 columns apart!
    { 1, 1, '1'}, { 6, 1, '2'}, {11, 1, '3'}, {16, 1, '4'}, {21, 1, '5'},
    {26, 1, '6'}, {31, 1, '7'}, {36, 1, '8'}, {41, 1, '9'}, {46, 1, '0'},
    {51, 1, '-'}, {56, 1, '='},

    // --- Row 2: Top QWERTY Row (y = 3) ---
    { 3, 3, 'q'}, { 8, 3, 'w'}, {13, 3, 'e'}, {18, 3, 'r'}, {23, 3, 't'},
    {28, 3, 'y'}, {33, 3, 'u'}, {38, 3, 'i'}, {43, 3, 'o'}, {48, 3, 'p'},

    // --- Row 3: Home Row (y = 5) ---
    { 5, 5, 'a'}, {10, 5, 's'}, {15, 5, 'd'}, {20, 5, 'f'}, {25, 5, 'g'},
    {30, 5, 'h'}, {35, 5, 'j'}, {40, 5, 'k'}, {45, 5, 'l'},

    // --- Row 4: Bottom Row (y = 7) ---
    { 7, 7, 'z'}, {12, 7, 'x'}, {17, 7, 'c'}, {22, 7, 'v'}, {27, 7, 'b'},
    {32, 7, 'n'}, {37, 7, 'm'},

    //space
    {19,9, ' '}
};

void remove_element(char *elements, char target, int size) {
    int index=-1;
    for (int i=0;i<size;i++) {
        if (elements[i] == target) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return;
    }
    for (int i = index;i<size-1;i++) {
        elements[i] = elements[i+1];
    }
    elements[size-1] = '\0';
}
bool if_in(char *elements, char target) {
    for (int i =0;i<strlen(elements);i++) {
        if (elements[i] == target) {
            return true;
        }
    }
    return false;
}
int main() {

    int fd;
    if ((fd = open("/dev/input/event3", O_RDONLY)) == -1) {
        printf("failure");
    }
    char keys[26] = {0};
    int keys_cursor=0;
    tb_init();
    while (read(fd, &iv, sizeof(iv)) != -1) {   //if success
        if (iv.type == EV_KEY && (iv.value == 1 || iv.value == 2 || iv.value == 0)) {   //from the keyboard
            char ch = ascii_map_unshifted[iv.code];
            if (iv.value == 1) {
                if (ch != '\0') {
                    keys[keys_cursor] = ascii_map_unshifted[iv.code];
                    keys_cursor++;
                    keys[keys_cursor] = '\0';
                }
            }
            else if (iv.value == 0 && keys_cursor < 24) {
                if (if_in(keys, ascii_map_unshifted[iv.code])) {
                    remove_element(keys, ascii_map_unshifted[iv.code], keys_cursor);
                    if (keys_cursor > 0) {
                        keys_cursor--;
                    }
                }
            }
            tb_clear();
            for (int i=0;i<39;i++) {
                if (if_in(keys, keyboard_pos[i].key)) {
                    if (i == 38) {
                        tb_printf(keyboard_pos[i].x,keyboard_pos[i].y,TB_WHITE, TB_BLACK, " [   %c   ] ", keyboard_pos[i].key);
                        continue;
                    }
                    tb_printf(keyboard_pos[i].x,keyboard_pos[i].y,TB_WHITE, TB_BLACK, " [%c] ", keyboard_pos[i].key);
                    continue;
                }
                
                if (i == 38) {
                    tb_printf(keyboard_pos[i].x,keyboard_pos[i].y,TB_RED, TB_BLUE, " [   %c   ] ", keyboard_pos[i].key);
                    continue;
                }
                tb_printf(keyboard_pos[i].x,keyboard_pos[i].y,TB_RED, TB_BLUE, " [%c] ", keyboard_pos[i].key);
            }
            tb_present();
            struct tb_event ev;
            tb_peek_event(&ev, 0);
            if (ev.key == TB_KEY_ESC && ev.type == TB_EVENT_KEY) {
                tb_shutdown();
                break;
            }
        }
    }

    
    return 0;
}