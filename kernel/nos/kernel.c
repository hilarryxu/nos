void kmain(unsigned long magic, unsigned long addr)
{
    const char hw[] = "Hello World!";
    int i;
    char* video = (char*) 0xb8000;

    for (i = 0; hw[i] != '\0'; i++) {
        video[i * 2] = hw[i];
        video[i * 2 + 1] = 0x07;
    }
}
