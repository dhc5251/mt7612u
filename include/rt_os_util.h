/****************************************************************************

    Module Name:
	rt_os_util.h

	Abstract:
	All function prototypes are provided from UTIL modules.

	Note:
	But can not use any OS key word and compile option here.
	All functions are provided from UTIL modules.

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------

***************************************************************************/

#ifndef __RT_OS_UTIL_H__
#define __RT_OS_UTIL_H__

/* ============================ rt_linux.c ================================== */
/* General */
VOID RtmpUtilInit(VOID);

/* OS Time */
VOID RtmpusecDelay(ULONG usec);
VOID RtmpOsMsDelay(ULONG msec);

void RTMP_GetCurrentSystemTime(LARGE_INTEGER *time);

ULONG RTMPMsecsToJiffies(UINT msec);
void RTMP_GetCurrentSystemTick(ULONG *pNow);

VOID RtmpOsWait(UINT32 Time);
UINT32 RtmpOsTimerAfter(ULONG a, ULONG b);
UINT32 RtmpOsTimerBefore(ULONG a, ULONG b);
VOID RtmpOsGetSystemUpTime(ULONG *pTime);
UINT32 RtmpOsTickUnitGet(VOID);


/*
	OS Memory
*/
/*
	Function:
		allocate memory
	Parameters:

	Return:

		mem shall be set as NULL if allocation failed
*/
int os_alloc_mem(VOID *pReserved, UCHAR **mem, ULONG size);

int AdapterBlockAllocateMemory(
	IN	VOID *handle,
	OUT	VOID **ppAd,
	IN	UINT32 SizeOfpAd);

VOID *RtmpOsVmalloc(ULONG Size);
VOID RtmpOsVfree(VOID *pMem);

ULONG RtmpOsCopyFromUser(VOID *to, const void *from, ULONG n);
ULONG RtmpOsCopyToUser(VOID *to, const void *from, ULONG n);

BOOLEAN RtmpOsStatsAlloc(VOID **ppStats, VOID **ppIwStats);

/* OS Packet */
struct sk_buff *RtmpOSNetPktAlloc(VOID *pReserved, int size);

struct sk_buff *RTMP_AllocateFragPacketBuffer(VOID *pReserved, ULONG Length);

int RTMPAllocateNdisPacket(
	IN	VOID					*pReserved,
	OUT struct sk_buff *		*ppPacket,
	IN	PUCHAR					pHeader,
	IN	UINT					HeaderLen,
	IN	PUCHAR					pData,
	IN	UINT					DataLen);

VOID RTMPFreeNdisPacket(VOID *pReserved, struct sk_buff *pPacket);

void RTMP_QueryPacketInfo(
	IN  struct sk_buff *pPacket,
	OUT PACKET_INFO *pPacketInfo,
	OUT PUCHAR *pSrcBufVA,
	OUT	UINT *pSrcBufLen);

struct sk_buff *DuplicatePacket(
	IN	struct net_device *pNetDev,
	IN	struct sk_buff *pPacket,
	IN	UCHAR FromWhichBSSID);

struct sk_buff *duplicate_pkt(
	IN	struct net_device *pNetDev,
	IN	PUCHAR pHeader802_3,
    IN  UINT HdrLen,
	IN	PUCHAR pData,
	IN	ULONG DataSize,
	IN	UCHAR FromWhichBSSID);

struct sk_buff *duplicate_pkt_with_TKIP_MIC(
	IN	VOID					*pReserved,
	IN	struct sk_buff *		pOldPkt);

struct sk_buff *duplicate_pkt_with_VLAN(
	IN	struct net_device *			pNetDev,
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	PUCHAR					pHeader802_3,
    IN  UINT            		HdrLen,
	IN	PUCHAR					pData,
	IN	ULONG					DataSize,
	IN	UCHAR					FromWhichBSSID,
	IN	UCHAR					*TPID);

typedef void (*RTMP_CB_8023_PACKET_ANNOUNCE)(
			IN	VOID			*pCtrlBkPtr,
			IN	struct sk_buff *pPacket,
			IN	UCHAR			OpMode);

BOOLEAN RTMPL2FrameTxAction(
	IN  VOID					*pCtrlBkPtr,
	IN	struct net_device *			pNetDev,
	IN	RTMP_CB_8023_PACKET_ANNOUNCE _announce_802_3_packet,
	IN	UCHAR					apidx,
	IN	PUCHAR					pData,
	IN	UINT32					data_len,
	IN	UCHAR			OpMode);

