#include "nrf24l01.h"
#include "spi.h"
#include "utility.h"  // delay

#define NRF_CS_HIGH GPIO_SetBits(NRF_CS_GPIO, NRF_CS_PIN)
#define NRF_CS_LOW GPIO_ResetBits(NRF_CS_GPIO, NRF_CS_PIN)
#define NRF_CE_HIGH GPIO_SetBits(NRF_CE_GPIO, NRF_CE_PIN)
#define NRF_CE_LOW GPIO_ResetBits(NRF_CE_GPIO, NRF_CE_PIN)

#define nrfSPI(x) SPI2_Transfer(x)

uint8_t nrfReadReg(uint8_t regAddr) {
    uint8_t returnData;

    NRF_CS_LOW;
    nrfSPI(regAddr);
    returnData = nrfSPI(0x00);
    NRF_CS_HIGH;
    return returnData;
}

uint8_t nrfWriteReg(uint8_t regAddr, uint8_t writeData) {
    uint8_t returnData;

    NRF_CS_LOW;
    returnData = nrfSPI(regAddr);
    nrfSPI(writeData);
    NRF_CS_HIGH;
    return returnData;
}

//SPI读取RXFIFO寄存器的值
uint8_t nrfReadRxData(uint8_t regAddr, uint8_t *rxData, uint8_t dataLen) {
    //寄存器地址//读取数据存放变量//读取数据长度//用于接收
    uint8_t returnData;
    uint8_t i;

    NRF_CS_LOW;
    returnData = nrfSPI(regAddr);
    for (i=0; i<dataLen; i++) {
        rxData[i]=nrfSPI(0x00);
    }
    NRF_CS_HIGH;
    return returnData; 
}


//SPI写入TXFIFO寄存器的值
uint8_t nrfWriteTxData(uint8_t regAddr, uint8_t *txData, uint8_t dataLen) {
    //寄存器地址//写入数据存放变量//读取数据长度//用于发送
    uint8_t returnData;
    uint8_t i;

    NRF_CS_LOW;
    returnData = nrfSPI(regAddr);//写入要写入寄存器的地址
    for (i=0; i<dataLen; i++) {
        nrfSPI(*txData++);
    }
    NRF_CS_HIGH;
    return returnData;
}

///// export /////

uint8_t NRF24L01_Read_Status(void) {
    uint8_t status;
    NRF_CS_LOW;
    status = nrfSPI(NOP);
    NRF_CS_HIGH;
    return status;
}

void nrf24l01Init(void) {
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

    /***下面这些寄存器的配置，在这个程序运行期间不变化，故在初始化芯片时配置。***/
    nrfWriteReg(W_REGISTER + EN_AA, 0x01);     // 使能接收通道0自动应答
    nrfWriteReg(W_REGISTER + EN_RXADDR, 0x01); // 使能接收通道0

    //nrfWriteReg(W_REGISTER+SETUP_AW, 0x03); // Set up address width to 5 bytes
    //nrfWriteReg(W_REGISTER+SETUP_RETR,0x1f); // 自动重发延时等待500us，自动重发15次
    //nrfWriteReg(W_REGISTER+SETUP_RETR,0x3f); // 自动重发延时等待1000us，自动重发15次
    //nrfWriteReg(W_REGISTER+SETUP_RETR,0x5f); // 自动重发延时等待1500us，自动重发15次
    nrfWriteReg(W_REGISTER + SETUP_RETR,0x7f); // 自动重发延时等待2000us，自动重发15次
    //nrfWriteReg(W_REGISTER+SETUP_RETR,0xbf); // 自动重发延时等待3000us，自动重发15次
    //nrfWriteReg(W_REGISTER+SETUP_RETR,0xff); // 自动重发延时等待4000us，自动重发15次

    nrfWriteReg(W_REGISTER+STATUS, 0x7e); //清除RX_DR,TX_DS,MAX_RT状态位
    nrfWriteReg(W_REGISTER+CONFIG, 0x7e); //屏蔽3个中断，CRC使能，2字节CRC校验，上电，PTX
}

