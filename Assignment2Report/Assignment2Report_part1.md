# Assignment 2 - Part 1


##1.
I've decided to go for a scanline approach, scanning through a bounding box for each triangle,and for each point in the bounding box, i've checked whether the pixel is in the triangle or not.
The triangle check is done by looking at which side of the line equation is the point,
returning true only when each of the line functions have a positive result.

Snapshots:

![](pre_zbuff1.png)

![](pre_zbuff2.png)


##2.
After implementing the z-buffer:

No Depth Image:

![](nodepth_dolphin_over_banana.png)

Depth Image:

![](depth_dolphin_over_banana.png)


#Adding a few more examples in case not enough:
Depth Images from different cameras:
![](depth_cam1.png)
![](depth_cam2.png)
![](depth_cam3.png)
![](depth_cam4.png)

Regular Images from different cameras:
![](reg_cam1.png)
![](reg_cam2.png)
![](reg_cam3.png)
![](reg_cam4.png)


####Bonus:
Beethoven revolving around the banana:


![](beethoven_banana_1.png)
![](beethoven_banana_2.png)
![](beethoven_banana_3.png)
![](beethoven_banana_4.png)
![](beethoven_banana_5.png)