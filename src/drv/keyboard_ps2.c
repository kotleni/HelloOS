#define SC_MAX 57
#define SC_MIN 0

const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char *sc_ascii[] = { "?", "?", "1", "2", "3", "4", "5", "6",     
    "7", "8", "9", "0", "-", "=", "?", "?", "q", "w", "e", "r", "t", "y", 
        "u", "i", "o", "p", "[", "]", "?", "?", "a", "s", "d", "f", "g", 
        "h", "j", "k", "l", ";", "\"", "`", "?", "\\", "z", "x", "c", "v", 
"b", "n", "m", ",", ".", "/", "?", "?", "?", " "};

struct KeyboardKey{
    unsigned int num;
    unsigned int state;
    char* ascii;
};

struct KeyboardKey* keyboard_ps2_read(){
    struct KeyboardKey* k;

    port_byte_out(0x20 , 0x20);
    k->state = (port_byte_in(0x64) & 0x01);
	k->num = port_byte_in(0x60);
    k->ascii = (char*)sc_ascii[k->num];

	return k;
}