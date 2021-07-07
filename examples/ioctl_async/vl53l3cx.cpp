/**
 * This is the native c++ node.js module interface for the STMicro VL53L3CX ToF sensor.
 *
 * @module src/vl53l3cx.cpp
 * @version 0.2.0
 * @file vl53l3cx.cpp
 * @copyright nisient pty. ltd. 2021
 * @license
 * BSD 3-Clause License
 * 
 * Copyright (c) 2021, nisient
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <napi.h>

#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <errno.h>
#include <assert.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "vl53l3cxWorker.h"

/**
 * initSensor
 * This function initialises a sensor
 */
Napi::String initSensor(const Napi::CallbackInfo& info) {

	std::string deviceId = info[0].ToString();
	std::string i2cDevice = info[1].ToString();
	Napi::Function callback = info[2].As<Napi::Function>();

	vl53l3cxInit* asyncWorker = new vl53l3cxInit(callback, deviceId, i2cDevice);
	asyncWorker->Queue();

	std::string msg = "vl53l3cxInit queued";
	return Napi::String::New(info.Env(),msg.c_str());

}

/**
 * readSensor
 * This function performs a single read of the sensor
 */
//Napi::Object readSensor(const Napi::CallbackInfo& info) {
Napi::String readSensor(const Napi::CallbackInfo& info) {

	std::string deviceId = info[0].ToString();
	std::string i2cDevice = info[1].ToString();
	Napi::Function callback = info[2].As<Napi::Function>();

	vl53l3cxRead* asyncWorker = new vl53l3cxRead(callback, deviceId, i2cDevice);
	asyncWorker->Queue();

	std::string msg = "vl53l3cxRead queued";
	return Napi::String::New(info.Env(),msg.c_str());

}

/**
 * Init
 * This is the module initialisation
 */
Napi::Object Init(Napi::Env env, Napi::Object exports) {

	exports.Set(
		Napi::String::New(env, "initSensor"),
		Napi::Function::New(env, initSensor)
	);
	exports.Set(
		Napi::String::New(env, "readSensor"),
		Napi::Function::New(env, readSensor)
	);
	
	return exports;
}

NODE_API_MODULE(vl53l3cx, Init)
