#include &quot;Arduino.h&quot;
#if !defined(SERIAL_PORT_MONITOR)
#error &quot;Arduino version not supported. Please update your IDE to the latest version.&quot;
#endif
#if defined(__SAMD21G18A__)
// Shield Jumper on HW (for Zero, use Programming Port)
#define port SERIAL_PORT_HARDWARE
#define pcSerial SERIAL_PORT_MONITOR
#elif defined(SERIAL_PORT_USBVIRTUAL)
// Shield Jumper on HW (for Leonardo and Due, use Native Port)
#define port SERIAL_PORT_HARDWARE
#define pcSerial SERIAL_PORT_USBVIRTUAL
#else
// Shield Jumper on SW (using pins 12/13 or 8/9 as RX/TX)
#include &quot;SoftwareSerial.h&quot;
SoftwareSerial port(12, 13);
#define pcSerial SERIAL_PORT_MONITOR
#endif
#include &quot;EasyVR.h&quot;
EasyVR easyvr(port);
//Groups and Commands
enum Groups
{
GROUP_0 = 0,
GROUP_1 = 1,
GROUP_2 = 2,
};
enum Group0
{ G0_HI_ROSEY = 0,
};
enum Group1
{ G1_LIGHT_ON = 0,
G1_LIGHT_OFF = 1,
G1_GREEN_LIGHT_ON = 2,
};
enum Group2
{
G2_LIGHT_ON = 0,
G2_LIGHT_OFF = 1,
};
//Grammars and Words
enum Wordsets
{
SET_1 = -1,
SET_2 = -2,
SET_3 = -3,
};
enum Wordset1
{ S1_ACTION = 0,
S1_MOVE = 1,
S1_TURN = 2,
S1_RUN = 3,
S1_LOOK = 4,
S1_ATTACK = 5,
S1_STOP = 6,
S1_HELLO = 7,
};
enum Wordset2
{
S2_LEFT = 0,
S2_RIGHT = 1,
S2_UP = 2,
S2_DOWN = 3,
S2_FORWARD = 4,
S2_BACKWARD = 5,
};

- 35 -

