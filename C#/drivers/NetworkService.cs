using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BSS_Testbench
{
    public class NetworkService : IDisposable
    {
        /* Logs file */
        Logs logFile = null;

        /* TCP recv buffer */
        private const int RECV_BUFF_LEN = 64;
        /* TCP port used by listener */
        private int TCP_PORT = -1;
        /* Store whether there is a client connected or not */
        public bool isTcpClientConnected = false;
        /* New connection assignement */
        private TcpClient tcpClient = null;
        /* TCP server handler */
        private TcpListener tcpServer = null;
        /* Stream used to read/write to sockst */
        private NetworkStream clientStream = null;
        /* Event called in case of data received */
        public event Action<byte[]> RecvCallback;
        /* Thread used for listener */
        private Thread bgTask;
        /* Token to know when the thread is aborted */
        private CancellationTokenSource cts = new CancellationTokenSource();


        /* Contrstructors */
        public NetworkService(int port)
        {
            this.TCP_PORT = port;
            log("Network service created...");
        }

        public NetworkService(int port, Logs log_file)
        {
            logFile = log_file;
            this.TCP_PORT = port;
            log("Network service created...");
        }

        /* Clean up */
        public void Dispose()
        {
            /* Close everything and cleanup */
            this.StopNetworkingService();
            logFile.log("Network service destroyed...");
        }      

        /* Write logs to given file */
        public void log(string str)
        {
            if(logFile != null)
                logFile.log(str);
        }

        public int writeBytes(byte[] bytes)
        {
            try
            {
                int sendBytes = tcpClient.Client.Send(bytes);

                var hexString = BitConverter.ToString(bytes).Replace("-", " ");
                log( string.Format("SEND: {0} bytes from {1}: {2}", sendBytes, bytes.Length, StringToHex( Encoding.ASCII.GetString(bytes))));
                return sendBytes;
            }
            catch
            {
                return -1;
            }
        }
        
        public byte[] readBytes(int max_number)
        {
            if (!isAlive(tcpClient))
            {
                return (new byte[0]);
            }

            /* Receive buffer */
            byte[] recvBuffer = new Byte[max_number];

            /* Read data if available */
            int readBytes = 0;
            try { readBytes = clientStream.Read(recvBuffer, 0, recvBuffer.Length); }
            catch { try { this.tcpClient.Close(); this.tcpClient.Dispose(); } catch { } }

            /* User callback - provide only valid bytes*/
            byte[] tmp = recvBuffer.Skip(0).Take(readBytes).ToArray();

            /* Log action */
            log(string.Format("RECV: {0} bytes: {1}", tmp.Length, StringToHex(Encoding.ASCII.GetString(tmp))));

            return tmp;
        }

        public void StopNetworkingService()
        {
            cts.Cancel();
            if (bgTask != null && bgTask.IsAlive)
            {
                this.bgTask.Abort();
            }

            try
            {
                if (this.clientStream != null)
                {
                    this.clientStream.Close();
                    this.clientStream.Dispose();
                }

                if (this.tcpClient != null)
                {
                    this.tcpClient.Close();
                    this.tcpClient.Dispose();
                }

                if (this.tcpServer != null)
                {
                    this.tcpServer.Stop();
                }
            }
            catch
            {
            }
        }

        public void StartNetworkServiceBackground()
        { 
            /* Launch network service on a new thread */
            this.bgTask = new Thread(() =>
            {
                Thread.CurrentThread.IsBackground = true;

                /* Loop to check conection periodically */
                while (!cts.Token.IsCancellationRequested)
                {
                    Thread.Sleep(1);

                    /* Check if a client is connected */
                    if (this.tcpClient != null && isAlive(this.tcpClient))
                    {
                        /* Set a flag so we can know that client is connected */
                        isTcpClientConnected = true;

                        try
                        {
                            /* If there is some data available to read on TCP stream */
                            if (this.tcpClient.Client.Available > 0)
                            {
                                /* Only read if there was an event set */
                                if (RecvCallback == null)
                                    continue;

                                /* Read data into temporary buffer */
                                byte[] recv = readBytes(RECV_BUFF_LEN);

                                /* Action event with received bytes */
                                RecvCallback(recv);
                            }
                        }
                        catch { }

                        /* If client is connected just restart loop otherwise wait for client to connect again */
                        continue;
                    }

                    /* If this code is reached it means that there is no client connected */
                    this.isTcpClientConnected = false;

                    /* Clean any previous session if there is any */
                    try
                    {
                        if (this.clientStream != null)
                        {
                            this.clientStream.Close();
                            this.clientStream.Dispose();
                        }

                        if (this.tcpClient != null)
                        {
                            this.tcpClient.Close();
                            this.tcpClient.Dispose();
                        }

                        if (this.tcpServer != null)
                        {
                            this.tcpServer.Stop();
                        }
                    }
                    catch
                    {
                    }

                    /* Start TCP Server */
                    log("Started TCP server on port " + TCP_PORT.ToString() + "...");
                    /* Listen on all interfaces and on all IP types */
                    this.tcpServer = new TcpListener(IPAddress.IPv6Any, TCP_PORT);
                    this.tcpServer.Server.SetSocketOption(SocketOptionLevel.IPv6, SocketOptionName.IPv6Only, false);
                    /* Configure keep-alive and timeouts */
                    this.tcpServer.Server.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 1000);
                    LingerOption lingerOption = new LingerOption(true, 3);
                    this.tcpServer.Server.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Linger, lingerOption);
                    this.tcpServer.Start();

                    /* Waiting for a TCP client */
                   log("Waiting for a TCP client...");

                    this.tcpClient = this.tcpServer.AcceptTcpClient();

                    /* Set keep-alive settings and timeouts for client */
                    this.tcpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, 1000);
                    LingerOption clientLingerOption = new LingerOption(true, 3);
                    this.tcpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Linger, clientLingerOption);

                    /* If client connection succeed, just print it's details*/
                    if ( isAlive(this.tcpClient) )
                    {
                        /* Print details in case of connection */
                        string clientIPAddress = ((IPEndPoint)this.tcpClient.Client.RemoteEndPoint).Address.ToString();
                        string clientPort = ((IPEndPoint)this.tcpClient.Client.RemoteEndPoint).Port.ToString();
                        log("Client " + clientIPAddress + " connected on " + clientPort + "...");

                        /* Also initialize stream for to read/write */
                        this.clientStream = this.tcpClient.GetStream();

                        /** TODO: Inspection required
                         *  !!! 21 bytes of garbage ot is it something else? 
                         *  !!! Read the garbage to clean the read buffer
                         */
                        byte[] tmp = new Byte[21];
                        this.clientStream.Read(tmp, 0, 21);
                        tmp = null;

                        /* Stop TCP Server to prevent multiple incoming clients*/
                        log("Stopped TCP server to prevent multiple clients...");
                        this.tcpServer.Stop();
                    }
                }

            });

            /* Start the task on a background thread */
            bgTask.Start();

            log("Network service started...");

        }

        public bool isAlive(TcpClient client)
        {
            if (client == null)
                return false;

            try
            {
                if (!(client.Client.Poll(-1, SelectMode.SelectWrite) && client.Client.Poll(-1, SelectMode.SelectRead)))
                {
                    client.Close();
                    client.Dispose();
                    return false;
                }
                
                if (client.Available == 0)
                {
                    byte[] tmp = new byte[1];
                    if (client.Client.Receive(tmp, 0, 1, SocketFlags.None) == 0)
                    {
                        client.Close();
                        client.Dispose();
                        return false;
                    }
                }

                return true;
            }
            catch
            {
                client.Close();
                client.Dispose();
                return false;
            }
        }

        public static string StringToHex(string hexstring)
        {
            StringBuilder sb = new StringBuilder();
            foreach (char t in hexstring)
            {
                //Note: X for upper, x for lower case letters
                sb.Append(Convert.ToInt32(t).ToString("x"));
                sb.Append(" ");
            }
            return sb.ToString();
        }

    }   /* Class */
}       /* Namespace */
