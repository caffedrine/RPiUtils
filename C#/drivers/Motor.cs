using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Raspberry.IO.GeneralPurpose;

namespace BSS_Testbench
{
    class MOTOR_TASK
    {
        public bool enabled = false;
        public Motor.MOTOR_SENSE sense = Motor.MOTOR_SENSE.FORWARD;

        public bool resetRequested = false;
        public int stepsToDo = 0;

        public void clear()
        {
            this.stepsToDo = 0;
            enabled = false;
            sense = Motor.MOTOR_SENSE.FORWARD;
        }
    }   

    class Motor
    {
        public enum MOTOR_SENSE
        {
            BACKWARD = 0,
            FORWARD = 1
        }

        private int STEP_INTERVAL_MS = 1000;
        Thread bgTask;
        public MOTOR_TASK motorTask;

        /* Pins */
        GPIO protectPin;
        GPIO sensePin;
        GPIO inpulsPin;
        GPIO resetPin;
        GPIO enablePin;

        /* Constructor */
        public Motor(ConnectorPin protect, ConnectorPin sense, ConnectorPin inpuls, ConnectorPin reset, ConnectorPin enable)
        {
            /* Config pins */
            protectPin = new GPIO(protect, PinDirection.Input);
            sensePin = new GPIO(sense, PinDirection.Output);
            inpulsPin = new GPIO(inpuls, PinDirection.Output);
            resetPin = new GPIO(reset, PinDirection.Output);
            enablePin = new GPIO(enable, PinDirection.Output);

            /* Init motor task */
            motorTask = new MOTOR_TASK();

            /* Disable motor by default to prevent heating */
            setEnable(false);

            /* Set sense as FORWARD by default */
            sensePin.write( true );

            /* Launch worker on a new thread */
            this.bgTask = new Thread(() =>
            {
                while (true)
                {
                    /* If there is an reset request */
                    if (this.motorTask.resetRequested)
                    {
                        resetPin.write(true);
                        Thread.Sleep(5);
                        resetPin.write(false);
                        this.motorTask.resetRequested = false;
                    }

                    /* If there are no more steps to do */
                    if (motorTask.stepsToDo == 0)
                    {
                        Thread.Sleep(1);
                        continue;
                    }

                    /* Do requested steps */
                    this.step();   // make one step
                    Thread.Sleep(50);
                    
                    if (this.motorTask.stepsToDo != -1) // an exact ammount of steps required steps requested
                    {
                        this.motorTask.stepsToDo--;
                    }
                }
            });

            /* Start the task on a background thread */
            bgTask.Start();
        }

        /* Destructor */
        ~Motor()
        {
            if(this.bgTask != null)
                this.bgTask.Abort();

        }

        public void setEnable(bool state)
        {
            /* 0: Motor enabled
             * 1: Motor disabled
             */
            enablePin.write(!state);
            motorTask.enabled = state;
        }

        public bool isFault()
        {
            return protectPin.read();
        }

        public void setSense(MOTOR_SENSE sense)
        {
            if (sense == MOTOR_SENSE.FORWARD)
                sensePin.write(true);
            else
                sensePin.write(false);

            this.motorTask.sense = sense;
        }

        public void reset(bool blocking = true)
        {
            if (blocking)
            {
                resetPin.write(true);
                Thread.Sleep(3);
                resetPin.write(false);
            }
            else
            {
                this.motorTask.resetRequested = true;
            }
        }

        public void setStepsIntervalMs(int intervalMs)
        {
            this.STEP_INTERVAL_MS = intervalMs;
        }

        public void run(int steps = -1) // by default -1 is unlimited 
        {
            this.motorTask.stepsToDo = steps;
        }

        public void stop()
        {
            motorTask.clear();
        }

        ///  This is a blocking call!!!
        public void step()
        {
            this.inpulsPin.write(true);
            Thread.Sleep(2);
            this.inpulsPin.write(false);
        }

    }   /* Class */
}   /* Namespace */