struct sk_buff *ExpandPacket(
	IN	VOID					*pReserved,
	IN	struct sk_buff *		pPacket,
	IN	UINT32					ext_head_len,
	IN	UINT32					ext_tail_len);

struct sk_buff *ClonePacket(
	IN	VOID					*pReserved,
	IN	struct sk_buff *		pPacket,
	IN	PUCHAR					pData,
	IN	ULONG					DataSize);

void wlan_802_11_to_802_3_packet(
	IN	struct net_device *			pNetDev,
	IN	UCHAR					OpMode,
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	struct sk_buff *		pRxPacket,
	IN	UCHAR					*pData,
	IN	ULONG					DataSize,
	IN	PUCHAR					pHeader802_3,
	IN  UCHAR					FromWhichBSSID,
	IN	UCHAR					*TPID);


UCHAR VLAN_8023_Header_Copy(
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	PUCHAR					pHeader802_3,
	IN	UINT            		HdrLen,
	OUT PUCHAR					pData,
	IN	UCHAR					FromWhichBSSID,
	IN	UCHAR					*TPID);

VOID RtmpOsPktBodyCopy(
	IN	struct net_device *			pNetDev,
	IN	struct sk_buff *		pNetPkt,
	IN	ULONG					ThisFrameLen,
	IN	PUCHAR					pData);

INT RtmpOsIsPktCloned(struct sk_buff *pNetPkt);
struct sk_buff *RtmpOsPktCopy(struct sk_buff *pNetPkt);
struct sk_buff *RtmpOsPktClone(struct sk_buff *pNetPkt);

VOID RtmpOsPktDataPtrAssign(struct sk_buff *pNetPkt, UCHAR *pData);

VOID RtmpOsPktLenAssign(struct sk_buff *pNetPkt, LONG Len);
VOID RtmpOsPktTailAdjust(struct sk_buff *pNetPkt, UINT removedTagLen);

PUCHAR RtmpOsPktTailBufExtend(struct sk_buff *pNetPkt, UINT len);
PUCHAR RtmpOsPktHeadBufExtend(struct sk_buff *pNetPkt, UINT len);
VOID RtmpOsPktReserve(struct sk_buff *pNetPkt, UINT len);

VOID RtmpOsPktProtocolAssign(struct sk_buff *pNetPkt);
VOID RtmpOsPktInfPpaSend(struct sk_buff *pNetPkt);
VOID RtmpOsPktRcvHandle(struct sk_buff *pNetPkt);
VOID RtmpOsPktNatMagicTag(struct sk_buff *pNetPkt);
VOID RtmpOsPktNatNone(struct sk_buff *pNetPkt);
VOID RtmpOsPktInit(struct sk_buff *pNetPkt, struct net_device *pNetDev, UCHAR *buf, USHORT len);

struct sk_buff *RtmpOsPktIappMakeUp(struct net_device *pNetDev, UINT8 *pMac);

BOOLEAN RtmpOsPktOffsetInit(VOID);

UINT16 RtmpOsNtohs(UINT16 Value);
UINT16 RtmpOsHtons(UINT16 Value);
UINT32 RtmpOsNtohl(UINT32 Value);
UINT32 RtmpOsHtonl(UINT32 Value);

/* OS File */
RTMP_OS_FD RtmpOSFileOpen(char *pPath,  int flag, int mode);
int RtmpOSFileClose(RTMP_OS_FD osfd);
void RtmpOSFileSeek(RTMP_OS_FD osfd, int offset);
int RtmpOSFileRead(RTMP_OS_FD osfd, char *pDataPtr, int readLen);
int RtmpOSFileWrite(RTMP_OS_FD osfd, char *pDataPtr, int writeLen);

INT32 RtmpOsFileIsErr(VOID *pFile);

void RtmpOSFSInfoChange(RTMP_OS_FS_INFO *pOSFSInfoOrg, BOOLEAN bSet);

/* OS Network Interface */
int RtmpOSNetDevAddrSet(
	IN UCHAR OpMode,
	IN struct net_device *pNetDev,
	IN PUCHAR pMacAddr,
	IN PUCHAR dev_name);

void RtmpOSNetDevClose(struct net_device *pNetDev);
void RtmpOSNetDevFree(struct net_device *pNetDev);
INT RtmpOSNetDevAlloc(struct net_device **new_dev_p, UINT32 privDataSize);
INT RtmpOSNetDevOpsAlloc(PVOID *pNetDevOps);


#ifdef CONFIG_STA_SUPPORT
INT RtmpOSNotifyRawData(struct net_device *pNetDev, UCHAR *buf, INT len, ULONG type, USHORT proto);

#endif /* CONFIG_STA_SUPPORT */