enum Wordset3
{ S3_ZERO = 0,
S3_ONE = 1,
S3_TWO = 2,
S3_THREE = 3,
S3_FOUR = 4,
S3_FIVE = 5,
S3_SIX = 6,
S3_SEVEN = 7,
S3_EIGHT = 8,
S3_NINE = 9,
S3_TEN = 10,
};
// use negative group for wordsets
int8_t group, idx;
const int greenLight = 8;
const int redLight = 9;
void setup()
{ // setup PC serial port
pcSerial.begin(9600);
pinMode(redLight, OUTPUT);
pinMode(greenLight, OUTPUT);
bridge:
// bridge mode?
int mode = easyvr.bridgeRequested(pcSerial);
switch (mode)
{
case EasyVR::BRIDGE_NONE:
// setup EasyVR serial port
port.begin(9600);
// run normally
pcSerial.println(F(&quot;Bridge not requested, run normally&quot;));
pcSerial.println(F(&quot;---&quot;));
break;
case EasyVR::BRIDGE_NORMAL:
// setup EasyVR serial port (low speed)
port.begin(9600);
// soft-connect the two serial ports (PC and EasyVR)
easyvr.bridgeLoop(pcSerial);
// resume normally if aborted
pcSerial.println(F(&quot;Bridge connection aborted&quot;));
pcSerial.println(F(&quot;---&quot;));
break;
case EasyVR::BRIDGE_BOOT:
// setup EasyVR serial port (high speed)
port.begin(115200);
pcSerial.end();
pcSerial.begin(115200);
// soft-connect the two serial ports (PC and EasyVR)
easyvr.bridgeLoop(pcSerial);
// resume normally if aborted
pcSerial.println(F(&quot;Bridge connection aborted&quot;));
pcSerial.println(F(&quot;---&quot;));
break;
}
// initialize EasyVR
while (!easyvr.detect())
{
pcSerial.println(F(&quot;EasyVR not detected!&quot;));
for (int i = 0; i &lt; 10; ++i)
{
if (pcSerial.read() == &#39;?&#39;)
goto bridge;
delay(100);
}
}
pcSerial.print(F(&quot;EasyVR detected, version &quot;));
pcSerial.print(easyvr.getID());
if (easyvr.getID() &lt; EasyVR::EASYVR3)
easyvr.setPinOutput(EasyVR::IO1, LOW); // Shield 2.0 LED off
if (easyvr.getID() &lt; EasyVR::EASYVR)

- 36 -
pcSerial.print(F(&quot; = VRbot module&quot;));
else if (easyvr.getID() &lt; EasyVR::EASYVR2)
pcSerial.print(F(&quot; = EasyVR module&quot;));
else if (easyvr.getID() &lt; EasyVR::EASYVR3)
pcSerial.print(F(&quot; = EasyVR 2 module&quot;));
else
pcSerial.print(F(&quot; = EasyVR 3 module&quot;));
pcSerial.print(F(&quot;, FW Rev.&quot;));
pcSerial.println(easyvr.getID() &amp; 7);
easyvr.setDelay(0); // speed-up replies
easyvr.setTimeout(5);
easyvr.setLanguage(0); //&lt;-- same language set on EasyVR Commander when code
was generated
group = EasyVR::TRIGGER; //&lt;-- start group (customize)
}
void loop()
{
if (easyvr.getID() &lt; EasyVR::EASYVR3)
easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)
if (group &lt; 0) // SI wordset/grammar
{
pcSerial.print(&quot;Say a word in Wordset &quot;);
pcSerial.println(-group);
easyvr.recognizeWord(-group);
}
else // SD group
{
pcSerial.print(&quot;Say a command in Group &quot;);
pcSerial.println(group);
easyvr.recognizeCommand(group);
}
do
{
// allows Commander to request bridge on Zero (may interfere with user

protocol)

if (pcSerial.read() == &#39;?&#39;)
{
setup();
return;
}
// &lt;&lt;-- can do some processing here, while the module is busy
} while (!easyvr.hasFinished());
if (easyvr.getID() &lt; EasyVR::EASYVR3)
easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off
idx = easyvr.getWord();
if (idx == 0 &amp;&amp; group == EasyVR::TRIGGER)
{
// beep
easyvr.playSound(0, EasyVR::VOL_FULL);
// print debug message
pcSerial.println(&quot;Word: ROBOT&quot;);
// write your action code here
// group = GROUP_X\SET_X; &lt;-- jump to another group or wordset
return;
}
else if (idx &gt;= 0)
{
// beep
easyvr.playSound(0, EasyVR::VOL_FULL);
// print debug message
uint8_t flags = 0, num = 0;
char name[32];
pcSerial.print(&quot;Word: &quot;);
pcSerial.print(idx);
if (easyvr.dumpGrammar(-group, flags, num))
{
for (uint8_t pos = 0; pos &lt; num; ++pos)
{
if (!easyvr.getNextWordLabel(name))

- 37 -
break;
if (pos != idx)
continue;
pcSerial.print(F(&quot; = &quot;));
pcSerial.println(name);
break;
}
}
// perform some action
action();
return;
}
idx = easyvr.getCommand();
if (idx &gt;= 0)
{
// beep
easyvr.playSound(0, EasyVR::VOL_FULL);
// print debug message
uint8_t train = 0;
char name[32];
pcSerial.print(&quot;Command: &quot;);
pcSerial.print(idx);
if (easyvr.dumpCommand(group, idx, name, train))
{
pcSerial.print(&quot; = &quot;);
pcSerial.println(name);
}
else
pcSerial.println();
// perform some action
action();
}
else // errors or timeout
{
if (easyvr.isTimeout())
pcSerial.println(&quot;Timed out, try again...&quot;);
int16_t err = easyvr.getError();
if (err &gt;= 0)
{
pcSerial.print(&quot;Error &quot;);
pcSerial.println(err, HEX);
}
}
}
void action()
{
switch (group)
{
case GROUP_0:
switch (idx)
{
case G0_HI_ROSEY:
//Turn acknowledging light on
digitalWrite(greenLight, HIGH);
delay(1000); // wait for a second
digitalWrite(greenLight, LOW); // turn the LED off by making

the voltage LOW

delay(1000);
group = GROUP_1;
break;
}
break;
case GROUP_1:
switch (idx)
{
case G1_LIGHT_ON:
easyvr.playSound(0, EasyVR::VOL_FULL);
digitalWrite(greenLight, HIGH);
break;
case G1_LIGHT_OFF:
easyvr.playSound(0, EasyVR::VOL_FULL);
digitalWrite(greenLight, LOW);
break;
case G1_GREEN_LIGHT_ON:
easyvr.playSound(0, EasyVR::VOL_FULL);

- 38 -
digitalWrite(greenLight, HIGH);
}
break;
case GROUP_2:
switch (idx)
{
case G2_LIGHT_ON:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands

easyvr.playSound(0, EasyVR::VOL_FULL);
digitalWrite(redLight, HIGH);
break;
case G2_LIGHT_OFF:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands

digitalWrite(redLight, LOW);
easyvr.playSound(0, EasyVR::VOL_FULL);
break;
}
break;
case SET_1:
switch (idx)
{
case S1_ACTION:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S1_MOVE:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S1_TURN:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S1_RUN:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S1_LOOK:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S1_ATTACK:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S1_STOP:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S1_HELLO:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
}
break;
case SET_2:
switch (idx)
{
case S2_LEFT:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S2_RIGHT:
// write your action code here

- 39 -

// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S2_UP:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S2_DOWN:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S2_FORWARD:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S2_BACKWARD:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
}
break;
case SET_3:
switch (idx)
{
case S3_ZERO:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_ONE:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_TWO:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_THREE:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_FOUR:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_FIVE:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_SIX:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_SEVEN:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_EIGHT:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
case S3_NINE:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands

- 40 -

break;
case S3_TEN:
// write your action code here
// group = GROUP_X\SET_X; &lt;-- or jump to another group or

wordset for composite commands
break;
}
break;
}
}