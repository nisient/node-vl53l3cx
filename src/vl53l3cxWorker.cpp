/**
 * This is the async worker c++ node.js module for the STMicro VL53L3CX ToF sensor.
 *
 * @module src/vl53l3cxWorker.cpp
 * @version 0.2.0
 * @file vl53l3cxWorker.cpp
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

#include "vl53l3cxWorker.h"

#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
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

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "vl53lx_class.h"

#define I2C_DEVICE  0x29

std::map<std::string, VL53LX*> deviceMap;

vl53l3cxInit::vl53l3cxInit(Function& callback, std::string deviceId, std::string i2cDevice)
	: AsyncWorker(callback), deviceId(deviceId), i2cDevice(i2cDevice) {};

void vl53l3cxInit::Execute() {

	char * deviceId_ref = new char[deviceId.length() + 1];
	std::strcpy (deviceId_ref, deviceId.c_str());

	char * i2c_adapter = new char[i2cDevice.length() + 1];
	std::strcpy (i2c_adapter, i2cDevice.c_str());

	int fd_i2c;
	char filename[20];

	// open i2c
	snprintf(filename, 19, i2c_adapter);
	fd_i2c = open(filename, O_RDWR);
	if (fd_i2c < 0) {
		exit(1);
	}

	if (ioctl(fd_i2c, I2C_SLAVE, I2C_DEVICE) < 0) {
		exit(1);
	}

	// constructor
	deviceMap.insert(std::make_pair(deviceId_ref, new VL53LX()));
	// added since fd_i2c not passed to constructor now
	deviceMap[deviceId_ref]->VL53LX_SetDeviceAddress(fd_i2c);
	// added to prevent need to power cycle/hardware reset device
	deviceMap[deviceId_ref]->VL53LX_software_reset();
	deviceMap[deviceId_ref]->InitSensor(fd_i2c);
	deviceMap[deviceId_ref]->VL53LX_StartMeasurement(); 

	// close i2c
	close(fd_i2c);

};

void vl53l3cxInit::OnOK() {
	std::string msg = "init finished";
	Callback().Call({Env().Null(), String::New(Env(), msg)});
};

vl53l3cxRead::vl53l3cxRead(Function& callback, std::string deviceId, std::string i2cDevice)
	: AsyncWorker(callback), deviceId(deviceId), i2cDevice(i2cDevice) {};

void vl53l3cxRead::Execute() {

	char * deviceId_ref = new char[deviceId.length() + 1];
	std::strcpy (deviceId_ref, deviceId.c_str());

	char * i2c_adapter = new char[i2cDevice.length() + 1];
	std::strcpy (i2c_adapter, i2cDevice.c_str());

	int fd_i2c;
	char filename[20];

	// open i2c
	snprintf(filename, 19, i2c_adapter);
	fd_i2c = open(filename, O_RDWR);
	if (fd_i2c < 0) {
		exit(1);
	}

	if (ioctl(fd_i2c, I2C_SLAVE, I2C_DEVICE) < 0) {
		exit(1);
	}

	VL53LX_MultiRangingData_t MultiRangingData;
	VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
	uint8_t NewDataReady = 0;
	int no_of_object_found = 0, i;
	int stream_count = 0;
	int status = 0;
	int wait_for_sample_counter = 0;

	// update fd_i2c in case it has changed
	deviceMap[deviceId_ref]->VL53LX_SetDeviceAddress(fd_i2c);
	
    while (no_of_object_found == 0 && wait_for_sample_counter < 10) {
    	wait_for_sample_counter++;
		do {
			status = deviceMap[deviceId_ref]->VL53LX_GetMeasurementDataReady(&NewDataReady);
		} while (!NewDataReady);

		if ((!status)&&(NewDataReady!=0)) {
			status = deviceMap[deviceId_ref]->VL53LX_GetMultiRangingData(pMultiRangingData);
			no_of_object_found = pMultiRangingData->NumberOfObjectsFound;
			stream_count = pMultiRangingData->StreamCount;
			if (status == 0) {
				status = deviceMap[deviceId_ref]->VL53LX_ClearInterruptAndStartMeasurement();
			}
		}
	}

	// create JSON string to return results
	result = "{'deviceId':'" + deviceId + "','streamCount':" + std::to_string(stream_count) + ",'numberOfObjectsFound':" + std::to_string(no_of_object_found) + ",'rangeData':[";

	for (i = 0; i < no_of_object_found; i++) {
		result.append("'status':" + std::to_string(pMultiRangingData->RangeData[i].RangeStatus) + ",");
		result.append("'mm':" + std::to_string(pMultiRangingData->RangeData[i].RangeMilliMeter));
		if (i < no_of_object_found - 1) {
			result.append(",");
		}
	}
	
	result.append("]}");


};

void vl53l3cxRead::OnOK() {
	std::string msg = result;
	Callback().Call({Env().Null(), String::New(Env(), msg)});
};