// 将24L01设置为发送模式PTX，并发送数据
// 参数如下：
// rfChannel: 无线电频道  取值范围是0-125（即，0x00到0x7d）
// addrWidth: 发送地址宽度
// txAddr: 发送的地址（接收方地址）
// dataWidth: 发送数据的宽度
// txData: 发送的数据
// 例子：
// uint8_t rfChannel = 0x64; //选择无线电频道
// uint8_t rec_addr[3]= { 0x54, 0x53, 0x95 };  //接收方地址
// uint8_t data_to_send[5] = {0x01, 0x02, 0x03, 0x04, 0x05 };　//要发送的数据
// nrfSendData( rfChannel, 3, rec_addr, 5, data_to_send );  //发送
//
// 返回值：
// 255-表示大重试次数达到后仍然未收到ACK，发送失败
// 0到159的一个值，表示发送完成且成功。返回值是自动重传(auto retransmit)的次数，例如：
//     返回0：没有re-transmit，直接发送成功
//     返回1: 表示芯片re-transmit 1次成功
//     返回2：表示芯片re-transmit 2次成功
//     以此类推，最多re-transmit 159次成功。(SEND_MAX_RETRY最大可以设为9）
uint8_t nrfSendData(uint8_t rfChannel, uint8_t addrWidth, uint8_t *txAddr, uint8_t dataWidth, uint8_t *txData) {
    uint8_t ret = 0;
    uint8_t retryCnt = 0;

    /* CE=0; */
    NRF_CE_LOW;
	
    nrfWriteReg( W_REGISTER+SETUP_AW, addrWidth-2 ); //设置地址宽度
    nrfWriteTxData( W_REGISTER+TX_ADDR, txAddr, addrWidth );//写寄存器指令+接收地址使能指令+接收地址+地址宽度
    nrfWriteTxData( W_REGISTER+RX_ADDR_P0, txAddr,addrWidth );//为了应答接收设备，接收通道0地址和发送地址相同

    nrfWriteReg( W_REGISTER+RF_CH, rfChannel ); // 选择射频通道
    nrfWriteReg( W_REGISTER+CONFIG,0x7e ); //屏蔽3个中断，CRC使能，2字节CRC校验，上电，PTX
	
    for (retryCnt=0; retryCnt<= SEND_MAX_RETRY; retryCnt++) {
        nrfWriteTxData(W_TX_PAYLOAD, txData, dataWidth);//写入要发送的数据

        //CE=1;
        NRF_CE_HIGH;
        //delayFor24L01();
        delay_ms(2);
        //CE=0;
        //待发送完毕后转为Standby-1模式
        NRF_CE_LOW;

        do {
            ret = nrfCheckACK();
        } while (ret==100);//检测是否发送完毕
		
        //如果返回值小于等于15，说明本次数据传送成功
        if (ret <= 15) {
            //计算函数返回值，即Retransmission的次数
            //一次性发送成功，返回0，即没有重传(re-transmission)
            //retry第1次时，一次性发送成功，则retransit 16次 （之前15次加本次）
            //一次类推，
            //retry第9次时，（加开始retry之前的一次，共发射了10次），retransmit 15次成功，则，总共retransmit了16*9+15=159次
            //每次try是transmit 16次 （本身1次，加15次retransmit）
            ret += (16*retryCnt);
            break;
        }
    }
    return ret;
}


//设置24L01为接收模式PRX，准备接收数据
//参数如下：
//rfChannel：接收的频道号。取值范围是0－125
//addrWidth: 地址宽度（3-5字节）
//rxAddr：本节点接收的地址
//本节点的接收数据宽度在"nrf24L01Node.h"头文件里面设置
//例：
//   uint8_t myAddr[3] = {53, 69, 160};
//   nrfSetRxMode( 76, 3, myAddr);
//那么节点将在76频道上接收数据。地址宽度为3字节，地址是：53/69/160。
//一旦接收到数据，将触发INT0 (硬件接线提示：IRQ需连接到INT0上）
void nrfSetRxMode(uint8_t rfChannel, uint8_t addrWidth, uint8_t *rxAddr) {
    if (addrWidth !=3 && addrWidth != 5)
        return;
    /* CE=0; */
    NRF_CE_LOW;

    nrfWriteReg(W_REGISTER+SETUP_AW, addrWidth-2); //设置地址宽度
    nrfWriteTxData(W_REGISTER+RX_ADDR_P0, rxAddr, addrWidth); //接收设备接收通道0使用和发送设备相同的发送地址

    nrfWriteReg(W_REGISTER+RF_CH, rfChannel); //设置射频通道
    nrfWriteReg(W_REGISTER+RX_PW_P0, RECEIVE_DATA_WIDTH); //接收通道0选择和发送通道相同有效数据宽度

    //nrfWriteReg( W_REGISTER+RF_SETUP, 0x26 ); // 数据传输率250Kbps，发射功率0dBm
    nrfWriteReg(W_REGISTER+RF_SETUP, 0x27); // 数据传输率250Kbps，发射功率0dBm, LNA_HCURR (Low Noise Amplifier, High Current?)
    //nrfWriteReg( W_REGISTER+RF_SETUP,0x21 ); // 数据传输率250Kbps，发射功率-18dBm, LNA_HCURR (Low Noise Amplifier, High Current?)
    nrfWriteReg(W_REGISTER+CONFIG, 0x3f); //使能RX_DR中断，屏蔽TX_DS和MAX_RT中断，CRC使能，2字节CRC校验，上电，接收模式

    //CE = 1;
    //设为接收模式 PRX
    NRF_CE_HIGH;
}