struct net_device *RtmpOSNetDevGetByName(struct net_device *pNetDev, PSTRING pDevName);

void RtmpOSNetDeviceRefPut(struct net_device *pNetDev);

INT RtmpOSNetDevDestory(VOID *pReserved, struct net_device *pNetDev);
void RtmpOSNetDevDetach(struct net_device *pNetDev);
int RtmpOSNetDevAttach(
	IN	UCHAR					OpMode,
	IN	struct net_device *			pNetDev,
	IN	RTMP_OS_NETDEV_OP_HOOK	*pDevOpHook);

void RtmpOSNetDevProtect(
	IN BOOLEAN lock_it);

struct net_device *RtmpOSNetDevCreate(
	IN	INT32					MC_RowID,
	IN	UINT32					*pIoctlIF,
	IN	INT 					devType,
	IN	INT						devNum,
	IN	INT						privMemSize,
	IN	PSTRING					pNamePrefix);

BOOLEAN RtmpOSNetDevIsUp(struct net_device *pDev);

unsigned char *RtmpOsNetDevGetPhyAddr(struct net_device *pDev);

VOID RtmpOsNetQueueStart(struct net_device *pDev);
VOID RtmpOsNetQueueStop(struct net_device *pDev);
VOID RtmpOsNetQueueWake(struct net_device *pDev);

VOID RtmpOsSetPktNetDev(VOID *pPkt, struct net_device *pDev);
struct net_device *RtmpOsPktNetDevGet(VOID *pPkt);

char *RtmpOsGetNetDevName(struct net_device *pDev);

UINT32 RtmpOsGetNetIfIndex(struct net_device *pDev);

VOID RtmpOsSetNetDevPriv(struct net_device *pDev, struct rtmp_adapter *pPriv);
struct rtmp_adapter *RtmpOsGetNetDevPriv(struct net_device *pDev);

VOID RtmpOsSetNetDevWdev(struct net_device *net_dev, struct rtmp_wifi_dev *wdev);
struct rtmp_wifi_dev *RtmpOsGetNetDevWdev(struct net_device  *pDev);

USHORT RtmpDevPrivFlagsGet(struct net_device *pDev);
VOID RtmpDevPrivFlagsSet(struct net_device *pDev, USHORT PrivFlags);

VOID RtmpOsSetNetDevType(struct net_device *pDev, USHORT Type);
VOID RtmpOsSetNetDevTypeMonitor(struct net_device *pDev);
UCHAR get_sniffer_mode(struct net_device *pDev);
VOID set_sniffer_mode(struct net_device *pDev, UCHAR mode);

/* OS Semaphore */
VOID RtmpOsCmdUp(RTMP_OS_TASK *pCmdQTask);
BOOLEAN RtmpOsSemaInitLocked(RTMP_OS_SEM *pSemOrg, LIST_HEADER *pSemList);
BOOLEAN RtmpOsSemaInit(RTMP_OS_SEM *pSemOrg, LIST_HEADER *pSemList);
BOOLEAN RtmpOsSemaDestory(RTMP_OS_SEM *pSemOrg);
INT RtmpOsSemaWaitInterruptible(RTMP_OS_SEM *pSemOrg);
VOID RtmpOsSemaWakeUp(RTMP_OS_SEM *pSemOrg);
VOID RtmpOsMlmeUp(RTMP_OS_TASK *pMlmeQTask);

VOID RtmpOsInitCompletion(RTMP_OS_COMPLETION *pCompletion);
VOID RtmpOsExitCompletion(RTMP_OS_COMPLETION *pCompletion);
VOID RtmpOsComplete(RTMP_OS_COMPLETION *pCompletion);
ULONG RtmpOsWaitForCompletionTimeout(RTMP_OS_COMPLETION *pCompletion, ULONG Timeout);

/* OS Task */
BOOLEAN RtmpOsTaskletSche(RTMP_NET_TASK_STRUCT *pTasklet);

BOOLEAN RtmpOsTaskletInit(
	RTMP_NET_TASK_STRUCT *pTasklet,
	VOID (*pFunc)(unsigned long data),
	ULONG Data,
	LIST_HEADER *pTaskletList);

BOOLEAN RtmpOsTaskletKill(RTMP_NET_TASK_STRUCT *pTasklet);
VOID RtmpOsTaskletDataAssign(RTMP_NET_TASK_STRUCT *pTasklet, ULONG Data);

VOID RtmpOsTaskWakeUp(RTMP_OS_TASK *pTaskOrg);
INT32 RtmpOsTaskIsKilled(RTMP_OS_TASK *pTaskOrg);

