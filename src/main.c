/**
 * Playing with i2c to read the battery status.
 */

#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DATA_ADDRESS 0x34
#define REG_PERCENTAGE 0xb9
#define REG_CHARGE 0x01


int main()
{
    int file;
    char *file_name = "/dev/i2c-0";

    if ((file = open(file_name, O_RDWR)) < 0) {
        perror("Opening the device");
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE_FORCE, DATA_ADDRESS) < 0) {
        perror("Setting the device address");
        close(file);
        exit(1);
    }

    u_int8_t reg = REG_PERCENTAGE;
    int32_t batt_value = i2c_smbus_read_word_data(file, reg);
    reg = REG_CHARGE;
    int32_t charge_value = i2c_smbus_read_word_data(file, reg);
    bool charging = charge_value & 0x40;
    printf("%02d%%%c\n", batt_value, charging? '+': '\0');
    close(file);
    printf("Done!\n");
}
