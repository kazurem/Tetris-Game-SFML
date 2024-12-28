# Tetris-Game-SFML

I will be making a tetris game using the graphics library SFML (Simple and Fast Media Layer). This was made as a Semester Project.

### Note:
I used code from another git repository and modified it. The repository I cloned was: https://github.com/ThomasGITH/Tetris-SFML


### Modifications:
1. Added a section which shows the next piece that will come.
2. Improved rotation of blocks.
3. Added a Game Over fucntionality instead of the game just closing.
4. Changed Forwa font to Roboto.

### Game Picture:
![Tetris Game](./images/game-picture/image-1.png)

### How to run?
#### 1. Using bash in Linux:
```
./run.sh
```

#### 2. if you don't have bash:
```
g++ -std=c++17 main.cpp -o tetris -lsfml-graphics -lsfml-window -lsfml-system  && ./tetris
```

#### 3. if you are on windows:
The above command should also work on windows (i think).

### About SFML:
I was on Linux so downloading SFML was fairly easy. I just ran a single command that downloaded SFML from the distribution's official repository. 
If you are on windows then too bad for you since I don't know how to download and setup SFML there. Just watch some youtube video.