BOOLEAN RtmpOsCheckTaskLegality(RTMP_OS_TASK *pTaskOrg);

BOOLEAN RtmpOSTaskAlloc(RTMP_OS_TASK *pTask, LIST_HEADER *pTaskList);

VOID RtmpOSTaskFree(RTMP_OS_TASK *pTask);

int RtmpOSTaskKill(RTMP_OS_TASK *pTaskOrg);

INT RtmpOSTaskNotifyToExit(RTMP_OS_TASK *pTaskOrg);

VOID RtmpOSTaskCustomize(RTMP_OS_TASK *pTaskOrg);

int RtmpOSTaskAttach(
	IN	RTMP_OS_TASK *pTaskOrg,
	IN	RTMP_OS_TASK_CALLBACK	fn,
	IN	ULONG arg);

int RtmpOSTaskInit(
	IN	RTMP_OS_TASK *pTaskOrg,
	IN	PSTRING pTaskName,
	IN	VOID *pPriv,
	IN	LIST_HEADER *pTaskList,
	IN	LIST_HEADER *pSemList);

BOOLEAN RtmpOSTaskWait(
	IN	VOID *pReserved,
	IN	RTMP_OS_TASK *pTaskOrg,
	IN	INT32 *pStatus);

VOID *RtmpOsTaskDataGet(RTMP_OS_TASK *pTaskOrg);

INT32 RtmpThreadPidKill(RTMP_OS_PID	 PID);

/* OS Cache */
VOID RtmpOsDCacheFlush(ULONG AddrStart, ULONG Size);

/* OS Timer */
VOID RTMP_SetPeriodicTimer(
	IN	NDIS_MINIPORT_TIMER *pTimerOrg,
	IN	unsigned long timeout);

VOID RTMP_OS_Init_Timer(
	IN	VOID *pReserved,
	IN	NDIS_MINIPORT_TIMER *pTimerOrg,
	IN	TIMER_FUNCTION function,
	IN	PVOID data,
	IN	LIST_HEADER *pTimerList);

VOID RTMP_OS_Add_Timer(NDIS_MINIPORT_TIMER *pTimerOrg, unsigned long timeout);
VOID RTMP_OS_Mod_Timer(NDIS_MINIPORT_TIMER *pTimerOrg, unsigned long timeout);
VOID RTMP_OS_Del_Timer(NDIS_MINIPORT_TIMER *pTimerOrg, BOOLEAN *pCancelled);
VOID RTMP_OS_Release_Timer(NDIS_MINIPORT_TIMER *pTimerOrg);

BOOLEAN RTMP_OS_Alloc_Rsc(LIST_HEADER *pRscList, VOID *pRsc, UINT32 RscLen);
VOID RTMP_OS_Free_Rscs(LIST_HEADER *pRscList);

/* OS Lock */
BOOLEAN RtmpOsAllocateLock(NDIS_SPIN_LOCK *pLock, LIST_HEADER *pLockList);
VOID RtmpOsFreeSpinLock(NDIS_SPIN_LOCK *pLockOrg);
VOID RtmpOsSpinLockBh(NDIS_SPIN_LOCK *pLockOrg);
VOID RtmpOsSpinUnLockBh(NDIS_SPIN_LOCK *pLockOrg);
VOID RtmpOsIntLock(NDIS_SPIN_LOCK *pLockOrg, ULONG *pIrqFlags);
VOID RtmpOsIntUnLock(NDIS_SPIN_LOCK *pLockOrg, ULONG IrqFlags);

/* OS PID */
VOID RtmpOsGetPid(ULONG *pDst, ULONG PID);
VOID RtmpOsTaskPidInit(RTMP_OS_PID *pPid);

/* OS I/O */
VOID RTMP_PCI_Writel(ULONG Value, VOID *pAddr);
VOID RTMP_PCI_Writew(ULONG Value, VOID *pAddr);
VOID RTMP_PCI_Writeb(ULONG Value, VOID *pAddr);
ULONG RTMP_PCI_Readl(VOID *pAddr);
ULONG RTMP_PCI_Readw(VOID *pAddr);
ULONG RTMP_PCI_Readb(VOID *pAddr);

int RtmpOsPciConfigReadWord(
	IN	VOID					*pDev,
	IN	UINT32					Offset,
	OUT UINT16					*pValue);

int RtmpOsPciConfigWriteWord(VOID *pDev, UINT32 Offset, UINT16 Value);
int RtmpOsPciConfigReadDWord(VOID *pDev, UINT32 Offset, UINT32 *pValue);
int RtmpOsPciConfigWriteDWord(VOID *pDev, UINT32 Offset, UINT32 Value);

