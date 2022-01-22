# Post Beyond Light Package Format

Mhm.

## Package Header

| Offset | Type |  Size   |    Description     |
|:------:|:----:|:-------:|:------------------:|
|  0x10  | u16  | 2 Bytes |     Package ID     |
|  0x30  | u16  | 2 Bytes |      Patch ID      |
|  0x44  | u32  | 4 Bytes | Entry Table Offset |
|  0x60  | u32  | 4 Bytes |  Entry Table Size  |
|  0x68  | u32  | 4 Bytes |  Block Table Size  |
|  0x6C  | u32  | 4 Bytes | Block Table Offset |
|  0xB8  | u32  | 4 Bytes |  Hash Table Size   |
|  0xBC  | u32  | 4 Bytes | Hash Table Offset  |


## Package Entry - 16 Bytes

| Offset | Type |  Size   |     Description     |
|:------:|:----:|:-------:|:-------------------:|
|  0x0   | u32  | 4 Bytes |    Reference IDs    | 
|  0x4   | u32  | 4 Bytes |     File Flags      | 
|  0x8   | u64  | 8 Bytes | Parsing Information | 

### Reference IDs - 32 Bits
| `AAAAAAAA ABBBBBBB BBBCCCCC CCCCCCCC` |
|:-------------------------------------:|

| Identifier | Type |  Size   |     Description      |
|:----------:|:----:|:-------:|:--------------------:|
|    `A`     | ???? | 9 Bits  |      `Unknown`       |
|    `B`     | uint | 10 Bits | Reference Package ID |
|    `C`     | uint | 13 Bits |     Reference ID     |

### File Flags - 32 Bits
| `DDDDDDDD DDDDDDDD EEEEEEEF FFGGGGGG` |
|:-------------------------------------:|

| Identifier | Type |  Size   | Description |
|:----------:|:----:|:-------:|:-----------:|
|    `D`     | uint | 16 Bits |  `Unknown`  |
|    `E`     | uint | 7 Bits  |  File Type  |
|    `F`     | uint | 3 Bits  |  Sub Type   |
|    `G`     | ???? | 6 Bits  |  `Unknown`  |

### Parsing Information - 64 Bits
| `HHHHHHII IIIIIIII IIIIIIII IIIIIIII IIIIJJJJ JJJJJJJJ JJKKKKKK KKKKKKKK` |
|:-------------------------------------------------------------------------:|

| Identifier | Type |  Size   |      Description      |
|:----------:|:----:|:-------:|:---------------------:|
|    `H`     | ???? | 6 Bits  |       `Unknown`       |
|    `I`     | uint | 30 Bits |       File Size       |
|    `J`     | uint | 14 Bits | Starting Block Offset |
|    `K`     | ???? | 14 Bits |    Starting Block     |

## File Block

| Offset |  Type  |   Size   | Description |
|:------:|:------:|:--------:|:-----------:|
|  0x00  |  u32   | 4 Bytes  |   Offset    |
|  0x04  |  u32   | 4 Bytes  |    Size     |
|  0x08  |  u16   | 2 Bytes  |  Patch ID   |
|  0x0A  |  u16   | 2 Bytes  |    Flags    |
|  0x0C  | String | 20 Bytes |    Hash     |
|  0x20  | String | 16 Bytes |   GCM Tag   |