/**
 * This is the native c++ node.js module interface for the STMicro VL53L3CX ToF sensor.
 *
 * @module src/vl53l3cx.cpp
 * @version 0.0.1
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

Napi::String initSensor(const Napi::CallbackInfo& info) {

	Napi::Env env = info.Env();

	std::string deviceId = info[0].ToString();
	char * deviceId_ref = new char[deviceId.length() + 1];
	std::strcpy (deviceId_ref, deviceId.c_str());
	std::string i2cDevice = info[1].ToString();
	char * i2c_adapter = new char[i2cDevice.length() + 1];
	std::strcpy (i2c_adapter, i2cDevice.c_str());

	int fd_i2c;
	char filename[20];

	fprintf(stdout, "initSensor\n");
	std::cout << i2cDevice << "\n";
	
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
	VL53LX sensor_vl53lx_sat(fd_i2c);
	deviceMap[deviceId_ref] = &sensor_vl53lx_sat;
	sensor_vl53lx_sat.begin();
	// added to prevent need to power cycle/hardware reset device
	sensor_vl53lx_sat.VL53LX_software_reset();
	sensor_vl53lx_sat.InitSensor(fd_i2c);
	sensor_vl53lx_sat.VL53LX_StartMeasurement(); 

	// close i2c
	close(fd_i2c);

//	return sensor_vl53lx_sat;
	return Napi::String::New(env, deviceId);

}

//void readSensor(VL53LX sensor_vl53lx_sat) {
Napi::Boolean readSensor(const Napi::CallbackInfo& info) {

	Napi::Env env = info.Env();

	std::string deviceId = info[0].ToString();
	char * deviceId_ref = new char[deviceId.length() + 1];
	std::strcpy (deviceId_ref, deviceId.c_str());
	std::string i2cDevice = info[1].ToString();
	char * i2c_adapter = new char[i2cDevice.length() + 1];
	std::strcpy (i2c_adapter, i2cDevice.c_str());

	int fd_i2c;
	char filename[20];
	VL53LX_MultiRangingData_t MultiRangingData;
	VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
	uint8_t NewDataReady = 0;
	int no_of_object_found = 0, j;
	int status = 0;

	fprintf(stdout, "readSensor\n");

	// open i2c
	snprintf(filename, 19, i2c_adapter);
	fd_i2c = open(filename, O_RDWR);
	if (fd_i2c < 0) {
		exit(1);
	}

	if (ioctl(fd_i2c, I2C_SLAVE, I2C_DEVICE) < 0) {
		exit(1);
	}
	fprintf(stdout, "readSensor 1\n");

	// update i2c fd in constructor
	deviceMap[deviceId_ref]->VL53LX_SetDeviceAddress(fd_i2c);
	fprintf(stdout, "readSensor 2\n");
	
    while (status == 0) {
		do {
			status = deviceMap[deviceId_ref]->VL53LX_GetMeasurementDataReady(&NewDataReady);
//			fprintf(stdout, "NewDataReady %x\n", NewDataReady);
		} while (!NewDataReady);

		if((!status)&&(NewDataReady!=0)) {
			status = deviceMap[deviceId_ref]->VL53LX_GetMultiRangingData(pMultiRangingData);
			no_of_object_found=pMultiRangingData->NumberOfObjectsFound;
			fprintf(stdout, "Count=%u, #Objs=%u\n", pMultiRangingData->StreamCount, no_of_object_found);
			for(j=0;j<no_of_object_found;j++)
			{
				fprintf(stdout, "status=");
				fprintf(stdout, "%u", pMultiRangingData->RangeData[j].RangeStatus);
				fprintf(stdout, ", D=");
				fprintf(stdout, "%u", pMultiRangingData->RangeData[j].RangeMilliMeter);
				fprintf(stdout, "mm\n");
			}
			if (status==0) {
				status = deviceMap[deviceId_ref]->VL53LX_ClearInterruptAndStartMeasurement();
			}
		}
	} // end while loop
	
//	return;
	return Napi::Boolean::New(env, true);

}

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

// register vl53l3cx module which calls Init method
NODE_API_MODULE(vl53l3cx, Init)
