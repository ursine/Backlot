package com.thosedarngeeks.rpg;

public enum Options {

    TC_OPT_BINARY           (0),  // from RFC 856
    TC_OPT_ECHO             (1),  // from RFC 857
    TC_OPT_SUPPRESS_GO_AHEAD(3),  // from RFC 858
    TC_OPT_STATUS           (5),  // from RFC 859
    TC_OPT_TIMING_MARK      (6),  // from RFC 860
    TC_OPT_TTYPE            (24), // from RFC 930, 1091
    TC_OPT_WINDOW_SIZE      (31), // from RFC 1073
    TC_OPT_LINE_MODE        (34), // from RFC 1184
    TC_OPT_NEW_ENVIRON      (39), // from RFC 1572
    TC_OPT_COMPRESS2        (86); // from http://www.zuggsoft.com/zmud/mcp.htm

    private final int optionByte;

    private Options(final int optByte) {
	this.optionByte = optByte;
    }

    public int optionByte() { return optionByte; }
    
}
