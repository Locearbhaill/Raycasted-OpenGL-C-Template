# Raycasted-OpenGL-C-Template
This is my first attempt at creating a raycaster, mimicking 3D operations with the use of maths and 2D vectors.

To compile this first ensure all of your includes at the top are pointing at the right files for your own setup. 

Then run the following command(in MinGW): 

        gcc raycaster.c -o yourNameForTheExE GL/glut32.lib -lopengl32 -lglu32 -lm 

Edit this command to suit your own setup, this is just a guideline

Controls in the game are standard "wsad"

w = forward.

s = backwards.

a = rotate left.

d = rotate right.

Edit the map by changing 1's and 0's as you need, what they do is commeneted in the code.
Turning the ray drawing on or off can give you a good idea of how the program works and gets a 3D viewing angle from a 2D space.

