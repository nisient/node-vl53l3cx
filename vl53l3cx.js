// import native addon
const addonVL53L3CX = require('bindings')('node-vl53l3cx');

// expose module API
exports.initSensor = addonVL53L3CX.initSensor;