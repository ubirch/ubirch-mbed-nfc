#include "mbed.h"
#include "inc/ntag_defines.h"
#include "inc/ntag_driver.h"
#include "inc/HAL_timer_driver.h"
#include "inc/HAL_I2C_driver.h"
#include "inc/ntag_bridge.h"

#define NFC_SDA I2C_SDA
#define NFC_SCL I2C_SCL

I2C nfc_i2c(NFC_SDA, NFC_SCL);
DigitalInOut led1(LED1);
DigitalOut extPower(PTC8);


void dbg_dump(const char *prefix, const uint8_t *b, size_t size) {
    for (int i = 0; i < size; i += 16) {
        if (prefix && strlen(prefix) > 0) printf("%s %06x: ", prefix, i);
        for (int j = 0; j < 16; j++) {
            if ((i + j) < size) printf("%02x", b[i + j]); else printf("  ");
            if ((j + 1) % 2 == 0) putchar(' ');
        }
        putchar(' ');
        for (int j = 0; j < 16 && (i + j) < size; j++) {
            putchar(b[i + j] >= 0x20 && b[i + j] <= 0x7E ? b[i + j] : '.');
        }
        printf("\r\n");
    }
}

/*
 * HAL Function prototype description
 */
#ifdef __cplusplus
extern "C" {
#endif

void HAL_Timer_delay_ms(uint32_t ms){
    wait_ms(ms);
}

uint16_t uNFC_send(HAL_I2C_HANDLE_T i2cbus, uint8_t address, const uint8_t *bytes, uint8_t len) {
    int ret = nfc_i2c.write(address, (char *) bytes, len);
    return (uint16_t) ret;
}

uint16_t uNFC_recv(HAL_I2C_HANDLE_T i2cbus, uint8_t address, uint8_t *bytes, uint8_t len) {
    int ret = nfc_i2c.read(address, (char *) bytes, len);
    return (uint16_t) ret;
}
#ifdef __cplusplus
}
#endif


void led_thread(void const *args) {
    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}

osThreadDef(led_thread, osPriorityNormal, DEFAULT_STACK_SIZE);

int main() {

    osThreadCreate(osThread(led_thread), NULL);

    printf("Lets write to the NFC\r\n");

    extPower.write(1);

    NTAG_HANDLE_T *ntag = new NTAG_HANDLE_T;

    uint8_t sram_buf[NTAG_MEM_SIZE_SRAM];
    memset(sram_buf, 0, NTAG_MEM_SIZE_SRAM);

    uint8_t rxbuffer[4 * NTAG_BLOCK_SIZE];

    uint16_t index = 0;
    uint8_t reg = 0;

    NTAG_DisableSRAM(ntag);

    /* Header of the NDEF Message
     * record_len     = message to print + 3
     * message_length = record_len + 4
     */
    sram_buf[index++] = 0x03;
    sram_buf[index++] = 0x0D; // Message length
    sram_buf[index++] = 0xd1;

    sram_buf[index++] = 0x01; // Text Record
    sram_buf[index++] = 0x09; // Record length
    sram_buf[index++] = 0x54;
    sram_buf[index++] = 0x02;

    sram_buf[index++] = 0x65;
    sram_buf[index++] = 0x6e;

    sram_buf[index++] = 'U';
    sram_buf[index++] = 'B';
    sram_buf[index++] = 'I';
    sram_buf[index++] = 'R';
    sram_buf[index++] = 'C';
    sram_buf[index++] = 'H';

    sram_buf[index++] = 0xFE;
    sram_buf[index++] = 0x00;

    /*
     * write to eeprom stuff goes here
     */
    NTAG_WriteBytes(ntag, NTAG_MEM_START_ADDR_USER_MEMORY,
                    sram_buf, (uint16_t)(index+1));
    NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NS_REG, &reg);
    while (reg & NTAG_NS_REG_MASK_I2C_IF_ON_OFF) {
        NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NS_REG, &reg);
    }

    /*
     * read from eeprom stuff goes here
     */
    NTAG_ReadBytes(ntag, NTAG_MEM_START_ADDR_USER_MEMORY, rxbuffer,
                  4 * NTAG_BLOCK_SIZE);
    dbg_dump("NTAG", rxbuffer, index);

}


