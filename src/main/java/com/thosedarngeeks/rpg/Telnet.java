package com.thosedarngeeks.rpg;

public class Telnet {

    static final int TC_OPT_BINARY            = 0;  // from RFC 856
    static final int TC_OPT_ECHO              = 1;  // from RFC 857
    static final int TC_OPT_SUPPRESS_GO_AHEAD = 3;  // from RFC 858
    static final int TC_OPT_STATUS            = 5;  // from RFC 859
    static final int TC_OPT_TIMING_MARK       = 6;  // from RFC 860
    static final int TC_OPT_TTYPE             = 24; // from RFC 930, 1091
    static final int TC_OPT_WINDOW_SIZE       = 31; // from RFC 1073
    static final int TC_OPT_LINE_MODE         = 34; // from RFC 1184
    static final int TC_OPT_NEW_ENVIRON       = 39; // from RFC 1572
    static final int TC_OPT_COMPRESS2         = 86; // from http://www.zuggsoft.com/zmud/mcp.htm

    static final int TC_IAC            = 255;  //  Is A Command -- Data Byte 255.
    static final int TC_DONT           = 254;  //  Demaind other party stop option
    static final int TC_DO             = 253;  //  Request to other party of option	  
    static final int TC_WONT           = 252;  //  Refusal of option
    static final int TC_WILL           = 251;  //  Confirmation of option
    static final int TC_SB             = 250;  //  Indicates that what follows is subnegotiation
    static final int TC_GO_AHEAD       = 249;  //  The GA signal.
    static final int TC_ERASE_LINE     = 248;  //  The function EL.
    static final int TC_ERASE_CHAR     = 247;  //  The function EC.
    static final int TC_AYT            = 246;  //  The function AYT.
    static final int TC_ABORT_OUTPUT   = 245;  //  The function AO.
    static final int TC_INTERRUPT_PROC = 244;  //  The function IP.
    static final int TC_BREAK          = 243;  //  NVT character BRK.
    static final int TC_DATA_MARK      = 242;  // The data stream portion of a Synch.
    static final int TC_NOP            = 241;  //  No operation.
    static final int TC_SE             = 240;  // End of subnegotiation parameters.
    static final int TC_EOR            = 239;  // End of record (transparent mode)
    static final int TC_ABORT          = 238;  // Abort process
    static final int TC_SUSP           = 237;  // Suspend process
    static final int TC_EOF            = 236;  // End of file






    




}
