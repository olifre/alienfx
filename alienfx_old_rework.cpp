//Include the zone presets & useful consts
#include "presets.h"
#include "consts.h"

libusb_context* context;
libusb_device_handle* alienFx;
int alienFXid, START_BYTE, /*magicnum,*/ preset=-1, speed, usb_sleep=DEFAULT_USB_SLEEP, power_block, command_count;
char command[1024]; unsigned char rawcmd[10];
bool user_is_root, debug, debug2, help, reset, reboot, saving, darkness, wait, rawmode;

char hextoint(char c) {
	if (c >= '0' && c <= '9') return c-'0';
	if (c >= 'A' && c <= 'F') return c-'A'+10;
	if (c >= 'a' && c <= 'f') return c-'a'+10;
	return -1;
}

void print_help() {
	printf("Alienware AlienFX Light Manipulation Program v1.0\n");
	printf("Usage:    alienfx [options]\n");
	printf("   or     alienfx -h\n");
	printf("Options:\n");
	printf("  -c ZnnM[RGB[RGB]]..   Lightchip Command\n");
	printf("  -d <num>  Set morph/flash delay time. (1=Fastest to 10=Slowest)\n");
	printf("  -h        This helpful message\n");
	printf("  -P <hex>  Specify the hex ProductID of lightchip\n");
	printf("  -r        Reset chip; software reset of lightchip\n");
	printf("  -R        Reboot chip; hardware reset of lightchip\n");
	printf("  -s <num>  Save to PowerBlock (for load-on-boot, etc)\n");
	printf("  -t <num>  Increase USB timing delay by increment (1 to 100)\n");
	printf("  -u        Show USB debug if needed\n");
	printf("  -v        Show verbose debug output\n");
	printf("  -w        Check for lightchip ready (excess,slow)\n");
	printf("  -X <num>  Raw command mode. Expects nine 0-255 values\n");
	printf("  -z <num>  Apply a preset. Use '0' to get a list. Implies ignorance of -c.\n");
	printf("Each Command is made up of Z (zone), M (mode) and RGB (colour) values.\n");
	printf("Zone is either 00=All or 01-27.\n");
	printf("Mode is either B=Blink, M=Morph or F=Fixed.\n");
	printf("RGB is three single-digits of hex. Specify two colours, even if 2nd unused.\n");
	printf("Zones are either 1 or 0. 1 is on, 0 is off. There are sixteen zones.\n");
	printf("PowerBlock is either 0=Default, 1=Bootup, 5=AC, 6=Charge, 7=BattLow or 8=Batt.\n");
	printf("Defaults: Power=ALL, Zones=ALL, Mode=FIXED, RGB=000(black)\n");
	printf("Permanently saving the settings into lightchip not available on some models.\n");
	printf("If called with no arguments the lights will silently turn off.\n");
	printf("For example commands see the preset list.\n");
	printf("See 'man alienfx' for more information.\n");
}

int is_valid_command_string(char *cmd) {
  //i know this could be better, but it works and i can't be bothered.
  int i=0; command_count=0;
  bool next_zone, next_action;
  next_zone=true;
  while (next_zone) {
    if (cmd[i]=='Z'|cmd[i]=='z') { i++;
      if (hextoint(cmd[i])>=0) i++; else return i+1;
      if (hextoint(cmd[i])>=0) i++; else return i+1;
      if ((hextoint(cmd[i-2])*10)+hextoint(cmd[i-1])>27) return i-1;
      next_action=true;
      while (next_action) {
        if (cmd[i]=='B'|cmd[i]=='b'|cmd[i]=='F'|cmd[i]=='f') { i++;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          command_count++;
        } else //if B or F
        if (cmd[i]=='M'|cmd[i]=='m') { i++;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          if (hextoint(cmd[i])>=0) i++; else return i+1;
          command_count++;
        } else //if M
          if (cmd[i]=='Z'|cmd[i]=='z') next_action=false;
        else
          if (strlen(cmd)==i) return 0; else return i+1;
      } //while actions
    } else { if (i==0) return 1; else next_zone=false; } //if Z
  } //while zones
  return 0; //we made it passed the checks
}