int RtmpOsPciFindCapability(VOID *pDev, INT Cap);

VOID *RTMPFindHostPCIDev(VOID *pPciDevSrc);

int RtmpOsPciMsiEnable(VOID *pDev);
VOID RtmpOsPciMsiDisable(VOID *pDev);

/* OS Wireless */
ULONG RtmpOsMaxScanDataGet(VOID);

/* OS Interrutp */
INT32 RtmpOsIsInInterrupt(VOID);

/* OS USB */
VOID *RtmpOsUsbUrbDataGet(VOID *pUrb);
NTSTATUS RtmpOsUsbUrbStatusGet(VOID *pUrb);
ULONG RtmpOsUsbUrbLenGet(VOID *pUrb);

/* OS Atomic */
BOOLEAN RtmpOsAtomicInit(RTMP_OS_ATOMIC *pAtomic, LIST_HEADER *pAtomicList);
VOID RtmpOsAtomicDestroy(RTMP_OS_ATOMIC *pAtomic);
LONG RtmpOsAtomicRead(RTMP_OS_ATOMIC *pAtomic);
VOID RtmpOsAtomicDec(RTMP_OS_ATOMIC *pAtomic);
VOID RtmpOsAtomicInterlockedExchange(RTMP_OS_ATOMIC *pAtomicSrc, LONG Value);

/* OS Utility */
void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);

typedef VOID (*RTMP_OS_SEND_WLAN_EVENT)(
	IN	struct rtmp_adapter			*pAdSrc,
	IN	USHORT					Event_flag,
	IN	PUCHAR 					pAddr,
	IN  UCHAR					BssIdx,
	IN	CHAR					Rssi);

VOID RtmpOsSendWirelessEvent(
	IN	struct rtmp_adapter	*pAd,
	IN	USHORT			Event_flag,
	IN	PUCHAR 			pAddr,
	IN	UCHAR			BssIdx,
	IN	CHAR			Rssi,
	IN	RTMP_OS_SEND_WLAN_EVENT pFunc);

#ifdef CONFIG_AP_SUPPORT
void SendSignalToDaemon(
	IN	INT sig,
	IN	RTMP_OS_PID	 pid,
	IN	unsigned long pid_no);
#endif /* CONFIG_AP_SUPPORT */

int RtmpOSWrielessEventSend(
	IN	struct net_device *			pNetDev,
	IN	UINT32					eventType,
	IN	INT						flags,
	IN	PUCHAR					pSrcMac,
	IN	PUCHAR					pData,
	IN	UINT32					dataLen);

int RtmpOSWrielessEventSendExt(
	IN	struct net_device *			pNetDev,
	IN	UINT32					eventType,
	IN	INT						flags,
	IN	PUCHAR					pSrcMac,
	IN	PUCHAR					pData,
	IN	UINT32					dataLen,
	IN	UINT32					family);

UINT RtmpOsWirelessExtVerGet(VOID);

VOID RtmpDrvAllMacPrint(
	IN VOID						*pReserved,
	IN UINT32					*pBufMac,
	IN UINT32					AddrStart,
	IN UINT32					AddrEnd,
	IN UINT32					AddrStep);

VOID RtmpDrvAllE2PPrint(
	IN	VOID					*pReserved,
	IN	USHORT					*pMacContent,
	IN	UINT32					AddrEnd,
	IN	UINT32					AddrStep);

VOID RtmpDrvAllRFPrint(
	IN VOID *pReserved,
	IN UCHAR *pBuf,
	IN UINT32 BufLen);

int RtmpOSIRQRelease(
	IN	struct net_device *			pNetDev,
	IN	UINT32					infType,
	IN	PPCI_DEV				pci_dev,
	IN	BOOLEAN					*pHaveMsi);

VOID RtmpOsWlanEventSet(
	IN	VOID					*pReserved,
	IN	BOOLEAN					*pCfgWEnt,
	IN	BOOLEAN					FlgIsWEntSup);

UINT16 RtmpOsGetUnaligned(UINT16 *pWord);

UINT32 RtmpOsGetUnaligned32(UINT32 *pWord);

ULONG RtmpOsGetUnalignedlong(ULONG *pWord);

long RtmpOsSimpleStrtol(
	IN	const char				*cp,
	IN	char 					**endp,
	IN	unsigned int			base);


/* ============================ rt_os_util.c ================================ */
VOID RtmpDrvRateGet(
	IN VOID *pReserved,
	IN UINT8 MODE,
	IN UINT8 ShortGI,
	IN UINT8 BW,
	IN UINT8 MCS,
	IN UINT8 Antenna,
	OUT UINT32 *pRate);

