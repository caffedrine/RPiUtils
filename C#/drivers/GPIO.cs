using Raspberry.IO;
using Raspberry.IO.GeneralPurpose;

namespace BSS_Testbench
{
    class GPIO
    {
        private object _syncRoot;
        private ConnectorPin _pin;
        private IInputOutputBinaryPin _connection;
        private IGpioConnectionDriver _gpioDriver;
        public  bool currStateOut;

        public GPIO(ConnectorPin pin, PinDirection direction)
        {
            /* Init a GPIO driver */
            _gpioDriver = new FileGpioConnectionDriver();
            _gpioDriver.InOut(pin);

            _syncRoot = _gpioDriver;
            _pin = pin;
            _connection = _gpioDriver.InOut(pin);

            if (direction == PinDirection.Input)
                _connection.AsInput();
            else
                _connection.AsOutput();
        }

        public void write(bool state)
        {
            lock (_syncRoot)
            {
                currStateOut = state;
                _connection.Write(state);
            }
        }

        public bool read()
        {
            lock (_syncRoot)
            {
                return _connection.Read();
            }
        }
    }   /* Class */
}   /* Namespace */
