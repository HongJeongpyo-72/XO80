


#define Main_WD(IconIndex)	{																						\
								static bool tOnFlag = 0;															\
								if (((MainWarnigData[IconIndex]&0x80)==0x80)||(MainWarnigData[IconIndex]==0))		\
								{																					\
									if ((MainWarnigData[IconIndex]&0x03)==0x03)										\
										OnFlag = gOnFlag_300ms;														\
									else if ((MainWarnigData[IconIndex]&0x03)==0x02)								\
										OnFlag = gOnFlag_500ms;														\
									else if ((MainWarnigData[IconIndex]&0x03)==0x01)								\
										OnFlag = 1;																	\
									else  																			\
										OnFlag = 0;																	\
									{ 																				\
										if ((MainWarnigData[IconIndex]&0x03)>0x01) 									\
										{ 																			\
											if (tOnFlag!=OnFlag) 													\
											{ 																		\
												stMainWarnigData[IconIndex] = Main_Warning_1Disp(stMainWarnigData[IconIndex] ,MainWarnigData[IconIndex] ,IconIndex, OnFlag); 	\
												tOnFlag = OnFlag; 															\
											} 																				\
										} 																					\
										else 																				\
										{ 																					\
											if (stMainWarnigData[IconIndex]!=MainWarnigData[IconIndex])																		\
												stMainWarnigData[IconIndex] = Main_Warning_1Disp(stMainWarnigData[IconIndex] ,MainWarnigData[IconIndex] ,IconIndex, OnFlag);	\
										}						 													\
									}																				\
								}																					\
								else																				\
								{																					\
									stMainWarnigData[IconIndex] = MainWarnigData[IconIndex];							\
								}																					\
							}

#define Main_Warn_2Disp(cMW1,cMW2)							\
		{													\
			if ((MainWarnigData[cMW1]&0x7)!=0)				\
			{												\
				Main_WD(cMW1);								\
				if (((MainWarnigData[cMW2]&0xC0)!=0x00)||((MainWarnigData[cMW2]&0x0F)==0x00))	\
					stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);	\
			}												\
			else if((MainWarnigData[cMW2]&0x7)!=0)			\
			{												\
				Main_WD(cMW2);								\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);		\
			}												\
			else											\
			{												\
				stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);		\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);		\
			}												\
		}
		
#define Main_Warn_3Disp(cMW1,cMW2,cMW3)						\
		{													\
			if ((MainWarnigData[cMW1]&0x7)!=0)				\
			{												\
				Main_WD(cMW1);								\
				if (((MainWarnigData[cMW2]&0xC0)!=0x00)||((MainWarnigData[cMW2]&0x0F)==0x00))	\
					stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);	\
				if (((MainWarnigData[cMW3]&0xC0)!=0x00)||((MainWarnigData[cMW3]&0x0F)==0x00))	\
					stMainWarnigData[cMW3] = Main_Warning_1Disp(stMainWarnigData[cMW3] ,0x80 ,cMW3,DISP_OFF);	\
			}											\
			else if((MainWarnigData[cMW2]&0x7)!=0)		\
			{											\
				Main_WD(cMW2);							\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);	\
				if (((MainWarnigData[cMW3]&0xC0)!=0x00)||((MainWarnigData[cMW3]&0x0F)==0x00))	\
					stMainWarnigData[cMW3] = Main_Warning_1Disp(stMainWarnigData[cMW3] ,0x80 ,cMW3,DISP_OFF);	\
			}											\
			else if((MainWarnigData[cMW3]&0x7)!=0)		\
			{											\
				Main_WD(cMW3);							\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);	\
				stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);	\
			}											\
			else										\
			{											\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);	\
				stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);	\
				stMainWarnigData[cMW3] = Main_Warning_1Disp(stMainWarnigData[cMW3] ,0x80 ,cMW3,DISP_OFF);	\
			}											\
		}		
		
		
