using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading;
using Raspberry.IO.GeneralPurpose;
using TestBench.Concrete;
using TestBench.Interfaces;

namespace TestBench.Drivers
{
    public class Stepper
    {
        private ILogger _logger;

        /* Pins */
        private GPIO _protectionPin;
        private GPIO _directionPin;
        private GPIO _impulsePin;
        private GPIO _resetPin;
        private GPIO _enablePin;
        private PWM _pwm;
        private int _gpio_pwm;
        private float _pwm_duty = (float)0.09;

        public event DriverEvent Event;
        public ILogger Logger => _logger;
        public IDriverContainer Container { get; set; }

        public int stepsToDo = 0;

        /* Constructor */
        protected Stepper(ConnectorPin protection, ConnectorPin direction, ConnectorPin impulse, ConnectorPin reset, ConnectorPin enable)
        {
            _logger = new Logger(nameof(Stepper));
            /* Config pins */
            _protectionPin = new GPIOInstance(protection, PinDirection.Input);
            _directionPin = new GPIOInstance(direction, PinDirection.Output);
            _impulsePin = new GPIOInstance(impulse, PinDirection.Output);
            _resetPin = new GPIOInstance(reset, PinDirection.Output);
            _enablePin = new GPIOInstance(enable, PinDirection.Output);

            /* Disable motor by default to prevent heating */
            Enable(false);

            /* Set direction as FORWARD by default */
            SetDirection(Direction.Forward);

            /* Reset pin */
            _resetPin.Write(true);

            /* Launch PWM */
            _gpio_pwm = 23;
			System.Diagnostics.Process.Start("/usr/sbin/pi-blaster", "--gpio 23").WaitForExit();
            _pwm = new PWM(_gpio_pwm);
            _pwm.SetDuty(0);
        }

        public void Enable(bool state)
        {
            /* 0: Motor disabled
             * 1: Motor enabled
             */
            _enablePin.Write(state);
        }

        public bool IsFault()
        {
            return !(_protectionPin.Read());
        }

        public void SetDirection(Direction direction)
        {
            if (direction == Direction.Forward)
                _directionPin.Write(false);
            else
                _directionPin.Write(true);
        }

        public void Reset()
        {
            _resetPin.Write(false);
            Thread.Sleep(1);
            _resetPin.Write(true);
        }

        public void SetDuty(float duty)
        {
            _pwm_duty = duty;
			_pwm.SetDuty(duty);
        }

        /// <param name="steps">-1 is infinite</param>
        public void Run(int steps = -1)
        {
            Enable(true);
        }

        public void Stop()
        {
            Enable(false);
        }

        public void Step()
        {
            _impulsePin.Write(true);

            // Delay for 300 microseconds
            long currTicks = DateTime.Now.Ticks;
            while ((DateTime.Now.Ticks - currTicks) / 10 < 50) ;   // wait for 80 microseconds

            _impulsePin.Write(false);
        }

        public void Dispose()
        {
            Enable(false);
            _protectionPin.Dispose();
            _directionPin.Dispose();
            _impulsePin.Dispose();
            _resetPin.Dispose();
            _enablePin.Dispose();
        }

        internal class GPIOInstance : GPIO
        {
            internal GPIOInstance(ConnectorPin pin, PinDirection direction) : base(pin, direction)
            {
            }
        }

        public enum Direction
        {
            Backward = 0,
            Forward = 1
        }

        public class PWM
        {
            string _fifoName = "/dev/pi-blaster";

            int _GPIO;
            FileStream _file;
            StreamWriter _write;

            public PWM(int GPIO)
            {
                _GPIO = GPIO;
                _file = new FileInfo(_fifoName).OpenWrite();
                _write = new StreamWriter(_file, Encoding.ASCII);

                _write.Flush();
            }

            public void SetDuty(float value)
            {
                string s = _GPIO.ToString() + "=" + value + "\n";
                _write.Write(s);
                _write.Flush();
            }
        }
    }
}   
