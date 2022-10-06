# neuralSPOT Peripheral Library

The peripheral library is a collection of useful tools for interacting with Ambiq's SOC peripherals. It currently includes support for

1. Controlling Apollo's power configuration
2. Reading the EVB buttons

## Controlling Power Configuration

This part of the component allows developers to easily configure the platform's power modes, which involves turning peripherals on or off, and configuring the MCU's cache, memory, and clock modes. Configuration can be used in two ways: the developer can choose one of the predefined optimized power settings (defined [here](https://github.com/AmbiqAI/neuralSPOT/blob/70438d631a160988412aa3ba0c27e15d589ac92c/neuralspot/ns-peripherals/src/ns_power.c#L51)), or can specify their own like so:

```c
const ns_power_config_t myConfig = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = true,
    .bNeedUSB = false,
    .bNeedIOM = true,
    .bNeedAlternativeUART = true,
    .b128kTCM = false
};

main() {
  ...
  ns_power_config(&myConfig);
  ...
}
```

## Reading EVB Buttons

This part of the component eases reading the EVB user buttons, a common operation when creating AI demos or tests. 



```c
main() {
	  // Init the button handler
    int g_intButtonPressed = 0;
    ns_button_config_t button_config = {
        .button_0_enable = true,
        .button_1_enable = false,
        .button_0_flag = &g_intButtonPressed,
        .button_1_flag = NULL
    };
    ns_peripheral_button_init(&button_config);

    // Wait until button is pressed
    while (g_intButtonPressed == 0) {
        do_something_maybe();
        ns_delay_us(1000);
    }
  	// button pressed, get ready for next press
    g_intButtonPressed = 0;
  	// ...
}
```

