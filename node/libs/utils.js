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

'use strict';

const fs = require('fs');
const path = require('path');

module.exports.loadModules = function(baseModule, directory, logger) {
  const loadStartTm = process.hrtime();

    let dirList = [];
    try {
	dirList = fs.readdirSync(directory);
    }
    catch(err) {
	logger.log('error', 'Unable to read directory %s: %s', directory, err);
    }

  for (let i = 0; i < dirList.length; i++) {
    const modName = dirList[i];
    const theFile = path.join(directory, modName);
    const fileDetails = path.parse(theFile);

    // Throw away non-javascript files
    if (fileDetails.ext !== '.js') {
      continue;
    }

    const baseName = fileDetails.name;

      // Is this being reloaded
      const reloaded = typeof (global[baseName]) !== 'undefined';
      const loadType = reloaded ? 'Reloaded' : 'Loaded';
      
      try {
	  if (reloaded) {
	      delete require.cache[require.resolve(theFile)];
	  }
	  module = require(theFile);
	  global[baseName] = module;
	  baseModule.on('init', module.onInit);
      }
      catch(err) {
	  logger.log('error', 'Unable to load module %s: %s', theFile, err);
	  continue;
      }

    const moduleBase = global[baseName];
    const moduleName = moduleBase.moduleName;

    let typeString = 'unknown';
    if (moduleBase.moduleType == 'language') {
      typeString = 'I18N strings'
    } else if (moduleBase.moduleType == 'service') {
	typeString = 'providing'
    }

    const loadEndTm = process.hrtime(loadStartTm);
    logger.log('info', '%s %s for %s %s from %s: %ds %dms',
      loadType, baseName, typeString, moduleName, theFile, loadEndTm[0], loadEndTm[1] / 1000000);
  }
};

