using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Threading;

namespace BSS_Testbench
{
    class SerialConn : IDisposable
    {
        /* Logs file */
        Logs logFile = null;
        /* Recv buffer */
        private const int RECV_BUFF_LEN = 64;
        /* Serial port handler */
        private SerialPort serialPort;
        /* Data recv callback */
        public event Action<byte[]> RecvCallback;
        /* Serial listener thread */
        private Thread bgTask;
        /* Let thread know if it shall continue listening */
        private bool _continue = true;

        /* Serial port configs */
        private string portName;
        private int baudRate;
        private Parity parity;
        private int dataBits;
        private StopBits stopBits;
        private int retrySeconds = 10;

        /* Contructors */
        public SerialConn(string _portName, int _baudRate, Parity _parity = Parity.None, int _dataBits = 8, StopBits _stopBits = StopBits.One)
        {
            this.portName = _portName;
            this.baudRate = _baudRate;
            this.parity = _parity;
            this.dataBits = _dataBits;
            this.stopBits = _stopBits;
        }

        public SerialConn(string _portName, int _baudRate, Parity _parity = Parity.None, int _dataBits = 8, StopBits _stopBits = StopBits.One, Logs logs_file = null)
        {
            this.portName = _portName;
            this.baudRate = _baudRate;
            this.parity = _parity;
            this.dataBits = _dataBits;
            this.stopBits = _stopBits;

            logFile = logs_file;
            log("Serial service created...");
        }

        /* Make clean */
        public void Dispose()
        {
            this.StopService();
            log("Serial service destroyed...");
        }

        /* Write logs to given file */
        public void log(string str)
        {
            if (logFile != null)
                logFile.log(str);
        }

        public bool isAlive()
        {
            if (this.serialPort == null)
                return false;

            if (!this.serialPort.IsOpen)
                return false;

            return true;
        }

        public int Write(byte[] sendData)
        {
            if (serialPort == null)
                return -1;

            int sendBytes = 0;
            try
            {
                /* Write bytes one by one */
                foreach (byte b in sendData)
                {
                    byte[] z = new Byte[1]; z[0] = b;

                    serialPort.Write(z, 0, 1);

                    /* Increment number of bytes send */
                    sendBytes++;
                }

                /* Show logs */
                byte[] tmp = sendData.Skip(0).Take(sendBytes).ToArray();
                var hexString = BitConverter.ToString(tmp).Replace("-", " ");
                log(string.Format("SEND: {0} bytes from {1}: {2}", sendBytes, tmp.Length, StringToHex(Encoding.ASCII.GetString(tmp))));
                return sendBytes;
            }
            catch
            {
                log("SEND: FAILED!");
                return sendBytes;
            }                      
        }

        public byte[] Read(int max_len)
        {
            try
            {
                /* Recdv buffer */
                byte[] recvBuffer = new Byte[max_len];

                /* Read data into buffer */
                int readBytes = 0;
                try { readBytes = serialPort.Read(recvBuffer, 0, recvBuffer.Length); }
                catch { }

                /* Provide read data to defined callback */
                byte[] tmp = recvBuffer.Skip(0).Take(readBytes).ToArray();

                return tmp;
            }
            catch(Exception e)
            {
                log("Error while reading: " + e.Message);
                try
                {
                    serialPort.Close();
                }
                catch
                {
                    serialPort.Dispose();
                }
                return new byte[0];
            }
        }

        public void StopService()
        {
            this._continue = false;
            try
            {
                if (serialPort != null)
                {
                    serialPort.Close();
                    serialPort.Dispose();
                }

                if (bgTask != null)//bgTask.IsAlive)
                {
                    bgTask.Abort();
                }
            }
            catch { }
        }

        public void StartServiceBackground()
        {
            this.bgTask = new Thread(() =>
            {
                /* Set this job as a background task */
                Thread.CurrentThread.IsBackground = true;

                /* Loop to monitor connection periodically */
                while (true)
                {
                    Thread.Sleep(1);

                    /* If thread not needed anymore*/
                    if (!_continue)
                        break;

                    /* If there is already a serial connection established */
                    if (this.serialPort != null && this.serialPort.IsOpen && this.isAlive())
                    {
                        try
                        {
                            if (serialPort.BytesToRead > 0)  // If there is data to read in the buffer
                            {
                                /* Recv event was not implemented */
                                if (RecvCallback == null)
                                    continue;

                                byte[] tmp = Read(RECV_BUFF_LEN);
                                RecvCallback(tmp);
                            }
                        }
                        catch
                        {
                            serialPort.Close();
                        }

                        /* Go back to beginning if there s a serial session - no need to reconnect*/
                        continue;
                    }

                    /***** This section is reached when serial connection is lost/broken/does not exist */

                    /* Check for previous sessions and clean them up */
                    if (serialPort != null)
                    {
                        try
                        {
                            serialPort.Close();
                            serialPort.Dispose();
                        }
                        catch { }
                    }

                    /* Proceed creating a new serial session */
                    log( string.Format( "Connecting to {0}...", this.portName) );
                    this.serialPort = new SerialPort(this.portName);

                    /* Config port and attempt to connect */
                    this.serialPort.BaudRate = this.baudRate;
                    this.serialPort.Parity = this.parity;
                    this.serialPort.DataBits = this.dataBits;
                    this.serialPort.StopBits = this.stopBits;
                    this.serialPort.DtrEnable = true;
                    this.serialPort.RtsEnable = true;
                    // This seems not to work on raspberry
                    //this.serialPort.DataReceived += SerialDataReceivedEventHandler;

                    try { this.serialPort.Open(); }
                    catch { }

                    /* Check whether port was opened or not */
                    if(!this.isAlive())
                    {
                        log ( String.Format("Failled to connect to {0}...", this.portName) );
                        log( String.Format("Will try again to connect in {0} seconds...", retrySeconds.ToString()));
                        Console.Out.Flush();
                        Thread.Sleep(this.retrySeconds * 1000);
                    }
                    else
                    {
                        log( String.Format("Established serial communication channel with {0} baud {1}...", this.portName, this.baudRate));
                    }
                }
            });

            /* Start service on backgorund */
            this.bgTask.Start();
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
}   /* Namespace */
