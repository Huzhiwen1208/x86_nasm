#include "include/console.h"
#include "include/type.h"
#include "include/io.h"

#define CRT_ADDR_REG 0x3D4 // CRT(6845) address register
#define CRT_DATA_REG 0x3D5 // CRT(6845) data register

#define CRT_START_ADDR_H 0xC // video memory offset in GPU: high 8bit
#define CRT_START_ADDR_L 0xD // video memory offset in GPU: low 8bit
#define CRT_CURSOR_H 0xE     // cursor position: high 8bit
#define CRT_CURSOR_L 0xF     // cursor position: low 8bit

#define GPU_MEM_BASE 0xB8000              // GPU memory base
#define GPU_MEM_SIZE 0x4000               // GPU memory size
#define WIDTH 80                      // text character number in a row: columns
#define HEIGHT 25                     // text character number in a column: rows
#define ROW_SIZE (WIDTH * 2)          // row size in bytes
#define SCR_SIZE (WIDTH * HEIGHT * 2)  // screen size in bytes

// control character
#define NUL 0x00
#define ENQ 0x05
#define ESC 0x1B // ESC
#define BEL 0x07 // \a
#define BS 0x08  // \b
#define HT 0x09  // \t
#define LF 0x0A  // \n
#define VT 0x0B  // \v
#define FF 0x0C  // \f
#define CR 0x0D  // \r
#define DEL 0x7F

static u16 space = 0x0720; // space character

struct corsor {
    u32 row;
    u32 col;
};

// get current video memory base address, byte dimenson
static u32 get_video_memory_base();
// reset video memory base address with arg {screen}, it can be used to clear screen & scroll screen
static void set_video_memory_base(u32 screen);
// get current cursor position with row & col in character dimension
static struct corsor* get_current_cursor();
// set cursor position, row: c->row, col: c->col
static void set_cursor(struct corsor* c);
// clear screen
static void console_clear();
// write string to console
void console_write(const char* buffer, u32 len);

void console_init() {
    console_clear();
}

static u32 get_video_memory_base() {
    writeb(CRT_ADDR_REG, CRT_START_ADDR_H);
    u32 offset = readb(CRT_DATA_REG) << 8;
    writeb(CRT_ADDR_REG, CRT_START_ADDR_L);
    offset |= readb(CRT_DATA_REG);   // offset in GPU, character number, should multiply by 2 to get byte offset

    offset *= 2;

    return GPU_MEM_BASE + offset;
}

static void set_video_memory_base(u32 screen) {
    screen -= GPU_MEM_BASE;
    writeb(CRT_ADDR_REG, CRT_START_ADDR_H);
    writeb(CRT_DATA_REG, (u8) (screen >> 9));
    writeb(CRT_ADDR_REG, CRT_START_ADDR_L);
    writeb(CRT_DATA_REG, (u8) (screen >> 1));
}

static struct corsor* get_current_cursor() {
    static struct corsor c;

    writeb(CRT_ADDR_REG, CRT_CURSOR_H);
    u32 position = readb(CRT_DATA_REG) << 8;
    writeb(CRT_ADDR_REG, CRT_CURSOR_L);
    position |= readb(CRT_DATA_REG); // character position

    position <<= 1;

    u32 cursor_address = GPU_MEM_BASE + position;
    u32 offset = cursor_address - get_video_memory_base();

    c.row = offset / ROW_SIZE;
    c.col = offset % ROW_SIZE;
    return &c;
}

void set_cursor(struct corsor* c) {
    u32 offset = c->row * ROW_SIZE + c->col * 2;
    u32 position = offset + get_video_memory_base() - GPU_MEM_BASE;
    position >>= 1;
    
    writeb(CRT_ADDR_REG, CRT_CURSOR_H);
    writeb(CRT_DATA_REG, (u8) (position >> 8));
    writeb(CRT_ADDR_REG, CRT_CURSOR_L);
    writeb(CRT_DATA_REG, (u8) position);
}

void console_clear() {
    // reset cursor position and screen
    u32 screen = GPU_MEM_BASE;
    set_video_memory_base(screen);
    struct corsor c = {0, 0};
    set_cursor(&c);

    // fill screen with space character
    u16* ptr = (u16*) GPU_MEM_BASE;
    while (ptr < (u16 *)(GPU_MEM_SIZE + GPU_MEM_BASE)) {
        *ptr++ = space;
    }
}

void console_write_with_color(const char* buffer, u32 len, u8 color) {
    struct corsor* c = get_current_cursor();
    u32 row = c->row;
    u32 col = c->col;

    u16* ptr = (u16*) (get_video_memory_base() + row * ROW_SIZE + col*2);
    while (len--) {
        char ch = *buffer++;
        switch (ch) {
            case NUL:
                break;
            case ENQ:
                break;
            case ESC:
                break;
            case BEL:
                break;
            case BS:
                if (col > 0) {
                    col--;
                }
                break;
            case HT:
                col = (col + 8) & ~(8 - 1);
                break;
            case LF:
                row++;
                col=0;
                break;
            case VT:
                break;
            case FF:
                break;
            case CR:
                col = 0;
                break;
            case DEL:
                break;
            default:
                *ptr++ = (u16) (ch | (u16)(color << 8));
                col++;
                break;
        }

        if (col == WIDTH) {
            col = 0;
            row++;
        }

        if (row == HEIGHT) {
            row --;
            u32 screen = get_video_memory_base() + ROW_SIZE;
            set_video_memory_base(screen);
        }

        ptr = (u16*) (get_video_memory_base() + row * ROW_SIZE + col*2);
    }

    c->row = row;
    c->col = col;
    set_cursor(c);
}

void console_write(const char* buffer, u32 len) {
    struct corsor* c = get_current_cursor();
    u32 row = c->row;
    u32 col = c->col;

    u16* ptr = (u16*) (get_video_memory_base() + row * ROW_SIZE + col*2);
    while (len--) {
        char ch = *buffer++;
        switch (ch) {
            case NUL:
                break;
            case ENQ:
                break;
            case ESC:
                break;
            case BEL:
                break;
            case BS:
                if (col > 0) {
                    col--;
                }
                break;
            case HT:
                col = (col + 8) & ~(8 - 1);
                break;
            case LF:
                row++;
                col=0;
                break;
            case VT:
                break;
            case FF:
                break;
            case CR:
                col = 0;
                break;
            case DEL:
                break;
            default:
                *ptr++ = (u16) (ch | 0x0700);
                col++;
                break;
        }

        if (col == WIDTH) {
            col = 0;
            row++;
        }

        if (row == HEIGHT) {
            row --;
            u32 screen = get_video_memory_base() + ROW_SIZE;
            set_video_memory_base(screen);
        }

        ptr = (u16*) (get_video_memory_base() + row * ROW_SIZE + col*2);
    }

    c->row = row;
    c->col = col;
    set_cursor(c);
}