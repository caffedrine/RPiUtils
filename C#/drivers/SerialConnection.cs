using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;
using TestBench.Concrete;
using TestBench.Interfaces;

namespace TestBench.Drivers
{
    public class SerialConnection : IDriver
    {
        private static ILogger _loggerInstnace;
        public static ILogger SerialLogger
        {
            get
            {
                if (_loggerInstnace == null)
                    _loggerInstnace = new Logger(nameof(SerialConnection));
                return _loggerInstnace;
            }
        }

        public ILogger Logger => SerialLogger;
        public event DriverEvent Event;
        public IDriverContainer Container { get; set; }

        private SerialPort _port;
        private Queue<byte[]> _queue;
        private object _syncRoot;
        private byte[] _buffer;

        protected SerialConnection(string portName, int baudRate, Parity parity = Parity.None, int dataBits = 8, StopBits stopBits = StopBits.One)
        {
            _buffer = new byte[16000];
            _queue = new Queue<byte[]>();

            _syncRoot = new object();
            _port = new SerialPort(portName, baudRate, parity, dataBits, stopBits);

            //_port.ReadBufferSize = 32727;
            _port.Open();
            //Logger.Log(portName + " opened");


        }

        public void ClearBuffers()
        {
            try
            {
                _port.DiscardInBuffer();
                _port.DiscardOutBuffer();
            }
            catch { }
        }

        private void tryRead()
        {
            var readSize = _port.Read(_buffer, 0, _buffer.Length);
            //Logger.Log(_port.PortName + " received " + readSize + ": " + _buffer.Hex(0, readSize));
            byte[] buffer = new byte[readSize];
            Buffer.BlockCopy(_buffer, 0, buffer, 0, readSize);
            lock (_syncRoot)
                _queue.Enqueue(buffer);
        }
        
        public void Send(byte[] buffer)
        {
            VirtualSend(buffer);
        }

        protected virtual void VirtualSend(byte[] buffer)
        {
            lock (_syncRoot)
                _port.Write(buffer, 0, buffer.Length);
            //Logger.Log(_port.PortName + " sent " + buffer.Length + ": " + buffer.Hex());
        }

        protected virtual void VirtualReceive(byte[] buffer)
        {
        }

        void IDriver.Tick()
        {
            var timeout = _port.ReadTimeout;
            try
            {
                _port.ReadTimeout = 1;
                tryRead();
            }
            catch (TimeoutException) { }
            finally { _port.ReadTimeout = timeout; }


            lock (_syncRoot)
            {
                while (_queue.Count > 0)
                {
                    var buffer = _queue.Dequeue();
                    VirtualReceive(buffer);
                    Event.Invoke(this, buffer);
                }
            }
        }

        public void Dispose()
        {
            _port.Dispose();
        }
    }
}
