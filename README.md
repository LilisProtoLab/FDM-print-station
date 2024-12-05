# FDM print station

## Main features
This FDM print station contains:
+ One dry-air storage drawer with two compartments for spools of filament, flushed with dry compressed air to reach a humidity setpoint controlled via Home Assistant.
	+ Building-supplied compressed air is fed through a membrane filter to reduce the moisture level
   	+ Dried air is distributed to five compartments: two drawer compartments, and three printer compartments
   	+ Home Assistant integration controls the humidity setpoints for the drawer compartments and the printer enclosures
   	+ A screen on the front of the drawer displays the current humidity, temperature and door sensors + flushing status
   	+ Door sensors make sure flushing is only active while the compartments are closed
+ Three printer enclosures with: vertical sliding doors; temperature, humidity and door sensors; dry air flushing; exhaust ventilation.
	+ The enclosure is made from aluminium extrusions with 3mm PMMA sheet walls. The vertical sliding doors are 5mm PMMA sheets, with 3D printed handles and latches.
 	+ The sensors are connected to the same dried compressed air delivery system as the drawer compartments to optionally control humidity in the printer enclosures. This system only operates when the doors are closed.
	+  The exhaust ventilation is connected to the building exhaust system and controlled via manual, 3D printed gate valves.

```
.
|--- Build 
|--- Docs
|--- Results
|--- Software
| LICENSE
| README.md

```

The purpose of each subfolder is explained below:
+ Build: Contains all the (binary) design files that are needed for the hardware. Use subfolder for more complex assemblies.
+ Docs: Contains the documentation of the project: background information, sources, warnings, and build instructions. 
+ Results: For measurement or functional devices, it is good to report some results as a benchmark for others who try to replicate the project.
+ Software: For hardware projects that have an operating software or firmware, it is a good versioning practice to separate the software code from build instructions.

## Build instructions
_Guide the reader with the order of browsing your project repository for an optimum building experience_
 
When you want to start a new repository, it is better that you copy this repository to your project repository and start by changing all the description.
All instructions and (sub)headers are suggestions. You can change the style, order, or composition as you see necessary. 

It would be kind if you put a shortcut to your Bill of Materials file.
This template can be copied free of charge. 

## Outcomes


## Team
+ Project initiator: Pieter Kooijman @pkooijman
+ Contributors:
	+ Josh Hauser @catovector
 	+ Nico van Hijningen

## Get involved
_Especially for open source projects, it is benefitial to motivate the potential users of the project to contribute back or share their feedback. Make it easy for them._

Comments and suggestions on this folder structure are always welcome. Please create an issue to share your feedback or question, or if you prefer send a pull request. 

Better structured projects can explain a number of options for contributors such as: 
+ (where to start)
+ (issue template)
+ (direct contact)
+ (pull requests)

## License
_After the README, A LICENSE is the most important file in the project documentation. Without a license, there is too much uncertainty to try building anything on top of the original project._

This project is released under CC0 1.0 Universal. 
You can modify an reuse as you like.
The project team appreciates your suggestions or examples for enhancing the repository, but your consistent documentation of your project is the best gift to the world. Hopefully, this template could make that a bit easier for you. 

### (How to cite:)
_Additionally, you can specify how other can cite your project._

## (Funding)
_Be kind to your funders and mention their support visibly and consistently. They also need to show their resources are wisely spent._

![LPL sharing image](./Docs/Images/lpl_sharing.jpg)
