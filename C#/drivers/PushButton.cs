using Raspberry.IO.GeneralPurpose;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BSS_Testbench
{
    class PushButton : IDisposable
    {
        /* Button states */
        public enum BUTTON_STATE
        {
            BUTTON_UP,
            BUTTON_DOWN
        }

        /* Public variables */
        public BUTTON_STATE currState = BUTTON_STATE.BUTTON_UP;
        public event Action<BUTTON_STATE> stateChanged;

        /* Private variables */
        private GPIO switchPin = null;
        private Thread bgTask = null;
        private CancellationTokenSource cts = new CancellationTokenSource();

        /* Constructor */
        public PushButton(ConnectorPin switch_pin)
        {
            /* Create a new GPIO instance */
            this.switchPin = new GPIO(switch_pin, PinDirection.Input);

            /* Start the background reader */
            this.bgTask = new Thread(buttonReader);
            this.bgTask.Start();
        }

        public void Dispose()
        {
            cts.Cancel();
        }

        /* Read button in a loop */
        private void buttonReader()
        {
            bool curr = false, prev = false;

            while (!cts.Token.IsCancellationRequested)
            {
                /* Read current button state */
                curr = switchPin.read();

                if (curr != prev)
                {
                    if (curr == true && prev == false)
                    {
                        this.currState = BUTTON_STATE.BUTTON_DOWN;
                        stateChanged?.Invoke(BUTTON_STATE.BUTTON_DOWN);
                    }
                    else
                    {
                        this.currState = BUTTON_STATE.BUTTON_UP;
                        stateChanged?.Invoke(BUTTON_STATE.BUTTON_UP);
                    }
                    prev = curr;
                }
                Thread.Sleep(1);
            }
        }
    }
}
