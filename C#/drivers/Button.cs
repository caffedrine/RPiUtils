using Raspberry.IO.GeneralPurpose;
using TestBench.Concrete;

namespace TestBench.Drivers
{
    public class Button : GPIO
    {
        /* Button states */
        public enum State
        {
            Up,
            Down
        }

        public bool reversedPolarity = true;
        public bool CurrentRead = false, PreviousRead = false;
        public State CurrentState = State.Up, PreviousState = State.Down;

        protected Button(ConnectorPin pin) : base(pin, PinDirection.Input)
        {
            CurrentRead = Read();
        }

        public State ReadState()
        {
            if(reversedPolarity)
            {
                if (base.Read() == true)
                {
                    PreviousState = CurrentState;
                    CurrentState = State.Up;
                    return State.Up;
                }
                else
                {
                    PreviousState = CurrentState;
                    CurrentState = State.Down;
                    return State.Down;
                }
            }
            else
            {
                if (base.Read() == false)
                {
                    PreviousState = CurrentState;
                    CurrentState = State.Up;
                    return State.Up;
                }
                else
                {
                    PreviousState = CurrentState;
                    CurrentState = State.Down;
                    return State.Down;
                }
            }
        }

        protected virtual void StateChanged(State NewState)
        {
            PreviousState = CurrentState;
            CurrentState = NewState;
        }

        protected override void VirtualTick()
        {
            CurrentRead = Read();
            if (CurrentRead != PreviousRead)
            {
                if (CurrentRead == false && PreviousRead == true)
                {
                    /* Button down */
                    StateChanged(reversedPolarity?State.Down:State.Up);
                }
                else
                {
                    /* Button UP */
                    StateChanged(reversedPolarity?State.Up:State.Down);
                }
                PreviousRead = CurrentRead;
            }

            /* Debug */
            //GeneralLogger.Instance.Log($"Current {CurrentRead.ToString()} Previous {PreviousRead.ToString()}");
        }

    }   /* Class */
}   /* Namespace */
