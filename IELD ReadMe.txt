IELD is a tool to help Indigo Engine developers create levels.

It can add objects, rearrange them, set specific positions, save, and open.

To move the camera, use WSAD and QE for down and up.
To move faster, hold 2. To move slower, hold 1.
To look around, hold left mouse button and move the mouse.

Press escape to clear a menu. Press it again to go back one field or remove the menu.

The hide or show the Panel in the bottom-left, press P.

To add objects, press Space. Then type the directory to your mesh. Press Enter.
Then type the texture directory. It may be filled in for you. Press Enter.
The object will be added at the camera's position, but will face North.

To delete an object, press X, Delete, or Backspace while it is selected.

To select an object you have added, right click on it.
It should turn wireframe. All camera operations now act on it.
To deselect, right click on the object again, or anywhere else.

To set your position (or the active object), press Tab. Type your X, Y, and Z coordinates.

To set your facing direction (or an object's), press F. Type your X angle right from North.
Then type your Y angle up from North.

To snap objects to a Grid, press G. Selecting objects will now snap them to the nearest G.
For example, with G of 1 all objects you select will align to the nearest whole position, and will move in units of 1.

To copy the selected object, press Control-C. Pressing Control-V will now paste a duplicate to the camera.

To change the color, opacity, or shininess of an object, press M for Materials options. Type the R G B A components from 0 to 1 and the Shine from 0 to infinite. Transparent objects will not be sorted; use them sparingly.

To undo a change, press Control-Z. You can only undo one action. Then undo will undo the undo.

To discard the current world and start with a new file, press Control-N. Known issue: Position panel disappears.

To save the current World, press Control + S.
To save the current World for #include by Indigo, press Control + Shift + S.
(Then in Indigo: main() { Initialize(...); #include "MyWorld.ield" } )
To open a World, press Control + O. You cannot open Compile Worlds.

To view Help, press H. You can close out of this window when you are done.
