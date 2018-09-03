using Raspberry.IO.GeneralPurpose;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Raspberry.IO;

namespace BSS_Testbench
{
    class HC595
    {
        private GPIO _clock, _data, _latch;
        private uint _registers_no;
        private bool[] bits; 

        public HC595(ConnectorPin clockPin, ConnectorPin dataPin, ConnectorPin latchPin, uint registers_no)
        {
            /* SRCLK - Serial clock */
            _clock = new GPIO(clockPin, PinDirection.Output);
            /* SER - Serial data */
            _data = new GPIO(dataPin, PinDirection.Output);
            /* RCLK - Clear shift - must be high to enable output */
            _latch = new GPIO(latchPin, PinDirection.Output);
            /* Set number of registers */
            registers_no = _registers_no;
            /* Set number of bits */
            bits = new bool[registers_no*8];
        }

        public void writeBits(bool[] bitArray)
        {
            if (bitArray.Length != bits.Length)
                throw new System.ArgumentException("Length of array is incorrect!", "original");

            /* Disable output */
            _latch.write(false);

            for (uint i = 0; i < bitArray.Length; i++)
            {
                /* Serial clock to low */
                _clock.write(false);

                /* Write current bit to the output */
                _data.write(bitArray[i]);

                /* Serial clock to high again */
                _clock.write(true);
            }

            /* Enable output again */
            _latch.write(true);
        }

        public void clearAll()
        {
            /* Set all bits to false */
            for (uint i = 0; i < bits.Length; i++)
                bits[i] = false;

            /* Send values to shift register */
            writeBits(bits);
        }

        public void setAll()
        {
            /* Set all bits to false */
            for (uint i = 0; i < bits.Length; i++)
                bits[i] = true;

            /* Send values to shift register */
            writeBits(bits);
        }

        public void setBit(uint bit)
        {
            /* Set selected bit to 1 */
            bits[bit] = true;

            /* Send values to shift register */
            writeBits(bits);
        }

        public void clearBit(uint bit)
        {
            /* Set selected bit to 0 */
            bits[bit] = false;

            /* Send values to shift register */
            writeBits(bits);
        }

        public void toggleBit(uint bit)
        {
            /* Toggle bit value */
            bits[bit] = !bits[bit];

            /* Send values to shift register */
            writeBits(bits);
        }
    }
}
