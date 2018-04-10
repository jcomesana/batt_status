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


/**
 * Basic battery status.
 */
typedef struct _batt_status
{
    int percentage;
    bool charging;
} batt_status;

/**
 * Leaves in status the current values.
 * Returns a non-zero value in case of error.
 */
int get_batt_status(batt_status *status);


int main()
{
    batt_status status;
    if (!get_batt_status(&status)) {
        printf("%02d%%%s\n", status.percentage, status.charging? "+": "");
    } else {
        printf("An error happened, please check the previous output\n");
        return 1;
    }
}


int get_batt_status(batt_status *status)
{
    int file;
    char *file_name = "/dev/i2c-0";

    if ((file = open(file_name, O_RDWR)) < 0) {
        perror("Opening the device");
        return 1;
    }

    if (ioctl(file, I2C_SLAVE_FORCE, DATA_ADDRESS) < 0) {
        perror("Setting the device address");
        close(file);
        return 1;
    }

    u_int8_t reg = REG_PERCENTAGE;
    int32_t value = i2c_smbus_read_word_data(file, reg);
    status->percentage = value;
    reg = REG_CHARGE;
    value = i2c_smbus_read_word_data(file, reg);
    status->charging = value & 0x40;
    close(file);
    return 0;
}
