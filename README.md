# Lab 05: The Inter-Integrated Circuit (I2C) Interface

Authors : Kyle G. Gayliyev <br>
Date: 9-March-2024<br>
Course: ECE 6780 - Embedded System Design, ECE Department, The University of Utah<br>
GitHub IDs: ggayliye <br>
Repo: https://github.com/ggayliye/Lab5_ECE6780Embedded_SR24 <br>
Date: By 21-March-2024, 11:59pm (Time of when submission is due/will be ready to be evaluated)<br>
Copyright: ECE 6780, Kyle G. Gayliyev - This work may not be copied for use in Academic Coursework.

## Overview of the Lab 05
Within this lab’s exercises I will be using I2C to communicate between the STM32F072 and the L3GD20 digital gyroscope sensor. The first exercise involves reading a single register to familiarize myself with the basic concepts of writing and reading over I2C. The second exercise initializes and reads measured data from the sensor to implement a rotation indicator using the LEDs on the discovery board.

Lab 5 is consisted of 2 Parts:<br>

* Part 1: Reading the L3GD20 “WHO_AM_I” Register.
* Part 2: Using the L3GD20 to Implement a Rotation Indicator.


### Part 1: Reading the L3GD20 “WHO_AM_I” Register.
Instructions:<br>
#### Connecting The Sensor Pins 
The Discovery board is designed to use the SPI interface to connect to the sensor, so some minor rewiring is necessary. Your lab aide should provide cables for in-lab use. Outside the scheduled lab sessions, you will need to provide your own jumpers and pull-up resistors. The provided cables follow the schematic shown in Figure 5.9. The connections should be pretty intuitive if you examine the cable closely. Use the schematic above to connect the cable to your Discovery board. I recommend connecting the cables to the pins on the underside of the board, because they are longer. These cables make external pullup connections for the SCL and SDA lines, and also connect PB11 directly to PB15.

#### Setting the GPIO Modes
Aside from the SCL and SDA pins, the L3GD20 requires additional signals to set the interface mode and slave address. Pin PC0 is connected to the SPI/I2C mode select pin, and PB14 controls the slave address when in I2C mode. <br>
1. Enable GPIOB and GPIOC in the RCC. 
2. Set PB11 to alternate function mode, open-drain output type, and select I2C2_SDA as its alternate function.
3. Set PB13 to alternate function mode, open-drain output type, and select I2C2_SCL as its alternate function. 
4. Set PB14 to output mode, push-pull output type, and initialize/set the pin high. 
5. Set PC0 to output mode, push-pull output type, and initialize/set the pin high.

#### Initializing the I2C Peripheral
In this lab we will be using only the basic operation of the I2C peripheral. Because of this, most of the control bits can be left in their default state. You will only need to modify the few registers listed in the lab manual.<br>

1. Enable the I2C2 peripheral in the RCC.
* The I2C2 peripheral is simpler and requires less configuration than I2C1.
2. Set the parameters in the TIMINGR register to use 100 kHz standard-mode I2C. 
* See section 5.5.2 and figure 5.4 in this lab manual. 
* You will need to shift the parameter values into the proper bit-location and use bitwise operators to apply them to the register. 
3. Enable the I2C peripheral using the PE bit in the CR1 register.

#### Reading the Register
Figure 5.5 shows a complete flowchart indicating the steps and actions 
required to perform blocking reads and writes to an I2C device. 
However, in this exercise, each step will be listed as a complete 
walk-through of the process of performing a basic I2C transaction. 
For debugging purposes you may want to set LED patterns or print 
USART messages as you advance through the different portions. <br>
Set the transaction parameters in the CR2 register.
* Set the L3GD20 slave address = 0x6B
-- The slave address is documented in the I2C section of the sensor datasheet. It is
modified by the state of the SDO pin.
* Set the number of bytes to transmit = 1.
* Set the RD_WRN bit to indicate a write operation.
* Set the START bit.
2. Wait until either of the TXIS (Transmit Register Empty/Ready) or NACKF (Slave Not-
Acknowledge) flags are set.
* If the NACKF flag is set, the slave did not respond to the address frame. You may have
a wiring or configuration error.
* Continue if the TXIS flag is set.
3. Write the address of the “WHO_AM_I” register into the I2C transmit register. (TXDR)
4. Wait until the TC (Transfer Complete) flag is set.
5. Reload the CR2 register with the same parameters as before, but set the RD_WRN bit to
indicate a read operation.
* Don’t forget to set the START bit again to perform a I2C restart condition.
6. Wait until either of the RXNE (Receive Register Not Empty) or NACKF (Slave Not-
Acknowledge) flags are set.
* Continue if the RXNE flag is set.
7. Wait until the TC (Transfer Complete) flag is set.
8. Check the contents of the RXDR register to see if it matches 0xD4. (expected value of the
“WHO_AM_I” register)
9. Set the STOP bit in the CR2 register to release the I2C bus.

