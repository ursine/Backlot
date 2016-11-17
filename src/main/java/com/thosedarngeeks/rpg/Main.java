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

class Main {

    public static void main(String[] args) {
	Logger logger = LoggerFactory.getLogger(Main.class);
	logger.info("Starting world...");

	//Collection<Version> nettyVers = Version.identify().values();

	List<String> nettyVers = Version.identify().values().stream()
	    .map(Version::toString)
	    .collect(Collectors.toList());
	
	StringBuilder vers = new StringBuilder();

	// Get the Netty versions
	vers.append(String.join(", ", nettyVers));

	// TODO: Get the database versions
	// "select value from information_schema.settings where name = 'info.VERSION';" 

	// TODO: Get the javascript versions
	// TODO: Iterate through the plugins and get those versions
	
	logger.info("Java version: "+System.getProperty("java.version"));
	logger.info("Home Directory: "+System.getProperty("user.home"));
	
	logger.info("Software Library Versions: " + vers.toString());

	logger.info("Shutting down world...");
    }


}
