This is a GUI tool to suppliment Mod Organizer's root_builder plugin.
It moves files and folders that belong in the "root" folder into the "root" folder of every mod that is detected to require such actions, creating such a directory if it is required and is missing.

The tool supports game profiles and some config, all done in GUI. Profiles for Cyberpunk 2077, Skyrim and Fallout are provided. All you'll need to do is to set the path to the "mods" folder in your Mod Organizer's profile folder for the respective game.

Have fun.


Configuration:

All the configuration fields, except the mods dir path, are PCRE regular expressions delimited with ||.

Some examples of trivial but sufficient PCRE (excluding quotes):
To match "engine", write "^engine$".
To match "engine" and "r6", write "^engine$||^r6$".
To match something ending with ".exe", write ".*\.exe$".

And those magic characters are:
"^" - beginning of a string you're trying to match.
"$" - end of said string.
"." (dot) - matches any character.
"*" (star) - this is a multiplier symbol. Use it after whatever symbol, like so: ".*" - matches any number of any characters.
To match "." or "*" (actual characters themselves), preceed them with an "\" (it's called an escape character), like so: "\." or "\*" (and these are the escape sequences).

That's about everything you need to know to configure the app.

Like it's said above, the app comes with several profiles ready to use. You can base your new profile off them. To do that just pick a profile and modify it however you want in the app. The app will save it in custom.cfg. You can use the same name for the profile, the app will save it in custom.cfg and load it "above" a bundled one of the same name.

The app has a feature to update the bundled.cfg shall the need ever arise.