// 用于检查发送结果(Ack)
// 返回值：100-表示还在发送中
//         255-表示大重发次数达到后仍然未收到ACK，发送失败
//         0到15的一个值，表示发送完成且成功。返回值是自动重发的次数，例如：
//           0：没有重发，直接发送成功
//           1: 重发了1次后成功收到ack
//           2: 重发了2次后成功收到ack
//           以此类推
//           最大值是SETUP_RETR这个寄存器里面设置的最大重发次数。（不会超过15）
uint8_t nrfCheckACK(void) {
    uint8_t status;
    status = nrfReadReg(R_REGISTER+STATUS); // 读取状态寄存器

    if (status & 0x20) { //检查TX_DS位，置位则发送成功
        nrfWriteReg(W_REGISTER+STATUS,0x7f);  // 清除TX_DS标志
        //发送成功后，FIFO自动清空，这里就不用再清了
        //返回自动重发次数
        return nrfReadReg(R_REGISTER+OBSERVE_TX) & 0x0f;
    } else if (status & 0x10) { //检查MAX_RT位，置位则说明最大重发次数达到后仍然未收到ACK，发送失败
        nrfWriteReg(W_REGISTER+STATUS,0x7f);  // 清除MAX_RT标志
        //发送失败，FIFO不会自动清空，必须手动清空
        //关键！！不然会出现意想不到的后果
        //CSN = 0;
        NRF_CS_LOW;
        nrfSPI(FLUSH_TX);
        /* CSN = 1; */
        NRF_CS_HIGH;
        return 255;
    } else { //还在发送中
        return 100;
    }
}

//获取24L01接收到的数据。
//当24L01收到数据触发中断后，调用本方法来取得24L01收到的数据
uint8_t nrfGetReceivedData(uint8_t *dataBuffer, uint8_t len) {
    //static uint8_t dataBuffer[RECEIVE_DATA_WIDTH];
    uint8_t status;

    //读取状态寄存器
    status = nrfReadReg(R_REGISTER + STATUS);
    if (status & 0x40) {  //检查RX_DR位，如果置位，则说明接收到数据
        //CE=0;//进入Standby-I模式

        // 从RX FIFO读取数据
        //nrfReadRxData(R_RX_PAYLOAD,dataBuffer,RECEIVE_DATA_WIDTH);
        nrfReadRxData(R_RX_PAYLOAD, dataBuffer, len);

        //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清除中断标
        //nrfWriteReg(W_REGISTER+STATUS,0xff);
        //网上找到的例子是用0xff来清中断标志，我通过试验发现用0x40也可以清掉RX_DR
        nrfWriteReg(W_REGISTER + STATUS, 0x40);

        //用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住
        //我经过试验发现，不清FIFO的话，发送方有时候会出现收不到ACK的现象
        //CSN=0;
        NRF_CS_LOW;
        nrfSPI(FLUSH_RX);
        //CSN=1;
        NRF_CS_HIGH;
    }
    return len;
}

// Check if nRF24L01 present (send byte sequence, read it back and compare)
// return:
//   0 - looks like an nRF24L01 is online
//   1 - received sequence differs from original
uint8_t nRF24_Check(void) {
    uint8_t txbuf[5] = { 0xA9,0xA9,0xA9,0xA9,0xA9 };
    uint8_t rxbuf[5];
    uint8_t i;

    //nRF24_WriteBuf
    nrfWriteTxData(W_REGISTER | TX_ADDR, txbuf, 5); // Write fake TX address
    //nRF24_ReadBuf
    nrfReadRxData(TX_ADDR, rxbuf, 5); // Try to read TX_ADDR register
    for (i = 0; i < 5; i++)
        if (rxbuf[i] != txbuf[i])
            return 1;
    return 0;
}
