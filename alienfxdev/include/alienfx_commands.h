#ifndef __alienfx_commands_h__
#define __alienfx_commands_h__

namespace alienFx {
	enum alienFx_commands {
		/// End Storage block
		END_STORAGE = 0x00,
		/// Set morph color
		SET_COLOUR_MORPH = 0x01,
		/// Set blink color
		SET_COLOUR_BLINK = 0x02,
		/// Set color
		SET_COLOUR_FIXED = 0x03,
		/// Loop Block end
		LOOP_BLOCK_END = 0x04,
		/// End transmition and execute
		TRANSMIT_EXECUTE = 0x05,
		/// Get device status
		GET_STATUS = 0x06,
		/// Reset
		RESET = 0x07,
		/// Save next instruction in storage block (see storage)
		SAVE_NEXT = 0x08,
		/// Save storage data (See storage)
		SAVE = 0x09,
		UNKNOWN_1 = 0x0A,
		UNKNOWN_2 = 0x0B,
		/// Reboot Chip
		REBOOT_CHIP = 0x0C, 
		UNKNOWN_3 = 0x0D,
		/// Set display speed (see set speed)
		SET_SPEED = 0x0E,
		/// Set battery state (See set commands)
		BATTERY_STATE = 0x0F
	};
};

#endif /* __alienfx_commands_h__ */
