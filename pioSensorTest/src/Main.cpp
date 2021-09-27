#include <iostream>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define SPI_PORT spi0
#define MISO 1
#define CS   2
#define SCLK 4
#define MOSI 5

using namespace std;
int main(){
    stdio_init_all();
    uint8_t buffer[6], reg;

    reg = 0x88 | 0x80;
    gpio_put(CS, 0);
    spi_write_blocking(SPI_PORT, &reg, 1);
    spi_read_blocking(SPI_PORT, 0, buffer, 6);
    gpio_put(CS, 1);
    cout << buffer << reg << endl;
    printf("SPI READ");
    sleep_ms(500);
}