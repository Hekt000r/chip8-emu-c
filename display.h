#ifndef display
#define display

int display_init(void);
void display_clear(void);
void display_render(void);
void display_shutdown(void);
uint32_t display_get_ticks(void);
void display_delay(uint32_t ms);

#endif