# Root Builder Mover Tool for Mod Organizer's root_builder plugin

This is a GUI tool that supplements Mod Organizer's `root_builder` plugin. It automates the process of moving files and folders that belong in the "root" folder into the "root" folder of every mod that is detected to require such actions. The tool will create a directory if it is required and missing.

## Features

- **Saves you from the hassle of remembering which folders and files have to be moved into the root folder and doing it all manually where needed.**
- Doesn't have any specific requirements. Supports anything. Profiles for **Cyberpunk 2077**, **Skyrim**, and **Fallout** are bundled in.
- Simple configuration through an intuitive GUI.
- Automatically detects and organizes files into the correct mod directories.
- Simulation run shows what actions would be undertaken without actually touching your files.

## Installation Instructions

- Download the latest release.
- Unpack in a folder on your system.
- Note: don't unpack game tools or install games in general into Program Files, because it's a special folder in Windows and opertaing on files within it requires administrator's rights.

## Usage

- Install mods in Mod Organizer regularly. **Don't** create the **root** folders and move files and directories manually.
- Run **root_builder_mover.exe**.
- Pick the profile for your game.
- Set the Mods path. It should point to the **Mods** dir in your Mod Organizer's profile dir for the respective game.
  - *If unsure, open Mod Organizer. In its Menu: **Tools -> Settings -> Paths**. Copy the **Mods** value.*
  - *If Mods value starts with %BASE_DIR%, then combine **Base Directory** value with whatever **follows %BASE_DIR%** in the Mods value.*
- Click **Simulate** to see what it would do. This step is optional.
- Click **Run** to apply changes.
- *The **Update** button checks if the bundled.cfg in this repository contains new profiles and downloads it if it does.*

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

## Conclusion

That's about everything you need to know to configure and utilize the application effectively. Enjoy using the tool and have fun modding!

## Contact

For issues, questions, or contributions, please open an issue on the repository or contact the developer.
