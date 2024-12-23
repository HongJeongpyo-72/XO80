
註1：Air720SL 4G模塊不需要撥號流程，直接就可以ECM上網。
     設定可參考Kconfig.Air720SL

註2：使用前需先確認有接上天線，且有插上SIM卡。

注意事項：
1.	需先確認模塊是否為ECM模式。(若不是就需發送AT+SETUSB=2，然后重?。) 
2.	使用Air720SL 4G LTE開發板 + ITE970 spi run，操作流程為 
    a.	reset ITE970 
    b.	將Air720SL的供電開關撥到ON，長按開機鍵至NET_LED亮起 (約3~6秒) 
    c.	執行spi run將ITE970跑起來 (必需在4或5秒之內完成) 
