using Raspberry.IO.GeneralPurpose;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Raspberry.IO;
using TestBench.Interfaces;
using TestBench.Concrete;

namespace TestBench.Drivers
{
	public class HC595 : IDriver
	{
		private static ILogger _loggerInstnace;
		public static ILogger HC595Logger
		{
			get
			{
				if (_loggerInstnace == null)
					_loggerInstnace = new Logger(nameof(HC595));
				return _loggerInstnace;
			}
		}

		public event DriverEvent Event;
		public ILogger Logger => HC595Logger;
		public IDriverContainer Container { get; set; }
		private object _syncRoot;

		private GPIO _clock, _data, _latch;
		private uint _registers_no;
		private bool[] bits;
		private bool Enabled = true;

		public HC595(ConnectorPin clockPin, ConnectorPin dataPin, ConnectorPin latchPin, uint registers_no)
		{
			/* SRCLK - Serial clock */
			_clock = new GPIOInstance(clockPin, PinDirection.Output);
			/* SER - Serial data */
			_data = new GPIOInstance(dataPin, PinDirection.Output);
			/* RCLK - Clear shift - must be high to enable output */
			_latch = new GPIOInstance(latchPin, PinDirection.Output);

			/* Set number of registers */
			_registers_no = registers_no;
			/* Set number of bits */
			bits = new bool[registers_no * 8];

			_syncRoot = new object();
		}
        

		public void SetEnable(bool state)
		{
			Enabled = state;
		}

        public void WriteBits(bool[] bitArray)
		{
			if( !Enabled )
			{
				return;
			}

            if (bitArray.Length != bits.Length)
                throw new System.ArgumentException("Length of array is incorrect!", "original");

            for (uint i = 0; i < bitArray.Length; i++)
            {
                /* Write current bit to the output */
                _data.Write(bitArray[i]);

                /* Serial clock to high again */
                _clock.Write(true);

                /* Serial clock to low */
                _clock.Write(false);
            }

            /* Enable output again */
            _latch.Write(true);

            /* Disable output */
            _latch.Write(false);

        }

        public void clearAll()
        {
            //Logger.Log("ALL 0");

            lock (_syncRoot)
            {
                /* Set all bits to false */
                for (uint i = 0; i < bits.Length; i++)
                    bits[i] = false;

                /* Send values to shift register */
                WriteBits(bits);
            }
        }

        public void setAll()
        {
            //Logger.Log("ALL 1");

            lock (_syncRoot)
            {
                /* Set all bits to false */
                for (uint i = 0; i < bits.Length; i++)
                    bits[i] = true;
                /* Send values to shift register */
                WriteBits(bits);
            }
        }

        public void setBit(uint bit)
        {
            //Logger.Log(bit.ToString() + " > 1");

            lock (_syncRoot)
            {
                /* Set selected bit to 1 */
                bits[bit] = true;

                /* Send values to shift register */
                WriteBits(bits);
            }
        }

        public void clearBit(uint bit)
        {
            //Logger.Log(bit.ToString() + " > 0");

            lock (_syncRoot)
            {
                /* Set selected bit to 0 */
                bits[bit] = false;

                /* Send values to shift register */
                WriteBits(bits);
            }
        }

        public void toggleBit(uint bit)
        {
            //Logger.Log(bit.ToString() + "(" + bits[bit].ToString() + ") > " + (!bits[bit]).ToString());

            lock (_syncRoot)
            {
                /* Toggle bit value */
                bits[bit] = !bits[bit];

                /* Send values to shift register */
                WriteBits(bits);
            }
        }

        public bool bitState(uint bit_index)
        {
            lock (_syncRoot)
            {
                return bits[bit_index];
            }
        }

        void IDriver.Tick() { VirtualTick(); }
        protected virtual void VirtualTick()
        {
            //lock (_syncRoot)
            //{
            //    //WriteBits(bits);
            //}
        }

        public void Dispose()
        {
            
        }

        internal class GPIOInstance : GPIO
        {
            internal GPIOInstance(ConnectorPin pin, PinDirection direction) : base(pin, direction)
            {
            }
        }

        protected void InvokeEvent(object state)
        {

        }
    }
}
