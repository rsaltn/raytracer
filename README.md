Requirements:
Linux or Unix like OS(i just hope it works on other devices),
OpenGL,
glad,
GLFW,
gcc or any other C compiler, up to you, but how to compile this you will have to figure out yourself

How to compile
gcc main.c glad.c -lglfw -ldl -lm -o planets

Then Run:

./planets

Controls:
WASD: move
Space/CTRL: move up/down respectively
Mouse: look around
Esc: quit

if you want to add smth, create objects in main.c with:

addSphere(x, y, z, radius, red, green, blue, emission);

emission = 0.0 means normal object.
emission > 0.0 makes it glow.



P.S. for ship judge
Sorry, it was done really fast and maybe works only on my laptop, i dunno. I was really distracted by AS exams. and english is not my first language, so sorry if there some stupid mistakes in namings or here.