After successfully verifying that the “WHO_AM_I” register matches the expected value, include a
logic analyzer screenshot of the I2C transaction in your postlab.


### Part 2: Using the L3GD20 to Implement a Rotation Indicator.
Instructions:<br>

Now that you have completed basic reading and writing to the sensor, it is time to enable and use the data it produces. Since this lab is teaching basic I2C and not how to control the L3GD20 gyro effectively, you aren’t required to fully initialize the device to use the FIFO buffer and other features. In this section you will be repeatedly reading the sensor’s data registers within the main while loop of your program. Because the sensor only produces data at a defined rate (95 Hz default) you will want to introduce some delay between reads. Unlike the STM32F0 internal peripherals, you typically won’t perform read-modify-write operations on remote device registers. Instead, directly calculate the desired bit pattern and overwrite the entire register. If your device doesn’t appear to be operating correctly regardless of correct I2C transactions, you may want to perform a verification read after writing to check if things were written to the values you intended.<br>

#### Initializing the Gyroscope
Since we are polling the sensor in its basic mode, the only initialization step is enabling the axes and bringing the device out of power-down mode. This exercise does not provide step-by-step instructions on how to read and write to the required registers: you will want to refer to the flowcharts in figures 5.5 and 5.7.<br>
1. Enable the X and Y sensing axes in the CTRL_REG1 register.
2. Set the sensor into “normal or sleep mode” using the PD bit in the CTRL_REG1 register.
3. All other bits in the CTRL_REG1 register should be set to 0. These place the device in the
default low-speed mode.

All other control registers enable advanced features such as the FIFO, interrupt and filter systems. Unless you have the base system operating, don’t attempt to enable and use these.

#### Exercise Specifications
You will need to implement an application that fulfills the following requirements:
1. Initialize the L3GD20 gyroscope sensor to read the X and Y axes.
2. Read and save the value of the X and Y axis data registers every 100 ms.
* You will need to assemble the 16-bit measured value from the two data registers for
each axis.
3. Use the four LEDs to indicate whether each measured axis is positive or negative.
* Because of measurement noise and to prevent the lights from triggering due to small
vibrations, set a minimum threshold before changing the active LED.
* Design your application such that the LED nearest the direction of rotation lights up.
-- For example: Light the orange LED when the board is rotated/tilted in the positive
X-axis direction. Refer to figure 5.6 for axis orientation.


<pre><ins>Future extensions</ins> :  There will be no future additions to this project. </pre>

# Partnership

We're partnered in the lab with students of two, but each student is required to complete
their work individually.

# Progress Notes

<ins>1st Week Notes:</ins> <br>

On Sat, 9th of March-2024, these tasks are completed:
- Created the "Lab5_ECE6780Embedded_SR24" GitHub repo.
- Created the "lab05" project using STM32CubeMX Software

On Sat, 16th of March-2024, these tasks are completed:<br>
- After the lab yesterday, I completed some part of the lab's 1st task.
- Everything looks good so far. I will know once I test my code in the end.

<ins>2nd Week Notes:</ins> <br>
By March 17st, 2024 :<br>
- Completed the first part. I used 5kOhms resistors to be adjusted by two potentiometers.
- One of the challenges was figuring out TC flag. It was not set on the section of #7, Part1.
- I used TCR flag instead, as I used "reload" bit on the CR2.
- Other thing I figured out was assigning I2Cx_RXDR as the received data should be released
so that TCR can be flagged in the end. It was not mentioned on the lab manual.

By March 22nd, 2024 :<br>
- I finished the lab. The second part was a challenging lab for me. I spent extra 6-7 hours to complete this lab.


## Testing
No Unit Test files are created as the nature of the project. Manual testing 
are performed in each step to ensure code improvements. Check "Testing Instructions"
section below.

# Time Expenditures:
<pre>Lab05: Predicted Hours: 12h		Actual Hours:	13h		 </pre>

The actual hours recorded on top reflect the time spent for the assignment including the time 
spent in labs. It excludes time spent reading and understanding the lab assignment instructions 
at the beginning of the lab (pre-lab work).

# Comments to Evaluators:

I checked off the first part with the TA. However,
the TA allowed me to work on the 2nd part of the lab until the end of the day
on Friday the 22nd with an option of sending a video demonstration.
I sent my video to the TA on time. I am also adding a link to my demonstration video below:

Youtube link for Part 2 Demonstration : https://www.youtube.com/watch?v=H4dGtPt76pc&ab_channel=KyleG.Gayliyev


I am uploading the full project files in this repo. 
Therfore, you may not need some of the information below to
prepaire this project to run. However, I'm still including those parts below.


<em>To able to fully test the main.c, other files and tools are required.<br>
For example, I created the project using the STM32CubeMX software first. Then <br>
clicked "Code Generation" button from the top menu after adjusting necessary <br>
settings. The instructions on how to adjust the settings will be given below. Then, <br>
the software automatically opens the Keil uVision5 softwhere where we code main.c.<br>
The main.c will be located under the "Application/User/Core" folder on the left menu of 
the Keil uVision5 softwhere.</em><br>

