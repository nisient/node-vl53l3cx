/**
 * This is the async worker c++ node.js module header file for the STMicro VL53L3CX ToF sensor.
 *
 * @module src/vl53l3cxWorker.h
 * @version 0.2.0
 * @file vl53l3cxWorker.h
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

#pragma once

#include <napi.h>

using namespace Napi;

class vl53l3cxInit : public AsyncWorker {

	public:
		vl53l3cxInit (Function& callback, std::string deviceId, std::string i2cDevice);
		virtual ~vl53l3cxInit() {};

		void Execute();
		void OnOK();

	private: 
		std::string deviceId;
		std::string i2cDevice;

};

class vl53l3cxRead : public AsyncWorker {

	public:
		vl53l3cxRead (Function& callback, std::string deviceId, std::string i2cDevice);
		virtual ~vl53l3cxRead() {};

		void Execute();
		void OnOK();

	private: 
		std::string deviceId;
		std::string i2cDevice;
		std::string result;

};
