#!/bin/bash
# pip 26.2.2 is the minimum with the emSign CA fix; see AmbiqAI/neuralSPOT#308.
python -m pip install --upgrade "pip>=26.2.2"
pip install .
