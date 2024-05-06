# BOUNCE-2-game_DE1SoC

# BOUNCE 2

# Introduction:

Bounce the Ball is an engaging arcade-style game where the player controls a ball trapped in a world filled with thorns and oncoming bricks. The objective is to keep the ball alive by avoiding collisions with thorns and bricks for as long as possible while the game's difficulty gradually increases over time.
Game Description:
The game starts with the player being introduced to the premise: the ball is trapped in a thorn-infested world and must stay alive by avoiding collisions with thorns and bricks thrown by the enemy, Bob. As time progresses, the frequency of bricks thrown by Bob increases, making the game more challenging.
Controls:
The player can control the movement of the ball using the switches on the board. Switch '1' moves the ball down, while switch '2' moves it up. By strategically using these switches, the player can navigate the ball through the obstacles and survive for longer periods.
Scoring:
The player's score increases continuously as long as the ball stays alive. The longer the ball survives without colliding with obstacles, the higher the score.

# Language used : C
# Technologies used: 
De1SoC Board(FPGA)

# Gameplay Instructions:
1. Press button 1 to start the game.
2. Use switches '1' and '2' to control the ball's movement and navigate it through the obstacles.
3. Avoid collisions with thorns and oncoming bricks to keep the ball alive. 4. Survive for as long as possible to achieve a higher score.

# Code Description:
1. Initialization: The game environment is initialized, including setting up memory-mapped I/O for switches and keys.
2. Game Loop:
- The game operates in a continuous loop, updating the game state and
graphics based on user input and obstacle movement.
- It continuously checks for input from switches to control the ball's
movement.
- The game also handles collision detection between the ball and
obstacles, ending the game if a collision occurs.
3. Graphics and Display:
- Basic graphics operations are used to draw shapes like circles,
rectangles, and lines to represent the ball, obstacles, and background.
- Different colors are used to differentiate between various elements on
the screen.
4. Input Handling:
- The code continuously checks for input from switches and buttons to
control the game's flow, including starting and restarting the game.
5. Scoring Mechanism:
- The player's score increases continuously as long as the ball remains
alive without colliding with obstacles.
6. Obstacle Generation:
- Bricks are generated and thrown towards the ball at regular intervals,
with the frequency of brick throws increasing over time to increase the game's difficulty.

# How to run:
1.Go to https://cpulator.01xz.net
2.Choose "Nios II" Architecture and "Nios II De1-SoC" System and click "Go"
3.Or alternatively, you can directly go to https://cpulator.01xz.net/?sys=nios-de1soc
4.On top of the Editor, change the language from "Nios II" to "C"
5.Copy the code from the file "game.c" in this repository, delete everything from the editor and paste the text from the file.
6.Now click "Compile and Load". When it has been compiled, click "Continue"
7.Read further instructions from the game to play!

# Challenges
1. This game is still not 100% complete. The main reason you see those triangular "Thorns" on the sides of the board too, but the ball never goes there, and the reason I named this game as "Bounce 2" is that I wanted it to make a game where the ball jumps from one block to another and you have to keep jumping from one to another to survive. If you are in air, you fall on the thorns below, and if you are on a block you can "ride" it until it crashes into the thorns on the sides. This would have been much more addictive and entertaining to the player.

2. However, this had several challnges, as actually incorporating "physics" or elastic collisions into the game was difficult. This was introducing disruptions in maintaining the simultainety of the game and keep the blocks moving at their regular pace. I have not used threading (neither do I know much of it) hence there are multiple nested loops running the game, and hence I coulnd not incorporate much of physics into it.

3. The score could not be displayed on the 7-segment display, although as you can see in the code, I am keeping track of it. Given more time, I'd have easily incorporated it.

# Remarks

This game was a part of MiniProject-II given by our computer architecture Course Instructor, Dr. Jimson Matthew.
All the work that you see has been solely done by me within the span of 3 days. I hope to take out some time in the future to incorporate more elements into the game and make it more engaging.

Thanks for reading! In case of query, feel free to contact-hrishikeshchoudhary901@gmail.com.
Feel free to share if you are able to make improvements in this game! I'd love someone who shares my interests.