char * rtstrchr(const char * s, int c);

PSTRING   WscGetAuthTypeStr(USHORT authFlag);

PSTRING   WscGetEncryTypeStr(USHORT encryFlag);

USHORT WscGetAuthTypeFromStr(PSTRING arg);

USHORT WscGetEncrypTypeFromStr(PSTRING arg);

VOID RtmpMeshDown(
	IN VOID *pDrvCtrlBK,
	IN BOOLEAN WaitFlag,
	IN BOOLEAN (*RtmpMeshLinkCheck)(IN VOID *pAd));

USHORT RtmpOsNetPrivGet(struct net_device *pDev);

BOOLEAN RtmpOsCmdDisplayLenCheck(
	IN	UINT32					LenSrc,
	IN	UINT32					Offset);

VOID    WpaSendMicFailureToWpaSupplicant(
	IN	struct net_device *			pNetDev,
	IN const PUCHAR src_addr,
	IN BOOLEAN bUnicast,
	IN INT key_id,
	IN const PUCHAR tsc);

int wext_notify_event_assoc(
	IN	struct net_device *			pNetDev,
	IN	UCHAR					*ReqVarIEs,
	IN	UINT32					ReqVarIELen);

VOID    SendAssocIEsToWpaSupplicant(
	IN	struct net_device *			pNetDev,
	IN	UCHAR					*ReqVarIEs,
	IN	UINT32					ReqVarIELen);

