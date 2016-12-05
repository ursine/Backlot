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

module.exports.loadModules = function(directory, logger) {
  const loadStartTm = process.hrtime();
  const dirList = fs.readdirSync(directory);
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

    if (reloaded) {
      delete require.cache[require.resolve(theFile)];
    }
    global[baseName] = require(theFile);

    const moduleBase = global[baseName];
    const moduleName = moduleBase.moduleName;

    let typeString = 'unknown';
    if (moduleBase.moduleType == 'language') {
      typeString = 'I18N strings'
    }

    const loadEndTm = process.hrtime(loadStartTm);
    logger.log('info', '%s %s for %s(%s) from %s: %ds %dms',
      loadType, baseName, typeString, moduleName, theFile, loadEndTm[0], loadEndTm[1] / 1000000);
  }
};

