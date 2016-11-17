package com.thosedarngeeks.rpg;

import org.apache.commons.cli.Options;

public class CommandLine {

    public Options createOptions() {
	
	Options options = new Options();

	options.addOption('d', "directory", true, "Directory to store metadata about the game world in progress");
	options.addOption('v', "verbose", false, "Be extra verbose in the logging");

        return options;
    }

}
