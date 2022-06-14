Game!!

# Log Entries
## Tuesday, 5/17, 4:00pm - 7:00pm
**WHO:** Pablo

**WHAT:** Began the demo and created the balls and walls of the game

**BUGS:** N/A

**RESOURCES USED:** N/A


## Tuesday, 5/17, 9:30pm - 1:30am
**WHO:** Aditi Pablo Akul Jake

**WHAT:** Worked on physics, created pockets, drew sprites, began implementing mouse controls

**BUGS:**

**RESOURCES USED:** 


## Wednesday, 5/18, 4:00pm - 5:00pm
**WHO:** Jake

**WHAT:** Continue working basic game physics and mechanics

**BUGS:** N/A 

**RESOURCES USED:** N/A


## Thursday, 5/19, 6:00pm - 8:00pm
**WHO:** Aditi

**WHAT:** Continued creating sprites for balls, table, cue stick.

**BUGS:** N/A 

**RESOURCES USED:** N/A


## Saturday, 5/21, 9:00pm - 11:00pm
**WHO:** Aditi

**WHAT:** Finished creating sprites for balls, table, cue stick. Exported sprites.

**BUGS:** N/A 

**RESOURCES USED:** N/A


## Tuesday, 5/24, 1:00pm - 12:00am
**WHO:** Pablo Akul

**WHAT:** Created game_state.h, implemented mouse controls, updated sdl_wrapper to handle mouse input, added sprite files to repo

**BUGS:** body_get_centroid was wrong but now it's right

**RESOURCES USED:** N/A


## Thursday, 5/26, 1:00pm - 2:30pm
**WHO:** Jake

**WHAT:** Worked on in-game menu, adding reset button

**BUGS:** memory access out of bounds error when resetting the game

**RESOURCES USED:** N/A


## Thursday, 5/26, 5:00pm - 9:00 pm

**WHO:** Jake

**WHAT:** Debug reset button, add mute button, add basic implementation of menu state and smooth transition into game state

**BUGS:** Had a few initial bugs that I had to clear out while compiling with the new code, but those were easy to fix

**RESOURCES USED:** N/A


## Thursday, 5/26, 7:00 pm - 12:30 pm

**WHO:** Aditi

**WHAT:** Integrated sprites into the game-- changes to body.c to incorporate sprite parameter. Debugged.

**BUGS:** Sprites not showing up. Severe lag.

**RESOURCES USED:** Documentation for SDL_image


## Thursday, 5/26, 11:00 - 1:00 am

**WHO:** Akul

**WHAT:** Debugged the on_key mehtod, almost fully working, added a vector function

**BUGS:** Had quite a few bugs with the implementation, but figured it out.


## Friday, 5/27, 11:00 pm - 1:30 am

**WHO:** Aditi

**WHAT:** Debugged Sprites.

**BUGS:** Sprites appeared. Need to fix dimensions and image storage. Cue stick needs to rotate.

**RESOURCES USED:** Documentation for SDL_image


## Saturday, 5/28, 10:00 pm - 1:30 pm

**WHO:** Aditi

**WHAT:** Fixed image storage issues (reduced lag). Cleaned up and created additional graphics.

**BUGS:** Fix cue stick rotation. Fix scale factor issue. Fix cue stick collisions.

**RESOURCES USED:** 


## Sunday, 5/28, 4:00 pm - 6:00 pm

**WHO:** Aditi

**WHAT:** Tried to implement stick rotation.

**BUGS:** Not working. Currently not displaying sprite so that progress can be made elsewhere.

**RESOURCES USED:** 


## Sunday, 5/29, 10:00pm - 12:30am

**WHO:** Jake

**WHAT:** Add functionality to rules page, clean up code. Create background for rules page and
render it. Begin working on background music implementation. 

**BUGS:** The rules page background isn't fully stretching over the entire bacground - need to adjust the 
image scaling.

**RESOURCES USED:** Documentation on SDL_mixer.

## Monday, 5/30, 2:30am - 3:30am
**WHO:** Aditi

**WHAT:** Fixed Cue stick rotation

**BUGS:** N/A

## Monday, 5/30, 1:00pm - 2:00pm
**WHO:** Aditi

**WHAT:** Created new rules page background, rendered, fixed scaling issue

**BUGS:** Need to generalize scale factor solution

## Monday, 5/30, 3:00pm - 5:00pm

**WHO:** Jake

**WHAT:** Add background music, begin working on sound effects and sound handler

**BUGS:** N/A

**RESOURCES USED:** Documentation on SDL_mixer.

**RESOURCES USED:** N/A

## Monday, 5/30, 5:00pm - 9:00pm
**WHO:** Pablo

**WHAT:** Began implementing game rules, determining when a user can click or hit the ball, arrow key functionality, etc.

**BUGS:** Reset button breaks sometimes, tapping space bar doesn't really work.

**RESOURCES USED:** N/A

## Tuesday, 5/31 5:00pm - 6:00pm
**WHO:** Jake

**WHAT:** Continue working on sound effects

**BUGS:** N/A

**RESOURCES USED:** N/A

## Wednesday, 6/1 12:00am - 1:00am

**WHO:** Jake

**WHAT:** Continue working on sound effects, add sound set struct to group sound effects together

**BUGS:** Sound effects not working

**RESOURCES USED:** SDL_Mixer documentation

## Wednesday, 6/1 1:30pm - 2:30pm

**WHO:** Jake

**WHAT:** Debug sound effects

**BUGS:** None

**RESOURCES USED:** SDL_Mixer documentation

## Wednesday 6/1 12:00pm - 2:00pm
**WHO:** Akul

**WHAT:** FInalizing user control of cue by setting a held time limit, and implementing chalk

**BUGS:** Still have to figure out how to adjust the collision elasticity

**RESROUCES USED:** N/A

## Wednesday 6/1 3:00pm - 12:00am
**WHO:** Pablo

**WHAT:** Implemented game rules, fixed some bugs

**BUGS:** The reset button is no longer buggy, but now starting a new game after reset sometimes (seemingly randommly) crashes it.

**RESROUCES USED:** N/A

## Wednesday 6/1 6:00pm - 1:00am
**WHO:** Aditi

**WHAT:** Created and displayed background graphics, graphics for power bar. Fixed scale factor issue. Added button graphics for main menu. Made the mute button switch sprites based on status. Added shadows.

**BUGS:** Need to make power meter work.

**RESROUCES USED:** N/A

## Thursday 6/2 7:00 am - 12:00 pm
**WHO:** Akul

**WHAT:** Updated sdl wrapper to handle c key presses, edited cue collsion handler and game on key to adjust the chalk level and elasticity each player has

**BUGS:** Still need to figure out how to limit the amount the cue can go back, and fix cue collision

## Thursday 6/2 2:00 pm - 7:00 pm
**WHO:** Aditi

**WHAT:** Added variable cue velocity feature with power bar and slider

**BUGS:** Need to display points and chalk.

## Thursday 6/2 10:00 am - 6/3 12:00 am
**WHO:** Pablo

**WHAT:** Finished implementing game rules, added scoring, fixed pockets, created dotted lines, finished power bar and pool stick movement.

**BUGS:** Reset button still buggy.

**RESROUCES USED:** N/A