/* ============================ rt_rbus_pci_util.c ========================== */
void RtmpAllocDescBuf(
	IN PPCI_DEV pPciDev,
	IN UINT Index,
	IN ULONG Length,
	IN BOOLEAN Cached,
	OUT VOID **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RtmpFreeDescBuf(
	IN PPCI_DEV pPciDev,
	IN ULONG Length,
	IN VOID *VirtualAddress,
	IN NDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RTMP_AllocateFirstTxBuffer(
	IN PPCI_DEV pPciDev,
	IN UINT Index,
	IN ULONG Length,
	IN BOOLEAN Cached,
	OUT VOID **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RTMP_FreeFirstTxBuffer(
	IN	PPCI_DEV				pPciDev,
	IN	ULONG					Length,
	IN	BOOLEAN					Cached,
	IN	PVOID					VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress);

struct sk_buff *RTMP_AllocateRxPacketBuffer(
	IN	VOID					*pReserved,
	IN	VOID					*pPciDev,
	IN	ULONG					Length,
	IN	BOOLEAN					Cached,
	OUT	PVOID					*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND

int RTMP_Usb_AutoPM_Put_Interface(
	IN	VOID			*pUsb_Dev,
	IN	VOID			*intf);

int  RTMP_Usb_AutoPM_Get_Interface(
	IN	VOID			*pUsb_Dev,
	IN	VOID			*intf);

#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */



ra_dma_addr_t linux_pci_map_single(void *pPciDev, void *ptr, size_t size, int sd_idx, int direction);

void linux_pci_unmap_single(void *pPciDev, ra_dma_addr_t dma_addr, size_t size, int direction);

/* ============================ rt_usb_util.c =============================== */
#ifdef RTMP_MAC_USB
typedef VOID (*USB_COMPLETE_HANDLER)(VOID *);

void dump_urb(VOID *purb);

int rausb_register(VOID * new_driver);

void rausb_deregister(VOID * driver);

/*struct urb *rausb_alloc_urb(int iso_packets); */

void rausb_free_urb(VOID *urb);

void rausb_put_dev(VOID *dev);

struct usb_device *rausb_get_dev(VOID *dev);

int rausb_submit_urb(VOID *urb);

void *rausb_buffer_alloc(VOID *dev,
							size_t size,
							ra_dma_addr_t *dma);

void rausb_buffer_free(VOID *dev,
							size_t size,
							void *addr,
							ra_dma_addr_t dma);

int rausb_control_msg(VOID *dev,
						unsigned int pipe,
						__u8 request,
						__u8 requesttype,
						__u16 value,
						__u16 index,
						void *data,
						__u16 size,
						int timeout);

void rausb_fill_bulk_urb(void *urb,
						 void *dev,
						 unsigned int pipe,
						 void *transfer_buffer,
						 int buffer_length,
						 USB_COMPLETE_HANDLER complete_fn,
						 void *context);

unsigned int rausb_sndctrlpipe(VOID *dev, ULONG address);

unsigned int rausb_rcvctrlpipe(VOID *dev, ULONG address);


unsigned int rausb_sndbulkpipe(void *dev, ULONG address);
unsigned int rausb_rcvbulkpipe(void *dev, ULONG address);

void rausb_kill_urb(VOID *urb);

VOID	RtmpOsUsbInitHTTxDesc(
	IN	VOID			*pUrbSrc,
	IN	VOID			*pUsb_Dev,
	IN	UINT			BulkOutEpAddr,
	IN	PUCHAR			pSrc,
	IN	ULONG			BulkOutSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	VOID			*pTxContext,
	IN	ra_dma_addr_t		TransferDma);

VOID	RtmpOsUsbInitRxDesc(
	IN	VOID			*pUrbSrc,
	IN	VOID			*pUsb_Dev,
	IN	UINT			BulkInEpAddr,
	IN	UCHAR			*pTransferBuffer,
	IN	UINT32			BufSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	VOID			*pRxContext,
	IN	ra_dma_addr_t		TransferDma);

VOID *RtmpOsUsbContextGet(
	IN	VOID			*pUrb);

NTSTATUS RtmpOsUsbStatusGet(
	IN	VOID			*pUrb);

VOID RtmpOsUsbDmaMapping(
	IN	VOID			*pUrb);
#endif /* RTMP_MAC_USB */

#if defined(RTMP_RBUS_SUPPORT) || defined(RTMP_FLASH_SUPPORT)
void RtmpFlashRead(
	UCHAR * p,
	ULONG a,
	ULONG b);

void RtmpFlashWrite(
	UCHAR * p,
	ULONG a,
	ULONG b);
#endif /* defined(RTMP_RBUS_SUPPORT) || defined(RTMP_FLASH_SUPPORT) */

UINT32 RtmpOsGetUsbDevVendorID(
	IN VOID *pUsbDev);

UINT32 RtmpOsGetUsbDevProductID(
	IN VOID *pUsbDev);

/* CFG80211 */
#ifdef RT_CFG80211_SUPPORT
typedef struct __CFG80211_BAND {

	UINT8 RFICType;
	UINT8 MpduDensity;
	UINT8 TxStream;
	UINT8 RxStream;
	UINT32 MaxTxPwr;
	UINT32 MaxBssTable;

	UINT16 RtsThreshold;
	UINT16 FragmentThreshold;
	UINT32 RetryMaxCnt; /* bit0~7: short; bit8 ~ 15: long */
	BOOLEAN FlgIsBMode;
} CFG80211_BAND;

VOID CFG80211OS_UnRegister(
	IN VOID						*pCB,
	IN VOID						*pNetDev);

BOOLEAN CFG80211_SupBandInit(
	IN VOID						*pCB,
	IN CFG80211_BAND 			*pBandInfo,
	IN VOID						*pWiphyOrg,
	IN VOID						*pChannelsOrg,
	IN VOID						*pRatesOrg);

BOOLEAN CFG80211OS_SupBandReInit(
	IN VOID						*pCB,
	IN CFG80211_BAND 			*pBandInfo);

VOID CFG80211OS_RegHint(
	IN VOID						*pCB,
	IN UCHAR					*pCountryIe,
	IN ULONG					CountryIeLen);

VOID CFG80211OS_RegHint11D(
	IN VOID						*pCB,
	IN UCHAR					*pCountryIe,
	IN ULONG					CountryIeLen);

BOOLEAN CFG80211OS_BandInfoGet(
	IN VOID						*pCB,
	IN VOID						*pWiphyOrg,
	OUT VOID					**ppBand24,
	OUT VOID					**ppBand5);

UINT32 CFG80211OS_ChanNumGet(
	IN VOID						*pCB,
	IN VOID						*pWiphyOrg,
	IN UINT32					IdBand);

BOOLEAN CFG80211OS_ChanInfoGet(
	IN VOID						*pCB,
	IN VOID						*pWiphyOrg,
	IN UINT32					IdBand,
	IN UINT32					IdChan,
	OUT UINT32					*pChanId,
	OUT UINT32					*pPower,
	OUT BOOLEAN					*pFlgIsRadar);

BOOLEAN CFG80211OS_ChanInfoInit(
	IN VOID						*pCB,
	IN UINT32					InfoIndex,
	IN UCHAR					ChanId,
	IN UCHAR					MaxTxPwr,
	IN BOOLEAN					FlgIsNMode,
	IN BOOLEAN					FlgIsBW20M);

VOID CFG80211OS_Scaning(
	IN VOID						*pCB,
	IN UINT32					ChanId,
	IN UCHAR					*pFrame,
	IN UINT32					FrameLen,
	IN INT32					RSSI,
	IN BOOLEAN					FlgIsNMode,
	IN UINT8					BW);

VOID CFG80211OS_ScanEnd(
	IN VOID						*pCB,
	IN BOOLEAN					FlgIsAborted);

void CFG80211OS_ConnectResultInform(
	IN VOID						*pCB,
	IN UCHAR					*pBSSID,
	IN UCHAR					*pReqIe,
	IN UINT32					ReqIeLen,
	IN UCHAR					*pRspIe,
	IN UINT32					RspIeLen,
	IN UCHAR					FlgIsSuccess);

void CFG80211OS_P2pClientConnectResultInform(
	IN struct net_device *				pNetDev,
	IN UCHAR					*pBSSID,
	IN UCHAR					*pReqIe,
	IN UINT32					ReqIeLen,
	IN UCHAR					*pRspIe,
	IN UINT32					RspIeLen,
	IN UCHAR					FlgIsSuccess);

BOOLEAN CFG80211OS_RxMgmt(IN struct net_device *pNetDev, IN INT32 freq, IN PUCHAR frame, IN UINT32 len);
VOID CFG80211OS_TxStatus(IN struct net_device *pNetDev, IN INT32 cookie, 	IN PUCHAR frame, IN UINT32 len, IN BOOLEAN ack);
VOID CFG80211OS_NewSta(IN struct net_device *pNetDev, IN const PUCHAR mac_addr, IN const PUCHAR assoc_frame, IN UINT32 assoc_len);
VOID CFG80211OS_DelSta(IN struct net_device *pNetDev, IN const PUCHAR mac_addr);
VOID CFG80211OS_MICFailReport(IN struct net_device *pNetDev, IN const PUCHAR src_addr, IN BOOLEAN unicast, IN INT key_id, IN const PUCHAR tsc );
VOID CFG80211OS_Roamed(struct net_device *pNetDev, IN UCHAR *pBSSID,
					   UCHAR *pReqIe, UINT32 ReqIeLen, UCHAR *pRspIe, UINT32 RspIeLen);
VOID CFG80211OS_RecvObssBeacon(VOID *pCB, const PUCHAR pFrame, INT frameLen, INT freq);
#endif /* RT_CFG80211_SUPPORT */




/* ================================ MACRO =================================== */
#define RTMP_UTIL_DCACHE_FLUSH(__AddrStart, __Size)

/* ================================ EXTERN ================================== */
extern UCHAR SNAP_802_1H[6];
extern UCHAR SNAP_BRIDGE_TUNNEL[6];
extern UCHAR EAPOL[2];
extern UCHAR TPID[];
extern UCHAR IPX[2];
extern UCHAR APPLE_TALK[2];
extern UCHAR NUM_BIT8[8];
extern ULONG RTPktOffsetData, RTPktOffsetLen, RTPktOffsetCB;

extern ULONG OS_NumOfMemAlloc, OS_NumOfMemFree;

extern UINT32 RalinkRate_Legacy[];
extern UINT32 RalinkRate_HT_1NSS[Rate_BW_MAX][Rate_GI_MAX][Rate_MCS];
extern UINT32 RalinkRate_VHT_1NSS[Rate_BW_MAX][Rate_GI_MAX][Rate_MCS];
extern UINT8 newRateGetAntenna(UINT8 MCS);


#ifdef PLATFORM_UBM_IPX8
#include "vrut_ubm.h"
#endif /* PLATFORM_UBM_IPX8 */

INT32  RtPrivIoctlSetVal(VOID);

void OS_SPIN_LOCK(NDIS_SPIN_LOCK *lock);
void OS_SPIN_UNLOCK(NDIS_SPIN_LOCK *lock);
void OS_SPIN_LOCK_IRQSAVE(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void OS_SPIN_UNLOCK_IRQRESTORE(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void OS_SPIN_LOCK_IRQ(NDIS_SPIN_LOCK *lock);
void OS_SPIN_UNLOCK_IRQ(NDIS_SPIN_LOCK *lock);
void RtmpOsSpinLockIrqSave(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void RtmpOsSpinUnlockIrqRestore(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void RtmpOsSpinLockIrq(NDIS_SPIN_LOCK *lock);
void RtmpOsSpinUnlockIrq(NDIS_SPIN_LOCK *lock);
int OS_TEST_BIT(int bit, unsigned long *flags);
void OS_SET_BIT(int bit, unsigned long *flags);
void OS_CLEAR_BIT(int bit, unsigned long *flags);
void OS_LOAD_CODE_FROM_BIN(unsigned char **image, char *bin_name, void *inf_dev, UINT32 *code_len);

#endif /* __RT_OS_UTIL_H__ */
