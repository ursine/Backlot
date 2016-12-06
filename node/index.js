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

(function () {
  'use strict';

  // Set up local logging
  const winston = require('winston');
  const logger = new (winston.Logger)({
    transports: [
      new (winston.transports.Console)({
        'timestamp': true,
        'colorize': true
      })
    ]
  });

  // Set up requirements
  const events = require('events');
  const path = require('path');
  const util = require('util');
  const utils = require('./libs/utils.js');

  process.chdir(__dirname);

  logger.info('Starting Backlot server process');

  const Backlot = function () {
    events.EventEmitter.call(this);
  };
  util.inherits(Backlot, events.EventEmitter);

  Backlot.prototype.init = function () {
    this.loadI18NStrings();
    this.loadModules();
    logger.info('Alerting modules that initialization is complete');
    backlot.emit('init');
  };

  Backlot.prototype.loadI18NStrings = function() {
    utils.loadModules(path.join(__dirname,'i18n'), logger);
  }

  Backlot.prototype.loadModules = function() {
    utils.loadModules(path.join(__dirname, 'modules'), logger);
  }
    
  Backlot.prototype.die = function () {
    logger.warn('Starting to shutdown server');
    process.exit();
  };

  // Configure the process to handle various signals
  const signals = ['SIGINT', 'SIGABRT', 'SIGSEGV', 'SIGTERM'];
  for (let i = 0; i < signals.length; i++) {
    const currentSignal = signals[i];
    logger.log('info', 'Adding handler for %s', currentSignal);
    //process.on(currentSignal, function() {
    //logger.log('info', 'Received %s', currentSignal);
    //backlot.die();
  }

  const backlot = new Backlot();

  backlot.init();

  logger.info('Exiting Backlot server process');
}());
