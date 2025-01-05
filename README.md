# Root Builder Mover Tool for Mod Organizer's root_builder plugin

This is a GUI tool that supplements Mod Organizer's `root_builder` plugin. It automates the process of moving files and folders that belong in the "root" folder into the "root" folder of every mod that is detected to require such actions. The tool will create a directory if it is required and missing.

## Features

- Doesn't have any specific requirements. Supports anything. Profiles for **Cyberpunk 2077**, **Skyrim**, and **Fallout** are bundled in.
- Simple configuration through an intuitive GUI.
- Automatically detects and organizes files into the correct mod directories.
- Simulation run shows what actions would be undertaken without actually touching your files.

## Installation Instructions

- Download the latest release.
- Unpack in a folder on your system.
- Note: don't unpack game tools or install games in general into Program Files, because it's a special folder in Windows and opertaing on files within it requires administrator's rights.

## Configuration

### General Information

- Configuration fields (except for the mods directory path) use **PCRE regular expressions** delimited with `||`.

### Regular Expression Examples

- To match the exact string "engine": 

^engine$

- To match either "engine" or "r6": 

^engine$||^r6$

- To match any string ending with ".exe": 

.*\\.exe$


### Special Characters

Here are some magic characters and their meanings in PCRE:

- **`^`** - Matches the beginning of a string.
- **`$`** - Matches the end of a string.
- **`.`** (dot) - Matches any single character.
- **`*`** (star) - Matches zero or more occurrences of the preceding character.
- To match literal characters `.` or `*`, precede them with a backslash `\` (the escape character), like so:
  - `\.`, `\*`.

### Profiles

The app comes with several pre-configured profiles ready for use. You can base your new profile on the existing ones for easy customization.
- To base your profile off a bundled one, select a profile and modify it however you want.
- You can use the same name for your profile if you want to.
- Your profiles are saved into the **custom.cfg** file next to the tool's exe file. Profiles with the same name in custom.cfg are loaded over the bundled ones.

## Usage

- Open the tool and select the desired game profile.
- Configure the settings as necessary.
  - The **mods path** directory should point to the **mods** dir in your Mod Organizer's profile dir for the respective game.
- (Optional, but recommended, especially when configuring a new profile) Click the **Simulate** button to see what actions would an actual Run execute.
- Click the **Run** button to execute the file organization.

## Conclusion

That's about everything you need to know to configure and utilize the application effectively. Enjoy using the tool and have fun modding!

## Contact

For issues, questions, or contributions, please open an issue on the repository or contact the developer.
