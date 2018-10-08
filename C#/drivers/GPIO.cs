using Raspberry.IO;
using Raspberry.IO.GeneralPurpose;
using TestBench.Concrete;
using TestBench.Interfaces;

namespace TestBench.Drivers
{
    public class GPIO : IDriver
    {
        private static ILogger _loggerInstnace;
        public static ILogger GPIOLogger
        {
            get
            {
                if (_loggerInstnace == null)
                    _loggerInstnace = new Logger(nameof(GPIO));
                return _loggerInstnace;
            }
        }

        public event DriverEvent Event;

        private object _syncRoot;
        private ConnectorPin _pin;
        private IInputOutputBinaryPin _connection;
        private IGpioConnectionDriver _gpioDriver;
        private bool _lastWriteState;

        public bool LastWriteState => _lastWriteState;
        public ILogger Logger => GPIOLogger;
        public IDriverContainer Container { get; set; }

        public GPIO(ConnectorPin pin, PinDirection direction)
        {
            //_gpioDriver = new FileGpioConnectionDriver();
            _gpioDriver = new MemoryGpioConnectionDriver();
            _gpioDriver.InOut(pin);

            _syncRoot = _gpioDriver;
            _pin = pin;
            _connection = _gpioDriver.InOut(pin);

            if (direction == PinDirection.Input)
                _connection.AsInput();
            else
                _connection.AsOutput();
        }

        public void Write(bool state)
        {
            lock (_syncRoot)
            {
                _lastWriteState = state;
                _connection.Write(state);
                //Logger.Log(_pin + " > " + state);
            }
        }

        public bool Read()
        {
            lock (_syncRoot)
            {
                var value = _connection.Read();
                //Logger.Log(_pin + " < " + value);
                return value;
            }
        }

        void IDriver.Tick() { VirtualTick(); }
        protected virtual void VirtualTick() { }

        protected void InvokeEvent(object state)
        {
            Event.Invoke(this, state);
        }

        public void Dispose()
        {
            _gpioDriver = null;
        }
    }
}
