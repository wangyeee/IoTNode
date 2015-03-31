#include "nrf24l01.h"
#include "spi.h"
#include "utility.h"  // delay

#define NRF_CS_HIGH GPIO_SetBits(NRF_CS_GPIO, NRF_CS_PIN)
#define NRF_CS_LOW GPIO_ResetBits(NRF_CS_GPIO, NRF_CS_PIN)
#define NRF_CE_HIGH GPIO_SetBits(NRF_CE_GPIO, NRF_CE_PIN)
#define NRF_CE_LOW GPIO_ResetBits(NRF_CE_GPIO, NRF_CE_PIN)

#define nRF24_ReadWrite(x) SPI2_Transfer(x)

uint8_t nRF24_TX_addr[nRF24_TX_ADDR_WIDTH] = {'W','a','l','k','T'};
uint8_t nRF24_RX_addr[nRF24_RX_ADDR_WIDTH] = {'W','o','l','k','T'};

// GPIO and SPI initialization
void nRF24_init() {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable CS CE GPIO clock */
    RCC_AHBPeriphClockCmd(NRF_CE_RCC_CLOCK | NRF_CS_RCC_CLOCK | NRF_INT_RCC_CLOCK, ENABLE);

    /* Configure Chip select (CS) pin */
    GPIO_InitStructure.GPIO_Pin = NRF_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(NRF_CS_GPIO, &GPIO_InitStructure);

    /* Configure CE pin */
    GPIO_InitStructure.GPIO_Pin = NRF_CE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(NRF_CE_GPIO, &GPIO_InitStructure);

    /* Configure INT pin */
    GPIO_InitStructure.GPIO_Pin = NRF_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(NRF_INT_GPIO, &GPIO_InitStructure);

    // Dis-select chip
    NRF_CS_HIGH;
    // Standby mode
    NRF_CE_LOW;
}

// Write new value to register
// input:
//   reg - register number
//   value - new value
// output: nRF24L01 status
uint8_t nRF24_RWReg(uint8_t reg, uint8_t value) {
    uint8_t status;

    NRF_CS_LOW;
    status = nRF24_ReadWrite(reg); // Select register
    nRF24_ReadWrite(value); // Write value to register
    NRF_CS_HIGH;

    return status;
}

// Read nRF24L01 register
// input:
//   reg - register number
// output: register value
uint8_t nRF24_ReadReg(uint8_t reg) {
    uint8_t value;

    NRF_CS_LOW;
    nRF24_ReadWrite(reg);
    value = nRF24_ReadWrite(0x00);
    NRF_CS_HIGH;

    return value;
}

// Get data from nRF24L01 into buffer
// input:
//   reg - register number
//   pBuf - pointer to buffer
//   count - bytes count
// output: nRF24L01 status
uint8_t nRF24_ReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t count) {
    uint8_t status,i;

    NRF_CS_LOW;
    status = nRF24_ReadWrite(reg);
    for (i = 0; i < count; i++)
        pBuf[i] = nRF24_ReadWrite(0);
    NRF_CS_HIGH;

    return status;
}

// Send buffer to nRF24L01
// input:
//   reg - register number
//   pBuf - pointer to buffer
//   count - bytes count
// output: nRF24L01 status
uint8_t nRF24_WriteBuf(uint8_t reg, uint8_t *pBuf, uint8_t count) {
    uint8_t status,i;

    NRF_CS_LOW;
    status = nRF24_ReadWrite(reg);
    for (i = 0; i < count; i++) nRF24_ReadWrite(*pBuf++);
    NRF_CS_HIGH;

    return status;
}

// Check if nRF24L01 present (send byte sequence, read it back and compare)
// return:
//   0 - looks like an nRF24L01 is online
//   1 - received sequence differs from original
uint8_t nRF24_Check(void) {
    uint8_t txbuf[5] = { 0xA9,0xA9,0xA9,0xA9,0xA9 };
    uint8_t rxbuf[5];
    uint8_t i;

    nRF24_WriteBuf(nRF24_CMD_WREG | nRF24_REG_TX_ADDR,txbuf,5); // Write fake TX address
    nRF24_ReadBuf(nRF24_REG_TX_ADDR,rxbuf,5); // Try to read TX_ADDR register
    for (i = 0; i < 5; i++)
        if (rxbuf[i] != txbuf[i])
            return 1;
    return 0;
}

// Put nRF24L01 in RX mode
void nRF24_RXMode(uint8_t RX_PAYLOAD) {
    NRF_CE_LOW;
    nRF24_WriteBuf(nRF24_CMD_WREG | nRF24_REG_RX_ADDR_P0,nRF24_RX_addr,nRF24_RX_ADDR_WIDTH); // Set static RX address
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_EN_AA,0x01); // Enable ShockBurst for data pipe 0
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_EN_RXADDR,0x01); // Enable data pipe 0
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_RF_CH,0x6E); // Set frequency channel 110 (2.510MHz)
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_RX_PW_P0,RX_PAYLOAD); // Set RX payload length (10 bytes)
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_RF_SETUP,0x06); // Setup: 1Mbps, 0dBm, LNA off
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_CONFIG,0x0F); // Config: CRC on (2 bytes), Power UP, RX/TX ctl = PRX
    NRF_CE_HIGH;
    // Delay_us(10); // Hold CE high at least 10us
}

