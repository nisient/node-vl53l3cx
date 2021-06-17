// import native addon
const addonVL53L3CX = require('bindings')('node-vl53l3cx');

// expose module API
exports.initSensor = addonVL53L3CX.initSensor;

let deviceId = '00000003c46a';
let busId = 3;

console.log('initSensor:',  addonVL53L3CX.initSensor(deviceId, busId));
console.log();