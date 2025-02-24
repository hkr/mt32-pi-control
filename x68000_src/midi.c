// MIDI I/O slot card stuff

#define MIDI_A_BASE_ADDR 0xEAFA00
#define MIDI_B_BASE_ADDR 0xEAFA10

volatile char* MIDI_R00= 0;
volatile char* MIDI_R01= 0;
volatile char* MIDI_R02= 0;
volatile char* MIDI_R03= 0;
volatile char* MIDI_R04= 0;
volatile char* MIDI_R05= 0;
volatile char* MIDI_R06= 0;
volatile char* MIDI_R07= 0;

// 'which' picks which IO address to use
void midi_init(int which)
{
    if(which == 0)
    {
        MIDI_R00 = (char*)(MIDI_A_BASE_ADDR+0x1);
        MIDI_R01 = (char*)(MIDI_A_BASE_ADDR+0x3);
        MIDI_R02 = (char*)(MIDI_A_BASE_ADDR+0x5);
        MIDI_R03 = (char*)(MIDI_A_BASE_ADDR+0x7);
        MIDI_R04 = (char*)(MIDI_A_BASE_ADDR+0x9);
        MIDI_R05 = (char*)(MIDI_A_BASE_ADDR+0xB);
        MIDI_R06 = (char*)(MIDI_A_BASE_ADDR+0xD);
        MIDI_R07 = (char*)(MIDI_A_BASE_ADDR+0xF);
    }
    else
    {
        MIDI_R00 = (char*)(MIDI_B_BASE_ADDR+0x1);
        MIDI_R01 = (char*)(MIDI_B_BASE_ADDR+0x3);
        MIDI_R02 = (char*)(MIDI_B_BASE_ADDR+0x5);
        MIDI_R03 = (char*)(MIDI_B_BASE_ADDR+0x7);
        MIDI_R04 = (char*)(MIDI_B_BASE_ADDR+0x9);
        MIDI_R05 = (char*)(MIDI_B_BASE_ADDR+0xB);
        MIDI_R06 = (char*)(MIDI_B_BASE_ADDR+0xD);
        MIDI_R07 = (char*)(MIDI_B_BASE_ADDR+0xF);
    }
   
    // initial reset
    *MIDI_R01 = 0x80;
   
    // group 5 (R5n)
    *MIDI_R01 = 5;
    // Tx reset
    *MIDI_R05 = 0x81;
   
    // set Tx clock (group 4)
    *MIDI_R01 = 4;
    *MIDI_R04 = 8;
   
    // finally, set group 5
    *MIDI_R01 = 5;
}

void midi_quit()
{
    // hw reset
    //*MIDI_R01 = 0x80;
}

void midi_write(char c)
{
    int wait = 0;
    // wait for Tx FIFO ready
    while(!(*MIDI_R04 & 0x40))
        wait++;
    *MIDI_R06 = c;
   
}
