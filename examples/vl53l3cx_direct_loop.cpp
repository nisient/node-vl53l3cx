#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <errno.h>
#include <string.h>
#include <assert.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "./vl53lx_class.h"

#define I2C_ADAPTER "/dev/i2c-3"
//#define I2C_DEVICE  0x52
#define I2C_DEVICE  0x29

VL53LX initSensor() {

	int fd_i2c;
	char filename[20];

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

	// constructor
	VL53LX sensor_vl53lx_sat(fd_i2c);

	sensor_vl53lx_sat.begin();
	// added to prevent need to power cycle/hardware reset device
	sensor_vl53lx_sat.VL53LX_software_reset();
	sensor_vl53lx_sat.InitSensor(fd_i2c);
	sensor_vl53lx_sat.VL53LX_StartMeasurement(); 

	// close i2c
	close(fd_i2c);

	return sensor_vl53lx_sat;

}

void readSensor(VL53LX sensor_vl53lx_sat) {

	int fd_i2c;
	char filename[20];
	VL53LX_MultiRangingData_t MultiRangingData;
	VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
	uint8_t NewDataReady = 0;
	int no_of_object_found = 0, j;
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

	// update i2c fd in constructor
	sensor_vl53lx_sat.VL53LX_SetDeviceAddress(fd_i2c);
	
    while (status == 0) {
		do {
			status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);
//			fprintf(stdout, "NewDataReady %x\n", NewDataReady);
		} while (!NewDataReady);

		if((!status)&&(NewDataReady!=0)) {
			status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
			no_of_object_found=pMultiRangingData->NumberOfObjectsFound;
			fprintf(stdout, "Count=%u, #Objs=%u\n", pMultiRangingData->StreamCount, no_of_object_found);
			for(j=0;j<no_of_object_found;j++)
			{
				fprintf(stdout, "status=");
				fprintf(stdout, "%u", pMultiRangingData->RangeData[j].RangeStatus);
				fprintf(stdout, ", D=");
				fprintf(stdout, "%u", pMultiRangingData->RangeData[j].RangeMilliMeter);
				fprintf(stdout, "mm\n");
//				fprintf(stdout, ", Signal=");
//				fprintf(stdout, "%f", (float)pMultiRangingData->RangeData[j].SignalRateRtnMegaCps/65536.0);
//				fprintf(stdout, " Mcps, Ambient=");
//				fprintf(stdout, "%f", (float)pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps/65536.0);
//				fprintf(stdout, " Mcps\n");
			}
			if (status==0) {
				status = sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
			}
		}
	} // end while loop
	
	return;

}

int main () {

	VL53LX sensor_vl53lx_sat = initSensor();
	readSensor(sensor_vl53lx_sat);
	
}

