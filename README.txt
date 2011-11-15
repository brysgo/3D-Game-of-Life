3D Game of Life
Bryan Goldstein
Computer Graphics 3
Project 3

This 3D cellular automata was made chiefly to preserve the patterns of the original 2D game. Cells are initially exist (as cubes) on all three planes. They run on these sliced planes under the original game of life rules except for 1 caveat.

In order for cells to have an impact beyond their slice, there needs to be a rule for impacting cells on other planes (or shapes). That is where this game slightly differs from the game of life. If you remember the original rules of life, they are as follows:

1) If a cell has two neighbors it will stay alive.
2) If a cell has three neighbors, it will be born (or stay alive).
3) A cell in any other configuration will die.

The modification I made is that if two living cells overlap on their plane's intersection, a cell will be born on that intersection on the third plane. 

Use the '4' key to toggle this special rule and see what the board looks like as three sets of isolated slices.

+----------------------+
|  Keyboard Commands   |
+---+------------------+
| p | play/pause       |
| r | reset/randomize  |
| c | clear            |
| + | faster           |
| - | slower           |
| 0 | step             |
| ↑ | edit cursor up   |
| ↓ | edit cursor down |
| ← | edit cursor left |
| → | edit cursor right|
| , | edit cursor back |
| . | edit cursor fwd  |
| / | toggle edit cell |
| e | toggle edit mode |
| w | move fwd         |
| a | strafe left      |
| s | move backward    |
| d | strafe right     |
| z | move down        |
|   | move up          |
| 1 | toggle torus'    |
| 2 | toggle spheres   |
| 3 | toggle cones     |
| 4 | toggle teapots*  |
| y | toggle wireframe |
+---+------------------+

*Teapots represent a cell that is occupied on two intersecting planes. The four key toggles the inter-planar rule effectively turning on/off teapots.