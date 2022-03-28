# Heart-rate-measurement-system
## About the project:
Electronic Design Team Project (ENG2025) Software code to run the FRDM-KL25Z microcontroller.

## Abstract:
The purpose of this project is to showcase our understanding of electronics and design ability by means of designing a heart rate monitor which can calculate the pulse rate and it consists of an amplifier and a microprocessor The Heart Rate Monitor is an electrical circuit that clips onto a fingertip and detects heartbeat. 
This is accomplished by shining a light emitting diode (LED) into or through your finger and measuring the amount of light reflected or absorbed. We designed a printed circuit board (PCB) using provided materials and used the documents on Moodle for guidance, we started off with constructing functional circuits of a signal amplifier and a display matrix, then designed a full heart rate monitor circuit using computer-assisted design (CAD) tools and eventually ended on designing a PCB. However, we were unable to produce a functional PCB after extended trial and error attempts. 

## Introduction:
This project aims to produce a heart rate monitor, a tool used to track biological conditions of humans which is the heartbeat, a heartbeat is a two-part pumping action that takes about a second, the heart monitor has several applications most notably in the medical field. The reason why an amplifier is used in designing a heart rate monitor is because it acquires the analogue signal directly from the source which allows us to get an accurate reading after the analog signal is read it registers in the microprocessor which therefore translates the analog signal into a digital signal. This design utilizes both analogue and digital circuitry, taking advantage of the smaller error of an analogue system and the ease of data access of a digital system. When assembling the circuit, we used a bread board to place and assemble all the components making a heart monitor and the heartbeats were displayed on the oscilloscope.

## Methodology:
This design uses an optical sensor to detect the pulse in blood vessels, which then produces electrical signals based on the detected pulses, an amplifier amplifies the signal so it can be registered by the microprocessor. The microprocessor then translates the analogue signal into digital data, which gets input into the display driver, which then controls the display matrix to display the heart rate.

![image](https://user-images.githubusercontent.com/92950538/160391451-f7d2924d-8946-4c64-a306-ea2ccb07ef3f.png)

The sensor functions by detecting the light from an LED through the finger of the user with a phototransistor, the transparency of the finger change in relation to the flow of blood, thus the change in current through the phototransistor can be correlated with the heartbeat.
The amplifiers are crucial components of the design, the blocking capacitors make sure the signal that passes through the amplifier is direct current, as the power source is alternating current. As there’s a range of limited voltage the microprocessor can register, we need to make sure there is gain through the amplifier, so the signal can reach a detectable voltage.
The bandpass filters set up at the amplifier act as a filter for noise in the electrical system and prevent damage to the circuit. The frequency range of 1 to 20 Hz has been selected for the bandpass filter; this range of frequencies has been chosen as the amplifier is expected to operate between 1 to 5 Hz, and all signal with frequencies above 50 Hz must be excluded.

![image](https://user-images.githubusercontent.com/92950538/160391511-2cab4952-4c0c-4a24-8eb9-535f85c63cd1.png)
![image](https://user-images.githubusercontent.com/92950538/160391525-2eba48da-bb00-4106-983e-4b74c87bfdc3.png)

The Freedom Board microprocessor is selected for this design because it is based on the Cortex M0 architecture, which is relatively simple to program, while having all the necessary functions, including analogue input and master/slave protocol support.
The use of a display driver helps us convert digital data from the microprocessor to a visual output on the matrix display, this allows us to program the microprocessor to give a simple output to the driver rather than having to give output to each pin on the display individually.
This design aims to operate with conventional batteries, so a regulator is needed to limit the voltage input to 5 volts. Heatsinks are not implemented in this design as the circuit runs on a relatively low voltage and current, thus overheating isn’t a major concern and heatsinks are omitted.

## Software Design:
The software written for this device is composed of several functions, each handling different tasks in the system. The voltage of the analogue input from the amplifier is translated into a number between 0 and 65000, the value is then categorized into 1 of 8 groups depending on its magnitude, each corresponding a row on the display matrix. 
Once the categorization process is complete, a corresponding signature is added into a queue, each item on the queue represents 1 column on the matrix display. Whenever the queue updates, the display matrix block with the corresponding rows and columns as specified on the queue will be lit up.
It can be shown from the flowchart that the program runs on a simple linear loop that refreshes about every 50 ms, where the analogue input signal is fetched once per cycle, same goes for digital output. The aforementioned queue updates every once per such cycle, and this is how we achieve a semi-continues heart rate display.

## PCB Layout:
![image](https://user-images.githubusercontent.com/92950538/160391637-51bacdda-4ab0-4ba6-9d88-2734d067e3c3.png)
![image](https://user-images.githubusercontent.com/92950538/160391644-234c598f-baeb-4239-bd68-b7ae75070acd.png)

Tracks for the matrix display are soldered onto the bottom layer, as they cannot be top mounted. To the left of our display is the driver, you can see that the tracks that come out from this must route via the bottom layer before reaching the pins of the display. The FRDM board is placed to the right side of the board and takes up a third of the area. And the upper left area is used for the OpAmp circuit. Our battery connector and senor connector are placed near the freedom board, so they’re fairly centralized to the components.
One improvement that could be made to this layout is placing the decoupling capacitors currently located above the OpAmp closer to the device power pins on the freedom board.

## Results:
Despite our greatest attempt, we have failed to produce a functional PCB for this project up till the last lab session. While testing the final circuit design, the system failed to operate, despite each component of the design working individually in prior tests. Due to time constrains, we were unable to modify the hardware to a point where the system functions as a whole.
Using the multimeter, we were able to discern that no power was flowing past the OpAmp part of the circuit, specifically our matrix display was not receiving power to be able to output a trace.
On an individual component level, each component has shown to function in respective tests. The amplifier was giving the expected output values during tests and the display are generating a wave function as programmed.

## Conclusion:
As our final PCB failed to work, it can hardly be considered to have met most of the required specifications, though the amplifier and the matrix display each individually achieved their functioning requirements on its own. We were not able to determine the reason that caused our system to fail, further examination on the design would be required.
For further improvements in the future, we have considered adding the following features: a heartbeat per minute display can show the heart rate of the user, and a Bluetooth connection feature so the user can link the heart rate monitor to health apps on their other devices. We have also considered ways to improve aspect of our existing design, we can replace the microprocessor with a microcontroller and add only the necessary peripherals; in the current design, a lot of functionalities of the microprocessor is unutilized and can be omitted, a cut in redundancy would make the PCB more compact and improve the power efficiency of the device.
This project has been a precious learning experience in not only electronic knowledge, design skills and practical skills but also in teamwork and organization. Though we have failed to deliver the final product, the challenges and failures we have encountered throughout the course would certainly prove useful in our future, a well worth experience that allows us to reflect and improve ourselves upon.

## Circuit Schematics:
![image](https://user-images.githubusercontent.com/92950538/160391757-3e6e3912-b654-4ba9-95c7-6eb57a4ed3a3.png)
![image](https://user-images.githubusercontent.com/92950538/160391764-79d282c7-f93f-460b-ba6f-02506fa563b0.png)
![image](https://user-images.githubusercontent.com/92950538/160391770-edc6735b-10a0-4cdf-a12f-60a3959f22e3.png)
![image](https://user-images.githubusercontent.com/92950538/160391789-535770fe-19c4-4cf6-a04c-834fa24e2a19.png)
![image](https://user-images.githubusercontent.com/92950538/160391798-e40daf93-3531-49e2-b1be-e333316ab33d.png)
![image](https://user-images.githubusercontent.com/92950538/160391807-b7c32aa9-83c2-40f9-bd4f-7147678bb0e9.png)




