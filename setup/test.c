#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>

int main()
{
	int fdev = open("/dev/i2c-1", O_RDWR); // open i2c bus
	char dev_path[] = "Path";
	if (fdev < 0) {
		fprintf(stderr, "Failed to open I2C interface %s Error: %s\n", dev_path, strerror(errno));
		return -1;
	}

	unsigned char i2c_addr = 0x5A;

	// set slave device address, default MLX is 0x5A
	if (ioctl(fdev, I2C_SLAVE, i2c_addr) < 0) {
		fprintf(stderr, "Failed to select I2C slave device! Error: %s\n", strerror(errno));
		return -1;
	}

	 // enable checksums control
	 if (ioctl(fdev, I2C_PEC, 1) < 0) {
		fprintf(stderr, "Failed to enable SMBus packet error checking, error: %s\n", strerror(errno));
		return -1;
	}
	
			
	// trying to read something from the device unsing SMBus READ request
	i2c_data data;
	char command = 0x06; // command 0x06 is reading thermopile sensor, see datasheet for all commands
	
	// build request structure
	struct i2c_smbus_ioctl_data sdat = {
		.read_write = I2C_SMBUS_READ,
		.command = command,
		.size = I2C_SMBUS_WORD_DATA,
		.data = &data
	};
			
	// do actual request
	if (ioctl(fdev, I2C_SMBUS, &sdat) < 0) {
		fprintf(stderr, "Failed to perform I2C_SMBUS transaction, error: %s\n", strerror(errno));
		return -1;
	}
			
	// calculate temperature in Celsius by formula from datasheet
	double temp = (double) data.word;
	temp = (temp * 0.02)-0.01;
	temp = temp - 273.15;
			
	// print temp
	printf("Tamb = %04.2f\n", temp);
	return 0;
}
