using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Threading;
using TestBench.Concrete;
using TestBench.Interfaces;

namespace TestBench.Drivers
{
    public class NetworkConnection : IDriver
    {
        private static ILogger _loggerInstnace;
        public static ILogger SocketLogger
        {
            get
            {
                if (_loggerInstnace == null)
                    _loggerInstnace = new Logger(nameof(NetworkConnection));
                return _loggerInstnace;
            }
        }

        private Socket _socket;
        private SemaphoreSlim _semaphore;
        private byte[] _receiveBuffer;
        private object _syncRoot;
        private Queue<byte[]> _receiveQueue;

        public ILogger Logger => SocketLogger;
        public event DriverEvent Event;
        public IDriverContainer Container { get; set; }
        public bool AutoProcessQueue { get; set; } = true;

        protected NetworkConnection(string ip, int port) : this(createSocket(ip, port))
        {
        }

        private static Socket createSocket(string ip, int port)
        {
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.Connect(ip, port);
            return socket;
        }

        public NetworkConnection(Socket socket)
        {
            _socket = socket;
            _semaphore = new SemaphoreSlim(1);
            _receiveBuffer = new byte[1024];
            _syncRoot = new object();
            _receiveQueue = new Queue<byte[]>();

            beginReceive();
        }

        private void beginReceive()
        {
            _socket.BeginReceive(_receiveBuffer, 0, _receiveBuffer.Length, SocketFlags.None, endReceive, this);
        }

        private void endReceive(IAsyncResult ar)
        {
            var connection = (NetworkConnection)ar.AsyncState;
            bool disconnected = false;
            try
            {
                var size = connection._socket.EndReceive(ar);
                if (size <= 0)
                {
                    disconnected = true;
                    lock (connection._syncRoot)
                        connection._receiveQueue.Enqueue(new byte[0]);
                    VirtualDisconnect();
                    connection.Logger.Log($"{connection._socket.RemoteEndPoint} - disconnect - 0byte packet");
                }
                else
                {
                    byte[] buffer = new byte[size];
                    Buffer.BlockCopy(_receiveBuffer, 0, buffer, 0, size);

                    lock (_syncRoot)
                        connection._receiveQueue.Enqueue(buffer);
                    //connection.Logger.Log($"{connection._socket.RemoteEndPoint} - receive {size}: {buffer.Hex()}");
                }
            }
            catch (Exception ex)
            {
                VirtualDisconnect();
                //GeneralLogger.Instance.Exception(ex);
            }
            finally
            {
                if (!disconnected)
                    connection.beginReceive();
            }
        }

        public void Send(byte[] buffer, bool copy = false)
        {
            Send(buffer, 0, buffer.Length, copy);
        }

        public void Send(byte[] buffer, int offset, int length, bool copy = true)
        {
            if (copy)
            {
                var _buffer = new byte[length];
                Buffer.BlockCopy(buffer, offset, _buffer, 0, length);
                buffer = _buffer;
                offset = 0;
            }

            VirtualSend(buffer, offset, length);
        }

        protected virtual void VirtualReceive(byte[] buffer)
        {

        }

        protected virtual void VirtualSend(byte[] buffer, int offset, int length)
        {
            _semaphore.Wait();
            try
            {
                _socket.BeginSend(buffer, 0, length, SocketFlags.None, endSend, this);
                //Logger.Log($"{_socket.RemoteEndPoint} - send {length}: {buffer.Hex(offset, length)}");
            }
            catch (Exception ex)
            {
                //GeneralLogger.Instance.Exception(ex);
                _semaphore.Release();
                VirtualDisconnect();
            }
        }

        protected virtual void VirtualDisconnect()
        {

        }

        private static void endSend(IAsyncResult ar)
        {
            var connection = (NetworkConnection)ar.AsyncState;
            try
            {
                connection._socket.EndSend(ar);
            }
            catch (Exception ex)
            {
                GeneralLogger.Instance.Exception(ex);
            }
            finally
            {
                connection._semaphore.Release();
            }
        }

        void IDriver.Tick()
        {
            if (AutoProcessQueue)
            {
                lock (_syncRoot)
                {
                    while (_receiveQueue.Count > 0)
                    {
                        var buffer = _receiveQueue.Dequeue();
                        VirtualReceive(buffer);
                        InvokeEvent(buffer);
                    }
                }
            }
        }

        public int IsAvailable()
        {
            lock (_syncRoot)
            {
                return _receiveQueue.Count;
            }
        }

        public byte[] Read()
        {
            if (!AutoProcessQueue)
            {
                lock (_syncRoot)
                {
                    if (_receiveQueue.Count > 0)
                    {
                        var buffer = _receiveQueue.Dequeue();
                        return buffer;
                    }
                }
            }
            return null;
        }

        protected void InvokeEvent(object state)
        {
            Event.Invoke(this, state);
        }

        public void Dispose()
        {
            _socket.Dispose();
        }
    }
}
