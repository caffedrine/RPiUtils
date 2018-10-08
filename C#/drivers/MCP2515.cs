using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Threading;
using TestBench.Concrete;
using TestBench.Interfaces;
using System.Runtime.InteropServices;

namespace TestBench.Drivers
{
	unsafe public class MCP2515 : IDriver
	{
		public const int CAN_MAX_DLEN = 8;

		[StructLayout(LayoutKind.Explicit)]
		public struct can_frame
		{
			[FieldOffset(0)] public UInt32 can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
			[FieldOffset(4)] public byte can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
			[FieldOffset(5)] public byte __pad;   /* padding */
			[FieldOffset(6)] public byte __res0;  /* reserved / padding */
			[FieldOffset(7)] public byte __res1;  /* reserved / padding */
			[FieldOffset(8)] public fixed byte can_data[MCP2515.CAN_MAX_DLEN];
            
			unsafe public void SetData(int index, byte value)
			{
				fixed (byte* buffer = can_data)
					buffer[index] = value;
			}
			unsafe public void SetData(byte[] array)
			{
				for (int i = 0; i < Math.Min(array.Length, MCP2515.CAN_MAX_DLEN); i++)
				{
					SetData(i, array[i]);
				}
			}
			unsafe public byte[] GetData()
			{
				byte[] array = new byte[MCP2515.CAN_MAX_DLEN];
				fixed (byte* buffer = can_data)
					for (int i = 0; i < array.Length; i++)
						array[i] = buffer[i];
				return array;
			}
        };

		[DllImport("CSocketCAN.so")]
		public static extern int CAN_Open( void *p );

		[DllImport("/usr/lib/CSocketCAN.so")]
		public static extern int CAN_ReadFrame(int sockfd, void *RecvFrame);

		[DllImport("/usr/lib/CSocketCAN.so")]
		public static extern int CAN_WriteFrame(int sockfd, void *SendFrame);

		[DllImport("/usr/lib/CSocketCAN.so")]
		public static extern int CAN_IsAvailable(int sockfd);

		[DllImport("/usr/lib/CSocketCAN.so")]
		public static extern void CAN_Close(int sockfd);

		[DllImport("/usr/lib/CSocketCAN.so")]
		public static extern int CAN_SetSocketBlockingEnabled(int fd, int blocking);

		private static ILogger _loggerInstnace;
        public static ILogger MCP2515Logger
        {
            get
            {
                if (_loggerInstnace == null)
                    _loggerInstnace = new Logger(nameof(MCP2515));
                return _loggerInstnace;
            }
        }

		int canfd;
		private IntPtr recvFrame, sendFrame;

		public event DriverEvent Event;
        public ILogger Logger => MCP2515Logger;
		public IDriverContainer Container { get; set; }
		private object _syncRoot;
		public bool AutoProcessQueue { get; set; } = true;

		public MCP2515(string CanIf)
		{
			var ptr = Marshal.AllocHGlobal(255);
			byte[] arr = System.Text.Encoding.ASCII.GetBytes(CanIf + '\0');
			Marshal.Copy(arr, 0, ptr, arr.Length);
            
			recvFrame = Marshal.AllocHGlobal(255);
			sendFrame = Marshal.AllocHGlobal(255);

			canfd = CAN_Open(ptr.ToPointer());

			Marshal.FreeHGlobal(ptr);
		}

		public int Send(  can_frame frame  )
		{
			try
			{
				var pSend = (can_frame*)sendFrame.ToPointer();
				*pSend = frame;
				int result = CAN_WriteFrame(canfd, pSend);
				return result;
			}
			catch(Exception e)
			{
				GeneralLogger.Instance.Log("Send(): CAN iface down: " + e.Message);
				return -1;
			}
		} 
        
		public int Receive()
		{
			can_frame frame = new can_frame();
			try
			{
				if (CAN_IsAvailable(canfd) > 0)
				{
					var pReceive = (can_frame*)recvFrame.ToPointer();
					int result = CAN_ReadFrame(canfd, pReceive);
					frame = *pReceive;
					VirtualReceive(frame);
					return result;
				}
				return 0;
			}
			catch(Exception e)
			{
				GeneralLogger.Instance.Log("Receive(): CAN iface down: " + e.Message);
				VirtualDisconnect();
				return -1;
			}
		}

        public virtual void VirtualDisconnect()
		{
			
		}

		public virtual void VirtualReceive(can_frame frame)
        {
			
        }

		public virtual int VirtualSend(can_frame frame)
		{
			return Send(frame);
		}

        public int IsAvailable()
		{
			return CAN_IsAvailable(canfd);
		}

        public void Dispose()
		{
			Marshal.FreeHGlobal(recvFrame);
			Marshal.FreeHGlobal(sendFrame);
			CAN_Close(canfd);
		}

		void IDriver.Tick() { VirtualTick(); }
        protected virtual void VirtualTick()
        {
			if( IsAvailable() > 0 )
			{
				Receive();
			}
        }
        
        

	} /* Class */
} /* Namespace */
