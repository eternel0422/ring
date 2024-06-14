#===================================================================#
# Based on Original Sample from RayLib (https://www.raylib.com/)
# Ported to RingRayLib by Ring Team  
# Uses Vec3(), Vec3SetX(), Vec3SetY() and Vec3SetZ() 
# This example doesn't use threads
# Vector3(x,y,z) --> Accept 3 parameters and Return a Ring object 
# Vec3() --> Doesn't accept parameters and Return a C pointer
#===================================================================#

/*
**	Performance on my machine 
**	Using Ring 1.21 (32bit) ---> 100 FPS  **FASTER**
**	Using Ring 1.21 (64bit) ---> 73  FPS
*/

load "raylib.ring"

screenWidth = 800
screenHeight = 450

InitWindow(screenWidth, screenHeight, "Waving Cubes")

camera = Camera3D( 16 , 14 , 16 ,
		   0  ,  0 , 0  ,
		   0  ,  1 , 0  , 
		   45 , 0 )

camera.position = Vector3( 30, 20 , 30 )
camera.target   = Vector3( 0 , 0  , 0  )
camera.up       = Vector3( 0 , 1  , 0  )
camera.fovy     = 70
camera.type     = CAMERA_PERSPECTIVE

numBlocks = 15
nMax      = numBlocks-1 
halfBlocks= numBlocks/2

cubepos    = Vec3()

SetTargetFPS(120)

color = Vec3()
Vec3SetY(color,0.75)
Vec3SetZ(color,0.9)

colors = []
for c = 0 to 359
	Vec3SetX(color, c)
	Add(colors, ColorFromHSV_2(color))
next

while !WindowShouldClose()

	time  = GetTime()
	scale = (2.0 + sin(time))*0.7

	cameraTime        = time*0.3
	camera.position.x = cos(cameraTime)*40
	camera.position.z = sin(cameraTime)*40

	BeginDrawing()

	ClearBackground(RAYWHITE)

	BeginMode3D(camera)

	DrawGrid(10, 5.0)

	for x = 1 to nMax 
		for y = 1 to nMax 
			for z = 1 to nMax 

				blockScale = (x + y + z)/30

				scatter = sin(blockScale*20 + (time*4))

				Vec3Set(cubepos,(x - halfBlocks)*(scale*3) + scatter,
						(y - halfBlocks)*(scale*2) + scatter,
						(z - halfBlocks)*(scale*3) + scatter)

				newcubeSize = (2.4 - scale)*blockScale
				DrawCube_2(cubePos,newcubeSize,newcubeSize,newcubeSize, colors[((x + y + z)*18.0)%360 + 1])

			next
		next
	next

	EndMode3D()

	DrawFPS(10, 10)

	EndDrawing()

end

CloseWindow()
