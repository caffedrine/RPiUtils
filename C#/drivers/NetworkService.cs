using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using TestBench.Concrete;
using TestBench.Interfaces;

namespace TestBench.Drivers
{
    public class NetworkService : IDriver
    {
        private static ILogger _loggerInstnace;
        public static ILogger SocketsService
        {
            get
            {
                if (_loggerInstnace == null)
                    _loggerInstnace = new Logger(nameof(NetworkService));
                return _loggerInstnace;
            }
        }

        public IDriverContainer Container { get; set; }
        public ILogger Logger => SocketsService;
        public event DriverEvent Event;

        private int _port;
        private Func<Socket, NetworkConnection> _generator;
        private Socket _boundSocket;
        private object _syncRoot;
        private Queue<Socket> _socketQueue;

        protected NetworkService(int port, Func<Socket, NetworkConnection> generator)
        {
            _port = port;
            _generator = generator;
            _syncRoot = new object();
            _socketQueue = new Queue<Socket>();

			_boundSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _boundSocket.Bind(new IPEndPoint(IPAddress.Any, port));
            _boundSocket.Listen(1);
            beginAccept();
        }

        private void beginAccept()
        {
            _boundSocket.BeginAccept(socketAccept, this);
        }

        private static void socketAccept(IAsyncResult ar)
        {
            var service = (NetworkService)ar.AsyncState;
            try
            {
                var socket = service._boundSocket.EndAccept(ar);
                lock (service._syncRoot)
                    service._socketQueue.Enqueue(socket);
                //service.Logger.Log($"{service._port} - new connection: {socket.RemoteEndPoint}");
            }
            catch (Exception ex) {
                //GeneralLogger.Instance.Exception(ex); 
            }
            finally
            {
                service.beginAccept();
            }
        }

        void IDriver.Tick()
        {
            lock (_syncRoot)
            {
                while (_socketQueue.Count > 0)
                {
                    var socket = _socketQueue.Dequeue();
                    var connection = _generator(socket);
                    Container.Add(connection);
                }
            }
        }

        public void Dispose()
        {
            _boundSocket.Dispose();
        }
    }
}
