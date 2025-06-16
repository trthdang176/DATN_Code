#ifndef __APP_EEPROM_DATA_H__
#define __APP_EEPROM_DATA_H__

const char data_program1[] = "Program 1,744051,5";
const char data_program2[] = "Program 2,744051,2";
const char data_program3[] = "Program 3,744051,3";
const char data_program4[] = "Program 4,744051,5";

const char wifi_info[] = "dang123,12345678";

const char data_744051[] = 
    "16|8|"
    "|"
    "Bidirectional 8-channel analog multiplexer/demultiplexer"
    "|"
    "A4\nA6\nCOMA\nA7\nA5\nE\nVEE\nGND\nS2\nS1\nS0\nA3\nA0\nA1\nA2\nVCC\n"
    "|"
    "11L1100G0001011V\n"
    "11L1100G0011101V\n"
    "11L1100G0101110V\n"
    "11L1100G0110111V\n"
    "01L1100G1001111V\n"
    "11L1000G1011111V\n"
    "10L1100G1101111V\n"
    "11L0100G1111111V\n";

const char data_7400[] = 
    "14|4|"
    "|"
    "four independent, 2-input NAND gates"
    "|"
    "A1\nB1\nY1\nA2\nB2\nY2\nGND\nY3\nB3\nA3\nY4\nA4\nB4\nVCC\n"
    "|"
    "00H00HGH00H00V\n"
    "10H10HGH10H10V\n"
    "01H01HGH01H01V\n"
    "11L11LGL11L11V\n";

const char data_74139[] = 
    "16|6|"
    "|"
    "Dual 2-to-4 line decoder/demultiplexer"
    "|"
    "1E\n1A0\n1A1\n1Y0\n1Y1\n1Y2\n1Y3\nGND\n2Y3\n2Y2\n2Y1\n2Y0\n2A1\n2A0\n2E\nVCC\n"
    "|"
    "101HHHHGHHHH011V\n"
    "110HHHHGHHHH101V\n"
    "000LHHHGHHHL000V\n"
    "010HLHHGHHLH010V\n"
    "001HHLHGHLHH100V\n"
    "011HHHLGLHHH110V\n";

const char data_7408[] = 
    "14|4|"
    "|"
    "four independent 2-input AND gates"
    "|"
    "A1\nB1\nY1\nA2\nB2\nY2\nGND\nY3\nB3\nA3\nY4\nA4\nB4\nVCC\n"
    "|"
    "00L00LGL00L00V\n"
    "01L01LGL01L01V\n"
    "10L10LGL10L10V\n"
    "11H11HGH11H11V\n";

const char data_74157[] = 
    "16|10|"
    "|"
    "Quad 2 input multiplexers with common select"
    "|"
    "S\n1I0\n1I1\n1Y\n2I0\n2I1\n2Y\nGND\n3Y\n3I1\n3I0\n4Y\n4I1\n4I0\nE\nVCC\n"
    "|"
    "111H11HGH11H110V\n"
    "101L01LGL10L101V\n"
    "000L00LGL00L000V\n"
    "001L01LGL10L100V\n"
    "010H10HGH01H010V\n"
    "011H11HGH11H110V\n"
    "100L00LGL00L000V\n"
    "110L10LGL01L010V\n"
    "101H01HGH10H100V\n"
    "010L10LGL01L011V\n";

const char data_40175[] = 
    "16|4|"
    "|"
    "Quad positive edge triggered D type flip flops with individual D inputs"
    "|"
    "!MR\nQ0\n!Q0\nD0\nD1\n!Q1\nQ1\nGND\nCP\nQ2\n!Q2\nD2\nD3\n!Q3\nQ3\nVCC\n"
    "|"
    "1HL11LHGUHL11LHV\n"
    "1LH00HLGULH00HLV\n"
    "0LH00HLGULH00HLV\n"
    "0LH11HLGULH11HLV\n";

const char data_40174[] = 
    "16|4|"
    "|"
    "Hex positive-edge D type flip flops with common clock & master reset"
    "|"
    "!MR\nQ0\nD0\nD1\nQ\nD1\nQ2\nGND\nCP\nQ3\nD3\nQ4\nD4\nD5\nQ5\nVCC\n"
    "|"
    "1H11H1HGCH1H11HV\n"
    "1L00L0LGCL0L00LV\n"
    "0L00L0LGCL0L00LV\n"
    "0L11L1LGCL1L11LV\n";


const char data_4027[] = 
    "16|5|"
    "|"
    "Dual positive edge triggered JK flip flops with independent asynchronous set/clear"
    "|"
    "2Q\n2!Q\n2CP\n2CD\n2K\n2J\n2SD\nGND\n1SD\n1J\n1K\n1CD\n1CP\n1!Q\n1Q\nVCC\n"
    "|"
    "HLC0001G1000CLHV\n"
    "LHC1000G0001CHLV\n"
    "HLC0010G0100CLHV\n"
    "HLC0000G0000CLHV\n"
    "LHC0100G0010CHLV\n";


#endif /* __APP_EEPROM_DATA_H__ */
