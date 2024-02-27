
rootnode = gr.node('root')

white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torso:set_material(white)
torso:scale(1, 2, 1)


shoulders = gr.mesh('sphere','shoulders')
torso:add_child(shoulders)
shoulders:set_material(red)
shoulders:translate(0, 3.5, 0)
shoulders:scale(1.6, 0.25, 0.5)


neck = gr.mesh('sphere', 'neck')
shoulders:add_child(neck)
neck:set_material(white)
neck:translate(0, 1.5, 0)
neck:scale(0.2, 0.8, 0.5)

neckJoint = gr.joint('neckJoint', {-50, 0, 50}, {-100, 0, 100})
neck:add_child(neckJoint)

head = gr.mesh('sphere', 'head')
neckJoint:add_child(head)
head:set_material(white)
head:translate(0, 0.8, 0)
head:scale(2, 1.5, 2)

lefteye = gr.mesh('cube', 'lefteye')
head:add_child(lefteye)
lefteye:set_material(blue)
lefteye:translate(-1, 0, 5)
lefteye:scale(0.2,0.2,0.2)

righteye = gr.mesh('cube', 'righteye')
head:add_child(righteye)
righteye:set_material(blue)
righteye:translate(1, 0, 5)
righteye:scale(0.2,0.2,0.2)

leftupperarm = gr.mesh('sphere', 'leftupperarm')
shoulders:add_child(leftupperarm)
leftupperarm:set_material(white)
leftupperarm:translate(-4.5, -1, 0)
leftupperarm:scale(0.2, 2, 0.75)


leftforearm = gr.mesh('sphere', 'leftforearm')
leftupperarm:add_child(leftforearm)
leftforearm:set_material(white)
leftforearm:translate(0, -1.5, 0)
leftforearm:scale(0.5, 1, 0.5)

lefthand = gr.mesh('sphere', 'lefthand')
leftforearm:add_child(lefthand)
lefthand:set_material(white)
lefthand:translate(0, -5, 0)
lefthand:scale(1, 0.2, 1)

rightupperarm = gr.mesh('sphere', 'rightupperarm')
shoulders:add_child(rightupperarm)
rightupperarm:set_material(white)
rightupperarm:translate(4.5, -1, 0)
rightupperarm:scale(0.2, 2, 0.75)


rightforearm = gr.mesh('sphere', 'rightforearm')
rightupperarm:add_child(rightforearm)
rightforearm:set_material(white)
rightforearm:translate(0, -1.5, 0)
rightforearm:scale(0.5, 1, 0.5)

righthand = gr.mesh('sphere', 'righthand')
rightforearm:add_child(righthand)
righthand:set_material(white)
righthand:translate(0, -5, 0)
righthand:scale(1, 0.2, 1)


rootnode:translate(0, 0.0, -10.0)

return rootnode