# EdgeImpulse Examples
## ei_anomaly_detection
This simple anomaly detector has been trained to signal a fault when fan speeds changed.

## ei_yes_no
The audio classifier continuously monitors AUDADC and signals when someone speaks 'yes' or 'no'

## hello_ambiq
This WIP model continuously monitors AUDADC and signals when someone speaks 'hello ambiq'. 
IT DOESN'T CURRENTLY WORK - EI model code is triggering a hardfault due to a misaligned access somewhere in the reduce.c module. Will file EI bug.