The main.c includes the 1st and 2nd part of the assignment. One of the assignments<br>
must be commented out. To test the commented out part, you'll need to uncomment that <br>
section and comment out the already uncommented section. Follow the comments<br>
out the sections in the main.c file.

## Testing Instructions:
After reading the discussion above, let's adjust the settings of
STM32CubeMX and Keil uVision5 softwares. <br><br>
STM32CubeMX:<br>

Select STM32F0 in the Series filter.<br>
* Select STM32F0x2 in the Lines filter
* Select LQFP64 in the Package filter <br>
At this point, there should only be a few choices available, select STM32F072RBTx and press the OK
button.<br>
Come to Project ->Menu->Settings.<br>
Name the new project. Select a directory where STMCube can create subfolders to store project files.<br>
Change the Toolchain/IDE dropdown menu to MDK-ARM V5.<br>
On the Project tab, move to the Code Generator tab at the top of the window.<br>
STMCube may take a while to copy the files to the directory specified in the settings. Afterward,
you may be asked if you want to open the project folder or project file itself. Click "project file".<br>
Now you should be in the "Keil uVision5" program as it's opened automatically.<br><br>

Keil uVision5:<br>

Click "Flash" -> "Configure flash tools" from the top menu.<br>
Click "Target" from the top menu. Find "Arm complier" menu and select 
"use default compiler version..".


One the setting is done, replace the main.c file in the "Application/User/Core" folder <br>
with my main.c file you downloaded.<br>
From the top menu, "Project"->"Build Target".<br>
Plug in your STM32F072 Discovery Microcontroller to your computer. <br>
Click "Flash" -> "Download" from the top menu. Test it on your STM32F072 Discovery Microcontroller.

Thank you for evaluating this project and providing feedback. <br>

Have a wonderful day!

# Consulted Peers:
N/A

# Caution/Warnings
* Set the START bit in the CR2 register after configuring the slave 
address and transaction length. Similar to how the PE bit locks 
system-wide configurations, setting the START bit locks the 
transaction parameters until the peripheral has completed the address frame.

* Ensure that PB11 and PB13 are set to open-drain output type. 
Otherwise, the I2C slave will not be able to respond during 
communication. Leave PB15 in input mode since it is connected 
to PB11 through a jumper wire. Modifying the mode of pin PB15 
could cause a conflict if the two pins try to output different 
logic states.

* Most I2C devices will automatically advance to the next 
register when you attempt to read multiple data bytes. The 
L3GD20 also contains this feature, but you must explicitly 
request this feature when writing the starting register 
address. In order to read multiple bytes, you must set 
the most significant bit of the starting register 
address; otherwise the device will repeatedly send 
the same register data for each byte requested.


# Examples of Good Software Practice (GSP)
<pre><ins>DRY</ins> :</pre>
DRY is an abbreviation of "Don't repeat yourself". It's a principle of Software Engineering that
has the goal of reducing repetition and redundancy in the coding. Using abstractions and normalization
are advised over redundancy <a href="https://en.wikipedia.org/wiki/Don%27t_repeat_yourself">[2]</a>.

<pre><ins>Separation of Concerns</ins> :</pre>
This concept is similar to the "divide and conquer" principle where you divide
a big software project into small projects to complete. A software design technique that focuses on separating 
and freeing functionalities of a program is called Modular programming. Making each of the divided small 
projects independent and addressing a separate concern, it'll make it easy to reduce, detect 
and fix the errors. <a href="https://en.wikipedia.org/wiki/Separation_of_concerns">[3]</a>

<pre><ins>Good Code Typing Practices</ins> :</pre>
Good coding practices can be listed as: Naming variables with a relevant name, commenting 
in between code lines with a brief explanation if the code is not self-explanatory, creating 
helper methods that can be used multiple times and by other sections.


<pre><ins>Testing Strategies</ins> :</pre>
Unit Testing can be summarized in 3 types of techniques:<br>

1. <ins>Black Box Testing : </ins> In this testing, input, user interface, and output parts are covered.
2. <ins>White Box Testing : </ins> In this testing, functionality, design structure, and code models are covered.
3. <ins>Gray Box Testing : </ins> In this testing, analysis of code performance, relevant test cases,
methods, and functions are covered.<a href="https://www.geeksforgeeks.org/unit-testing-software-testing/">[4]</a>


# References:
1. Canvas Page Class Materials.(For example, lecture slides, additional resources and pdf files). <br>
2. https://en.wikipedia.org/wiki/Don%27t_repeat_yourself<br>
3. https://en.wikipedia.org/wiki/Separation_of_concerns<br>
4. https://www.geeksforgeeks.org/unit-testing-software-testing/<br>






