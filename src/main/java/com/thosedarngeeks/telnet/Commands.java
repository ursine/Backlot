package com.thosedarngeeks.rpg;

public enum Commands {

    TC_IAC           (255),  //  Is A Command -- Data Byte 255.
    TC_DONT          (254),  //  Demaind other party stop option
    TC_DO            (253),  //  Request to other party of option	  
    TC_WONT          (252),  //  Refusal of option
    TC_WILL          (251),  //  Confirmation of option
    TC_SB            (250),  //  Indicates that what follows is subnegotiation
    TC_GO_AHEAD      (249),  //  The GA signal.
    TC_ERASE_LINE    (248),  //  The function EL.
    TC_ERASE_CHAR    (247),  //  The function EC.
    TC_AYT           (246),  //  The function AYT.
    TC_ABORT_OUTPUT  (245),  //  The function AO.
    TC_INTERRUPT_PROC(244),  //  The function IP.
    TC_BREAK         (243),  //  NVT character BRK.
    TC_DATA_MARK     (242),  // The data stream portion of a Synch.
    TC_NOP           (241),  //  No operation.
    TC_SE            (240),  // End of subnegotiation parameters.
    TC_EOR           (239),  // End of record (transparent mode)
    TC_ABORT         (238),  // Abort process
    TC_SUSP          (237),  // Suspend process*/
    TC_EOF           (236);  // End of file

    private final int commandByte;

    Commands(final int cmd) {
	this.commandByte = cmd;
    }

    public int commandByte() { return commandByte; }
}
