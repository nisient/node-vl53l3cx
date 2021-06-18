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

#include "./src/vl53lx_class.h"

#define I2C_ADAPTER "/dev/i2c-3"
#define I2C_DEVICE  0x29

std::map<std::string, VL53LX*> deviceMap;

void initSensor() {

	int fd_i2c;
	char filename[20];
	std::string deviceId_ref = "abcdefgh";

	fprintf(stdout, "initSensor\n");

	// open i2c
	snprintf(filename, 19, I2C_ADAPTER);
	fd_i2c = open(filename, O_RDWR);
	if (fd_i2c < 0) {
		exit(1);
	}

	if (ioctl(fd_i2c, I2C_SLAVE, I2C_DEVICE) < 0) {
		exit(1);
	}

	deviceMap.insert(std::make_pair(deviceId_ref, new VL53LX()));
	// added since fd_i2c not passed to constructor now
	deviceMap[deviceId_ref]->VL53LX_SetDeviceAddress(fd_i2c);
	// added to prevent need to power cycle/hardware reset device
	deviceMap[deviceId_ref]->VL53LX_software_reset();
	deviceMap[deviceId_ref]->InitSensor(fd_i2c);
	deviceMap[deviceId_ref]->VL53LX_StartMeasurement(); 

	// close i2c
	close(fd_i2c);

	return;

}

void readSensor() {

	int fd_i2c;
	char filename[20];
	std::string deviceId_ref = "abcdefgh";

	VL53LX_MultiRangingData_t MultiRangingData;
	VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
	uint8_t NewDataReady = 0;
	int no_of_object_found = 0, i;
	int status = 0;

	fprintf(stdout, "readSensor\n");

	// open i2c
	snprintf(filename, 19, I2C_ADAPTER);
	fd_i2c = open(filename, O_RDWR);
	if (fd_i2c < 0) {
		exit(1);
	}

	if (ioctl(fd_i2c, I2C_SLAVE, I2C_DEVICE) < 0) {
		exit(1);
	}

	// update fd_i2c in case it has changed
	deviceMap[deviceId_ref]->VL53LX_SetDeviceAddress(fd_i2c);
	
    while (status == 0) {
		do {
			status = deviceMap[deviceId_ref]->VL53LX_GetMeasurementDataReady(&NewDataReady);
		} while (!NewDataReady);

		if ((!status) && (NewDataReady! = 0)) {
			status = deviceMap[deviceId_ref]->VL53LX_GetMultiRangingData(pMultiRangingData);
			no_of_object_found=pMultiRangingData->NumberOfObjectsFound;
			fprintf(stdout, "Count=%u, #Objs=%u\n", pMultiRangingData->StreamCount, no_of_object_found);
			for (i = 0; i < no_of_object_found; i++)
			{
				fprintf(stdout, "status=");
				fprintf(stdout, "%u", pMultiRangingData->RangeData[i].RangeStatus);
				fprintf(stdout, ", D=");
				fprintf(stdout, "%u", pMultiRangingData->RangeData[i].RangeMilliMeter);
				fprintf(stdout, "mm\n");
			}
			if (status==0) {
				status = deviceMap[deviceId_ref]->VL53LX_ClearInterruptAndStartMeasurement();
			}
		}
	} // end while loop
	
	return;

}

int main () {

	initSensor();
	readSensor();
	
}