bool parse_arguments(int argc, char *argv[], char **envp) {
	//Trap username from environment
	char *var,tag[5],uservar[9],arg[2],setzones[16];
	int i;
        while(*envp != NULL) {
		var = *(envp++);
		if (strcmp(var,"USER=root")|strcmp(var,"USERNAME=root")) user_is_root=true;
	}
	bool colour_option;
        while(*argv != NULL) {
		var = *(argv++);
		//if (debug2) printf("Debug: Var: \"%s\"\n", var);
		if (!strcmp(var,"-c")) { strcpy(command,*(argv++)); } //command!
		if (!strcmp(var,"-d")) { speed = atoi(*(argv++)); }  //flash/morph speed
		if (!strcmp(var,"-h")|!strcmp(var,"--help")) { help = true;   }  //help output
		if (!strcmp(var,"-p")) { preset = atoi(*(argv++)); } //presets!
		if (!strcmp(var,"-P")) { sscanf(*(argv++), "%llx", (unsigned int *)&alienFXid); }  //USB ProductID, ugly!
		if (!strcmp(var,"-r")) { darkness = true; }//Plunge into darkness before set colours
		if (!strcmp(var,"-R")) { reboot = true; }  //COMMAND_REBOOT_CHIP
		if (!strcmp(var,"-s")) { saving = true; power_block = atoi(*(argv++)); }  //saving (save to CMOS not available)
		if (!strcmp(var,"-t")) { usb_sleep = DEFAULT_USB_SLEEP * atoi(*(argv++)); } //USB delay
		if (!strcmp(var,"-u")) { debug2 = true; }  //libusb debugging
		if (!strcmp(var,"-v")) { debug = true;  }  //alienfx debugging
		if (!strcmp(var,"-w")) { wait = true;   }  //usb in waiting
		if (!strcmp(var,"-X")) { //Raw command mode, useful for light notifications
			for (i=0;i<10;i++) { rawcmd[i] = atoi(*(argv++)); };
			rawmode = true;
		};
	}
	if (preset==0) {
		printf("All presets are applied by default to all zones.  Arguments supplied regarding\n");
		printf("power mode, light zones, effects or colours are ignored when selecting presets.\n");
		printf("Available lightchip presets:\n");
		for (i=0;i<20;i++) printf(" alienfx -p %.2i == alienfx -c %s == %s\n", i+1, preset_data[i], preset_name[i]);
		exit(0);
	}
	if ((preset>0)&(preset<=MAX_PRESET)) {
		if (debug) printf("Debug: Chosen preset %.2i = %s\n", preset, preset_name[preset-1]);
		memset(command,0,sizeof(command));
		if (debug) printf("Debug: Preset Colour Command [%s]\n", preset_data[preset-1]);
		strcpy(command, preset_data[preset-1]);
	}
	if (strlen(command)>0) {
		i = is_valid_command_string(command);
		if (i>0) {
			printf("Error at character %i in specified command.\n", i);
			exit(2);
		}
	}
	return true;
}

bool afx_raw(unsigned char cmd[DATA_LENGTH]) {
	unsigned char chk[DATA_LENGTH];
	memcpy(chk,cmd,DATA_LENGTH);
	WriteDevice(cmd,DATA_LENGTH);
	//	usleep(DEFAULT_USB_SLEEP);
	if (memcmp(chk,cmd,DATA_LENGTH)) printf("AFX: OK\n"); else printf("AFX: ERR\n");
}

