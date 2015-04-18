#ifndef _NRF24L01_REGS_H_
#define _NRF24L01_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************命令寄存器***************************/
#define NRF24L01_R_REGISTER 0x00//读取配置寄存器
#define NRF24L01_W_REGISTER 0x20//写配置寄存器
#define NRF24L01_R_RX_PAYLOAD 0x61//读取RX有效数据
#define NRF24L01_W_TX_PAYLOAD 0xa0//写TX有效数据
#define NRF24L01_FLUSH_TX 0xe1//清除TXFIFO寄存器
#define NRF24L01_FLUSH_RX 0xe2//清除RXFIFO寄存器
#define NRF24L01_REUSE_TX_PL 0xe3//重新使用上一包有效数据
#define NRF24L01_NOP 0xff//空操作
#define NRF24L01_W_TX_PAYLOAD_NOACK 0xB0// Used in TX mode, Disable AUTOACK on this specific packet

/******************寄存器地址****************************/
#define NRF24L01_CONFIG 0x00//配置寄存器
#define NRF24L01_EN_AA 0x01//使能自动应答
#define NRF24L01_EN_RXADDR 0x02//接收通道使能0-5个通道
#define NRF24L01_SETUP_AW 0x03//设置数据通道地址宽度3-5
#define NRF24L01_SETUP_RETR 0x04//建立自动重发
#define NRF24L01_RF_CH 0x05//射频通道设置
#define NRF24L01_RF_SETUP 0x06//射频寄存器
#define NRF24L01_STATUS 0x07//状态寄存器
#define NRF24L01_OBSERVE_TX 0x08//发送检测寄存器
#define NRF24L01_CD 0x09//载波
#define NRF24L01_RX_ADDR_P0 0x0a//数据通道0接收地址
#define NRF24L01_RX_ADDR_P1 0x0b//数据通道1接收地址
#define NRF24L01_RX_ADDR_P2 0x0c//数据通道2接收地址
#define NRF24L01_RX_ADDR_P3 0x0d//数据通道3接收地址
#define NRF24L01_RX_ADDR_P4 0x0e//数据通道4接收地址
#define NRF24L01_RX_ADDR_P5 0x0f//数据通道5接收地址
#define NRF24L01_TX_ADDR 0x10//发送地址
#define NRF24L01_RX_PW_P0 0x11//P0通道数据宽度设置
#define NRF24L01_RX_PW_P1 0x12//P1通道数据宽度设置
#define NRF24L01_RX_PW_P2 0x13//P2通道数据宽度设置
#define NRF24L01_RX_PW_P3 0x14//P3通道数据宽度设置
#define NRF24L01_RX_PW_P4 0x15//P4通道数据宽度设置
#define NRF24L01_RX_PW_P5 0x16//P5通道数据宽度设置
#define NRF24L01_FIFO_STATUS 0x17//FIFO状态寄存器
#define NRF24L01_FEATURE 0x1D// Additional features register, needed to enable the additional commands

#ifdef __cplusplus
}
#endif

#endif /* _NRF24L01_REGS_H_ */
