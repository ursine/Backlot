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
  const fs = require('fs');
  const path = require('path');

  process.chdir(__dirname);

  logger.info('Starting Backlot server process');

  const backlot = {

    init: function () {
      this.loadI18NStrings();

      logger.info('Alerting modules that initialization is complete');
      backlot.emit('init');
    },

    loadI18NStrings: function () {
      const loadStartTm = process.hrtime();
      const directory = './i18n';
      const i18n = fs.readdirSync(directory);
      for (let i = 0; i < i18n.length; i++) {
        const modName = i18n[i];
        const theFile = path.join(__dirname, directory, modName);
        const fileDetails = path.parse(theFile);

        // Throw away non-javascript files
        if (fileDetails.ext !== '.js') {
          continue;
        }

        const baseName = fileDetails.name;

        // Is this being reloaded
        const reloaded = typeof (global[baseName]) !== 'undefined';
        const loadType = reloaded ? 'Reloaded' : 'Loaded';

        if (reloaded) {
          delete require.cache[require.resolve(theFile)];
        }
        global[baseName] = require(theFile);

        const theLangTag = global[baseName].LanguageTag;

        const loadEndTm = process.hrtime(loadStartTm);
        logger.log('info', '%s I18N strings for %s(%s) from %s: %ds %dms', loadType, baseName, theLangTag, theFile, loadEndTm[0], loadEndTm[1] / 1000000);
      }
    },

    loadModules: function () {

    },

    die: function () {
      logger.warn('Starting to shutdown server');
      process.exit();
    }

  };

  backlot.__proto__ = events.EventEmitter.prototype;

  // Configure the process to handle various signals
  const signals = ['SIGINT', 'SIGABRT', 'SIGSEGV', 'SIGTERM'];
  for (let i = 0; i < signals.length; i++) {
    const currentSignal = signals[i];
    logger.log('info', 'Adding handler for %s', currentSignal);
    /* process.on(currentSignal, function() {
     logger.log('info', 'Received %s', currentSignal);
     backlot.die();
     }); */
  }

  backlot.init();

  logger.info('Exiting Backlot server process');
}());