bool afx_process(char *cmd) {
//Command begins====================================================================
  int index = 1, ref=0, zone, z1,z2,z3,r1,g1,b1,r2,g2,b2;
  if (debug) printf("Debug: Begin lightchip command sequence\n");
  while (!(cmd[ref]==0)) { //Powermode begins=======================================
    if (debug&saving) printf("Debug:   PowerBlock: %i\n",power_block);
    if (debug&power_block==0&saving) printf("Debug:   Warning: saving to EEPROM may not be available\n");
    if (debug) printf("Debug:   Reset All Lights On\n");
    afx_cmd(COMMAND_RESET,RESET_ALL_LIGHTS_ON,0,0,0,0,0,0);
    if (speed > 0) {
      if (debug) printf("Debug:   Setting speed to %i of 10\n", speed);
      afx_spd(speed);
    }
    while (!(cmd[ref+1]==0)) { //Zone begins======================================
      if (cmd[ref]=='Z'|cmd[ref]=='z') { ref++;
        zone = (10*hextoint(cmd[ref]))+hextoint(cmd[ref+1]); ref+=2;
        if (debug) {
          if (zone==0) printf("Debug:      LightZone(All)\n");
          else printf("Debug:      LightZone(%i)\n",zone);
        }
        while (!(cmd[ref+1]==0)) { //Colour begins================================
          if (cmd[ref]=='M'|cmd[ref]=='m') { ref++;
            r1 = hextoint(cmd[ref++]); g1 = hextoint(cmd[ref++]); b1 = hextoint(cmd[ref++]);
            r2 = hextoint(cmd[ref++]); g2 = hextoint(cmd[ref++]); b2 = hextoint(cmd[ref++]);
            if (saving&debug) printf("Debug:         Save Powermode(%i)\n",power_block);
            if (saving) afx_cmd(COMMAND_SAVE_NEXT,power_block,0,0,0,0,0,0);
            if (debug) printf("Debug:         Set Colour[%i]: Morph %i %i %i into %i %i %i\n",index,r1,g1,b1,r2,g2,b2);
            afx_set(COMMAND_SET_COLOUR_MORPH,command_count,zone,r1,g1,b1,r2,g2,b2,0);
            index++;
          }
          if (cmd[ref]=='B'|cmd[ref]=='b') { ref++;
            r1 = hextoint(cmd[ref++]); g1 = hextoint(cmd[ref++]); b1 = hextoint(cmd[ref++]);
            if (saving&debug) printf("Debug:         Save Powermode(%i)\n",power_block);
            if (saving) afx_cmd(COMMAND_SAVE_NEXT,power_block,0,0,0,0,0,0);
            if (debug) printf("Debug:         Set Colour[%i]: Blink %i %i %i\n",index,r1,g1,b1);
            afx_set(COMMAND_SET_COLOUR_BLINK,command_count,zone,r1,g1,b1,255,255,255,0);
            index++;
          }
          if (cmd[ref]=='F'|cmd[ref]=='f') { ref++;
            r1 = hextoint(cmd[ref++]); g1 = hextoint(cmd[ref++]); b1 = hextoint(cmd[ref++]);
            if (saving&debug) printf("Debug:         Save Powermode(%i)\n",power_block);
            if (saving) afx_cmd(COMMAND_SAVE_NEXT,power_block,0,0,0,0,0,0);
            if (debug) printf("Debug:         Set Colour[%i]: Fixed %i %i %i\n",index,r1,g1,b1);
            afx_set(COMMAND_SET_COLOUR_FIXED,command_count,zone,r1,g1,b1,0,0,0,0);
            index++;
          }
          if (cmd[ref]=='Z'|cmd[ref]=='z'|cmd[ref]=='P'|cmd[ref]=='p') break; //else ref++;
        } //Colour ends===========================================================
        if (saving&debug) printf("Debug:      Save Powermode(%i)\n",power_block);
        if (saving) afx_cmd(COMMAND_SAVE_NEXT,power_block,0,0,0,0,0,0);
        if (debug) printf("Debug:      End Loop Block\n");
        afx_cmd(COMMAND_LOOP_BLOCK_END, 0, 0, 0, 0, 0, 0,wait);
      } else break;
    } //Zone ends=================================================================
//    if (saving&debug) printf("Debug:   Save Powermode(%i)\n",power_block);
//    if (saving) afx_cmd(COMMAND_SAVE_NEXT,power_block,0,0,0,0,0,0);
  } //Powermode ends================================================================
  if (debug) printf("Debug: Execute Instruction\n");
  afx_cmd(COMMAND_TRANSMIT_EXECUTE,0,0,0,0,0,0,wait);
//Command end=======================================================================
  if (debug) printf("Debug: End lightchip command sequence\n");
}

int main(int argc, char *argv[], char **envp) {
	memset(command,0,MAX_COMMAND);
	if (debug) printf("Debug: Hit main code block. Parsing arguments...\n");
	parse_arguments(argc, argv, envp);
	if (help) { print_help(); return 0; }
	if (debug) {
		if (user_is_root) printf("Debug: Detection suggests user may be root\n");
		else printf("Debug: Detection suggests user may NOT be root\n");
	}
	if (command[0]==0) printf("Warning: no command or preset supplied\n");
	if (AlienfxInit()) {
		if (debug) printf("Debug: Found an AlienFX chip\n");
		if (reboot) afx_reboot();
		if (darkness) afx_reset();
		if (rawmode) {
			afx_raw(rawcmd);
		} else {
			if (command[0]>0) { /*if (usb_is_ready())*/ afx_process(command); }
			else if (!darkness) { if (usb_is_ready()) afx_reset(); }
		}
		AlienfxDeinit();
	} else {
		if (debug) printf("Debug: USB driver could not locate AlienFX chip.  Consult manpage?\n");
		if (debug2) printf("Debug: Use the -P setting to specify the product ID of your AlienFX chip.\n");
		if (debug2) printf("Debug: You may find the product ID by running the 'lsusb' command.\n");
		printf("Error: AlienFX not detected.\n");
		return 1;
	}
	if (debug) printf("Debug: Exiting\n");
	return 0;
} //EOF