#define Main_Warn_4Disp(cMW1,cMW2,cMW3,cMW4)			\
		{												\
			if ((MainWarnigData[cMW1]&0x7)!=0)			\
			{											\
				Main_WD(cMW1);							\
				if (((MainWarnigData[cMW2]&0xC0)!=0x00)||((MainWarnigData[cMW2]&0x0F)==0x00))	\
					stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);	\
				if (((MainWarnigData[cMW3]&0xC0)!=0x00)||((MainWarnigData[cMW3]&0x0F)==0x00))	\
					stMainWarnigData[cMW3] = Main_Warning_1Disp(stMainWarnigData[cMW3] ,0x80 ,cMW3,DISP_OFF);	\
				if (((MainWarnigData[cMW4]&0xC0)!=0x00)||((MainWarnigData[cMW4]&0x0F)==0x00))	\
					stMainWarnigData[cMW4] = Main_Warning_1Disp(stMainWarnigData[cMW4] ,0x80 ,cMW4,DISP_OFF);	\
			}											\
			else if((MainWarnigData[cMW2]&0x7)!=0)		\
			{											\
				Main_WD(cMW2);							\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);	\
				if (((MainWarnigData[cMW3]&0xC0)!=0x00)||((MainWarnigData[cMW3]&0x0F)==0x00))	\
					stMainWarnigData[cMW3] = Main_Warning_1Disp(stMainWarnigData[cMW3] ,0x80 ,cMW3,DISP_OFF);	\
				if (((MainWarnigData[cMW4]&0xC0)!=0x00)||((MainWarnigData[cMW4]&0x0F)==0x00))	\
					stMainWarnigData[cMW4] = Main_Warning_1Disp(stMainWarnigData[cMW4] ,0x80 ,cMW4,DISP_OFF);	\
			}											\
			else if((MainWarnigData[cMW3]&0x7)!=0)		\
			{											\
				Main_WD(cMW3);							\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);	\
				stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);	\
				if (((MainWarnigData[cMW4]&0xC0)!=0x00)||((MainWarnigData[cMW4]&0x0F)==0x00))	\
					stMainWarnigData[cMW4] = Main_Warning_1Disp(stMainWarnigData[cMW4] ,0x80 ,cMW4,DISP_OFF);	\
			}											\
			else if((MainWarnigData[cMW4]&0x7)!=0)		\
			{											\
				Main_WD(cMW4);							\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);	\
				stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);	\
				stMainWarnigData[cMW3] = Main_Warning_1Disp(stMainWarnigData[cMW3] ,0x80 ,cMW3,DISP_OFF);	\
			}											\
			else										\
			{											\
				stMainWarnigData[cMW1] = Main_Warning_1Disp(stMainWarnigData[cMW1] ,0x80 ,cMW1,DISP_OFF);	\
				stMainWarnigData[cMW2] = Main_Warning_1Disp(stMainWarnigData[cMW2] ,0x80 ,cMW2,DISP_OFF);	\
				stMainWarnigData[cMW3] = Main_Warning_1Disp(stMainWarnigData[cMW3] ,0x80 ,cMW3,DISP_OFF);	\
				stMainWarnigData[cMW4] = Main_Warning_1Disp(stMainWarnigData[cMW4] ,0x80 ,cMW4,DISP_OFF);	\
			}											\
		}
		
		

//// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요 

#define Main_FD_On(cMF1)	{																					\
							static bool tOnFlag = 0;															\
							uint8_t IconIndex = cMF1-cF_01_B;													\
							if (((MainFunctionData[IconIndex]&0x80)==0x80)||(MainFunctionData[IconIndex]==0))	\
							{																					\
								if ((MainFunctionData[IconIndex]&0x03)==0x03)									\
									OnFlag = gOnFlag_300ms;														\
								else if ((MainFunctionData[IconIndex]&0x03)==0x02)								\
									OnFlag = gOnFlag_500ms;														\
								else if ((MainFunctionData[IconIndex]&0x03)==0x01)								\
									OnFlag = 1;																	\
								else  																			\
									OnFlag = 0;																	\
								{ 																				\
									if ((MainFunctionData[IconIndex]&0x03)>0x01) 								\
									{ 																			\
										if (tOnFlag!=OnFlag) 													\
										{ 																		\
											stMainFunctionData[IconIndex] = Main_Func_1Disp(stMainFunctionData[IconIndex] ,MainFunctionData[IconIndex] ,cMF1, OnFlag); 	\
											tOnFlag = OnFlag; 															\
										} 																				\
									} 																					\
									else 																				\
									{ 																					\
										if (stMainFunctionData[IconIndex]!=MainFunctionData[IconIndex])																	\
											stMainFunctionData[IconIndex] = Main_Func_1Disp(stMainFunctionData[IconIndex] ,MainFunctionData[IconIndex] ,cMF1, OnFlag);	\
									}						 													\
								}																				\
							}																					\
							else																				\
							{																					\
								stMainFunctionData[IconIndex] = MainFunctionData[IconIndex];						\
							}																					\
						}

#define Main_FD(cMF1)	{																						\
							static bool tOnFlag = 0;															\
							uint8_t IconIndex = cMF1-cF_01_B;													\
							if (((MainFunctionData[IconIndex]&0x80)==0x80)||(MainFunctionData[IconIndex]==0))	\
							{																					\
								if ((MainFunctionData[IconIndex]&0x03)==0x03)									\
									OnFlag = gOnFlag_300ms;														\
								else if ((MainFunctionData[IconIndex]&0x03)==0x02)								\
									OnFlag = gOnFlag_500ms;														\
								else if ((MainFunctionData[IconIndex]&0x03)==0x01)								\
									OnFlag = 1;																	\
								else  																			\
									OnFlag = 0;																	\
								{ 																				\
									if ((MainFunctionData[IconIndex]&0x03)>0x01) 								\
									{ 																			\
										if (tOnFlag!=OnFlag) 													\
										{ 																		\
											stMainFunctionData[IconIndex] = Main_Func_1Disp(stMainFunctionData[IconIndex] ,MainFunctionData[IconIndex] ,cMF1, OnFlag); 	\
											tOnFlag = OnFlag; 															\
										} 																				\
									} 																					\
									else 																				\
									{ 																					\
																														\
										{																																	\
											if (stMainFunctionData[IconIndex]!=MainFunctionData[IconIndex])																	\
												stMainFunctionData[IconIndex] = Main_Func_1Disp(stMainFunctionData[IconIndex] ,MainFunctionData[IconIndex] ,cMF1, OnFlag);	\
										}																																	\
									}						 																												\
								}																				\
							}																					\
							else																				\
							{																					\
								stMainFunctionData[IconIndex] = MainFunctionData[IconIndex];						\
							}																					\
						}
						
