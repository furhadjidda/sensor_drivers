# Some important notes on the implementation

These are some notes I came across while trying to understand the demo python code ( https://files.waveshare.com/upload/5/5c/Pico-Motor-Driver-code.7z ) provided by waveshare and then converting into the C code for pico.

# I2C
The lines gpio_pull_up(SDA_PIN); and gpio_pull_up(SCL_PIN); are used to enable the internal pull-up resistors on the SDA and SCL pins of the Raspberry Pi Pico.Pull-up resistors are necessary for I2C communication because the I2C bus requires the SDA (data line) and SCL (clock line) to be pulled up to the supply voltage when they are not being actively driven low by the master or slave devices.

Why Pull-Up Resistors Are Needed for I2C:

    Open-Drain Configuration: I2C lines use an open-drain (or open-collector) configuration, where the devices can pull the lines low but cannot drive them high. Instead, the lines are pulled high by resistors when no device is pulling them low.

    Bus Idle State: When no communication is happening, the SDA and SCL lines should be high. Pull-up resistors ensure that the lines are high when no device is actively driving them low.

    Ensuring Reliable Communication: Pull-up resistors help maintain the integrity of the signals on the I2C bus, ensuring that the voltage levels are correctly interpreted by the devices on the bus.

External vs. Internal Pull-Up Resistors:

    External Pull-Up Resistors: In many I2C implementations, external pull-up resistors (typically 4.7kΩ) are connected between the SDA and SCL lines and the supply voltage (e.g., 3.3V or 5V). This is the recommended approach for robust I2C communication, especially when multiple devices are connected to the bus or when longer cable lengths are used.

    Internal Pull-Up Resistors: The Raspberry Pi Pico (and many other microcontrollers) provides internal pull-up resistors that can be enabled via software. These internal resistors are typically weaker (higher resistance) than the recommended external pull-up resistors but can be sufficient for short, simple I2C setups with few devices.

# Controlling the Motors
To control your motor with the PCA9685 and the Waveshare Pico Motor Driver, you typically need to set PWM values for three channels because the motor driver uses an H-bridge configuration to control the motor. The three channels are used as follows:

    PWM for Motor Speed (Enable Pin)
    PWM for Motor Direction (Two Direction Pins)

Understanding the H-bridge Motor Control

In an H-bridge configuration, you have the following pins:

    Enable Pin: Controls the speed of the motor (PWM signal).
    Direction Pins: Two pins that determine the direction of the motor.

To move the motor, you typically need to:

    Set the Enable Pin: Control the speed using a PWM signal.
    Set the Direction Pins: Determine the direction of rotation by setting one pin high and the other low, and vice versa for the opposite direction.

Example Channels Assignment

Assuming you have the following setup:

    Motor 1
        Enable Pin: Channel 0
        Direction Pin 1: Channel 1
        Direction Pin 2: Channel 2



## References
    https://soldered.com/learn/hum-dc-motor-driver-dual-h-bridge/
    https://pico.pinout.xyz/
    https://www.waveshare.com/pico-motor-driver.htm
    https://files.waveshare.com/upload/b/b8/Pico_Motor_Driver_SCH.pdf
    https://files.waveshare.com/upload/5/5c/Pico-Motor-Driver-code.7z