// Put nRF24L01 in TX mode
void nRF24_TXMode(void) {
    NRF_CE_LOW;
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_CONFIG,0x02); // Config: Power UP
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_EN_AA,0x01); // Enable ShockBurst for data pipe 0
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_SETUP_RETR,0x1A); // Auto retransmit: wait 500us, 10 retries
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_RF_CH,0x6E); // Set frequency channel 110 (2.510MHz)
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_RF_SETUP,0x06); // Setup: 1Mbps, 0dBm, LNA off
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_CONFIG,0x0E); // Config: CRC on (2 bytes), Power UP, RX/TX ctl = PTX
}

// Check if data is available for reading
// return:
//   0 -> no data
//   1 -> RX_DR is set or some bytes present in FIFO
uint8_t nRF24_DataReady(void) {
    uint8_t status;

    status = nRF24_ReadReg(nRF24_REG_STATUS);
    if (status & nRF24_MASK_RX_DR) return 1;

    // Checking RX_DR isn't good enough, there's can be some data in FIFO
    status = nRF24_ReadReg(nRF24_REG_FIFO_STATUS);

    return (status & nRF24_FIFO_RX_EMPTY) ? 0 : 1;
}

uint8_t nRF24_RXPacket(uint8_t* pBuf, uint8_t RX_PAYLOAD) {
    uint8_t status;

    status = nRF24_ReadReg(nRF24_REG_STATUS); // Read status register
    if (status & nRF24_MASK_RX_DR) {
    	if ((status & 0x0E) == 0) {
            // pipe 0
            nRF24_ReadBuf(nRF24_CMD_R_RX_PAYLOAD,pBuf,RX_PAYLOAD); // read received payload from RX FIFO buffer
    	}
        nRF24_ReadWrite(nRF24_CMD_FLUSH_RX); // Flush RX FIFO buffer
        nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_STATUS,status | 0x70); // Clear RX_DR, TX_DS, MAX_RT flags
        //return nRF24_MASK_RX_DR;
        return status;
    }

    // Some banana happens
    nRF24_ReadWrite(nRF24_CMD_FLUSH_RX); // Flush RX FIFO buffer
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_STATUS,status | 0x70); // Clear RX_DR, TX_DS, MAX_RT flags
    return status;
}

// Send data packet
// input:
//   pBuf - buffer with data to send
// return:
//   nRF24_MASK_MAX_RT - if transmit failed with maximum auto retransmit count
//   nRF24_MAX_TX_DS - if transmit succeed
//   contents of STATUS register otherwise
uint8_t nRF24_TXPacket(uint8_t * pBuf, uint8_t TX_PAYLOAD) {
    uint8_t status;

    NRF_CE_LOW;
    nRF24_WriteBuf(nRF24_CMD_WREG | nRF24_REG_TX_ADDR,nRF24_TX_addr,nRF24_TX_ADDR_WIDTH); // Set static TX address
    nRF24_WriteBuf(nRF24_CMD_WREG | nRF24_REG_RX_ADDR_P0,nRF24_RX_addr,nRF24_RX_ADDR_WIDTH); // Set static RX address for auto ack
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_EN_AA,0x01); // Enable auto acknowledgement for data pipe 0
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_SETUP_RETR,0x1A); // Automatic retransmission: wait 500us, 10 retries
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_RF_CH,0x6E); // Set frequency channel 110 (2.510MHz)
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_RF_SETUP,0x07); // Setup: 1Mbps, 0dBm, LNA on
    nRF24_WriteBuf(nRF24_CMD_W_TX_PAYLOAD,pBuf,TX_PAYLOAD); // Write specified buffer to FIFO
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_CONFIG,0x0E); // Config: CRC on (2 bytes), Power UP, RX/TX ctl = PTX
    NRF_CE_HIGH; // CE pin high => Start transmit
    // Delay_us(10); // Must hold CE at least 10us
    //while(PB_IDR_bit.IDR2 != 0); // Wait for IRQ from nRF24L01
    NRF_CE_LOW;
    status = nRF24_ReadReg(nRF24_REG_STATUS); // Read status register
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_STATUS,status | 0x70); // Clear RX_DR, TX_DS, MAX_RT flags
    if (status & nRF24_MASK_MAX_RT) {
        // Auto retransmit counter exceeds the programmed maximum limit. FIFO is not removed.
        nRF24_RWReg(nRF24_CMD_FLUSH_TX,0xFF); // Flush TX FIFO buffer
        return nRF24_MASK_MAX_RT;
    };
    if (status & nRF24_MASK_TX_DS) {
        // Transmit ok
        nRF24_RWReg(nRF24_CMD_FLUSH_TX,0xFF); // Flush TX FIFO buffer
        return nRF24_MASK_TX_DS;
    }

    // Some banana happens
    return status;
}

// Clear all IRQ flags
void nRF24_ClearIRQFlags(void) {
    uint8_t status;

    status = nRF24_ReadReg(nRF24_REG_STATUS);
    nRF24_RWReg(nRF24_CMD_WREG | nRF24_REG_STATUS,status | 0x70); // Clear RX_DR, TX_DS, MAX_RT flags
}
