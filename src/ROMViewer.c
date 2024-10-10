#include "ROMViewer.h"

#define KEY_EVENT_NULL -100

#define KEY_EVENT_HOME 200

#define KEY_EVENT_UP 201
#define KEY_EVENT_LEFT 202
#define KEY_EVENT_DOWN 203
#define KEY_EVENT_RIGHT 204

#define KEY_EVENT_VIEWS 205

#define KEY_EVENT_ENTER 206
#define KEY_EVENT_DEL 207

int *cursor = 0x00000000;
int isVisible = 0;

int KEY_DELAY = 1250000;

void readROM(unsigned int offset);
int getOffset(int keyEvent);
void putstr(const char *str);
char nibToHex(int nib);
char numToHex(int num);
void cleanArea(int x, int y, int width, int height);
int displayDialog(int current);

int main(void){
    int offset = 0x00000000;
    int keycode = 0, temp = 0;
	readROM(offset);
	while(1) {
        keycode = get_key();
		if(keycode == KEY_EVENT_HOME){
			return 0;
		}else if(keycode >= KEY_EVENT_UP && keycode <= KEY_EVENT_RIGHT) {
            temp = getOffset(keycode);
            offset = ((offset + temp) > -1) ? (offset + temp) : 0x00000000;
            readROM(offset);
        }else if(keycode == KEY_EVENT_VIEWS && isVisible == 0) {
            isVisible = 1;
			KEY_DELAY = 937500;
            offset = displayDialog(offset);
            KEY_DELAY = 1250000;
		}   
	}
    return 0;
}

void readROM(unsigned int offset) {
    unsigned int data;
    unsigned int start = offset;
    //clear the screen
	clear_screen(); 
    putstr("addr   data\n");
    cursor = (unsigned int *)start;
    for(int n = 0; n < 9; n++) {
        for(int i = 20; i > -1; i-=4) {
            putchar(nibToHex(start >> i));
        }
        putchar(' ');
        //4bytes pre fetch
        for(int i = 0; i < 2; i++) {
            data = *(cursor + i);
			putchar(nibToHex(data >> 4));
			putchar(nibToHex(data >> 0));
			putchar(' ');
            putchar(nibToHex(data >> 12));
            putchar(nibToHex(data >> 8));
            putchar(' ');
            putchar(nibToHex(data >> 20));
            putchar(nibToHex(data >> 16));
            putchar(' ');
            putchar(nibToHex(data >> 28));
            putchar(nibToHex(data >> 24));
            putchar(' ');
			
        }   
        putchar('\n');
        cursor += 2;
        start += 8;
    }
}

void putstr(const char *str) {
	while(*str) {
		putchar(*str++);
	}
}

//converts a nibble to its hex form
char nibToHex(int nib){ 
	char * hex="0123456789ABCDEF";
	return hex[nib&0xf];
}

char numToHex(int num){ 
	char * hex="0123456789ABCDEF";
	return hex[num];
}

int getOffset(int keyEvent) {
    int events[] = {KEY_EVENT_UP, KEY_EVENT_LEFT, KEY_EVENT_DOWN, KEY_EVENT_RIGHT};
    int values[] = {-8, -64, 8, 64};
    for(int i = 0; i < 4; ++i) {
        if(keyEvent == events[i]) {
            return values[i];
        }
    }
    return 0;
}

void cleanArea(int x, int y, int width, int height) {
    hpg_init();
    hpg_set_color(hpg_stdscreen, HPG_COLOR_WHITE);
    hpg_fill_rect(x, y, x + width, y + height);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_BLACK);
}

int displayDialog(int current) {
	int x = 17, y = 8;
	int width = 96, height = 48;
    int keycode = 0, i = 0;
    unsigned char buffer[7] = {'_','_','_','_','_','_','\0'};
    unsigned int inputs[6] = {0,0,0,0,0,0};
   
    //container
    cleanArea(x - 1, y - 1, width + 2, height + 2);
	hpg_draw_rect(x, y, x + width, y + height);
	//title
    hpg_draw_text("Jump to address", x + 18, y + 2);
	hpg_draw_line(x, (y + 9), x + width, (y + 2 + 7));
	//input area
    hpg_draw_text("address:", x + 3, (y + 12));
	hpg_draw_rect(x+3, y+20, x + 93, y + 30);
	//buttons w=22,h=8
	hpg_draw_rect(88, 45, x + 93, 53);
	hpg_draw_text("ENTER", 88 + 2, 45 + 2);
	
	hpg_draw_rect(60, 45, 82, 53);
	hpg_draw_text("VIEWS", 60 + 2, 45 + 2);
	
    while(1) {
        cleanArea(x+34, y+22, 24, 6);
        hpg_draw_text(buffer, x + 35, (y + 23));
        keycode = get_key();
        if(keycode == KEY_EVENT_HOME){
			break;
		}else if(keycode >= KEY_EVENT_UP && keycode <= KEY_EVENT_RIGHT) {
            isVisible = 0;
            readROM(current);
            return current;
        }else if(keycode == KEY_EVENT_VIEWS && isVisible == 1) {
            isVisible = 0;
            readROM(current);
            return current;
		}else if(keycode == KEY_EVENT_DEL && i > 0) {
            i--;
            buffer[i] = '_';
            inputs[i] = 0;
        }else if(keycode == KEY_EVENT_ENTER) {
            if(inputs[0] == 0 || inputs[0] == 8) {
                isVisible = 0;
                int addr = (inputs[0] & 0x0000000F);
                addr |= ((inputs[1] & 0x0000000F) << 4);
                addr |= ((inputs[2] & 0x0000000F) << 8);
                addr |= ((inputs[3] & 0x0000000F) << 12);
                addr |= ((inputs[4] & 0x0000000F) << 16);
                addr |= ((inputs[5] & 0x0000000F) << 20);
                readROM(addr);
                return addr;
            }else {
                cleanArea(x + 3, y + 12, 32, 6);
                hpg_draw_text("must end with 0 or 8", x + 3, (y + 12));
                delay(6250000);
                cleanArea(x + 3, y + 12, 80, 6);
                hpg_draw_text("address:", x + 3, (y + 12));
            }
        }else if(keycode >= 0 && keycode <= 15 && i < 6) {
            inputs[5 - i] = keycode;
            buffer[i] = numToHex(keycode);
            i++;
        }
    }
    return 0;
}

int event_handler(unsigned col, unsigned row) {
	//[APLET], [ON], [HOME]// 
	if ((col == 7 && row == 0) || (col == 4 && row == 6) || 
        (col == 6 && row == 6)) {
		//exit immediately
		return KEY_EVENT_HOME;
	}
	//delay
    delay(KEY_DELAY);	
	if (col == 6) {
		return row + KEY_EVENT_HOME + 1;
	} 
    if (col == 7 && row == 1) {
		return KEY_EVENT_VIEWS;				
	}
    if (col == 0 && row == 6) {
		return KEY_EVENT_ENTER;				
	}
    if (col == 0 && row == 0) {
		return KEY_EVENT_DEL;				
	}
    //number keys(0~9)
    if(col > 0 && col < 4) {
        //1~9
        if(row > 2 && row < 6) {
            return (6 - row) * 3 - col + 1;
        }
        if(col == 3 && row == 6) {
            return 0;
        }
    }
    //alphabet keys(A~D)
    if(row == 0 && (col > 0 && col < 5)) {
        return 10 + (4 - col);
    }
    //alphabet keys(E~F)
    if(row == 1 && (col > 2 && col < 5)) {
        return 10 + (8 - col);
    }
	return KEY_EVENT_NULL;
}
