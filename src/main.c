/**
 * Playing with i2c to read the battery status.
 */

#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DATA_ADDRESS 0x34
#define REG_PERCENTAGE 0xb9
#define REG_CHARGE 0x01
#define REG_ADC 0x82
#define UNINITIALIZED_VALUE 0x7F
#define ENABLE_ADC 0xC3

#define TO_STR_BUFFER_LEN 64


/**
 * Basic battery status.
 */
typedef struct _batt_status
{
    int percentage;
    bool charging;
} batt_status;


/**
 * Returns a string representation of the status.
 */
const char * batt_status_to_string(const batt_status *status);


/**
 * Leaves in status the current values.
 * Returns a non-zero value in case of error.
 */
int get_batt_status(batt_status *status);


int main()
{
    batt_status status = {0, false};
    if (!get_batt_status(&status)) {
        printf("%s\n", batt_status_to_string(&status));
    } else {
        printf("An error happened, please check the previous output\n");
        return 1;
    }
}


int get_batt_status(batt_status *status)
{
    const char *file_name = "/dev/i2c-0";
    int file;

    if ((file = open(file_name, O_RDWR)) < 0) {
        perror("Opening the device");
        return 1;
    }

    if (ioctl(file, I2C_SLAVE_FORCE, DATA_ADDRESS) < 0) {
        perror("Setting the device address");
        close(file);
        return 1;
    }

    status->percentage = i2c_smbus_read_byte_data(file, REG_PERCENTAGE);
    if (status->percentage == UNINITIALIZED_VALUE) {
        /* Value not initialized, enable the ADC for voltage and current */
        i2c_smbus_write_byte_data(file, REG_ADC, ENABLE_ADC);
    }
    status->charging = i2c_smbus_read_byte_data(file, REG_CHARGE) & 0x40;
    close(file);
    return 0;
}


const char * batt_status_to_string(const batt_status *status)
{
    static char to_string_buffer[TO_STR_BUFFER_LEN];
    strncpy(to_string_buffer, "NA", TO_STR_BUFFER_LEN);

    if (status->percentage != UNINITIALIZED_VALUE) {
        snprintf(to_string_buffer, TO_STR_BUFFER_LEN, "%02d%%%s", status->percentage, status->charging? "+": "");
    }
    return to_string_buffer;
}
