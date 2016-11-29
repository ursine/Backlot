//#!/usr/bin/env node

/*
  Backlot, online game engine
  (C) 2017  Gary Coulbourne
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

// Set up local logging
var winston = require('winston');
var logger = new (winston.Logger)({
    transports: [
      new (winston.transports.Console)({
      	'timestamp':true, 'colorize':true
      })
    ]
});

// Set up requirements
var events = require('events');
var fs = require('fs');

process.chdir(__dirname);

logger.info('Starting Backlot server process');

backlot = {

	i18n: fs.readdirSync('./i18n'),

	init: function() {
		this.loadI18NStrings();
	},

	loadI18NStrings: function() {
		
		for(var lang in this.i18n) {
			logger.log('info', 'Loading I18N strings for %s', lang);
		}
		
	},

	die: function() {
		logger.warn('Starting to shutdown server');
		process.exit();
	}

};

backlot.__proto__ = events.EventEmitter.prototype;

// Configure the process to handle various signals
var signals = ['SIGINT', 'SIGABRT', 'SIGSEGV', 'SIGTERM'];
for(var i = 0; i < signals.length; i++) {
	var currentSignal = signals[i];
	logger.log('info', 'Adding handler for %s', currentSignal);
	/*process.on(currentSignal, function() { 
		logger.log('info', 'Received %s', currentSignal); 
		backlot.die();
	});*/
}

backlot.init();

logger.info('Exiting Backlot server process');
