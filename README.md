# No mqtt discovery yet, so add this to your home assistant configuration

Connect to the COM port* of the AVR IoT and use the CLI to get the "thing name." (9600 BAUD)

```yaml
sensor:
- platform: mqtt
    name: "AVR IoT Temperature"
    state_topic: "<thing name>/sensor"
    unit_of_measurement: "°C"
    value_template: "{{ value_json.temp }}"
- platform: mqtt
    name: "AVR IoT Light"
    state_topic: "<thing name>/sensor"
    unit_of_measurement: "lx"
    value_template: "{{ value_json.light }}"
```

*I like the [MPLAB Data Visualizer](https://www.microchip.com/en-us/tools-resources/debug/mplab-data-visualizer) a lot, at least if you want to output a lot of raw data from the MCU