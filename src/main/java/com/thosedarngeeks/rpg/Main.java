package com.thosedarngeeks.rpg;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import io.netty.util.Version;
import java.util.Map;
import java.util.Collection;
import java.lang.String;
import java.util.List;
import java.util.stream.Collectors;

import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;

import java.sql.*;

import org.apache.commons.cli.Options;

import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import jdk.nashorn.api.scripting.NashornScriptEngineFactory;

public class Main {

    public Options createOptions() {	
	final Options options = new Options();
	
	options.addOption("d", "directory", true, "Directory to store metadata about the game world in progress");
	options.addOption("v", "verbose", false, "Be extra verbose in the logging");
	
        return options;
    }

    public void run() throws Exception {
	EventLoopGroup bossGroup = new NioEventLoopGroup();
	EventLoopGroup workerGroup = new NioEventLoopGroup();

	try {

	    ServerBootstrap b = new ServerBootstrap();
	    b.group(bossGroup, workerGroup)
		.channel(NioServerSocketChannel.class);

	    
	} finally {
	    workerGroup.shutdownGracefully();
	    bossGroup.shutdownGracefully();
	}
	       
	
    }
    
    public static void main(String[] args) {
	Logger logger = LoggerFactory.getLogger(Main.class);
	logger.info("Starting world...");

	// TODO: Get the database versions
	// "select value from information_schema.settings where name = 'info.VERSION';" 

	// TODO: Get the javascript versions	
	NashornScriptEngineFactory nsef = new NashornScriptEngineFactory(); 
	
	// TODO: Iterate through the plugins and get those versions
	
	logger.info("Java version: "+System.getProperty("java.version"));
	logger.info("Home Directory: "+System.getProperty("user.home"));
	logger.info("Scripting Engine: " + nsef.getEngineName() + " - " + nsef.getEngineVersion());
	logger.info("Scripting Language: " + nsef.getLanguageName() + " - " + nsef.getLanguageVersion());

	List<String> nettyVers = Version.identify().values().stream()
	  .map(Version::toString)
	  .collect(Collectors.toList());
	
	logger.info("Netty Library Versions: " + String.join(", ", nettyVers));

	try {
	    new Main().run();
	} catch (Exception e) {
	    logger.error("Exception! " + e);
	}
	
	logger.info("Shutting down world...");
    }


}