#define Main_Func_2Disp(cMF1,cMF2)									\
		{															\
			if ((MainFunctionData[cMF1-cF_01_B]&0x7)!=0)			\
			{														\
				Main_FD(cMF1);										\
				if (((MainFunctionData[cMF2-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF2-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);			\
			}														\
			else if((MainFunctionData[cMF2-cF_01_B]&0x7)!=0)		\
			{											\
				Main_FD(cMF2);							\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);	\
			}											\
			else										\
			{											\
				stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);	\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);	\
			}											\
		}
		
#define Main_Func_3Disp(cMF1,cMF2,cMF3)					\
		{												\
			if ((MainFunctionData[cMF1-cF_01_B]&0x7)!=0)			\
			{														\
				Main_FD(cMF1);										\
				if (((MainFunctionData[cMF2-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF2-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);			\
				if (((MainFunctionData[cMF3-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF3-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF3-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF3-cF_01_B] ,0x80,cMF3,DISP_OFF);			\
			}														\
			else if((MainFunctionData[cMF2-cF_01_B]&0x7)!=0)		\
			{														\
				Main_FD(cMF2);										\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);				\
				if (((MainFunctionData[cMF3-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF3-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF3-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF3-cF_01_B] ,0x80,cMF3,DISP_OFF);			\
			}														\
			else if((MainFunctionData[cMF3-cF_01_B]&0x7)!=0)		\
			{											\
				Main_FD(cMF3);							\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);	\
				stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);	\
			}											\
			else										\
			{											\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);	\
				stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);	\
				stMainFunctionData[cMF3-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF3-cF_01_B] ,0x80,cMF3,DISP_OFF);	\
			}											\
		}		
		
		
#define Main_Func_4Disp(cMF1,cMF2,cMF3,cMF4)						\
		{															\
			if ((MainFunctionData[cMF1-cF_01_B]&0x7)!=0)			\
			{														\
				Main_FD(cMF1);										\
				if (((MainFunctionData[cMF2-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF2-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);			\
				if (((MainFunctionData[cMF3-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF3-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF3-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF3-cF_01_B] ,0x80,cMF3,DISP_OFF);			\
				if (((MainFunctionData[cMF4-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF4-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF4-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF4-cF_01_B] ,0x80,cMF4,DISP_OFF);			\
			}														\
			else if((MainFunctionData[cMF2-cF_01_B]&0x7)!=0)		\
			{														\
				Main_FD(cMF2);										\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);				\
				if (((MainFunctionData[cMF3-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF3-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF3-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF3-cF_01_B] ,0x80,cMF3,DISP_OFF);			\
				if (((MainFunctionData[cMF4-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF4-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF4-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF4-cF_01_B] ,0x80,cMF4,DISP_OFF);			\
			}														\
			else if((MainFunctionData[cMF3-cF_01_B]&0x7)!=0)		\
			{														\
				Main_FD(cMF3);										\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);				\
				stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);				\
				if (((MainFunctionData[cMF4-cF_01_B]&0xC0)!=0x00)||((MainFunctionData[cMF4-cF_01_B]&0x0F)==0x00))	\
					stMainFunctionData[cMF4-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF4-cF_01_B] ,0x80,cMF4,DISP_OFF);			\
			}														\
			else if((MainFunctionData[cMF4-cF_01_B]&0x7)!=0)		\
			{											\
				Main_FD(cMF4);							\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);	\
				stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);	\
				stMainFunctionData[cMF3-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF3-cF_01_B] ,0x80,cMF3,DISP_OFF);	\
			}											\
			else										\
			{											\
				stMainFunctionData[cMF1-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF1-cF_01_B] ,0x80,cMF1,DISP_OFF);	\
				stMainFunctionData[cMF2-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF2-cF_01_B] ,0x80,cMF2,DISP_OFF);	\
				stMainFunctionData[cMF3-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF3-cF_01_B] ,0x80,cMF3,DISP_OFF);	\
				stMainFunctionData[cMF4-cF_01_B] = Main_Func_1Disp(stMainFunctionData[cMF4-cF_01_B] ,0x80,cMF4,DISP_OFF);	\
			}											\
		}


	#define FuncZoombitClear(MainFunc)			\
			{									\
				if (MainFunc!=0)				\
				{								\
					MainFunc = (MainFunc|0x80);	\
				}								\
			}
			
		
		