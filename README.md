# Arduino Easy VR

This Arduino sketch is in C++ and allows you to connect your Arduino to an Easy VR 3.0 voice shield and module.

This includes several command to do basic actions such as turn lights on and off using your voice.

The component used in this example is called Rosey and answers to that name. Device voice training needs to be done separately using the Easy VR software.

The commands are grouped together and this can accommodate different voices. In turn, the commands themselves correlate with the recording order on the device.
```enum Group2
{
G2_LIGHT_ON = 0,
G2_LIGHT_OFF = 1,
};```

