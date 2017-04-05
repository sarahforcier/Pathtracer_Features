Path Tracer: Nights of the Final Project I
======================

**University of Pennsylvania, CIS 561: Advanced Computer Graphics, Final Project
Part I**

Overview
------------
As the semester draws to its close, we embark on the final additions to your
path tracers. The first component of the final project will be to implement at
least one of the "basic" features listed in the writeup below, along with
a lens-based camera model capable of producing depth of field. We will not
provide any base code for this assignment; it is up to you to create any classes
you will need to complete your feature(s). These features will be due a week
into the final project, but don't let that stop you from working on the larger
features due at the end of the semester!

Useful Reading
---------
The following PBRT chapters are relevant to the features listed for this
assignment:
* Chapter 6.4: Realistic Cameras
* Chapter 12: Light Sources

Updating this README (5 points)
-------------
Make sure that you fill out this `README.md` file with your name and PennKey,
along with __embedded images__ of your test renders. If necessary, create a
custom JSON scene to show off your chosen feature(s).

Lens-based camera (REQUIRED) (10 points)
--------
Implement the thin-lens camera model as described in chapter 6.2.3 of PBRT.
You may wish to make this class inherit from the base `Camera` class we have
provided. You will also have to alter the `Scene` class to include an instance
of this new camera type. Render at least one scene with this new camera in order
to exhibit its depth-of-field effect.

One additional feature (60 points)
----------------
Choose one feature from the list below to implement in your path tracer. Note
that some features are not discussed in PBRT, such as constructive solid
geometry. Please refer to the course slides for information on these features,
or any links provided in this writeup or other course resources like Piazza.
* #### Constructive Solid Geometry (CSG) and De-Noising
Create a data structure, e.g. a binary
tree, that can be used to combine different geometries using the three basic
CSG operations: union, difference, and intersection. You will have to write an
`Intersect` function for this data structure, and would be best served to make
it inherit from the `Primitive` class. Your data structure does not need to
handle materials or light sources; these may be properties of the `Primitives`
contained _within_ the data structure. In addition to your CSG, you must also
implement a post-processing algorithm to reduce the __low-level__ noise in your
final renders. We recommend using something as simple as a k-means algorithm
with a high number of buckets to reduce noise without also blurring edges or
reducing texture quality.
* #### Other light sources
Implement at least two of the light sources described in Chapter 12 of PBRT,
aside from `DiffuseAreaLight`, which has already been provided to you. If
you choose to implement additional light types, each one will be worth 10 extra
credit points.
* #### Complex implicit surfaces
Implement a type of `Shape` that uses ray marching to find an intersection with
itself, testing the ray march against a mathematical equation representing a
surface rather than, say, a voxel data structure. The [lecture slides](https://cis700-procedural-graphics.github.io/files/implicit_surfaces_2_21_17.pdf) from Penn's
procedural graphics course will be particularly helpful in this task. Please
implement several different (and interesting!) surface formulas for your
implicit surface class. [Wolfram Mathworld](http://mathworld.wolfram.com/topics/Surfaces.html) has a nice collection of 3D surface
formulas.
* #### Motion blur (65 points rather than 60 points)
The JSON scene loader class presently half-supports storing multiple frames of the
scene at different keyframe times. Update the `Scene` class so it can store an
array of object states, and update the `JSONReader` class to load these states
into the `Scene`'s array. Once you have achieved this, implement an `Integrator`
or extend an existing one to apply a motion blur effect to your scene when
rendered.

Extra credit (30 points maximum)
-----------
As always, you are free to implement other features for extra credit that are
not listed below. Make a private post on Piazza so we can help you determine how
many points your feature(s) would be worth.

You may implement additional features in the list above, each one worth 20
extra credit points.

Submitting your project
--------------
Along with your project code, make sure that you fill out this `README.md` file
with your name and PennKey, along with your test renders.

Rather than uploading a zip file to Canvas, you will simply submit a link to
the committed version of your code you wish us to grade. If you click on the
__Commits__ tab of your repository on Github, you will be brought to a list of
commits you've made. Simply click on the one you wish for us to grade, then copy
and paste the URL of the page into the Canvas submission form.
