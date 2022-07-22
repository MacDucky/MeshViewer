# Assignment 2 - Part 2

Each Point Light has a similar transformation matrix to that of a regular meshmodel for the purposes of rendering it.

For point light sources, unlike parallel light sources,the position matters,thus each point light source save a field of position as well.

As for parallel light sources, there is a single direction in which it shines,regardless of it's location.

###Point Light GUI contains:

######Ambient/Diffuse/Specular intensities as needed. (3 color bars - 9 bars in total)
######Position drag bars
######Shininess(even though it a material property,didn't have enough time to change it's location...)


###Parallel Light GUI contains:
######Ambient/Diffuse/Specular intensities as needed. (3 color bars - 9 bars in total)
######Light Direction sliders
######Position(redundant,didn't have enough time to remove all the cosmetics)
######Shininess(same as Point Lights, should be in material properties!);



In addition there are checkboxes for the shading techniques we've learned.


###Point light movement:

![](1.png)
![](2.png)
![](3.png)
![](4.png)

###Teapot from different angles:
![](teapot1.png)
![](teapot2.png)
![](teapot3.png)

###Multiple Light Sources:
![](Multiple_Point_Lights.png)

###Parallel examples:

![](Parallel1.png)
![](Parallel2.png)
![](Parallel_single1.png)
![](Parallel_single2.png)
![](Parallel_single3.png)
![](Parallel_single4.png)
![](Parallel_single5.png)
![](Parallel_single6.png)
Light from above,from a different angle:
![](Parallel_single6_other_view.png)
More Examples:
![](Mesh%20Viewer%2017_01_2021%2020_10_54.png)
![](Mesh%20Viewer%2017_01_2021%2020_11_12.png)


###Shading

I've Implemented Flat and Gouroud shading, didn't have enough time to implement Phong:

Flat vs Gouroud shading:
![](GvsF.png)
![](GvsF2.png)

![](F2.png)
![](G2.png)

Didn't have enough time to implement a post-processing effect,
Though I'm sure i will :)




