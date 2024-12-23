
註1：使用前需先確認4G模塊有接上天線(ANT_MAIN)，且有插上SIM卡。

註2：使用FIBOCOM L610 4G模塊需先確認它的 USB Configuration模式為 32。
     可先在PC上透過UART介面下 AT+GTUSBMODE? 而模塊需回覆 +GTUSBMODE: 32     
     若不在模式 32，可透過指令 AT+GTUSBMODE=32 來設定，設定完需重啟模塊。

註3：FIBOCOM L610 4G模塊的撥號流程可透過UART介面來設定，也可透過USB OPTION驅動來設定。

註4：若使用USB OPTION驅動來撥號，撥號的程序可參考test_dial_L610.c
     設定可參考Kconfg.L610


